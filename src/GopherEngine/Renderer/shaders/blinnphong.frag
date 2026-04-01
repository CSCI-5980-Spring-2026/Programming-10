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

layout(std140, binding = 1) uniform Material
{
    vec4 ambient_color;
    vec4 diffuse_color;
    vec4 specular_color;
    float shininess;
    int use_texture;
};

// The type of light is determined by the w component of the position:
// - If w == 0, the light is a directional light and the xyz components represent the direction.
// - If w == 1, the light is a point light and the xyz components represent the position.
struct Light
{
    vec4 position;
    vec4 ambient_intensity;
    vec4 diffuse_intensity;
    vec4 specular_intensity;
};

#define MAX_LIGHTS 8
layout(std140, binding = 2) uniform Lights
{
    Light lights[MAX_LIGHTS];
    int num_lights;
};

uniform sampler2D texture_image;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec4 vertex_color;
in vec2 tex_coord;

out vec4 frag_color;

void main()
{
    // Normalize the interpolated normal vector
    vec3 n = normalize(vertex_normal);

    // Initialize the illumination accumulator to zero
    vec3 illumination = vec3(0, 0, 0);

    for(int i=0; i < num_lights; i++)
    {
        // Ambient component
        illumination += lights[i].ambient_intensity.rgb * ambient_color.rgb;

        // Compute the vector from the vertex position to the light
        vec3 l;
        if(lights[i].position.w == 0.0)
            l = normalize(lights[i].position.xyz);
        else
            l = normalize(lights[i].position.xyz - vertex_position);

        // Diffuse component
        float ndotl  = max(dot(n, l), 0.0);
        illumination += ndotl * lights[i].diffuse_intensity.rgb * diffuse_color.rgb;

        // Specular component (only if light is facing the surface and shininess > 0)
        if (ndotl > 0.0 && shininess > 0.0) {

            // Compute the vector from the vertex to the eye
            vec3 e = normalize(eye_position.xyz - vertex_position);

            // Compute the halfway vector
            vec3 h = normalize(l + e);

            // Specular component
            float specular_component = pow(max(dot(h, n), 0.0), shininess);
            illumination += specular_component * lights[i].specular_intensity.rgb * specular_color.rgb;

        }
    }

    frag_color = vertex_color;
    frag_color.rgb *= illumination;

    if(use_texture != 0)
    {
        frag_color *= texture(texture_image, tex_coord);
    }
}
