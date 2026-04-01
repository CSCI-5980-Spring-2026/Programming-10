#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace GopherEngine {
    
    enum class LightType {
        Directional,
        Point
    };

    struct Light {
        LightType type_{LightType::Directional};
        glm::vec3 world_position_{0, 0, 0};
        glm::vec3 ambient_intensity_{0, 0, 0};
        glm::vec3 diffuse_intensity_{1.f, 1.f, 1.f};
        glm::vec3 specular_intensity_{1.f, 1.f, 1.f};   
    };
}