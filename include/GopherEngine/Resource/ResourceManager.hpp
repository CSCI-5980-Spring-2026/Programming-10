#pragma once

#include <GopherEngine/Core/Service.hpp>
#include <GopherEngine/Core/FileLoader.hpp>
#include <GopherEngine/Core/Guid.hpp>
#include <GopherEngine/Resource/Texture.hpp>
#include <GopherEngine/Resource/Mesh.hpp>
#include <GopherEngine/Renderer/Material.hpp>

#include <vector>
#include <functional>
#include <memory>
#include <filesystem>
#include <optional>
#include <unordered_map>

namespace GopherEngine
{
    class ResourceManager: public Service<ResourceManager>
    {
        public:

            void load_texture(
                std::filesystem::path path,
                std::function<void(std::shared_ptr<Texture>)> on_success = nullptr,
                std::function<void(const std::string&)> on_error = nullptr);

            void load_texture(
                const Guid& guid,
                std::function<void(std::shared_ptr<Texture>)> on_success = nullptr,
                std::function<void(const std::string&)> on_error = nullptr);

            void load_mesh(
                std::filesystem::path path,
                std::function<void(std::shared_ptr<Mesh>)> on_success = nullptr,
                std::function<void(const std::string&)> on_error = nullptr);

            void load_mesh(
                const Guid& guid,
                std::function<void(std::shared_ptr<Mesh>)> on_success = nullptr,
                std::function<void(const std::string&)> on_error = nullptr);

            void load_material(
                std::filesystem::path path,
                std::function<void(std::shared_ptr<Material>)> on_success = nullptr,
                std::function<void(const std::string&)> on_error = nullptr);

            void load_material(
                const Guid& guid,
                std::function<void(std::shared_ptr<Material>)> on_success = nullptr,
                std::function<void(const std::string&)> on_error = nullptr);

            // Registers a one-shot callback to run on the main thread after all
            // currently pending resource loads have finished.
            void on_load_complete(std::function<void()> callback);

            bool poll();

            std::shared_ptr<Mesh> get_mesh(const Guid& guid) const;
            std::shared_ptr<Texture> get_texture(const Guid& guid) const;
            std::shared_ptr<Material> get_material(const Guid& guid) const;
            std::optional<Guid> get_referenced_material_guid(const Guid& mesh_guid) const;
            std::optional<Guid> get_referenced_texture_guid(const Guid& material_guid) const;

            bool has_mesh(const Guid& guid) const;
            bool has_texture(const Guid& guid) const;
            bool has_material(const Guid& guid) const;

            std::shared_ptr<Mesh> register_mesh(const std::shared_ptr<Mesh>& mesh);
            std::shared_ptr<Texture> register_texture(const std::shared_ptr<Texture>& texture);
            std::shared_ptr<Material> register_material(const std::shared_ptr<Material>& material);

            bool remove_mesh(const Guid& guid);
            bool remove_texture(const Guid& guid);
            bool remove_material(const Guid& guid);

            void remove_all_meshes();
            void remove_all_textures();
            void remove_all_materials();
            void remove_all();

            void set_verbose(bool v);
            bool get_verbose() const;

        private:
            void resolve_material_texture_references();

            std::vector<LoadHandle> load_handles_;
            std::vector<std::function<void()>> on_load_complete_callbacks_;

            std::unordered_map<Guid, std::shared_ptr<Mesh>, GuidHasher> mesh_registry_;
            std::unordered_map<Guid, std::shared_ptr<Texture>, GuidHasher> texture_registry_;
            std::unordered_map<Guid, std::shared_ptr<Material>, GuidHasher> material_registry_;
            
            std::unordered_map<Guid, Guid, GuidHasher> mesh_material_references_;
            std::unordered_map<Guid, Guid, GuidHasher> material_texture_references_;

            bool verbose{false};
    };
}