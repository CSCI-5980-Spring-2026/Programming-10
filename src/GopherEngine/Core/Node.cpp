#include <GopherEngine/Core/Node.hpp>
#include <GopherEngine/Core/LightComponent.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
using namespace std;

namespace GopherEngine {

    // Static member variable initialization for the id counter
    std::atomic<std::uint32_t> Node::id_counter_{0};

    // This constructor incremenents the static id counter and assigns a unique id to each node.
    Node::Node() : id_(id_counter_.fetch_add(1, std::memory_order_relaxed)) {
        
    }

    Node::~Node() {

    }

    void Node::update(float delta_time) {
        for (const auto& component : components_) {
            component->update(transform(), delta_time);
        }

        for(auto& node: children_) {
            node->update(delta_time);
        }
    }

    void Node::late_update(float delta_time) {
        for (const auto& component : components_) {
            component->late_update(transform(), delta_time);
        }

        for(auto& node: children_) {
            node->late_update(delta_time);
        }
    }

    void Node::pre_draw(RenderContext& context, const glm::mat4& parent_world_matrix, bool parent_matrix_dirty) {

        bool world_matrix_updated = parent_matrix_dirty || local_matrix_dirty_;

        if(local_matrix_dirty_) {
            local_matrix_ = glm::mat4(1.f); 
            local_matrix_ = glm::translate(local_matrix_, transform_.position_);
            local_matrix_ = local_matrix_ * glm::mat4_cast(transform_.rotation_);
            local_matrix_ = glm::scale(local_matrix_, transform_.scale_);
            local_matrix_dirty_ = false;
        }

        // Combine the local transformation with the parent's world matrix to get the world matrix for this node
        if(world_matrix_updated) {
             world_matrix_ = parent_world_matrix * local_matrix_;
        }

        for (const auto& component : components_) {
            component->pre_draw(local_matrix_, world_matrix_, context);
        }

        for(auto& node: children_) {
            node->pre_draw(context, world_matrix_, world_matrix_updated);
        }
    }
    
    void Node::draw(const RenderContext& context) {
        for (const auto& component : components_) {
            component->draw(world_matrix_, context);
        }

        for(auto& node: children_) {
            node->draw(context);
        }
    }

    void Node::add_component(shared_ptr<Component> component) {
        component->initialize(transform_);
        components_.push_back(component);
    }

    void Node::add_child(shared_ptr<Node> node) {
        children_.push_back(node);
    }

    shared_ptr<Node> Node::create_child() {
        shared_ptr<Node> node = make_shared<Node>();
        children_.push_back(node);
        return node;
    }

    std::shared_ptr<Node> Node::remove_child_by_id(std::uint32_t id)
    {
        for (auto it = children_.begin(); it != children_.end(); ++it)
        {
            auto& child = *it;
            if (!child) continue;

            if (child->id() == id)
            {
                auto result = std::move(child);
                children_.erase(it);
                return result;
            }

            if (auto result = child->remove_child_by_id(id))
                return result;
        }

        return {};
    }

    // mutable accessor that also marks the local matrix as dirty 
    Transform& Node::transform() {
        local_matrix_dirty_ = true;
        return transform_;
    }

    // the const accessor cannot be used to modify the transformation,
    // so it does not need to mark the local matrix as dirty
    const Transform& Node::transform() const {
        return transform_;
    }

    // const accessors
    const vector<shared_ptr<Node>>& Node::children() const { return children_; }
    const uint32_t&   Node::id()           const { return id_; }
    const glm::mat4&  Node::local_matrix() const { return local_matrix_; }
    const glm::mat4&  Node::world_matrix() const { return world_matrix_; }
}