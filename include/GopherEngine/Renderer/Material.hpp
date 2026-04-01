#pragma once

#include <GopherEngine/Resource/Geometry.hpp>
#include <GopherEngine/Renderer/ShaderProgram.hpp>
#include <GopherEngine/Core/RenderContext.hpp>
#include <GopherEngine/Core/Guid.hpp>

#include <glm/glm.hpp>
#include <memory>
#include <optional>

namespace GopherEngine
{
    class Material
    {
        public:
            Material() = default;
            virtual ~Material() = default;
            virtual void draw(const std::shared_ptr<Geometry>& geometry, const glm::mat4 &world_matrix, const RenderContext& context) = 0;

            // Optional name and GUID for associating this material with a source asset. This allows the runtime engine
            // to resolve cross-references between imported materials and their original source assets.
            std::optional<std::string> name_;
            std::optional<Guid> guid_;

        protected:
            void bind_shader(const ShaderProgram& shader_program, const glm::mat4 &world_matrix, const RenderContext& context, bool compute_normal_matrix, bool enable_lighting);
            void unbind_shader(const ShaderProgram& shader_program);
    };
}
