# Physics Module: Collision Detection Integration

## Context

GopherEngine needs a Physics module wrapping Bullet Physics for collision detection (no rigid body dynamics yet). Bullet is already integrated via FetchContent and builds successfully. The Physics module exists with empty `PhysicsWorld.hpp`/`.cpp` files. The goal is to provide clean, pedagogically transparent collision primitives (sphere/box) and collision testing, with a test scene demonstrating the system.

## Architecture Overview

Three new classes in the Physics module, plus modifications to CMake and MainLoopTest:

```
PhysicsWorld (Service)          -- wraps btCollisionWorld (collision-only)
  ColliderComponent (Component) -- base, owns btCollisionObject
    BoxColliderComponent         -- creates btBoxShape
    SphereColliderComponent      -- creates btSphereShape
```

**Key design decisions:**
- `PhysicsWorld` is a `Service<PhysicsWorld>` owned by `MainLoop` (like EventSystem, ResourceManager). The engine controls initialization and update ordering.
- **Bullet isolation via forward declarations:** `PhysicsWorld.hpp` forward-declares all Bullet types and uses `std::unique_ptr` for Bullet members. Bullet `#include`s live only in `PhysicsWorld.cpp`. This means `MainLoop.hpp` can include `PhysicsWorld.hpp` without Bullet leaking to other modules.
- Collision-only: uses `btCollisionWorld`, not `btDynamicsWorld`. No gravity, no rigid body simulation.
- Local transforms only (physics objects assumed to be root-level nodes). This is sufficient for the pedagogical use case and avoids complexity around world-space transform sync.
- Collision results stored per-frame as a contact map: `unordered_map<ColliderComponent*, vector<ColliderComponent*>>`.
- `btCollisionObject::setUserPointer(this)` links Bullet objects back to ColliderComponents.

## Implementation Steps

### Step 1: CMakeLists.txt — Wire up dependencies

**File:** `CMakeLists.txt`

Two new link lines:
```cmake
# Physics depends on Core (ColliderComponent inherits Component)
target_link_libraries(GopherEnginePhysics PUBLIC GopherEngineCore)

# Core depends on Physics (MainLoop owns PhysicsWorld) — PRIVATE because
# only MainLoop.cpp needs it; PhysicsWorld.hpp forward-declares Bullet types
# so no Bullet headers leak through MainLoop.hpp
target_link_libraries(GopherEngineCore PRIVATE GopherEnginePhysics)
```

This creates a circular static-library dependency (Core↔Physics), which CMake and the linker handle correctly for static libraries. The header-level dependency is one-way only: Physics headers include Core headers, not vice versa (MainLoop.hpp includes PhysicsWorld.hpp, which has no Bullet includes).

---

### Step 2: PhysicsWorld

**Files:**
- `include/GopherEngine/Physics/PhysicsWorld.hpp`
- `src/GopherEngine/Physics/PhysicsWorld.cpp`

**Header strategy:** Forward-declare all Bullet types in the header; `#include` Bullet only in the `.cpp`. Since the members are `std::unique_ptr` to forward-declared types, the destructor must be defined in the `.cpp` (where the full type is visible).

```cpp
// PhysicsWorld.hpp — NO Bullet #includes
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btDbvtBroadphase;
class btCollisionWorld;

class PhysicsWorld : public Service<PhysicsWorld>
{
public:
    PhysicsWorld();
    ~PhysicsWorld();  // defined in .cpp (unique_ptr needs full type for delete)

    void initialize();
    void shutdown();
    void update();

    void add_collider(ColliderComponent* collider);
    void remove_collider(ColliderComponent* collider);

    bool is_colliding(const ColliderComponent* collider) const;
    std::vector<ColliderComponent*> get_colliding_with(const ColliderComponent* collider) const;

private:
    std::unique_ptr<btDefaultCollisionConfiguration> configuration_;
    std::unique_ptr<btCollisionDispatcher> dispatcher_;
    std::unique_ptr<btDbvtBroadphase> broadphase_;
    std::unique_ptr<btCollisionWorld> collision_world_;

    std::vector<ColliderComponent*> colliders_;
    std::unordered_map<const ColliderComponent*, std::vector<ColliderComponent*>> contacts_;
};
```

**`update()` flow:**
1. For each registered collider: read its cached `Transform*`, convert `position_`/`rotation_` to `btTransform`, set on its `btCollisionObject`
2. Call `collision_world_->performDiscreteCollisionDetection()`
3. Clear `contacts_`
4. Iterate `dispatcher_->getNumManifolds()` manifolds — for each with `getNumContacts() > 0`, recover both `ColliderComponent*` via `getUserPointer()`, populate `contacts_` bidirectionally

