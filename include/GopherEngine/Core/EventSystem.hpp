#pragma once

#include <GopherEngine/Core/Service.hpp>
#include <GopherEngine/Core/EventHandler.hpp>

#include <vector>
#include <cstddef>

namespace GopherEngine
{
    class EventSystem: public Service<EventSystem>
    {
        public:
            void register_handler(EventHandler* handler);
            void unregister_handler(EventHandler* handler);
            void unregister_all_handlers();

            void on_mouse_move(const MouseMoveEvent& event);
            void on_mouse_down(const MouseButtonEvent& event);
            void on_mouse_up(const MouseButtonEvent& event);
            void on_mouse_scroll(const MouseScrollEvent& event);
            void on_key_down(const KeyEvent& event);
            void on_key_up(const KeyEvent& event);
            void on_key_press(const KeyEvent& event);

        private:
            enum class PendingOperationType
            {
                Register,
                Unregister,
                UnregisterAll
            };

            struct PendingOperation
            {
                PendingOperationType type;
                EventHandler* handler;
            };

            void begin_dispatch();
            void end_dispatch();
            void apply_pending_operations();
            void register_handler_immediate(EventHandler* handler);
            void unregister_handler_immediate(EventHandler* handler);

            std::vector<EventHandler*> handlers_;
            std::vector<PendingOperation> pending_operations_;
            std::size_t dispatch_depth_{0};
    };
}
