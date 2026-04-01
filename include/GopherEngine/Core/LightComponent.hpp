#pragma once

#include <GopherEngine/Core/Component.hpp>
#include <GopherEngine/Core/Light.hpp>

#include <memory>

namespace GopherEngine
{
    class LightComponent : public Component
    {
        public:
            LightComponent(LightType type);
            LightComponent(std::shared_ptr<Light> light);
            void initialize(Transform& transform) override;
            void update(Transform& transform, float delta_time) override;
            void late_update(Transform& transform, float delta_time) override;
            void pre_draw(const glm::mat4 &local_matrix, const glm::mat4 &world_matrix, RenderContext& context) override;
            void draw(const glm::mat4 &world_matrix, const RenderContext& context) override;

            void set_light(std::shared_ptr<Light> light);
            std::shared_ptr<Light> get_light() const;

        protected:
            std::shared_ptr<Light> light_{std::make_shared<Light>()};
    };
}