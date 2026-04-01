#include <GopherEngine/Renderer/Renderer.hpp>
#include <GopherEngine/Renderer/ShaderLoader.hpp>

#include <GL/glew.h>          // Must come first
#include <SFML/OpenGL.hpp>    // After glew

#include <iostream>
using namespace std;

namespace GopherEngine {
    Renderer::Renderer()
    {

    }

    Renderer::~Renderer()
    {
        
    }

    void Renderer::initialize()
    {
        // This must occur after creating the OpenGL context
        // (i.e. after creating the window and setting it to active)
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();

        if(err != GLEW_OK)
            throw std::runtime_error(std::string("GLEW init failed: ") + reinterpret_cast<const char*>(glewGetErrorString(err)));

        initialized_ = true;

        // Register the shader loader service
        ShaderLoader::provide(&shader_loader_);
        ResourceUploader::provide(&resource_uploader_);

        // Depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_TRUE);
        glClearDepth(1.f);

        // Disable fixed-function lighting because we are using shaders
        glDisable(GL_LIGHTING);

        // Back face culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        // Enable sRGB framebuffer for correct gamma correction when writing colors to the framebuffer
        glEnable(GL_FRAMEBUFFER_SRGB);

        // Background color
        glClearColor(background_color_.r, background_color_.g, background_color_.b, background_color_.a);
    }

    void Renderer::resize_viewport(unsigned int window_width, unsigned int window_height, float camera_aspect_ratio, ViewportMode mode) {

        if(window_width == 0 || window_height == 0) {
            cerr << "Warning: attempted to resize viewport to zero dimensions, skipping" << endl;
            return;
        }

        if(mode == ViewportMode::Fit) {

            float window_aspect_ratio = static_cast<float>(window_width) / static_cast<float>(window_height);

            // Window is wider than target aspect ratio, add pillarboxing
            if(window_aspect_ratio > camera_aspect_ratio) {
                int viewport_width = static_cast<float>(window_height) * camera_aspect_ratio;
                glViewport(
                    (static_cast<int>(window_width) - viewport_width) / 2, 
                    0, 
                    viewport_width, 
                    window_height
                );
            }
            // Window is taller than target aspect ratio, add letterboxing
            else {
                int viewport_height = static_cast<float>(window_width) / camera_aspect_ratio;
                glViewport(
                    0, 
                    (static_cast<int>(window_height) - viewport_height) / 2, 
                    window_width, 
                    viewport_height
                );
            }
        }
        else if(mode == ViewportMode::Crop) {

            float window_aspect_ratio = static_cast<float>(window_width) / static_cast<float>(window_height);
           
            // Window is wider than target aspect ratio, crop the viewport top and bottom
            if(window_aspect_ratio > camera_aspect_ratio) {
                int viewport_height = static_cast<float>(window_width) / camera_aspect_ratio;
                glViewport(
                    0,
                    (viewport_height - static_cast<int>(window_height)) / -2, 
                    static_cast<int>(window_width),
                    viewport_height
                );
            }
            // Window is taller than target aspect ratio, crop the viewport sides
            else {
                int viewport_width = static_cast<float>(window_height) * camera_aspect_ratio;
                glViewport(
                    (viewport_width - static_cast<int>(window_width)) / -2,
                    0,
                    viewport_width,
                    static_cast<int>(window_height)
                );
            }
        }
        else if(mode == ViewportMode::Stretch) {
            // Stretch to fill the entire window, ignoring aspect ratio
            glViewport(
                0, 
                0, 
                static_cast<int>(window_width), 
                static_cast<int>(window_height)
            );
        }
    }

    void Renderer::draw(Scene &scene) {

        // First, upload a portion of any pending resource uploads to the GPU, up to the per-frame budget
        ResourceUploader::get().process_pending_uploads();

        // Clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.draw();
        
    }

    void Renderer::set_background_color(const glm::vec4 &color) {
        background_color_ = color;

        if(initialized_)
            glClearColor(color.r, color.g, color.b, color.a);
    }

    glm::vec4 Renderer::get_background_color() const {
        return background_color_;
    }
}