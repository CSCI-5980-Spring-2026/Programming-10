#include <GopherEngine/Core/Camera.hpp>

using namespace std;

namespace GopherEngine {

    const glm::vec3& Camera::get_world_position() const {
        return world_position;
    }
    
    void Camera::set_world_position(const glm::vec3& world_position) {
        this->world_position = world_position;
    }
    
    const glm::mat4& Camera::get_view_matrix() const {
        return view_matrix_;
    }

    void Camera::set_view_matrix(const glm::mat4& view_matrix) {
        view_matrix_ = view_matrix;
    }

    const glm::mat4& Camera::get_projection_matrix() const {
        return projection_matrix_;
    }

    void Camera::set_projection_matrix(const glm::mat4& projection_matrix) {
        projection_matrix_ = projection_matrix;
        projection_matrix_dirty_ = true;
    }

    bool Camera::get_projection_matrix_dirty() const {
        return projection_matrix_dirty_;
    }

    void Camera::set_projection_matrix_dirty(bool dirty) {
        projection_matrix_dirty_ = dirty;
    }

    void Camera::set_perspective(float fov, float aspect_ratio, float near, float far) {
        projection_matrix_ = glm::perspective(glm::radians(fov), aspect_ratio, near, far);
        aspect_ratio_ = aspect_ratio;
        projection_matrix_dirty_ = true;
    }

    void Camera::set_frustum(float left, float right, float bottom, float top, float near, float far) {
        projection_matrix_ = glm::frustum(left, right, bottom, top, near, far);
        aspect_ratio_ = (right - left) / (top - bottom);
        projection_matrix_dirty_ = true;
    }

    void Camera::set_orthographic(float left, float right, float bottom, float top, float near, float far) {
        projection_matrix_ = glm::ortho(left, right, bottom, top, near, far);
        aspect_ratio_ = (right - left) / (top - bottom);
        projection_matrix_dirty_ = true;
    }

    float Camera::get_aspect_ratio() const {
        return aspect_ratio_;
    }
}
