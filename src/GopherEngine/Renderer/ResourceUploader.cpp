#include <GopherEngine/Renderer/ResourceUploader.hpp>

#include <GL/glew.h>          // Must come first
#include <SFML/OpenGL.hpp>    // After glew

#include <iostream>
using namespace std;

namespace GopherEngine
{
    void ResourceUploader::upload_mesh(const std::shared_ptr<Mesh>& mesh) {

        mesh->pending_upload_ = true;

        // Create a pending upload struct to hold the mesh and its future GPU handles
        PendingMeshUpload upload;
        upload.mesh_ = std::move(mesh);

        // Generate the buffers
        glCreateVertexArrays(1, &upload.vao_);
        glCreateBuffers(1, &upload.vbo_);
        glCreateBuffers(1, &upload.ebo_);
        
        // Allocate GPU memory without uploading data yet
         glNamedBufferData(
            upload.vbo_,
            static_cast<GLsizeiptr>(upload.mesh_->array_buffer_.size() * sizeof(float)),
            upload.mesh_->array_buffer_.data(),
            GL_STATIC_DRAW
        );

        glNamedBufferData(
            upload.ebo_,
            static_cast<GLsizeiptr>(upload.mesh_->element_buffer_.size() * sizeof(unsigned int)),
            upload.mesh_->element_buffer_.data(),
            GL_STATIC_DRAW
        );

        // Add the pending upload to the queue
        pending_mesh_uploads_.push(std::move(upload));

    }


    void ResourceUploader::upload_texture(const std::shared_ptr<Texture>& texture) {

        texture->pending_upload_ = true;

        // Create a pending upload struct to hold the texture and its future GPU handle
        PendingTextureUpload upload;
        upload.texture_ = std::move(texture);

        // Generate the texture ID and set default parameters
        glGenTextures(1, &upload.texture_id_);
        glBindTexture(GL_TEXTURE_2D, upload.texture_id_);

        // Sensible defaults — caller can rebind and adjust wrap/filter modes afterward
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

         // Allocate storage without uploading pixel data yet
        GLenum gl_format = (texture->format_ == Texture::Format::RGBA) ? GL_RGBA : GL_RGB;
        glTexImage2D(
            GL_TEXTURE_2D, 
            0, 
            GL_SRGB8_ALPHA8,    // internal format
            texture->width_, 
            texture->height_,
            0,
            gl_format,          // source pixel format
            GL_UNSIGNED_BYTE, 
            nullptr
        );

        // Reset binding to avoid accidentally using the texture before it's fully uploaded
        glBindTexture(GL_TEXTURE_2D, 0);

        // Add the pending upload to the queue
        pending_texture_uploads_.push(std::move(upload));
    }

    void ResourceUploader::process_pending_uploads() {

        if(pending_mesh_uploads_.empty() && pending_texture_uploads_.empty()) {
            upload_status_ = UploadStatus::NotStarted;
            return;
        }

        upload_status_ = UploadStatus::InProgress;
        size_t budget_remaining = upload_budget_;

        // Chunked upload loop for meshes — uploads vertex and index data in pieces
        // until the budget is exhausted, then configures the VAO once both are fully uploaded
        while (budget_remaining > 0 && !pending_mesh_uploads_.empty()) {
            PendingMeshUpload& upload = pending_mesh_uploads_.front();
            const Mesh& mesh = *upload.mesh_;

            const std::size_t total_vertex_bytes = mesh.array_buffer_.size() * sizeof(float);
            const std::size_t total_index_bytes  = mesh.element_buffer_.size() * sizeof(unsigned int);

            // Upload vertex data in chunks until the budget or buffer is exhausted
            if (upload.vertex_bytes_uploaded_ < total_vertex_bytes) {
                std::size_t remaining = total_vertex_bytes - upload.vertex_bytes_uploaded_;
                std::size_t chunk = std::min(remaining, budget_remaining);

                glNamedBufferSubData(upload.vbo_,
                                     upload.vertex_bytes_uploaded_,
                                     chunk,
                                     reinterpret_cast<const char*>(mesh.array_buffer_.data()) + upload.vertex_bytes_uploaded_);

                upload.vertex_bytes_uploaded_ += chunk;
                budget_remaining -= chunk;
            }

            // Upload index data in chunks until the budget or buffer is exhausted
            if (budget_remaining > 0 && upload.index_bytes_uploaded_ < total_index_bytes) {
                std::size_t remaining = total_index_bytes - upload.index_bytes_uploaded_;
                std::size_t chunk = std::min(remaining, budget_remaining);

                glNamedBufferSubData(upload.ebo_,
                                     upload.index_bytes_uploaded_,
                                     chunk,
                                     reinterpret_cast<const char*>(mesh.element_buffer_.data()) + upload.index_bytes_uploaded_);

                upload.index_bytes_uploaded_ += chunk;
                budget_remaining -= chunk;
            }

            // Both buffers fully transferred — configure the VAO and hand off to the mesh
            if (upload.vertex_bytes_uploaded_ == total_vertex_bytes && upload.index_bytes_uploaded_  == total_index_bytes) {

                const GLsizei stride = static_cast<GLsizei>(mesh.array_stride_ * sizeof(float));

                // Attach buffers to the VAO
                glVertexArrayVertexBuffer(upload.vao_, 0, upload.vbo_, 0, stride);
                glVertexArrayElementBuffer(upload.vao_, upload.ebo_);

                // location 0: position
                glEnableVertexArrayAttrib(upload.vao_, 0);
                glVertexArrayAttribFormat(upload.vao_, 0, 3, GL_FLOAT, GL_FALSE, 0);
                glVertexArrayAttribBinding(upload.vao_, 0, 0);

                // location 1: normal
                glEnableVertexArrayAttrib(upload.vao_, 1);
                glVertexArrayAttribFormat(upload.vao_, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
                glVertexArrayAttribBinding(upload.vao_, 1, 0);

                // location 2: color
                glEnableVertexArrayAttrib(upload.vao_, 2);
                glVertexArrayAttribFormat(upload.vao_, 2, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float));
                glVertexArrayAttribBinding(upload.vao_, 2, 0);

                // location 3: uv
                glEnableVertexArrayAttrib(upload.vao_, 3);
                glVertexArrayAttribFormat(upload.vao_, 3, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float));
                glVertexArrayAttribBinding(upload.vao_, 3, 0);

                // Transfer GPU handles to the mesh
                mesh.vao_ = upload.vao_;
                mesh.vbo_ = upload.vbo_;
                mesh.ebo_ = upload.ebo_;
                mesh.pending_upload_ = false;

                mesh.gpu_release_ = [&mesh]() {
                    if(mesh.vao_ != 0) glDeleteVertexArrays(1, &mesh.vao_);
                    if(mesh.vbo_ != 0) glDeleteBuffers(1, &mesh.vbo_);
                    if(mesh.ebo_ != 0) glDeleteBuffers(1, &mesh.ebo_);
                    mesh.vao_ = mesh.vbo_ = mesh.ebo_ = 0;
                    mesh.pending_upload_ = false;
                };

                pending_mesh_uploads_.pop();
            }
        }

