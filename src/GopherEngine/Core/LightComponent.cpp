#include <GopherEngine/Core/LightComponent.hpp>
using namespace GopherEngine;

using namespace std;

namespace GopherEngine {

    LightComponent::LightComponent(LightType type): Component() {
        light_->type_ = type;
    }

    LightComponent::LightComponent(std::shared_ptr<Light> light): Component() {
        light_ = std::move(light);
    }

    void LightComponent::initialize(Transform& transform) {
        // Empty, but can be overriden by derived classes
    }

    void LightComponent::update(Transform& transform, float delta_time) {
        // Empty, but can be overriden by derived classes
    }   

    void LightComponent::late_update(Transform& transform, float delta_time) {
        // Empty, but can be overriden by derived classes
    }

    void LightComponent::pre_draw(const glm::mat4 &local_matrix, const glm::mat4 &world_matrix, RenderContext& context) {

        // The position of the light can be extracted from the 
        // world matrix's translation component (the fourth column)
        light_->world_position_ = glm::vec3(world_matrix[3]);
        context.lights_.push_back(light_);
        
    }

    void LightComponent::draw(const glm::mat4 &world_matrix, const RenderContext& context) {
        // Lights are not drawn directly
    }

    void LightComponent::set_light(std::shared_ptr<Light> light) {
        light_ = std::move(light);
    }

    std::shared_ptr<Light> LightComponent::get_light() const {
        return light_;
    }
}