#pragma once

#include <GopherEngine/Core/Transform.hpp>
#include <GopherEngine/Core/Component.hpp>
#include <GopherEngine/Core/RenderContext.hpp>

#include <memory> // for std::shared_ptr
#include <vector> // for std::vector
#include <atomic> // for the atomic id counter

namespace GopherEngine
{
    class Node
    {
        public:
            Node();
            ~Node();
            void update(float delta_time);
            void late_update(float delta_time);
            void pre_draw(RenderContext& context, const glm::mat4& parent_world_matrix, bool parent_matrix_dirty);
            void draw(const RenderContext& context);

            void add_component(std::shared_ptr<Component> component);
            void add_child(std::shared_ptr<Node> node);
            std::shared_ptr<Node> create_child();
            std::shared_ptr<Node> remove_child_by_id(std::uint32_t id);

            // mutable accessors
            Transform& transform();
            
            // const accessors
            const Transform& transform() const;
            const std::vector<std::shared_ptr<Node>>& children() const;
            const uint32_t& id() const;
            const glm::mat4& local_matrix() const;
            const glm::mat4& world_matrix() const;

            // Template method to get all components of a specific type T. This uses 
            // dynamic_cast internally, so it will return an empty vector if T is not
            // a valid component type or if there are no components of that type attached.
            template <typename T>
            std::vector<std::shared_ptr<T>> get_components() const
            {
                std::vector<std::shared_ptr<T>> matches;

                for (const auto& component : components_)
                {
                    if (auto casted = std::dynamic_pointer_cast<T>(component))
                        matches.push_back(casted);
                }

                return matches;
            }

        private:
            uint32_t id_;
            Transform transform_;
            glm::mat4 local_matrix_{1.f}; 
            glm::mat4 world_matrix_{1.f}; 
            bool local_matrix_dirty_{true}; 

            std::vector<std::shared_ptr<Node>> children_;  
            std::vector<std::shared_ptr<Component>> components_;
            
            // Static member variable declaration for the id counter
            static std::atomic<std::uint32_t> id_counter_;
    };
}