#pragma once

#include <GopherEngine/Core/Events.hpp>

namespace GopherEngine
{
    class EventHandler
    {
        public:
            EventHandler();
            virtual ~EventHandler();

            virtual void on_mouse_move(const MouseMoveEvent& event) {};
            virtual void on_mouse_down(const MouseButtonEvent& event) {};
            virtual void on_mouse_up(const MouseButtonEvent& event) {};
            virtual void on_mouse_scroll(const MouseScrollEvent& event) {};
            virtual void on_key_down(const KeyEvent& event) {}
            virtual void on_key_up(const KeyEvent& event) {}
            virtual void on_key_press(const KeyEvent& event) {}
      };
}
