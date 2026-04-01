#include <GopherEngine/Resource/ResourceManager.hpp>
#include <GopherEngine/Resource/MeshFactory.hpp>
#include <GopherEngine/Renderer/BlinnPhongMaterial.hpp>

#include <algorithm>
#include <iostream>
#include <sstream>
using namespace std;

namespace GopherEngine
{

    // Loads a single texture file from disk asynchronously (for demonstration purposes only).
    // This method uses SFML to decode the image file. This functionality would not be used
    // by the runtime engine; it should performed by the asset conditioning pipeline instead. 
    // The runtime engine should only have to deal with raw pixel data, not image files.

    void ResourceManager::load_texture(
                const Guid& guid,
                std::function<void(std::shared_ptr<Texture>)> on_success,
                std::function<void(const std::string&)> on_error) {

        // If the same texture is already loaded, invoke the success callback immediately with
        // the cached resource from the registry. This allows multiple resources that reference
        // the same texture file to share a single Texture instance in memory.
        if(has_texture(guid)) {
            if(on_success)
                on_success(get_texture(guid));
            return;
        }

        LoadHandle handle = FileLoader::load_file_async("resources/textures/" + guid.to_string());

        handle.on_complete([this, guid, on_success = std::move(on_success), on_error = std::move(on_error)](const FileData& file_data)
        {
            if(!file_data.ok_) {
                if(on_error) 
                    on_error(file_data.error_);
                return;
            }



            // TO BE ADDED: Synchronously load the metadata file to retrieve information necessary to parse the texture data



            // Create a texture resource. The name should be the original file path, since
            // that's the canonical identity of the resource. The GUID is derived from
            // the name, so it will be stable and deterministic across runs.
            auto texture = std::make_shared<Texture>();
            texture->name_ = "REPLACE_WITH_ACTUAL_TEXTURE_NAME_FROM_METADATA";
            texture->guid_ = guid;

            // Add the loaded texture to the registry so it can be retrieved by GUID later
            texture = register_texture(texture);

            if(verbose) 
                cout << "Loaded texture: " << texture->name_ << " with GUID " << texture->guid_.to_string() << endl;

             if(on_success)
                on_success(texture);
        });

        load_handles_.push_back(std::move(handle));
    }

    void ResourceManager::load_texture(
                std::filesystem::path path,
                std::function<void(std::shared_ptr<Texture>)> on_success,
                std::function<void(const std::string&)> on_error) {
        
        Guid guid = Guid::from_name(path.string());
        load_texture(guid, std::move(on_success), std::move(on_error));
    }

    // Loads a single mesh file from disk asynchronously (for demonstration purposes only).
    // This method uses Assimp to decode the mesh file. This functionality would not be used
    // by the runtime engine; it should performed by the asset conditioning pipeline instead.
    // Note that if the file contains multiple meshes, only the first one is loaded, and
    // Embedded materials and textures are ignored. The asset conditioning pipeline should be
    // responsible for flattening complex source assets into simple runtime resources with 
    // explicit dependencies (e.g. separate mesh and texture files).
    void ResourceManager::load_mesh(
                const Guid& guid,
                std::function<void(std::shared_ptr<Mesh>)> on_success,
                std::function<void(const std::string&)> on_error) {

        // If the same mesh is already loaded, invoke the success callback immediately with
        // the cached resource from the registry. This allows multiple resources that reference
        // the same mesh file to share a single Mesh instance in memory.
        if(has_mesh(guid)) {
            if(on_success)
                on_success(get_mesh(guid));
            return;
        }

        LoadHandle handle = FileLoader::load_file_async("resources/meshes/" + guid.to_string());

        handle.on_complete([this, guid, on_success = std::move(on_success), on_error = std::move(on_error)](const FileData& file_data)
        {
            if(!file_data.ok_) {
                if(on_error) 
                    on_error(file_data.error_);
                return;
            }



            // TO BE ADDED: synchronously load the metadata file to retrieve information necessary to parse the mesh data



            // Create a mesh resource. The name should be the original file path, since
            // that's the canonical identity of the resource. The GUID is derived from
            // the name, so it will be stable and deterministic across runs.
            auto mesh = std::make_shared<Mesh>();
            mesh->name_ = "REPLACE_WITH_ACTUAL_MESH_NAME_FROM_METADATA";
            mesh->guid_ = guid;

            // Add the loaded mesh to the registry so it can be retrieved by GUID later
            mesh = register_mesh(mesh);

            if(verbose)
                cout << "Loaded mesh: " << mesh->name_ << " with GUID " << mesh->guid_.to_string() << endl;

            if(on_success)
                on_success(mesh);
        });

        load_handles_.push_back(std::move(handle));
    }

