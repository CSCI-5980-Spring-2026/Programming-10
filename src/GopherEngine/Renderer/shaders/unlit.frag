#version 420 core

layout(std140, binding = 1) uniform Material
{
    vec4 material_color;
    int use_texture;
};

uniform sampler2D texture_image;

in vec4 vertex_color;
in vec2 tex_coord;
out vec4 frag_color;

void main()
{
    frag_color = vertex_color * material_color;
    if(use_texture != 0)
    {
        frag_color *= texture(texture_image, tex_coord);
    }
}
