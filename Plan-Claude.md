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
- `PhysicsWorld` is a `Service<PhysicsWorld>` (follows existing ResourceManager/EventSystem pattern) but is **not** added to MainLoop — the user creates and drives it in their subclass. This avoids Core depending on Physics and is more pedagogically transparent.
- Collision-only: uses `btCollisionWorld`, not `btDynamicsWorld`. No gravity, no rigid body simulation.
- Local transforms only (physics objects assumed to be root-level nodes). This is sufficient for the pedagogical use case and avoids complexity around world-space transform sync.
- Collision results stored per-frame as a contact map: `unordered_map<ColliderComponent*, vector<ColliderComponent*>>`.
- `btCollisionObject::setUserPointer(this)` links Bullet objects back to ColliderComponents.

## Implementation Steps

### Step 1: CMakeLists.txt — Add Core dependency to Physics

**File:** `CMakeLists.txt` (line ~126)

Add after the existing `target_link_libraries` lines for GopherEnginePhysics:
```cmake
target_link_libraries(GopherEnginePhysics PUBLIC GopherEngineCore)
```

Physics depends on Core because ColliderComponent inherits Component. Must be `PUBLIC` so consumers get Core's include paths.

---

### Step 2: PhysicsWorld

**Files:**
- `include/GopherEngine/Physics/PhysicsWorld.hpp`
- `src/GopherEngine/Physics/PhysicsWorld.cpp`

**Class design:**
```cpp
class PhysicsWorld : public Service<PhysicsWorld>
{
public:
    PhysicsWorld();
    ~PhysicsWorld();

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
// BoxColliderComponent
BoxColliderComponent(const glm::vec3& half_extents = glm::vec3(0.5f));
// → creates btBoxShape(btVector3(half_extents.x, half_extents.y, half_extents.z))

// SphereColliderComponent
SphereColliderComponent(float radius = 1.0f);
// → creates btSphereShape(radius)
```

Each stores the created shape in the base class `shape_` member. No additional methods needed.

---

### Step 5: MainLoopTest — Collision Demo

**File:** `src/MainLoopTest/MainLoopTest.cpp`

**New members:**
```cpp
PhysicsWorld physics_world_;

// Track objects and their starting positions for reset
struct PhysicsObject {
    std::shared_ptr<Node> node;
    glm::vec3 original_position;
    glm::vec3 velocity;
};
std::vector<PhysicsObject> objects_;
```

**Constructor:** Register `Service<PhysicsWorld>::provide(&physics_world_)`.

**`initialize()`** additions:
- Call `physics_world_.initialize()`
- Keep existing camera, light, material setup
- Create distinct materials: blue for spheres, orange for boxes
- Create test objects (all root-level nodes via `scene_->create_node()`):

| Object | Type | Position | Velocity | Visual |
|--------|------|----------|----------|--------|
| Sphere A | radius 0.5 | (-3, 0, 0) | (+1, 0, 0) | blue sphere mesh |
| Sphere B | radius 0.5 | (3, 0, 0) | (-1, 0, 0) | blue sphere mesh |
| Box A | half-ext 0.5 | (0, -3, 0) | (0, +1, 0) | orange cube mesh |
| Box B | half-ext 0.5 | (0, 3, 0) | (0, -1, 0) | orange cube mesh |
| Box C | half-ext 0.5 | (0, 0, -3) | (0, 0, +1) | orange cube mesh |
| Sphere C | radius 0.5 | (0, 0, 3) | (0, 0, -1) | blue sphere mesh |

Each node gets a `MeshComponent` + collider component. Store in `objects_` vector with original position and velocity.

**`update()`:**
```
1. Move each object: position += velocity * delta_time
2. physics_world_.update()
3. For each object: if collider is_colliding(), reset position to original_position
```

This creates pairs of objects approaching each other along each axis. When they meet and collide, they snap back to start and begin again.

---

## Files Modified/Created

| File | Action |
|------|--------|
| `CMakeLists.txt` | Add GopherEngineCore dependency to Physics lib |
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
3. **Collision test:** When pairs meet in the center, they should snap back to their original positions and begin moving again
4. **Visual check:** Objects should have distinct colors (blue spheres, orange boxes) and be lit correctly
5. **Camera:** Orbit controls should still work — rotate around to verify objects move along all three axes