**`shutdown()`:** Remove all collision objects from `collision_world_`, then reset unique_ptrs in reverse creation order.

**GLM ↔ Bullet conversion** (helper in .cpp):
- `glm::vec3 position` → `btVector3(x, y, z)`
- `glm::quat rotation` → `btQuaternion(x, y, z, w)`

---

### Step 3: ColliderComponent (base)

**Files:**
- `include/GopherEngine/Physics/ColliderComponent.hpp`
- `src/GopherEngine/Physics/ColliderComponent.cpp`

**Class design:**
```cpp
class ColliderComponent : public Component
{
public:
    ~ColliderComponent() override;

    void initialize(Transform& transform) override;

    bool is_colliding() const;
    std::vector<ColliderComponent*> get_colliding_with() const;

    btCollisionObject* get_collision_object() const;
    Transform* get_transform() const;

protected:
    ColliderComponent(); // only constructed via subclasses

    std::unique_ptr<btCollisionShape> shape_;         // set by subclass constructor
    std::unique_ptr<btCollisionObject> collision_object_;
    Transform* transform_{nullptr};
};
```

**`initialize()`:** Stores `&transform`, creates `btCollisionObject`, assigns `shape_`, sets user pointer to `this`, calls `PhysicsWorld::get().add_collider(this)`.

**Destructor:** Calls `PhysicsWorld::get().remove_collider(this)`.

---

### Step 4: BoxColliderComponent / SphereColliderComponent

**Files:**
- `include/GopherEngine/Physics/BoxColliderComponent.hpp`
- `src/GopherEngine/Physics/BoxColliderComponent.cpp`
- `include/GopherEngine/Physics/SphereColliderComponent.hpp`
- `src/GopherEngine/Physics/SphereColliderComponent.cpp`

These are minimal — constructor-only subclasses:

```cpp
// BoxColliderComponent — takes full size, converts to half-extents internally
// so students don't need to think in Bullet's half-extent convention
BoxColliderComponent(const glm::vec3& size = glm::vec3(1.0f));
// → creates btBoxShape(btVector3(size.x/2, size.y/2, size.z/2))

// SphereColliderComponent
SphereColliderComponent(float radius = 1.0f);
// → creates btSphereShape(radius)
```

Each stores the created shape in the base class `shape_` member. No additional methods needed.

---

### Step 5: MainLoop Integration

**Files:**
- `include/GopherEngine/Core/MainLoop.hpp`
- `src/GopherEngine/Core/MainLoop.cpp`

**MainLoop.hpp changes:**
- `#include <GopherEngine/Physics/PhysicsWorld.hpp>`
- Add `PhysicsWorld physics_world_;` member alongside `event_system_` and `resource_manager_`

**MainLoop constructor:** Add `Service<PhysicsWorld>::provide(&physics_world_);`

**MainLoop::run() changes:**
- Call `physics_world_.initialize()` after `renderer_.initialize()` and before `initialize()`
- Call `physics_world_.shutdown()` after the main loop exits (before destructor)
- Insert `physics_world_.update()` in the game loop:

```cpp
while(window_.is_open()) {
    float delta_time = clock_.delta_time();
    resource_manager_.poll();
    window_.handle_events();

    physics_world_.update();        // sync transforms, detect collisions
    update(delta_time);             // user code: query collisions, move objects
    scene_->update(delta_time);     // component updates

    handle_resize();
    renderer_.draw(*scene_);
    window_.display();
}
```

Physics runs **before** user update so collision results are available in `update()`. The user moves objects, and those new positions are picked up by physics on the next frame.

---

### Step 6: MainLoopTest — Collision Demo

**File:** `src/MainLoopTest/MainLoopTest.cpp`

**New members:**
```cpp
// Track objects and their starting positions for reset
struct PhysicsObject {
    std::shared_ptr<Node> node;
    glm::vec3 original_position;
    glm::vec3 velocity;
};
std::vector<PhysicsObject> objects_;
```

**`initialize()`** additions:
- Keep existing camera, light, material setup
- Create distinct materials: blue for spheres, orange for boxes
- Create test objects (all root-level nodes via `scene_->create_node()`):

| Object | Type | Position | Velocity | Visual |
|--------|------|----------|----------|--------|
| Sphere A | radius 0.5 | (-3, 0, 0) | (+1, 0, 0) | blue sphere mesh |
| Sphere B | radius 0.5 | (3, 0, 0) | (-1, 0, 0) | blue sphere mesh |
| Box A | size 1.0 | (0, -3, 0) | (0, +1, 0) | orange cube mesh |
| Box B | size 1.0 | (0, 3, 0) | (0, -1, 0) | orange cube mesh |
| Box C | size 1.0 | (0, 0, -3) | (0, 0, +1) | orange cube mesh |
| Sphere C | radius 0.5 | (0, 0, 3) | (0, 0, -1) | blue sphere mesh |

