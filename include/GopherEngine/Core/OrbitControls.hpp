#pragma once

#include <GopherEngine/Core/Component.hpp>
#include <GopherEngine/Core/EventHandler.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace GopherEngine
{

    class OrbitControls: public Component, EventHandler
    {
        public:
            OrbitControls(
                const glm::vec3& target = {0.f, 0.f, 0.f},
                float distance = 1.f
            );

            void late_update(Transform& transform, float delta_time) override;

            void on_mouse_move(const MouseMoveEvent& event) override;
            void on_mouse_down(const MouseButtonEvent& event) override;
            void on_mouse_up(const MouseButtonEvent& event) override;
            void on_mouse_scroll(const MouseScrollEvent& event) override;

            void set_target_point(const glm::vec3& target);
            void set_distance(float distance);

        private:
            float rotation_speed_{glm::pi<float>() / 4.f}; 
            float zoom_speed_{10.f};
            glm::vec3 target_point_{0.f, 0.f, 0.f};
            float distance_{1.f};
            glm::quat orbit_x_{1.f, 0.f, 0.f, 0.f};
            glm::quat orbit_y_{1.f, 0.f, 0.f, 0.f};
            glm::vec2 mouse_movement_{0.f, 0.f};
            float zoom_{0.f};
            bool mouse_drag_{false};
    };

}