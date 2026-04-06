# Architecture Design: Physics Module Integration

To integrate the Bullet Physics SDK while maintaining the engine's pedagogical goals and modularity, the following architecture is proposed.

## 1. Core Design Philosophy
*   **Service-Based Access**: Following the pattern of `ResourceManager`, `PhysicsWorld` will be implemented as a `Service<PhysicsWorld>`. This provides a global access point without forcing a hard dependency on every object.
*   **Component-Based Colliders**: Physics representation will be decoupled from rendering. We will introduce `ColliderComponent`s that can be attached to `Node`s, mirroring how `MeshComponent`s are used.
*   **Collision-Only Phase**: We will use `btCollisionWorld` instead of `btDiscreteDynamicsWorld`. This avoids the complexity of rigid body dynamics (forces, constraints, integration) and focuses strictly on overlap testing.

## 2. Proposed Class Structure

### A. `PhysicsWorld` (Service)
*   **Responsibility**: Manages the Bullet collision lifecycle and provides the engine-facing API for collision queries.
*   **Internal Bullet State**:
    *   `btDefaultCollisionConfiguration`: Memory/algorithm config.
    *   `btCollisionDispatcher`: Handles collision pairs.
    *   `btDbvtBroadphase`: Fast broad-phase overlap detection.
    *   `btCollisionWorld`: The main container for collision objects.
*   **Key API**:
    *   `void add_collider(ColliderComponent* collider)`: Registers a collider with the Bullet world.
    *   `void remove_collider(ColliderComponent* collider)`: Unregisters a collider.
    *   `std::vector<CollisionPair> update()`: 
        1. Syncs all registered `ColliderComponent` world transforms to their corresponding `btCollisionObject`s.
        2. Performs discrete collision detection.
        3. Returns a list of colliding pairs.

### B. `ColliderComponent` (Base & Subclasses)
*   **`ColliderComponent`**: Base class that holds a `btCollisionObject*`.
*   **`SphereCollider`**: Adds a `radius` parameter and creates a `btSphereShape`.
*   **`BoxCollider`**: Adds `half_extents` (glm::vec3) and creates a `btBoxShape`.

## 3. Data Flow
1.  **Initialization**: `PhysicsWorld` is initialized at startup.
2.  **Setup**: User creates a `Node`, adds a `MeshComponent` (for visuals), and a `ColliderComponent` (for physics). The collider is registered with `PhysicsWorld`.
3.  **The Frame Loop**:
    *   `MainLoop::update()` $\rightarrow$ `PhysicsWorld::update()`.
    *   `PhysicsWorld` reads `Node` transforms $\rightarrow$ Updates `btCollisionObject` transforms.
    *   Bullet performs overlap tests $\rightarrow$ `PhysicsWorld` returns `CollisionPair`s.
    *   User handles collision logic (e.g., resetting positions in `MainLoopTest`).

---

# Actionable Implementation Plan

## Phase 1: Basic Infrastructure
1.  **Define Collider Components**:
    *   Create `ColliderComponent.hpp` as a base class.
    *   Create `SphereCollider.hpp` and `BoxCollider.hpp`.
    *   Implement the mapping from `glm` types to Bullet types (`btVector3`, `btQuaternion`).
2.  **Implement `PhysicsWorld`**:
    *   Update `PhysicsWorld.hpp` to inherit from `Service<PhysicsWorld>`.
    *   Implement Bullet boilerplate in `PhysicsWorld.cpp` (Configuration $\rightarrow$ Dispatcher $\rightarrow$ Broadphase $\rightarrow$ World).
    *   Implement `add_collider` and `remove_collider` to manage `btCollisionObject` lifecycles.
    *   Implement the `update()` method to sync transforms and collect manifolds (colliding pairs).

## Phase 2: Integration & Testing
1.  **Modify `MainLoopTest::initialize()`**:
    *   Instantiate multiple nodes with varying shapes (e.g., 3 spheres, 3 boxes).
    *   Assign each node a `MeshComponent` (using `MeshFactory::create_sphere()` / `create_box()`) and a corresponding `ColliderComponent`.
    *   Position them such that they are initially separated.
    *   Store their original positions in a data structure for the "reset" logic.
2.  **Modify `MainLoopTest::update()`**:
    *   Apply a constant velocity or interpolated movement to the test nodes.
    *   Call `PhysicsWorld::get().update()`.
    *   Iterate through the returned `CollisionPair`s.
    *   For each colliding pair, instantaneously reset the positions of both involved nodes to their stored original positions.

## Phase 3: Verification
1.  **Build**: Ensure the project compiles with the new Physics module.
2.  **Visual Test**: Verify that objects move and "snap back" immediately upon contact.
3.  **Leak Check**: Ensure that `PhysicsWorld` correctly cleans up all `btCollisionObject`s and shapes upon destruction.
