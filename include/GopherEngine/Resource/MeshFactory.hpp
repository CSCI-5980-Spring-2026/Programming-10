#pragma once

#include <GopherEngine/Resource/Mesh.hpp>
using namespace GopherEngine;

#include <memory>
#include <string>
#include <initializer_list>

namespace GopherEngine {

    class MeshFactory {
        public:
            static std::shared_ptr<Mesh> create_cube(float width = 1.f, float height = 1.f, float depth = 1.f);
            static std::shared_ptr<Mesh> create_cylinder(float radius = 1.f, float height = 1.f, int segments = 32);
            static std::shared_ptr<Mesh> create_cone(float radius = 1.f, float height = 1.f, int segments = 32);
            static std::shared_ptr<Mesh> create_plane(float width = 1.f, float height = 1.f);
            static std::shared_ptr<Mesh> create_sphere(float radius = 1.f, int slices = 32, int stacks = 16);
            
            static std::vector<float> interleave_vertex_data(
                const std::vector<glm::vec3>& vertices, 
                const std::vector<glm::vec3>& normals, 
                const std::vector<glm::vec4>& colors, 
                const std::vector<glm::vec2>& uvs);

        private:
            static void append_vertex(
                std::vector<float>& buffer,
                const glm::vec3& position,
                const glm::vec3& normal,
                const glm::vec2& uv,
                const glm::vec4& color = glm::vec4(1.f));

            static std::string make_generated_mesh_name(
                const std::string& type,
                std::initializer_list<std::pair<std::string, float>> params);

            static std::string make_canonical_float(float value, int precision = 6);
    };

} 
