#pragma once

#include <GopherEngine/Core/Guid.hpp>

#include <string>

namespace GopherEngine {

    struct Resource {
        std::string name_;
        Guid guid_;
    };
}