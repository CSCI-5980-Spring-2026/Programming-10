#pragma once

#include <GopherEngine/Core/Types.hpp>
#include <GopherEngine/Core/Scene.hpp>
#include <GopherEngine/Renderer/ShaderLoader.hpp>
#include <GopherEngine/Renderer/ResourceUploader.hpp>

#include <glm/vec4.hpp>

namespace GopherEngine
{
    class Renderer
    {
        public:
            Renderer();
            ~Renderer();
            void initialize();
            void resize_viewport(unsigned int window_width, unsigned int window_height, float camera_aspect_ratio, ViewportMode mode);
            void draw(Scene &scene);

            void set_background_color(const glm::vec4 &color);
            glm::vec4 get_background_color() const;

        private:
            ShaderLoader shader_loader_;
            ResourceUploader resource_uploader_;
            bool initialized_{false};
            glm::vec4 background_color_{0.0f, 0.0f, 0.0f, 1.0f};
    };
}