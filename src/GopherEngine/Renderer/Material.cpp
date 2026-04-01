#include <GopherEngine/Renderer/Material.hpp>
#include <GopherEngine/Renderer/ResourceUploader.hpp>

#include <GL/glew.h>          // Must come first
#include <SFML/OpenGL.hpp>    // After glew

#include <glm/gtc/type_ptr.hpp> // value_ptr function to convert glm types to pointer for OpenGL

using namespace glm;

namespace GopherEngine {

    struct MatricesUBO {
        mat4 projection_matrix;
        mat4 model_view_matrix;
        mat4 world_matrix;
        mat4 view_matrix;
        mat4 normal_matrix;
        vec4 eye_position;
    };

    // The type of light is determined by the w component of the position:
    // - If w == 0, the light is a directional light and the xyz components represent the direction.
    // - If w == 1, the light is a point light and the xyz components represent the position.
    struct LightUBO {
        vec4 position;
        vec4 ambient_intensity;
        vec4 diffuse_intensity;
        vec4 specular_intensity;
    };

    struct LightsUBO {
        LightUBO lights[ShaderProgram::MAX_LIGHTS];
        int num_lights;
    };
    
    void Material::bind_shader(const ShaderProgram& shader_program, const mat4 &world_matrix, const RenderContext& context, bool compute_normal_matrix, bool enable_lighting) {

        shader_program.bind();

        // Re-bind UBOs to their binding points every frame
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, shader_program.get_matrices_ubo());
        glBindBufferBase(GL_UNIFORM_BUFFER, 1, shader_program.get_material_ubo());
        glBindBufferBase(GL_UNIFORM_BUFFER, 2, shader_program.get_lights_ubo());

        MatricesUBO ubo;
        ubo.projection_matrix  = context.projection_matrix_;
        ubo.model_view_matrix  = context.view_matrix_ * world_matrix;
        ubo.world_matrix       = world_matrix;
        ubo.view_matrix        = context.view_matrix_;
        ubo.normal_matrix      = compute_normal_matrix
                                    ? transpose(inverse(world_matrix))
                                    : mat4(1.f);
        ubo.eye_position       = vec4(context.eye_position_, 1.f);

        glBindBuffer(GL_UNIFORM_BUFFER, shader_program.get_matrices_ubo());
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MatricesUBO), &ubo);

        if(enable_lighting) {
            LightsUBO lights_ubo;
            lights_ubo.num_lights = static_cast<int>(context.lights_.size());

            for(int i=0; i < lights_ubo.num_lights && i < ShaderProgram::MAX_LIGHTS; i++) {
                const auto& light = context.lights_[i];
                float light_type = (light->type_ == LightType::Directional) ? 0.f : 1.f;
                lights_ubo.lights[i].position = vec4(light->world_position_, light_type);
                lights_ubo.lights[i].ambient_intensity = vec4(light->ambient_intensity_, 1.f);
                lights_ubo.lights[i].diffuse_intensity = vec4(light->diffuse_intensity_, 1.f);
                lights_ubo.lights[i].specular_intensity = vec4(light->specular_intensity_, 1.f);
            }

            glBindBuffer(GL_UNIFORM_BUFFER, shader_program.get_lights_ubo());
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightsUBO), &lights_ubo);
        }
    }

    void Material::unbind_shader(const ShaderProgram& shader_program) {

        // Reset the uniform buffer binding to avoid accidental modification
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Reset the vertex array binding to avoid accidental modification
        glBindVertexArray(0);

        shader_program.unbind();
    }
}