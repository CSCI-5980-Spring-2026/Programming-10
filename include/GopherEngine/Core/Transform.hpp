#pragma once

#include <glm/glm.hpp> // general glm header for basic types like vec3 and mat4
#include <glm/gtc/quaternion.hpp> // glm quaternion functions

namespace GopherEngine
{
    struct Transform
    {
        glm::vec3 position_{0.f, 0.f, 0.f};
        glm::quat rotation_{1.f, 0.f, 0.f, 0.f};
        glm::vec3 scale_{1.f, 1.f, 1.f};
    };

}