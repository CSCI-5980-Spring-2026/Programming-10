#pragma once

#include <string>

namespace GopherEngine
{
    class ShaderProgram
    {
        public:
            ShaderProgram(const std::string& vert_src, const std::string& frag_src, const unsigned int material_ubo_size);
            ~ShaderProgram();

            // Non-copyable, moveable
            ShaderProgram(const ShaderProgram&) = delete;
            ShaderProgram& operator=(const ShaderProgram&) = delete;
            ShaderProgram(ShaderProgram&& other) noexcept;
            ShaderProgram& operator=(ShaderProgram&& other) noexcept;

            void bind() const;
            void unbind() const;

            unsigned int get_id() const;
            unsigned int get_matrices_ubo() const;
            unsigned int get_material_ubo() const;
            unsigned int get_lights_ubo() const;

            static constexpr int MAX_LIGHTS = 8;

        private:
            unsigned int compile_shader(unsigned int type, const std::string& src);

            unsigned int program_id_{0};
            unsigned int matrices_ubo_{0};
            unsigned int material_ubo_{0};
            unsigned int lights_ubo_{0};
    };
}