#pragma once

#include <GopherEngine/Renderer/Material.hpp>
#include <GopherEngine/Resource/Texture.hpp>

#include <memory>

namespace GopherEngine {

class BlinnPhongMaterial : public Material {
    public:
        BlinnPhongMaterial();
        void draw(const std::shared_ptr<Geometry>& geometry, const glm::mat4 &world_matrix, const RenderContext& context) override;

        void set_ambient_color(const glm::vec3 &color);
        glm::vec3 get_ambient_color() const;

        void set_diffuse_color(const glm::vec3 &color);
        glm::vec3 get_diffuse_color() const;

        void set_specular_color(const glm::vec3 &color);
        glm::vec3 get_specular_color() const;

        void set_shininess(float shininess);
        float get_shininess() const;

        void set_texture(std::shared_ptr<Texture> texture);
        std::shared_ptr<Texture> get_texture() const;
        void clear_texture();

    private:
        glm::vec3 ambient_color_{1.f, 1.f, 1.f}; 
        glm::vec3 diffuse_color_{1.f, 1.f, 1.f};
        glm::vec3 specular_color_{1.f, 1.f, 1.f};
        float shininess_{32.f}; 
        std::shared_ptr<Texture> texture_;
    };
}