        // Chunked upload loop for textures — uploads rows of pixel data until the 
        // budget is exhausted, then generates mipmaps and finalizes the texture 
        while (budget_remaining > 0 && !pending_texture_uploads_.empty()) {
            PendingTextureUpload& upload = pending_texture_uploads_.front();
            const Texture& texture = *upload.texture_;

            const GLenum gl_format = (texture.format_ == Texture::Format::RGBA) ? GL_RGBA : GL_RGB;
            const std::size_t channels = (texture.format_ == Texture::Format::RGBA) ? 4 : 3;

            const std::size_t bytes_per_row = static_cast<std::size_t>(texture.width_) * channels;

            if (bytes_per_row == 0 || texture.height_ <= 0 || texture.pixels_.empty()) {
                pending_texture_uploads_.pop();
                continue;
            }

            std::size_t rows_remaining = static_cast<std::size_t>(texture.height_) - upload.rows_uploaded_;

            std::size_t rows_this_frame = std::min(rows_remaining, budget_remaining / bytes_per_row);

            // Ensure forward progress if the budget is smaller than one row
            if (rows_this_frame == 0 && budget_remaining > 0) {
                rows_this_frame = 1;
            }

            // Clamp in case forcing one row exceeded remaining rows
            rows_this_frame = std::min(rows_this_frame, rows_remaining);

            const std::size_t upload_bytes = rows_this_frame * bytes_per_row;

            glBindTexture(GL_TEXTURE_2D, upload.texture_id_);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                0,
                static_cast<GLint>(upload.rows_uploaded_),
                texture.width_,
                static_cast<GLsizei>(rows_this_frame),
                gl_format,
                GL_UNSIGNED_BYTE,
                texture.pixels_.data() + (upload.rows_uploaded_ * bytes_per_row)
            );

            upload.rows_uploaded_ += rows_this_frame;

            if (upload_bytes <= budget_remaining) {
                budget_remaining -= upload_bytes;
            } else {
                budget_remaining = 0;
            }

            if (upload.rows_uploaded_ == static_cast<std::size_t>(texture.height_)) {

                glGenerateMipmap(GL_TEXTURE_2D);

                // Transfer GPU handle to the texture
                texture.texture_id_ = upload.texture_id_;
                texture.pending_upload_ = false;

                // Register the GPU cleanup callback on the texture
                texture.gpu_release_ = [&texture]() {
                    if (texture.texture_id_ != 0)
                        glDeleteTextures(1, &texture.texture_id_);
                    texture.texture_id_ = 0;
                    texture.pending_upload_ = false;
                };
                
                pending_texture_uploads_.pop();
            }

            glBindTexture(GL_TEXTURE_2D, 0);
        }
        

        // Update upload status based on whether there are still pending uploads
        if(pending_mesh_uploads_.empty() && pending_texture_uploads_.empty()) {
            upload_status_ = UploadStatus::Completed;
        }
        else {
            upload_status_ = UploadStatus::InProgress;
        }
    }

    
}