    void ResourceManager::load_mesh(
                std::filesystem::path path,
                std::function<void(std::shared_ptr<Mesh>)> on_success,
                std::function<void(const std::string&)> on_error) {

        Guid guid = Guid::from_name(path.string());
        load_mesh(guid, std::move(on_success), std::move(on_error));
    }

    // Loads a material file from resources/materials/{guid}.material and builds a BlinnPhongMaterial.
    // The file is plain ASCII text with labeled fields written by the asset conditioning pipeline.
    void ResourceManager::load_material(
                const Guid& guid,
                std::function<void(std::shared_ptr<Material>)> on_success,
                std::function<void(const std::string&)> on_error) {

        if(has_material(guid)) {
            if(on_success)
                on_success(get_material(guid));
            return;
        }

        LoadHandle handle = FileLoader::load_file_async("resources/materials/" + guid.to_string() + ".material");

        handle.on_complete([this, guid, on_success = std::move(on_success), on_error = std::move(on_error)](const FileData& file_data)
        {
            if(!file_data.ok_) {
                if(on_error)
                    on_error(file_data.error_);
                return;
            }

            auto blinn_phong_material = std::make_shared<BlinnPhongMaterial>();
            blinn_phong_material->name_ = "REPLACE_WITH_ACTUAL_MATERIAL_NAME_FROM_MATERIAL_FILE";
            blinn_phong_material->guid_ = guid;

            auto material = register_material(blinn_phong_material);

            if(verbose) 
                cout << "Loaded material: " << blinn_phong_material->name_.value() << " with GUID " << guid.to_string() << endl;
            
            if(on_success)
                on_success(material);
        });

        load_handles_.push_back(std::move(handle));
    }

    void ResourceManager::load_material(
                std::filesystem::path path,
                std::function<void(std::shared_ptr<Material>)> on_success,
                std::function<void(const std::string&)> on_error) {

        Guid guid = Guid::from_name(path.string());
        load_material(guid, std::move(on_success), std::move(on_error));
    }

    void ResourceManager::on_load_complete(std::function<void()> callback)
    {
        if (!callback)
            return;

        on_load_complete_callbacks_.push_back(std::move(callback));
    }

    void ResourceManager::resolve_material_texture_references()
    {
        for (const auto& [material_guid, texture_guid] : material_texture_references_)
        {
            auto material = get_material(material_guid);
            if (!material)
                continue;

            auto blinn_phong_material = std::dynamic_pointer_cast<BlinnPhongMaterial>(material);
            if (!blinn_phong_material || blinn_phong_material->get_texture())
                continue;

            auto texture = get_texture(texture_guid);
            if (!texture)
                continue;

            blinn_phong_material->set_texture(texture);
        }
    }

    bool ResourceManager::poll()
    {
        if(!load_handles_.empty())
        {
            // Partition load handles into "not ready" and "ready to fire"
            auto it = std::partition(
                load_handles_.begin(), load_handles_.end(),
                [](const LoadHandle& h) { return !h.is_ready(); }
            );

            // Record the ready range as indices before firing any callbacks.
            // A callback may push new handles onto load_handles_ (e.g. load_mesh
            // triggering load_material), and those new handles must survive into
            // the next poll() cycle — they must not be included in the erase below.
            size_t ready_start = std::distance(load_handles_.begin(), it);
            size_t ready_count = load_handles_.size() - ready_start;

            for (size_t i = 0; i < ready_count; ++i)
                load_handles_[ready_start + i].fire_callback();

            // Erase only the handles that were ready at the start of this frame
            load_handles_.erase(
                load_handles_.begin() + ready_start,
                load_handles_.begin() + ready_start + ready_count
            );
        }

        // If no loads are pending, run any queued post-load callbacks on the main thread.
        // Callbacks registered by these callbacks belong to the next idle poll.
        if (load_handles_.empty() && !on_load_complete_callbacks_.empty())
        {
            resolve_material_texture_references();

            auto callbacks = std::move(on_load_complete_callbacks_);
            on_load_complete_callbacks_.clear();

            for (auto& callback : callbacks)
            {
                if (callback)
                    callback();
            }
        }

        // Returns true if the pending load queue is now empty
        return load_handles_.empty();
    }

    
    std::shared_ptr<Mesh> ResourceManager::get_mesh(const Guid& guid) const
    {
        auto it = mesh_registry_.find(guid);
        if (it != mesh_registry_.end())
            return it->second;
        return nullptr;
    }

