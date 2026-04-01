#include <GopherEngine/Resource/MeshFactory.hpp>
#include <GopherEngine/Resource/ResourceManager.hpp>

#include <glm/gtc/constants.hpp>
#include <glm/glm.hpp>
using namespace glm;

#include <vector>
using namespace std;

namespace GopherEngine {

    std::shared_ptr<Mesh> MeshFactory::create_cube(float width, float height, float depth)
    {
        // The name and GUID of a procedurally generated mesh are derived entirely 
        // from its parameters and the type of mesh, so that identical meshes will
        // have the same GUID regardless of when or where they are generated. 
        string name = make_generated_mesh_name(
            "cube",
            {
                {"w", width},
                {"h", height},
                {"d", depth}
            }
        );
       Guid guid = Guid::from_name(name);

       // If an identical cube mesh was already generated before, return the cached 
       // resource from the registry.
       if(Service<ResourceManager>::get().has_mesh(guid)) {
            return Service<ResourceManager>::get().get_mesh(guid);
       }

        const float w = width  / 2.f;
        const float h = height / 2.f;
        const float d = depth  / 2.f;

        // Build the final packed layout directly to avoid creating temporary
        // attribute streams that are immediately interleaved afterward.
        vector<float> vertex_data;
        vertex_data.reserve(24 * 12);

        // Front face
        append_vertex(vertex_data, {-w, -h,  d}, {0.f,  0.f,  1.f}, {0.f, 1.f});
        append_vertex(vertex_data, { w, -h,  d}, {0.f,  0.f,  1.f}, {1.f, 1.f});
        append_vertex(vertex_data, { w,  h,  d}, {0.f,  0.f,  1.f}, {1.f, 0.f});
        append_vertex(vertex_data, {-w,  h,  d}, {0.f,  0.f,  1.f}, {0.f, 0.f});

        // Back face
        append_vertex(vertex_data, {-w, -h, -d}, {0.f,  0.f, -1.f}, {1.f, 1.f});
        append_vertex(vertex_data, { w, -h, -d}, {0.f,  0.f, -1.f}, {0.f, 1.f});
        append_vertex(vertex_data, { w,  h, -d}, {0.f,  0.f, -1.f}, {0.f, 0.f});
        append_vertex(vertex_data, {-w,  h, -d}, {0.f,  0.f, -1.f}, {1.f, 0.f});

        // Left face
        append_vertex(vertex_data, {-w, -h, -d}, {-1.f,  0.f,  0.f}, {0.f, 1.f});
        append_vertex(vertex_data, {-w, -h,  d}, {-1.f,  0.f,  0.f}, {1.f, 1.f});
        append_vertex(vertex_data, {-w,  h,  d}, {-1.f,  0.f,  0.f}, {1.f, 0.f});
        append_vertex(vertex_data, {-w,  h, -d}, {-1.f,  0.f,  0.f}, {0.f, 0.f});

        // Right face
        append_vertex(vertex_data, { w, -h, -d}, {1.f,  0.f,  0.f}, {1.f, 1.f});
        append_vertex(vertex_data, { w, -h,  d}, {1.f,  0.f,  0.f}, {0.f, 1.f});
        append_vertex(vertex_data, { w,  h,  d}, {1.f,  0.f,  0.f}, {0.f, 0.f});
        append_vertex(vertex_data, { w,  h, -d}, {1.f,  0.f,  0.f}, {1.f, 0.f});

        // Top face
        append_vertex(vertex_data, {-w,  h,  d}, {0.f,  1.f,  0.f}, {0.f, 1.f});
        append_vertex(vertex_data, { w,  h,  d}, {0.f,  1.f,  0.f}, {1.f, 1.f});
        append_vertex(vertex_data, { w,  h, -d}, {0.f,  1.f,  0.f}, {1.f, 0.f});
        append_vertex(vertex_data, {-w,  h, -d}, {0.f,  1.f,  0.f}, {0.f, 0.f});

        // Bottom face
        append_vertex(vertex_data, {-w, -h,  d}, {0.f, -1.f,  0.f}, {1.f, 1.f});
        append_vertex(vertex_data, { w, -h,  d}, {0.f, -1.f,  0.f}, {0.f, 1.f});
        append_vertex(vertex_data, { w, -h, -d}, {0.f, -1.f,  0.f}, {0.f, 0.f});
        append_vertex(vertex_data, {-w, -h, -d}, {0.f, -1.f,  0.f}, {1.f, 0.f});

        vector<uint32_t> indices = {
            // Front face
             0,  1,  2,  2,  3,  0,
            // Back face
             4,  6,  5,  6,  4,  7,
            // Left face
             8,  9, 10, 10, 11,  8,
            // Right face
            12, 14, 13, 14, 12, 15,
            // Top face
            16, 17, 18, 18, 19, 16,
            // Bottom face
            20, 22, 21, 22, 20, 23,
        };

        auto mesh = std::make_shared<Mesh>();
        mesh->array_buffer_ = std::move(vertex_data);
        mesh->element_buffer_ = indices;
        mesh->name_ = name;
        mesh->guid_ = guid;
       
        // Register the generated mesh in the resource manager registry.
        Service<ResourceManager>::get().register_mesh(mesh);

        return mesh;
    }


