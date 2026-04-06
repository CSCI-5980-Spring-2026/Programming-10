# Bullet Collision Module First Pass

## Summary
- Integrate Bullet in collision-only mode first by using `btCollisionWorld`, not `btDiscreteDynamicsWorld`. This keeps the class focused on overlap testing before introducing mass, forces, and simulation state.
- Keep Bullet entirely inside the new Physics module. `PhysicsWorld` becomes the engine-facing wrapper, and Bullet types stay hidden in the `.cpp` behind a private implementation.
- Expose colliders to the rest of the engine as scene `Component`s, because that matches the current node/component architecture and gives a clean path to future rigid body components.

## Key Changes
1. `PhysicsWorld`
- Make `PhysicsWorld` a `Service<PhysicsWorld>` owned by `MainLoop`, alongside `EventSystem` and `ResourceManager`.
- Store the Bullet collision configuration, dispatcher, broadphase, and `btCollisionWorld` inside `PhysicsWorld`.
- Keep a collider registry keyed by a small engine `ColliderId`; each entry owns one `btCollisionShape`, one `btCollisionObject`, a non-owning pointer to the node `Transform`, and a non-owning pointer back to the collider component.
- Add a single explicit `update()` method that:
  - copies current engine transforms into Bullet objects,
  - clears last-frame collision flags/pairs,
  - calls `performDiscreteCollisionDetection()`,
  - walks Bullet manifolds and records the current overlaps.
- Add private GLM/Bullet conversion helpers in `PhysicsWorld.cpp` only.

2. Collider components
- Add an abstract `ColliderComponent` in the Physics module that derives from `Component` and owns registration lifetime with `PhysicsWorld`.
- Add `SphereColliderComponent` and `BoxColliderComponent`.
- `SphereColliderComponent` exposes radius in engine terms.
- `BoxColliderComponent` exposes full size/extents in engine terms; the wrapper converts to Bullet half-extents internally so students do not need to think in Bullet-specific units.
- `initialize(Transform&)` stores a non-owning transform pointer and registers the collider with `PhysicsWorld`.
- The collider destructor unregisters from `PhysicsWorld` so node removal stays safe.
- `ColliderComponent` exposes `collider_id()` and `is_colliding()` for simple game-side queries.
- If collider dimensions change after registration, rebuild the underlying Bullet shape instead of mutating it in place. That is simpler to explain and sufficient for this teaching engine.

3. Main loop ownership
- Add a `PhysicsWorld physics_world_` member to `MainLoop` and register it in the constructor with `Service<PhysicsWorld>::provide(...)`.
- Do not auto-step physics inside `MainLoop::run()` yet. Keep collision evaluation explicit in application code so students can see when the world is queried.

4. `MainLoopTest`
- Replace the single demo sphere with a small set of test bodies that each store:
  - the scene node,
  - its collider component,
  - its authored initial position,
  - its constant velocity.
- Create at least 6 visible objects: 2 spheres, 2 boxes, and 1 mixed sphere/box pair, arranged in separate lanes so nothing overlaps at startup.
- Use matching render meshes and collider dimensions so the visual shape and collision primitive agree.
- In `update(delta_time)`:
  - move every test body by `velocity * delta_time`,
  - call `PhysicsWorld::get().update()`,
  - collect all colliders marked as colliding,
  - snap those objects back to their initialize-time positions immediately,
  - call `PhysicsWorld::get().update()` once more so collision state matches the restored transforms before rendering.
- Reset semantics: “original position” means the initialize-time spawn position.

## Public APIs / Types
- `using ColliderId = std::uint32_t;`
- `struct CollisionPair { ColliderId a; ColliderId b; };`
- `PhysicsWorld`
  - `void update();`
  - `bool has_collisions() const;`
  - `const std::vector<CollisionPair>& get_collision_pairs() const;`
- `ColliderComponent`
  - `ColliderId collider_id() const;`
  - `bool is_colliding() const;`
- `SphereColliderComponent`
  - `set_radius(float)` / `get_radius()`
- `BoxColliderComponent`
  - `set_size(const glm::vec3&)` / `get_size()`

## Test Plan
- Build with the required preset: `cmake --build --preset Debug --target MainLoopTest`
- Manual verification:
  - all spheres and boxes render and start separated,
  - sphere-sphere, box-box, and sphere-box contacts each trigger a reset,
  - colliders do not remain stuck in a perpetual collision state after reset,
  - rotated box nodes still collide as oriented boxes,
  - destroying a node with a collider does not crash or leave stale Bullet objects behind.
- There is no unit test harness in the repo now, so first-pass verification should be visual plus lightweight logging if needed.

## Assumptions / Defaults
- First pass is collision detection only: no rigid bodies, gravity, forces, restitution, friction, layers, triggers, raycasts, or callbacks.
- Collision response stays application-driven in `MainLoopTest`; the Physics module only detects/report overlaps.
- Collider dimensions come from component properties, not transform scale, for this initial classroom pass. Use unit-scale demo objects to keep behavior unambiguous.
- This intentionally differs from a production engine, which would often introduce a dynamics world earlier; here the simpler collision-only wrapper is the better teaching step.