    std::shared_ptr<Texture> ResourceManager::get_texture(const Guid& guid) const
    {
        auto it = texture_registry_.find(guid);
        if (it != texture_registry_.end())
            return it->second;
        return nullptr;
    }

    bool ResourceManager::has_mesh(const Guid& guid) const
    {
        return mesh_registry_.find(guid) != mesh_registry_.end();
    }

    bool ResourceManager::has_texture(const Guid& guid) const
    {
        return texture_registry_.find(guid) != texture_registry_.end();
    }

    std::shared_ptr<Material> ResourceManager::get_material(const Guid& guid) const
    {
        auto it = material_registry_.find(guid);
        if (it != material_registry_.end())
            return it->second;
        return nullptr;
    }

    std::optional<Guid> ResourceManager::get_referenced_material_guid(const Guid& mesh_guid) const
    {
        auto it = mesh_material_references_.find(mesh_guid);
        if (it != mesh_material_references_.end())
            return it->second;
        return std::nullopt;
    }

    std::optional<Guid> ResourceManager::get_referenced_texture_guid(const Guid& material_guid) const
    {
        auto it = material_texture_references_.find(material_guid);
        if (it != material_texture_references_.end())
            return it->second;
        return std::nullopt;
    }

    bool ResourceManager::has_material(const Guid& guid) const
    {
        return material_registry_.find(guid) != material_registry_.end();
    }

    std::shared_ptr<Mesh> ResourceManager::register_mesh(const std::shared_ptr<Mesh>& mesh)
    {
        if (!mesh)
            return nullptr;

        auto it = mesh_registry_.find(mesh->guid_);
        if (it != mesh_registry_.end())
            return it->second;

        mesh_registry_[mesh->guid_] = mesh;
        return mesh;
    }

    std::shared_ptr<Texture> ResourceManager::register_texture(const std::shared_ptr<Texture>& texture)
    {
        if (!texture)
            return nullptr;

        auto it = texture_registry_.find(texture->guid_);
        if (it != texture_registry_.end())
            return it->second;

        texture_registry_[texture->guid_] = texture;
        return texture;
    }

    std::shared_ptr<Material> ResourceManager::register_material(const std::shared_ptr<Material>& material)
    {
        if (!material)
            return nullptr;

        if(!material->guid_)
        {
            if(material->name_)
            {
                // If the material has a name but no GUID, we can still register it by deriving a GUID from the name. 
                material->guid_ = Guid::from_name(material->name_.value());
            }
            else
            {
                cerr << "Warning: Attempted to register a material without a name or GUID." << endl; 
                return nullptr;
            } 
        }

        auto it = material_registry_.find(material->guid_.value());
        if (it != material_registry_.end())
            return it->second;

        material_registry_[material->guid_.value()] = material;
        return material;
    }

    bool ResourceManager::remove_mesh(const Guid& guid)
    {
        mesh_material_references_.erase(guid);
        return mesh_registry_.erase(guid) > 0;
    }

    bool ResourceManager::remove_texture(const Guid& guid)
    {
        return texture_registry_.erase(guid) > 0;
    }

    bool ResourceManager::remove_material(const Guid& guid)
    {
        material_texture_references_.erase(guid);
        return material_registry_.erase(guid) > 0;
    }

    void ResourceManager::remove_all_meshes()
    {
        mesh_registry_.clear();
        mesh_material_references_.clear();
    }

    void ResourceManager::remove_all_textures()
    {
        texture_registry_.clear();
    }

    void ResourceManager::remove_all_materials()
    {
        material_registry_.clear();
        material_texture_references_.clear();
    }

    void ResourceManager::remove_all()
    {
        mesh_registry_.clear();
        texture_registry_.clear();
        material_registry_.clear();
        mesh_material_references_.clear();
        material_texture_references_.clear();
    }

    void ResourceManager::set_verbose(bool v)
    {
        verbose = v;
    }

    bool ResourceManager::get_verbose() const
    {
        return verbose;
    }
}
