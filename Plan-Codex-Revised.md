# Bullet Collision Module First Pass

## Summary
- Integrate Bullet in collision-only mode with `btCollisionWorld`, not `btDiscreteDynamicsWorld`. This keeps the first pass focused on collider representation and overlap testing.
- Keep `PhysicsWorld` owned by `MainLoop` so the engine controls initialization, per-frame update order, and destruction order.
- Preserve module hygiene by hiding Bullet behind the Physics module API. `PhysicsWorld.hpp` should not expose Bullet headers or Bullet types to the rest of the engine.

## Key Changes
- `GopherEnginePhysics` links `GopherEngineCore` publicly because collider components derive from `Component`.
- `PhysicsWorld` becomes `Service<PhysicsWorld>` and wraps Bullet’s collision configuration, dispatcher, broadphase, and `btCollisionWorld`.
- `MainLoop` owns `PhysicsWorld` directly as a member, alongside `EventSystem` and `ResourceManager`.
- `MainLoop` constructor registers `PhysicsWorld` with `Service<PhysicsWorld>::provide(...)`.
- Keep `physics_world_` declared before `scene_` so destruction order is correct:
  - `scene_` is destroyed first,
  - collider components unregister from the still-alive `PhysicsWorld`,
  - `PhysicsWorld` is destroyed afterward.
- `MainLoop.hpp` includes `PhysicsWorld.hpp`, but `PhysicsWorld.hpp` must hide Bullet internals using a private implementation or equivalent opaque storage so Bullet dependencies do not leak through engine headers.
- `PhysicsWorld` keeps a collider registry containing:
  - engine `ColliderId`,
  - non-owning `Transform*`,
  - owned Bullet shape,
  - owned Bullet collision object,
  - non-owning back-pointer to the collider component.
- `PhysicsWorld::update()` performs one frame of collision evaluation:
  - sync engine transforms into Bullet objects,
  - clear previous collision state,
  - run `performDiscreteCollisionDetection()`,
  - walk Bullet manifolds,
  - mark colliding components and store collision pairs.
- Internally use `btCollisionObject::setUserPointer(this)` on each collider so Bullet contact results can be mapped back to engine components without exposing Bullet in the public API.
- Add `ColliderComponent` as an abstract base component that owns registration lifetime with `PhysicsWorld`.
- Add `SphereColliderComponent` with radius in engine terms.
- Add `BoxColliderComponent` with full size in engine terms; convert to Bullet half-extents internally.
- `ColliderComponent::initialize(Transform&)` stores the transform pointer and registers with `PhysicsWorld`.
- `ColliderComponent` destructor unregisters from `PhysicsWorld`.
- If collider dimensions change after registration, rebuild the Bullet shape rather than mutating it in place.

## Public APIs / Interfaces
- `using ColliderId = std::uint32_t;`
- `struct CollisionPair { ColliderId a; ColliderId b; };`
- `PhysicsWorld`
  - `void update();`
  - `bool has_collisions() const;`
  - `const std::vector<CollisionPair>& get_collision_pairs() const;`
  - `bool is_colliding(ColliderId id) const;`
- `ColliderComponent`
  - `ColliderId collider_id() const;`
  - `bool is_colliding() const;`
- `SphereColliderComponent`
  - `set_radius(float)` / `get_radius()`
- `BoxColliderComponent`
  - `set_size(const glm::vec3&)` / `get_size()`

## MainLoopTest Demo
- Replace the single sphere demo with 6 root-level test bodies:
  - sphere A at `(-3, 0, 0)` moving `(+1, 0, 0)`
  - sphere B at `(3, 0, 0)` moving `(-1, 0, 0)`
  - box A at `(0, -3, 0)` moving `(0, +1, 0)`
  - box B at `(0, 3, 0)` moving `(0, -1, 0)`
  - box C at `(0, 0, -3)` moving `(0, 0, +1)`
  - sphere C at `(0, 0, 3)` moving `(0, 0, -1)`
- Use `MeshFactory::create_sphere()` for spheres and `MeshFactory::create_cube()` for boxes with matching visual dimensions and collider sizes.
- Store for each test body:
  - its node,
  - its collider component,
  - its initialize-time spawn position,
  - its constant velocity.
- In `update(delta_time)`:
  - move every body by `velocity * delta_time`,
  - call `PhysicsWorld::get().update()`,
  - reset every colliding body to its initialize-time spawn position,
  - call `PhysicsWorld::get().update()` again so rendered collision state matches the restored transforms.
- “Original position” means the initialize-time spawn position, not the previous frame’s safe position.

## Test Plan
- Build with the required preset: `cmake --build --preset Debug --target MainLoopTest`
- Manual verification:
  - all six objects render and start separated,
  - sphere-sphere, box-box, and sphere-box contacts all trigger snap-back,
  - objects resume motion from their authored spawn positions after reset,
  - collision flags clear after reset and do not remain latched,
  - orbit camera still works while the demo runs,
  - scene destruction does not leave stale Bullet objects or crash on shutdown.
- Accept the following v1 limitation as part of the test contract:
  - collider nodes must be root-level nodes with no transformed parent.

## Assumptions / Defaults
- First pass is collision detection only: no rigid bodies, gravity, impulses, constraints, restitution, friction, layers, triggers, raycasts, or callbacks.
- Collision response remains application-driven in `MainLoopTest`; the Physics module only detects and reports overlaps.
- Collider shape parameters come from component properties, not transform scale, in v1.
- `MainLoop` now depends on the Physics module conceptually, but Bullet remains encapsulated inside the Physics module implementation, which preserves the pedagogical engine-facing API.
