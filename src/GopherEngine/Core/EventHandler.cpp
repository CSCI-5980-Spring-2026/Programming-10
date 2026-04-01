#include <GopherEngine/Core/EventHandler.hpp>
#include <GopherEngine/Core/EventSystem.hpp>

namespace GopherEngine
{
    EventHandler::EventHandler()
    {
        Service<EventSystem>::get().register_handler(this);
    }

    EventHandler::~EventHandler()
    {
        Service<EventSystem>::get().unregister_handler(this);
    }
}