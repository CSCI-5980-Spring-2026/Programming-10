#include <GopherEngine/Renderer/ShaderProgram.hpp>

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <stdexcept>

namespace GopherEngine {

    ShaderProgram::ShaderProgram(const std::string& vert_src, const std::string& frag_src, const unsigned int material_ubo_size)
    {
        unsigned int vert = compile_shader(GL_VERTEX_SHADER, vert_src);
        unsigned int frag = compile_shader(GL_FRAGMENT_SHADER, frag_src);

        program_id_ = glCreateProgram();
        glAttachShader(program_id_, vert);
        glAttachShader(program_id_, frag);
        glLinkProgram(program_id_);

        glDeleteShader(vert);
        glDeleteShader(frag);

        GLint success;
        glGetProgramiv(program_id_, GL_LINK_STATUS, &success);
        if(!success)
        {
            char log[512];
            glGetProgramInfoLog(program_id_, 512, nullptr, log);
            throw std::runtime_error(std::string("Shader link error: ") + log);
        }

        // Matrices UBO (binding = 0): 5 * mat4 + (eye_position + padding)
        glGenBuffers(1, &matrices_ubo_);
        glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo_);
        glBufferData(GL_UNIFORM_BUFFER, 96 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

        // Material UBO (binding = 1): depends on the material/shader
        glGenBuffers(1, &material_ubo_);
        glBindBuffer(GL_UNIFORM_BUFFER, material_ubo_);
        glBufferData(GL_UNIFORM_BUFFER, material_ubo_size, nullptr, GL_DYNAMIC_DRAW);

        // Lights UBO (binding = 2): MAX_LIGHTS * LightUBO (16) + (num_lights + padding)(4)
        glGenBuffers(1, &lights_ubo_);
        glBindBuffer(GL_UNIFORM_BUFFER, lights_ubo_);
        glBufferData(GL_UNIFORM_BUFFER, (MAX_LIGHTS * 16 + 4) * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    ShaderProgram::~ShaderProgram()
    {
        glDeleteBuffers(1, &matrices_ubo_);
        glDeleteBuffers(1, &material_ubo_);
        glDeleteBuffers(1, &lights_ubo_);
        
        if(program_id_ != 0)
            glDeleteProgram(program_id_);
    }

    ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept : program_id_(other.program_id_)
    {
        other.program_id_ = 0;  // Prevent double-delete
    }

    ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
    {
        if(this != &other)
        {
            if(program_id_ != 0)
                glDeleteProgram(program_id_);
            program_id_ = other.program_id_;
            other.program_id_ = 0;
        }
        return *this;
    }

    void ShaderProgram::bind() const
    {
        glUseProgram(program_id_);
    }

    void ShaderProgram::unbind() const
    {
        glUseProgram(0);
    }

    unsigned int ShaderProgram::get_id() const
    {
        return program_id_;
    }

    unsigned int ShaderProgram::get_matrices_ubo() const 
    { 
        return matrices_ubo_; 
    }

    unsigned int ShaderProgram::get_material_ubo() const 
    { 
        return material_ubo_; 
    }

    unsigned int ShaderProgram::get_lights_ubo() const 
    { 
        return lights_ubo_; 
    }

    unsigned int ShaderProgram::compile_shader(unsigned int type, const std::string& src)
    {
        GLuint shader = glCreateShader(type);
        const char* src_cstr = src.c_str();
        glShaderSource(shader, 1, &src_cstr, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            char log[512];
            glGetShaderInfoLog(shader, 512, nullptr, log);
            throw std::runtime_error(std::string("Shader compile error: ") + log);
        }

        return shader;
    }
}