This tests all three collision pair types: **sphere-sphere** (x-axis), **box-box** (y-axis), and **sphere-box** (z-axis).

Each node gets a `MeshComponent` + collider component. Store in `objects_` vector with original position and velocity.

**`update()`:**
```
1. For each object: if collider is_colliding(), reset position to original_position
2. Move each object: position += velocity * delta_time
```

Physics has already run before `update()` (engine calls `physics_world_.update()` first in the loop), so collision results are ready to query. The user checks for collisions, resets if needed, then moves objects. New positions are picked up by physics next frame.

This creates pairs of objects approaching each other along each axis. When they meet and collide, they snap back to start and begin again.

---

## Files Modified/Created

| File | Action |
|------|--------|
| `CMakeLists.txt` | Add Core↔Physics link dependencies |
| `include/GopherEngine/Core/MainLoop.hpp` | Add PhysicsWorld member |
| `src/GopherEngine/Core/MainLoop.cpp` | Initialize, update, shutdown PhysicsWorld |
| `include/GopherEngine/Physics/PhysicsWorld.hpp` | Implement (currently empty) |
| `src/GopherEngine/Physics/PhysicsWorld.cpp` | Implement (currently empty) |
| `include/GopherEngine/Physics/ColliderComponent.hpp` | **New** |
| `src/GopherEngine/Physics/ColliderComponent.cpp` | **New** |
| `include/GopherEngine/Physics/BoxColliderComponent.hpp` | **New** |
| `src/GopherEngine/Physics/BoxColliderComponent.cpp` | **New** |
| `include/GopherEngine/Physics/SphereColliderComponent.hpp` | **New** |
| `src/GopherEngine/Physics/SphereColliderComponent.cpp` | **New** |
| `src/MainLoopTest/MainLoopTest.cpp` | Add physics demo scene |

## Existing Code to Reuse

- `Service<T>` CRTP pattern — `include/GopherEngine/Core/Service.hpp`
- `Component` base class — `include/GopherEngine/Core/Component.hpp`
- `Transform` struct — `include/GopherEngine/Core/Transform.hpp`
- `MeshFactory::create_cube()` / `create_sphere()` — `include/GopherEngine/Resource/MeshFactory.hpp`
- `BlinnPhongMaterial` — `include/GopherEngine/Renderer/BlinnPhongMaterial.hpp`
- `MeshComponent` — `include/GopherEngine/Core/MeshComponent.hpp`

## Verification

1. **Build:** `cmake --build build --config Debug` — should compile with no errors
2. **Run:** Launch `MainLoopTest` — should see 6 objects (3 spheres, 3 boxes) moving toward each other along x/y/z axes
3. **Collision pairs:** Verify all three pair types trigger resets: sphere-sphere (x-axis), box-box (y-axis), sphere-box (z-axis)
4. **Reset behavior:** When pairs meet in the center, they should snap back to their original positions and begin moving again — no objects get stuck in perpetual collision
5. **Visual check:** Objects should have distinct colors (blue spheres, orange boxes) and be lit correctly
6. **Camera:** Orbit controls should still work — rotate around to verify objects move along all three axes
7. **Destruction safety:** Closing the application should not crash — PhysicsWorld::shutdown() must cleanly remove all collision objects before destroying Bullet infrastructure

## Design Rationale (vs. alternatives considered)

These decisions were informed by reviewing two alternative plans (Plan-Codex.md, Plan-Gemma.md):

- **PhysicsWorld owned by MainLoop:** PhysicsWorld is a MainLoop member (like EventSystem, ResourceManager), giving the engine control over initialization and update ordering. Bullet types are forward-declared in PhysicsWorld.hpp so no Bullet dependencies leak through to other modules via MainLoop.hpp. The Core↔Physics circular static-library dependency is handled cleanly by CMake/linker.
- **Component owns its Bullet objects:** Codex centralizes ownership in PhysicsWorld via a ColliderId registry. Per-component ownership is simpler, follows RAII, and avoids an indirection layer that adds complexity without clear pedagogical benefit in a first pass.
- **Full-size BoxCollider API:** Adopted from Codex — students should not need to reason about Bullet's half-extent convention. The wrapper converts internally.
- **Query-based collision reporting:** Gemma returns collision pairs from update(); Codex uses both a pair vector and per-collider queries. The contact map with is_colliding()/get_colliding_with() is the most natural API for game-side code and avoids the caller searching through a pair list.
- **"Component" suffix in naming:** Gemma uses `SphereCollider`/`BoxCollider`. The engine consistently uses the suffix (MeshComponent, LightComponent, CameraComponent), so we follow that convention.