    shared_ptr<Mesh> MeshFactory::create_cone(float radius, float height, int segments)
    {
        // The name and GUID of a procedurally generated mesh are derived entirely 
        // from its parameters and the type of mesh, so that identical meshes will
        // have the same GUID regardless of when or where they are generated. 
        string name = make_generated_mesh_name(
            "cone",
            {
                {"r", radius},
                {"h", height},
                {"s", (float)segments}
            }
        );
       Guid guid = Guid::from_name(name);

       // If an identical cube mesh was already generated before, return the cached 
       // resource from the registry.
       if(Service<ResourceManager>::get().has_mesh(guid)) {
            return Service<ResourceManager>::get().get_mesh(guid);
       }

        std::vector<float> vertex_data;
        std::vector<uint32_t> indices;
        vertex_data.reserve((2 * segments + 4) * 12);

        const float angle = (glm::pi<float>() * 2.0f) / segments;

        // Top vertex
        append_vertex(vertex_data, {0.0f, height / 2.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.0f});
        // Side vertices
        for (int i = 0; i <= segments; i++)
        {
            append_vertex(
                vertex_data,
                {std::cos(angle * i) * radius, -height / 2.0f, std::sin(angle * i) * radius},
                {std::cos(angle * i), 0.0f, std::sin(angle * i)},
                {(float)i / segments, 1.0f});
        }
        // Bottom center vertex
        append_vertex(vertex_data, {0.0f, -height / 2.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.5f, 0.5f});
        // Bottom vertices
        for (int i = 0; i <= segments; i++)
        {
            append_vertex(
                vertex_data,
                {std::cos(angle * i) * radius, -height / 2.0f, std::sin(angle * i) * radius},
                {0.0f, -1.0f, 0.0f},
                {(std::cos(angle * i) + 1.0f) / 2.0f, (std::sin(angle * i) - 1.0f) / -2.0f});
        }

        // Side triangles
        for (int i = 0; i < segments; i++)
        {
            indices.push_back(0);
            indices.push_back(i + 2);
            indices.push_back(i + 1);
        }
        // Bottom triangles
        const uint32_t startIndex = segments + 2;
        for (int i = 0; i < segments; i++)
        {
            indices.push_back(startIndex);
            indices.push_back(startIndex + i + 1);
            indices.push_back(startIndex + i + 2);
        }

        auto mesh = std::make_shared<Mesh>();
        mesh->array_buffer_ = std::move(vertex_data);
        mesh->element_buffer_ = indices;
        mesh->name_ = name;
        mesh->guid_ = guid;
       
        // Register the generated mesh in the resource manager registry.
        Service<ResourceManager>::get().register_mesh(mesh);
        
        return mesh;
    }

