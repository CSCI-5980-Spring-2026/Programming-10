#include "UNLIT_VERT_SHADER.h"
#include "UNLIT_FRAG_SHADER.h"
#include "BLINN_PHONG_VERT_SHADER.h"
#include "BLINN_PHONG_FRAG_SHADER.h"

#include <GopherEngine/Renderer/ShaderLoader.hpp>

#include <stdexcept>
using namespace std;

namespace GopherEngine
{
    shared_ptr<ShaderProgram> ShaderLoader::load_shader(ShaderType shader_type, unsigned int material_ubo_size)
    {
        switch(shader_type)
        {
            case ShaderType::Unlit:
            {
                if(shader_cache_.find(shader_type) != shader_cache_.end())
                    return shader_cache_[shader_type];

                auto shader = make_shared<ShaderProgram>(UNLIT_VERT_SHADER, UNLIT_FRAG_SHADER, material_ubo_size);
                shader_cache_[shader_type] = shader;
                return shader;
            }

            case ShaderType::BlinnPhong:
            {
                if(shader_cache_.find(shader_type) != shader_cache_.end())
                    return shader_cache_[shader_type];

                auto shader = make_shared<ShaderProgram>(BLINN_PHONG_VERT_SHADER, BLINN_PHONG_FRAG_SHADER, material_ubo_size);
                shader_cache_[shader_type] = shader;
                return shader;
            }

            default:
                throw runtime_error("Unknown shader type");
        }
    }

    void ShaderLoader::release_shader(const ShaderType shader_type)
    {
        shader_cache_.erase(shader_type);
    }

    void ShaderLoader::release_all_shaders()
    {
        shader_cache_.clear();
    }
}