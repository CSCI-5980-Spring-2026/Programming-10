#pragma once

#include <GopherEngine/Core/Events.hpp>
#include <GopherEngine/Platform/Window.hpp>
#include <GopherEngine/Renderer/Renderer.hpp>
#include <GopherEngine/Core/Scene.hpp>
#include <GopherEngine/Core/Clock.hpp>
#include <GopherEngine/Core/EventSystem.hpp>
#include <GopherEngine/Resource/ResourceManager.hpp>

#include <memory>

namespace GopherEngine
{
    class MainLoop
    {
        public:
            MainLoop();
            ~MainLoop();
            int run();

        // Protected members and functions are accessible to subclasses, but not to outside code
        protected:

            // Pure virtual functions to be implemented by subclasses
            virtual void configure() = 0;
            virtual void initialize() = 0;
            virtual void update(float delta_time) = 0;

            // Protected member variables that can be accessed by subclasses
            Window window_;
            Renderer renderer_;
            Clock clock_;

            // Services
            EventSystem event_system_;
            ResourceManager resource_manager_;

            // Current scene being rendered and updated
            std::shared_ptr<Scene> scene_;

        // Private members and functions are only accessible within the class
        private:
            void handle_resize();
            void register_input_callbacks();

    };
}