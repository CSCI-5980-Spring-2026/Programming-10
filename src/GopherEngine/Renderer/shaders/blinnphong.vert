#version 420 core

layout(std140, binding = 0) uniform Matrices
{
    mat4 projection_matrix;
    mat4 model_view_matrix;
    mat4 model_matrix;
    mat4 view_matrix;
    mat4 normal_matrix;
    vec4 eye_position;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 uv;

out vec3 vertex_position;
out vec3 vertex_normal;
out vec4 vertex_color;
out vec2 tex_coord;

void main()
{
    vertex_position = (model_matrix * vec4(position, 1)).xyz;
    vertex_normal = normalize((normal_matrix * vec4(normal, 0)).xyz);
    tex_coord = uv.xy;
    vertex_color = color;
    gl_Position = projection_matrix * model_view_matrix * vec4(position, 1);
}
