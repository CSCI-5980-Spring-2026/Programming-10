#include <GopherEngine/Core/EventSystem.hpp>

#include <algorithm>

namespace GopherEngine
{
    void EventSystem::register_handler(EventHandler* handler)
    {
        if(dispatch_depth_ > 0)
        {
            pending_operations_.push_back({PendingOperationType::Register, handler});
            return;
        }

        register_handler_immediate(handler);
    }

    void EventSystem::unregister_handler(EventHandler* handler)
    {
        if(dispatch_depth_ > 0)
        {
            pending_operations_.push_back({PendingOperationType::Unregister, handler});
            return;
        }

        unregister_handler_immediate(handler);
    }

    void EventSystem::unregister_all_handlers()
    {
        if(dispatch_depth_ > 0)
        {
            pending_operations_.push_back({PendingOperationType::UnregisterAll, nullptr});
            return;
        }

        handlers_.clear();
    }

    void EventSystem::on_mouse_move(const MouseMoveEvent& event) {
        begin_dispatch();
        for(auto it = handlers_.begin(); it != handlers_.end(); ++it)
            (*it)->on_mouse_move(event);
        end_dispatch();
    }
            
    void EventSystem::on_mouse_down(const MouseButtonEvent& event) {
        begin_dispatch();
        for(auto it = handlers_.begin(); it != handlers_.end(); ++it)
            (*it)->on_mouse_down(event);
        end_dispatch();
    }

    void EventSystem::on_mouse_up(const MouseButtonEvent& event) {
        begin_dispatch();
        for(auto it = handlers_.begin(); it != handlers_.end(); ++it)
            (*it)->on_mouse_up(event);
        end_dispatch();
    }

    void EventSystem::on_mouse_scroll(const MouseScrollEvent& event) {
        begin_dispatch();
        for(auto it = handlers_.begin(); it != handlers_.end(); ++it)
            (*it)->on_mouse_scroll(event);
        end_dispatch();
    }

    void EventSystem::on_key_down(const KeyEvent& event)
    {
        begin_dispatch();
        for(auto it = handlers_.begin(); it != handlers_.end(); ++it)
            (*it)->on_key_down(event);
        end_dispatch();
    }

    void EventSystem::on_key_up(const KeyEvent& event)
    {
        begin_dispatch();
        for(auto it = handlers_.begin(); it != handlers_.end(); ++it)
            (*it)->on_key_up(event);
        end_dispatch();
    }

    void EventSystem::on_key_press(const KeyEvent& event)
    {
        begin_dispatch();
        for(auto it = handlers_.begin(); it != handlers_.end(); ++it)
            (*it)->on_key_press(event);
        end_dispatch();
    }

    void EventSystem::begin_dispatch()
    {
        ++dispatch_depth_;
    }

    void EventSystem::end_dispatch()
    {
        --dispatch_depth_;

        if(dispatch_depth_ == 0)
            apply_pending_operations();
    }

    void EventSystem::apply_pending_operations()
    {
        for(const auto& operation : pending_operations_)
        {
            switch(operation.type)
            {
                case PendingOperationType::Register:
                    register_handler_immediate(operation.handler);
                    break;

                case PendingOperationType::Unregister:
                    unregister_handler_immediate(operation.handler);
                    break;

                case PendingOperationType::UnregisterAll:
                    handlers_.clear();
                    break;
            }
        }

        pending_operations_.clear();
    }

    void EventSystem::register_handler_immediate(EventHandler* handler)
    {
        handlers_.push_back(handler);
    }

    void EventSystem::unregister_handler_immediate(EventHandler* handler)
    {
        // Erase-remove idiom:
        // 1) std::remove compacts all non-matching handlers toward the front and
        //    returns the new logical end of the valid range.
        // 2) erase then deletes the leftover tail so the vector size is updated.
        handlers_.erase(
            std::remove(handlers_.begin(), handlers_.end(), handler),
            handlers_.end()
        );
    }
}
