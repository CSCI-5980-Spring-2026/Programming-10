#pragma once

#include <GopherEngine/Core/Types.hpp>
#include <GopherEngine/Core/Events.hpp>
#include <SFML/Window.hpp>

#include <functional>
#include <string>
#include <array>

namespace GopherEngine
{    class Window
    {
        public:
            Window(std::string title = "GopherEngine", unsigned int width = 800, unsigned int height = 600);
            ~Window();

            void set_title(std::string title);
            void set_size(unsigned int width, unsigned int height);
            void set_style(int style);
            void set_state(sf::State state);
            void set_vertical_sync(bool vertical_sync);
            void set_framerate_limit(unsigned int limit);
            void set_dirty(bool dirty);
            bool set_active(bool active);

            int get_width();
            int get_height();
            bool get_dirty();
            bool is_open();
            void set_viewport_mode(ViewportMode mode);
            ViewportMode get_viewport_mode() const;

            void create_window();
            void handle_events();
            void display();

            // Event callback registration
            void set_on_mouse_move(std::function<void(const MouseMoveEvent&)> cb);
            void set_on_mouse_down(std::function<void(const MouseButtonEvent&)> cb);
            void set_on_mouse_up(std::function<void(const MouseButtonEvent&)> cb);
            void set_on_mouse_scroll(std::function<void(const MouseScrollEvent&)> cb);
            void set_on_key_down(std::function<void(const KeyEvent&)> cb);
            void set_on_key_up(std::function<void(const KeyEvent&)> cb);
            void set_on_key_press(std::function<void(const KeyEvent&)> cb);

        private:
            sf::Window* window_;
            std::string title_;
            unsigned int width_;
            unsigned int height_;
            bool dirty_;
            bool vertical_sync_;
            unsigned int framerate_limit_;
            int style_;
            sf::State state_;
            ViewportMode viewport_mode_;

            // Input state tracking
            int last_mouse_x_;
            int last_mouse_y_;
            std::array<bool, static_cast<size_t>(Key::KeyCount)> key_states_;

            // Callbacks — default to no-ops so null checks are unnecessary
            std::function<void(const MouseMoveEvent&)> on_mouse_move_;
            std::function<void(const MouseButtonEvent&)> on_mouse_down_;
            std::function<void(const MouseButtonEvent&)> on_mouse_up_;
            std::function<void(const MouseScrollEvent&)> on_mouse_scroll_;
            std::function<void(const KeyEvent&)> on_key_down_;
            std::function<void(const KeyEvent&)> on_key_up_;
            std::function<void(const KeyEvent&)> on_key_press_;

    };
}