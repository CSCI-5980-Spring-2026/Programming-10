#pragma once

#include <GopherEngine/Renderer/Material.hpp>
#include <GopherEngine/Resource/Texture.hpp>

#include <memory>

namespace GopherEngine {

class UnlitMaterial : public Material {
    public:
        UnlitMaterial();
        void draw(const std::shared_ptr<Geometry>& geometry, const glm::mat4 &world_matrix, const RenderContext& context) override;

        void set_color(const glm::vec3 &color);
        glm::vec3 get_color() const;

        void set_texture(std::shared_ptr<Texture> texture);
        std::shared_ptr<Texture> get_texture() const;
        void clear_texture();

    private:
        glm::vec3 color_{1.f, 1.f, 1.f}; 
        std::shared_ptr<Texture> texture_;
    };
}