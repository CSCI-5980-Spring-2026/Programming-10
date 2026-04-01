/* Programming 10: Physics System Integration
 * CSCI 5980, Spring 2026, University of Minnesota
 * Instructor: Evan Suma Rosenberg <suma@umn.edu>
 */ 


#include <GopherEngine/Core/MainLoop.hpp>
#include <GopherEngine/Core/LightComponent.hpp>
#include <GopherEngine/Core/MeshComponent.hpp>
#include <GopherEngine/Core/OrbitControls.hpp>
#include <GopherEngine/Renderer/BlinnPhongMaterial.hpp>
#include <GopherEngine/Resource/MeshFactory.hpp>
#include <GopherEngine/Core/FileLoader.hpp>
using namespace GopherEngine;

#include <memory>
#include <iostream>
using namespace std;

// A simple subclass of MainLoop to test that the main loop is working
// and the window, scene, and node classes are functioning correctly
class MainLoopTest: public MainLoop
{
	public:
		// Constructor and destructor
		MainLoopTest();
		~MainLoopTest();

	private:

		// Override the pure virtual functions from MainLoop
		void configure() override;
		void initialize() override;
		void update(float delta_time) override;
		
};


MainLoopTest::MainLoopTest() {

}

MainLoopTest::~MainLoopTest() {

}

// This function is called once at the beginning of the main loop, before the window is created.
// This means the OpenGL context is not yet available. It should be used for initial configuration.
void MainLoopTest::configure() {

	window_.set_title("CSCI 5980 Programming 10");
	window_.set_vertical_sync(true);
	window_.set_framerate_limit(60);
	renderer_.set_background_color(glm::vec4(0.5f, 0.75f, .9f, 1.f));
}

// This function is called once at the beginning of the main loop, after the window is created
// and the OpenGL context is available. It should be used for initializing the scene.
void MainLoopTest::initialize() {

	// Enable verbose logging in the resource manager so we can see when resources are loaded
	ResourceManager::get().set_verbose(true);
	
	// Create default camera and set its initial position
	auto camera_node = scene_->create_default_camera();

	// Add orbit controls to the camera so we can move it around with the mouse
	auto camera_controls = make_shared<OrbitControls>();
	camera_controls->set_distance(4.0f);
	camera_node->add_component(camera_controls);

	// Create a point light
	auto light_component = make_shared<LightComponent>(LightType::Point);
	light_component->get_light()->ambient_intensity_ = glm::vec3(0.2f, 0.2f, 0.2f);
	light_component->get_light()->diffuse_intensity_ = glm::vec3(1.f, 1.f, 1.f);
	light_component->get_light()->specular_intensity_ = glm::vec3(1.f, 1.f, 1.f);

	// Add the point light to the scene
	auto light_node = scene_->create_node();
	light_node->add_component(light_component);
	light_node->transform().position_ = glm::vec3(-10.f, 10.f, 10.f);

	// Create a default material
	auto default_material = make_shared<BlinnPhongMaterial>();
	default_material->set_ambient_color(glm::vec3(0.4f, 0.4f, 0.4f));
	default_material->set_diffuse_color(glm::vec3(0.6f, 0.6f, 0.6f));
	default_material->set_specular_color(glm::vec3(2.f, 2.f, 2.f));

	// Add a node to the scene
	auto test_node = scene_->create_node();

	// Add a mesh component to the node
	auto test_component = make_shared<MeshComponent>();
	test_node->add_component(test_component);

	// Add the mesh and material to the mesh component
	test_component->set_mesh(MeshFactory::create_sphere());
	test_component->set_material(default_material);

}

// This function is called once per frame, before the scene's update function is called.
void MainLoopTest::update(float delta_time) {


}

int main()
{
	// Create an instance of the MainLoop subclass and start the main game loop
	MainLoopTest app;
	return app.run();
}