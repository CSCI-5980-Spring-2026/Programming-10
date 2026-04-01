#pragma once

#include <GopherEngine/Renderer/ShaderProgram.hpp>
#include <GopherEngine/Core/Service.hpp>

#include <memory>
#include <map>

namespace GopherEngine
{
    enum class ShaderType
    {
        Unlit,
        BlinnPhong,
    };

    class ShaderLoader: public Service<ShaderLoader>
    {
        public:
            std::shared_ptr<ShaderProgram> load_shader(ShaderType shader_type, unsigned int material_ubo_size);
            void release_shader(ShaderType shader_type);
            void release_all_shaders();

        private:
            std::map <ShaderType, std::shared_ptr<ShaderProgram>> shader_cache_;
    };
}