#pragma once

#include <GopherEngine/Core/Component.hpp>
#include <GopherEngine/Resource/Mesh.hpp>
#include <GopherEngine/Renderer/Material.hpp>

#include <memory>

namespace GopherEngine
{
    class MeshComponent : public Component
    {
        public:
            void initialize(Transform& transform) override;
            void update(Transform& transform, float delta_time) override;
            void late_update(Transform& transform, float delta_time) override;
            void pre_draw(const glm::mat4 &local_matrix, const glm::mat4 &world_matrix, RenderContext& context) override;
            void draw(const glm::mat4 &world_matrix, const RenderContext& context) override;

            void set_mesh(std::shared_ptr<Mesh> mesh);
            std::shared_ptr<Mesh> get_mesh() const;

            void set_material(std::shared_ptr<Material> material);
            std::shared_ptr<Material> get_material() const;

        protected:
            std::shared_ptr<Mesh> mesh_;
            std::shared_ptr<Material> material_;
    };
}