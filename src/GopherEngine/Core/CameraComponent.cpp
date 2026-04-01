#include <GopherEngine/Core/CameraComponent.hpp>

using namespace std;

namespace GopherEngine {

    void CameraComponent::pre_draw(const glm::mat4 &local_matrix, const glm::mat4 &world_matrix, RenderContext& context) {
        
        // The position of the camera can be extracted from the 
        // world matrix's translation component (the fourth column)
        camera_->set_world_position(glm::vec3(world_matrix[3]));
        camera_->set_view_matrix(glm::inverse(world_matrix));
        
    }

    std::shared_ptr<Camera> CameraComponent::get_camera() const {
        return camera_;
    }

    void CameraComponent::set_camera(std::shared_ptr<Camera> camera) {
        camera_ = std::move(camera);
    }
}
