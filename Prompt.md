We are going to integrate the Bullet Physics SDK in the game engine. It has already been added to the CMakeLists.txt (using FetchContent), and the everything builds successfully.

Since this is a minimalistic pedagogical game engine, we should start with the fundamentals: basic collision primitives (sphere/box colliders) and collision testing. In the future, we will eventually add rigid body dynamics, but not for the initial implementation in class.

Keeping with the engine's modularity goals, Bullet's functionality should be wrapped inside a new Physics module, with unnecessary complexity obscured when possible behind a clean engine-facing API. There is already an empty PhysicsWorld class added to the new Physics module.

Please analyze the engine codebase and present a software architecture design and actionable implementation plan. Do not start writing any code yet.

Your plan should also include adding multiple boxes and spheres to MainLoopTest for testing purposes.  They should be set up initially so they are not colliding, and then should be gradually moved in the update() method.  When two objects collide, they should be instantaneously moved back to their original positions.
