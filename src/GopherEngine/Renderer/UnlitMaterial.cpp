
#include <GopherEngine/Renderer/UnlitMaterial.hpp>
#include <GopherEngine/Renderer/ResourceUploader.hpp>
#include <GopherEngine/Renderer/ShaderLoader.hpp>

#include <glm/gtc/type_ptr.hpp> // value_ptr function to convert glm types to pointer for OpenGL

#include <GL/glew.h>          // Must come first
#include <SFML/OpenGL.hpp>    // After glew

#include <memory>
using namespace std;

namespace GopherEngine {

    struct UnlitUBO
    {
        glm::vec4 color;    // vec3 color + 1.0 alpha
        int use_texture;    // 1 or 0
        float padding[3];   // pad to 16-byte boundary
    };

    UnlitMaterial::UnlitMaterial() {
        
    }

    void UnlitMaterial::draw(const shared_ptr<Geometry>& geometry, const glm::mat4 &world_matrix, const RenderContext& context) {

        // If the resource is not a mesh, we don't know how to draw it, so just return early.
        // This allows the renderer to support multiple resource types in the future 
        // without needing to change the Material interface (e.g., points, particle systems, etc.)
        if (geometry->type_ != GeometryType::Mesh)
            return;
        
        // Downcast to the resource to a mesh reference. This is safe because we checked the type above.
        const shared_ptr<Mesh> mesh = static_pointer_cast<Mesh>(geometry);

        // Skip drawing if mesh has no geometry
        if(mesh->array_buffer_.empty() || mesh->element_buffer_.empty())
            return;

        // Lazy upload to GPU on first draw
        if(mesh->vao_ == 0) {

            // If the mesh is not already pending upload, enqueue it for upload
            // and return to avoid drawing before GPU resources are ready
            if(!mesh->pending_upload_) 
                Service<ResourceUploader>::get().upload_mesh(mesh);
           
            return;
        }

        // Lazy load the texture to GPU
        bool use_texture = false;
        if(texture_) {

            // If we have a texture and it has already been uploaded to the GPU
            if(texture_->texture_id_ != 0)
                use_texture = true;

            // If the texture is not already pending upload, enqueue it for upload
            else if(!texture_->pending_upload_)
                Service<ResourceUploader>::get().upload_texture(texture_);
        }
            
        // Retrieve and bind the shader program
        auto shader_program = ShaderLoader::get().load_shader(ShaderType::Unlit, sizeof(UnlitUBO));
        bind_shader(*shader_program, world_matrix, context, false, false);

        // Update material uniform buffer object (UBO)
        UnlitUBO ubo;
        ubo.color = glm::vec4(color_, 1.f);
        ubo.use_texture = use_texture ? 1 : 0;

        // Upload the material UBO data to the GPU
        glBindBuffer(GL_UNIFORM_BUFFER, shader_program->get_material_ubo());
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UnlitUBO), &ubo);

        // If we are using a texture, bind it to unit 0
        if(use_texture) {    
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture_->texture_id_);
        }

        // Bind the vertex array and draw the mesh
        glBindVertexArray(mesh->vao_);
        glDrawElements(GL_TRIANGLES, mesh->element_buffer_.size(), GL_UNSIGNED_INT, 0);

        // Unbind the shader and vertex array to clean up state
        unbind_shader(*shader_program);
        
    }
    
    void UnlitMaterial::set_color(const glm::vec3 &color) {
        color_ = color;
    }

    glm::vec3 UnlitMaterial::get_color() const {
        return color_;
    }

    void UnlitMaterial::set_texture(std::shared_ptr<Texture> texture) {
        texture_ = std::move(texture);
    }

    std::shared_ptr<Texture> UnlitMaterial::get_texture() const {
        return texture_;
    }

    void UnlitMaterial::clear_texture() {
        texture_.reset();
    }
} 