    shared_ptr<Mesh> MeshFactory::create_cylinder(float radius, float height, int segments) {

        // The name and GUID of a procedurally generated mesh are derived entirely 
        // from its parameters and the type of mesh, so that identical meshes will
        // have the same GUID regardless of when or where they are generated. 
        string name = make_generated_mesh_name(
            "cylinder",
            {
                {"r", radius},
                {"h", height},
                {"s", (float)segments}
            }
        );
       Guid guid = Guid::from_name(name);

       // If an identical cube mesh was already generated before, return the cached 
       // resource from the registry.
       if(Service<ResourceManager>::get().has_mesh(guid)) {
            return Service<ResourceManager>::get().get_mesh(guid);
       }

        std::vector<float> vertex_data;
        std::vector<uint32_t> indices;
        vertex_data.reserve((4 * segments + 6) * 12);

        const float angle_increment = (glm::pi<float>() * 2.0f) / segments;
        const int num_vertices_x = segments + 1;

        // Create the cylinder barrel vertices
        for (int i = 0; i < num_vertices_x; i++)
        {
            const float angle = i * angle_increment;
            append_vertex(
                vertex_data,
                {std::cos(angle) * radius, height / 2.0f, std::sin(angle) * radius},
                {std::cos(angle), 0.0f, std::sin(angle)},
                {1.0f - (float)i / segments, 0.0f});
            append_vertex(
                vertex_data,
                {std::cos(angle) * radius, -height / 2.0f, std::sin(angle) * radius},
                {std::cos(angle), 0.0f, std::sin(angle)},
                {1.0f - (float)i / segments, 1.0f});
        }
        // Create the cylinder barrel triangles
        for (int i = 0; i < segments; i++)
        {
            indices.push_back(i * 2);
            indices.push_back(i * 2 + 2);
            indices.push_back(i * 2 + 1);
            indices.push_back(i * 2 + 1);
            indices.push_back(i * 2 + 2);
            indices.push_back(i * 2 + 3);
        }

        // Create a single vertex and normal at center for the top disc
        const uint32_t top_center_index = static_cast<uint32_t>(2 * num_vertices_x);
        append_vertex(vertex_data, {0.0f, height / 2.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.0f});

        // Create the top disc vertices
        for (int i = 0; i < num_vertices_x; i++)
        {
            const float angle = i * angle_increment;
            append_vertex(
                vertex_data,
                {std::cos(angle) * radius, height / 2.0f, std::sin(angle) * radius},
                {0.0f, 1.0f, 0.0f},
                {1.0f - (float)i / segments, 0.0f});
        }
        // Create the top disc triangles
        for (int i = 0; i < segments; i++)
        {
            indices.push_back(top_center_index);
            indices.push_back(top_center_index + i + 2);
            indices.push_back(top_center_index + i + 1);
        }

        // Create a single vertex and normal at center for the bottom disc
        const uint32_t bottom_center_index = top_center_index + 1 + num_vertices_x;
        append_vertex(vertex_data, {0.0f, -height / 2.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.5f, 1.0f});

        // Create the bottom disc vertices
        for (int i = 0; i < num_vertices_x; i++)
        {
            const float angle = i * angle_increment;
            append_vertex(
                vertex_data,
                {std::cos(angle) * radius, -height / 2.0f, std::sin(angle) * radius},
                {0.0f, -1.0f, 0.0f},
                {1.0f - (float)i / segments, 1.0f});
        }
        // Create the bottom disc triangles
        for (int i = 0; i < segments; i++)
        {
            indices.push_back(bottom_center_index);
            indices.push_back(bottom_center_index + i + 1);
            indices.push_back(bottom_center_index + i + 2);
        }

        auto mesh = std::make_shared<Mesh>();
        mesh->array_buffer_ = std::move(vertex_data);
        mesh->element_buffer_ = indices;
        mesh->name_ = name;
        mesh->guid_ = guid;
       
        // Register the generated mesh in the resource manager registry.
        Service<ResourceManager>::get().register_mesh(mesh);

        return mesh;
    }

    shared_ptr<Mesh> MeshFactory::create_plane(float width, float height) {

        // The name and GUID of a procedurally generated mesh are derived entirely 
        // from its parameters and the type of mesh, so that identical meshes will
        // have the same GUID regardless of when or where they are generated. 
        string name = make_generated_mesh_name(
            "plane",
            {
                {"w", width},
                {"h", height}
            }
        );
       Guid guid = Guid::from_name(name);

       // If an identical cube mesh was already generated before, return the cached 
       // resource from the registry.
       if(Service<ResourceManager>::get().has_mesh(guid)) {
            return Service<ResourceManager>::get().get_mesh(guid);
       }

        std::vector<float> vertex_data;
        std::vector<uint32_t> indices;
        vertex_data.reserve(4 * 12);

        append_vertex(vertex_data, {-width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f});
        append_vertex(vertex_data, { width / 2.0f, -height / 2.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f});
        append_vertex(vertex_data, { width / 2.0f,  height / 2.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f});
        append_vertex(vertex_data, {-width / 2.0f,  height / 2.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f});

        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(0);
        indices.push_back(3);

        auto mesh = std::make_shared<Mesh>();
        mesh->array_buffer_ = std::move(vertex_data);
        mesh->element_buffer_ = indices;
        mesh->name_ = name;
        mesh->guid_ = guid;
       
        // Register the generated mesh in the resource manager registry.
        Service<ResourceManager>::get().register_mesh(mesh);

        return mesh;
    }

