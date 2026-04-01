#pragma once

#include <GopherEngine/Core/Transform.hpp>
#include <GopherEngine/Core/RenderContext.hpp>

#include <glm/glm.hpp> // general glm header for basic types like vec3 and mat4
#include <glm/gtc/quaternion.hpp> // glm quaternion functions

#include <vector>
#include <memory>

namespace GopherEngine
{
    class Component
    {
        public:
            Component() = default;
            virtual ~Component() = default;
            virtual void initialize(Transform& transform) {};
            virtual void update(Transform& transform, float delta_time) {};
            virtual void late_update(Transform& transform, float delta_time) {};
            virtual void pre_draw(const glm::mat4 &local_matrix, const glm::mat4 &world_matrix, RenderContext& context) {};
            virtual void draw(const glm::mat4 &world_matrix, const RenderContext& context) {};
    };
}