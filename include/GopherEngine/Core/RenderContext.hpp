#pragma once

#include <GopherEngine/Core/Light.hpp>

#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace GopherEngine {
    struct RenderContext {
        glm::mat4 view_matrix_;
        glm::mat4 projection_matrix_;
        glm::vec3 eye_position_;
        std::vector<std::shared_ptr<Light>> lights_;
    };
}