    shared_ptr<Mesh> MeshFactory::create_sphere(float radius, int slices, int stacks)
    {
        // The name and GUID of a procedurally generated mesh are derived entirely 
        // from its parameters and the type of mesh, so that identical meshes will
        // have the same GUID regardless of when or where they are generated. 
        string name = make_generated_mesh_name(
            "sphere",
            {
                {"r", radius},
                {"s", (float)slices},
                {"t", (float)stacks}
            }
        );
       Guid guid = Guid::from_name(name);

       // If an identical cube mesh was already generated before, return the cached 
       // resource from the registry.
       if(Service<ResourceManager>::get().has_mesh(guid)) {
            return Service<ResourceManager>::get().get_mesh(guid);
       }
       
        std::vector<float> vertex_data;
        std::vector<uint32_t> indices;

        // Logical defaults
        const float phi_start   = 0.0f;
        const float phi_length  = glm::two_pi<float>();
        const float theta_start  = 0.0f;
        const float theta_length = glm::pi<float>();

        int width_segments  = std::max(3, slices);
        int height_segments = std::max(2, stacks);

        const float theta_end = std::min(theta_start + theta_length, glm::pi<float>());

        int index = 0;
        std::vector<std::vector<int>> grid;
        vertex_data.reserve((width_segments + 1) * (height_segments + 1) * 12);

        // Generate vertices, normals, and uvs
        for (int iy = 0; iy <= height_segments; iy++)
        {
            std::vector<int> vertices_row;
            float v = (float)iy / (float)height_segments;

            // Special case for the poles
            float u_offset = 0.0f;
            if (iy == 0 && theta_start == 0.0f)
            {
                u_offset = 0.5f / (float)width_segments;
            }
            else if (iy == height_segments && theta_end == glm::pi<float>())
            {
                u_offset = -0.5f / (float)width_segments;
            }

            for (int ix = 0; ix <= width_segments; ix++)
            {
                float u = (float)ix / (float)width_segments;

                glm::vec3 vertex;
                vertex.x = -radius * std::cos(phi_start + u * phi_length) * std::sin(theta_start + v * theta_length);
                vertex.y =  radius * std::cos(theta_start + v * theta_length);
                vertex.z =  radius * std::sin(phi_start + u * phi_length) * std::sin(theta_start + v * theta_length);

                glm::vec3 normal = glm::normalize(vertex);
                append_vertex(vertex_data, vertex, normal, glm::vec2(u + u_offset, 1.0f - v));

                vertices_row.push_back(index++);
            }

            grid.push_back(vertices_row);
        }

        // Indices
        for (int iy = 0; iy < height_segments; iy++)
        {
            for (int ix = 0; ix < width_segments; ix++)
            {
                int a = grid[iy    ][ix + 1];
                int b = grid[iy    ][ix    ];
                int c = grid[iy + 1][ix    ];
                int d = grid[iy + 1][ix + 1];

                if (iy != 0 || theta_start > 0.0f)
                    indices.insert(indices.end(), { (uint32_t)a, (uint32_t)b, (uint32_t)d });

                if (iy != height_segments - 1 || theta_end < glm::pi<float>())
                    indices.insert(indices.end(), { (uint32_t)b, (uint32_t)c, (uint32_t)d });
            }
        }

        auto mesh = make_shared<Mesh>();
        mesh->array_buffer_   = std::move(vertex_data);
        mesh->element_buffer_ = indices;
        mesh->name_ = name;
        mesh->guid_ = guid;
       
        // Register the generated mesh in the resource manager registry.
        Service<ResourceManager>::get().register_mesh(mesh);

        return mesh;
    }

    void MeshFactory::append_vertex(
        std::vector<float>& buffer,
        const vec3& position,
        const vec3& normal,
        const vec2& uv,
        const vec4& color)
    {
        buffer.push_back(position.x);
        buffer.push_back(position.y);
        buffer.push_back(position.z);

        buffer.push_back(normal.x);
        buffer.push_back(normal.y);
        buffer.push_back(normal.z);

        buffer.push_back(color.r);
        buffer.push_back(color.g);
        buffer.push_back(color.b);
        buffer.push_back(color.a);

        buffer.push_back(uv.x);
        buffer.push_back(uv.y);
    }

    vector<float> MeshFactory::interleave_vertex_data(const vector<vec3>& vertices, const vector<vec3>& normals, const vector<vec4>& colors, const vector<vec2>& uvs)
    {
        std::vector<float> buffer;

        // 3 + 3 + 4 + 2 floats per vertex
        buffer.reserve(vertices.size() * 12);  

        for(size_t i = 0; i < vertices.size(); i++)
        {
            const vec3 normal = (i < normals.size()) ? normals[i] : vec3(0.f);
            const vec4 color = (i < colors.size()) ? colors[i] : vec4(1.f);
            const vec2 uv = (i < uvs.size()) ? uvs[i] : vec2(0.f);
            append_vertex(buffer, vertices[i], normal, uv, color);
        }

        return buffer;
    }   

    std::string MeshFactory::make_canonical_float(float value, int precision)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        std::string s = oss.str();

        while (!s.empty() && s.back() == '0')
            s.pop_back();

        if (!s.empty() && s.back() == '.')
            s.pop_back();

        if (s == "-0")
            s = "0";

        return s;
    }

    std::string MeshFactory::make_generated_mesh_name(
        const std::string& type,
        std::initializer_list<std::pair<std::string, float>> params)
    {
        std::ostringstream oss;
        oss << "generated://mesh/" << type;

        for (const auto& param : params)
            oss << "/" << param.first << "=" << make_canonical_float(param.second);

        return oss.str();
    }
}
