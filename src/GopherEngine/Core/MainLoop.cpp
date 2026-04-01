#include <GopherEngine/Core/MainLoop.hpp>
#include <GopherEngine/Renderer/Renderer.hpp>
#include <GopherEngine/Core/Scene.hpp>
#include <GopherEngine/Core/CameraComponent.hpp>

#include <iostream>
using namespace std;

namespace GopherEngine {
    MainLoop::MainLoop()
    {
        scene_ = make_shared<Scene>();
        Service<EventSystem>::provide(&event_system_);
        Service<ResourceManager>::provide(&resource_manager_);
    }

    MainLoop::~MainLoop()
    {
    }

    int MainLoop::run()
    {
        configure();

        window_.create_window();

        if(!window_.set_active(true)) {
            cerr << "Failed to set window to active" << endl;
            return EXIT_FAILURE;
        }

        register_input_callbacks();

        renderer_.initialize();
        initialize();

        handle_resize();
        
        // Reset the clock to start measuring time from the beginning of the main loop
        clock_.reset();

        while(window_.is_open())
        {
            float delta_time = clock_.delta_time();

            resource_manager_.poll();
            window_.handle_events();

            update(delta_time);
            scene_->update(delta_time);

            handle_resize();

            renderer_.draw(*scene_);
            window_.display();
        }
        
        return EXIT_SUCCESS;
    }

    void MainLoop::handle_resize() {
        
        auto camera = scene_->get_main_camera();

        if(camera) 
        {
            // If either the window or the camera's projection matrix is dirty,
            // we need to resize the viewport and reset the dirty flags.
            if(window_.get_dirty() || camera->get_projection_matrix_dirty())
            {
                // This will be extended to support camera aspect ratio
                renderer_.resize_viewport(
                    window_.get_width(), 
                    window_.get_height(), 
                    camera->get_aspect_ratio(),
                    window_.get_viewport_mode()
                );

                // Reset the dirty flags after resizing the viewport
                window_.set_dirty(false);
                camera->set_projection_matrix_dirty(false);
            }

        }  
        else 
        {
            // If there is no camera in the scene, then we only need to check
            // if the window is dirty, and if so, resize the viewport.
            if(window_.get_dirty() ) 
            {
                renderer_.resize_viewport(
                    window_.get_width(), 
                    window_.get_height(), 
                    static_cast<float>(window_.get_width()) / static_cast<float>(window_.get_height()),
                    window_.get_viewport_mode()
                );

                window_.set_dirty(false);
            }
        }
    }

    void MainLoop::register_input_callbacks()
    {
        window_.set_on_mouse_move([this](const MouseMoveEvent& e) {
            event_system_.on_mouse_move(e);   
        });

        window_.set_on_mouse_down([this](const MouseButtonEvent& e) {
            event_system_.on_mouse_down(e);   
        });

        window_.set_on_mouse_up([this](const MouseButtonEvent& e) {
            event_system_.on_mouse_up(e);   
        });

        window_.set_on_mouse_scroll([this](const MouseScrollEvent& e) {
            event_system_.on_mouse_scroll(e);   
        });

        window_.set_on_key_down([this](const KeyEvent& e) {
            event_system_.on_key_down(e);   
        });

        window_.set_on_key_up([this](const KeyEvent& e) {
           event_system_.on_key_up(e);   
        });

        window_.set_on_key_press([this](const KeyEvent& e) {
            event_system_.on_key_press(e);
        });
    }
}

