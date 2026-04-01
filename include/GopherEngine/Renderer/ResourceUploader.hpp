#pragma once
#include <GopherEngine/Core/Service.hpp>
#include <GopherEngine/Resource/Mesh.hpp>
#include <GopherEngine/Resource/Texture.hpp>

#include <memory>
#include <queue>

namespace GopherEngine
{
    class ResourceUploader: public Service<ResourceUploader>
    {
        public:

            enum class UploadStatus {
                NotStarted,
                InProgress,
                Completed
            };

            struct PendingMeshUpload {
                std::shared_ptr<GopherEngine::Mesh> mesh_;
                unsigned int vao_{0};
                unsigned int vbo_{0};
                unsigned int ebo_{0};
                std::size_t vertex_bytes_uploaded_{0};
                std::size_t index_bytes_uploaded_{0};
            }; 

            struct PendingTextureUpload {
                std::shared_ptr<GopherEngine::Texture> texture_;
                unsigned int texture_id_{0};
                std::size_t rows_uploaded_{0};
            };

            void set_upload_budget(std::size_t bytes) { upload_budget_ = bytes; }
            std::size_t get_upload_budget() const { return upload_budget_; }
            UploadStatus get_upload_status() const { return upload_status_; }

            void upload_mesh(const std::shared_ptr<Mesh>& mesh);
            void upload_texture(const std::shared_ptr<Texture>& texture);
            void process_pending_uploads();
            
        private:

            std::queue<PendingMeshUpload> pending_mesh_uploads_;
            std::queue<PendingTextureUpload> pending_texture_uploads_;

            // 1MB per frame budget for uploading resources to GPU
            std::size_t upload_budget_{1024 * 1024}; 

            UploadStatus upload_status_{UploadStatus::NotStarted};
    };
}