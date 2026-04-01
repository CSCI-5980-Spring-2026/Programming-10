#pragma once

#include <glm/glm.hpp> // general glm header for basic types like vec3 and mat4
#include <glm/gtc/quaternion.hpp> // glm quaternion functions

namespace GopherEngine {

    class Camera
    {
        public:
            const glm::vec3& get_world_position() const;
            void set_world_position(const glm::vec3& world_position);

            const glm::mat4& get_view_matrix() const;
            void set_view_matrix(const glm::mat4& view_matrix);

            const glm::mat4& get_projection_matrix() const;
            void set_projection_matrix(const glm::mat4& projection_matrix);

            bool get_projection_matrix_dirty() const;
            void set_projection_matrix_dirty(bool dirty);

            float get_aspect_ratio() const;

            void set_perspective(float fov, float aspect_ratio, float near, float far);
            void set_frustum(float left, float right, float bottom, float top, float near, float far);
            void set_orthographic(float left, float right, float bottom, float top, float near, float far);

        private:
            glm::vec3 world_position{0.f, 0.f, 0.f};
            glm::mat4 view_matrix_{1.f};

            float aspect_ratio_{1.f};
            bool projection_matrix_dirty_{true};
            glm::mat4 projection_matrix_{1.f};
    };
} 