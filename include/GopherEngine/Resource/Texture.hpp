#pragma once

#include <GopherEngine/Resource/Resource.hpp>

#include <vector>
#include <functional>

namespace GopherEngine {

    // Holds raw pixel data and GPU handles for a 2D texture.
    struct Texture: public Resource {

        // Pixel format of the raw data stored in pixels_
        enum class Format { RGBA, RGB };

        // CPU-side data
        std::vector<uint8_t> pixels_;
        int width_{0};
        int height_{0};
        Format format_{Format::RGBA};

        // GPU-side handle (populated lazily by renderer)
        mutable unsigned int texture_id_{0};
        mutable std::function<void()> gpu_release_{};
        mutable bool pending_upload_{false};

        // Destructor releases GPU resources if they were allocated
        ~Texture() {
            if (gpu_release_) gpu_release_();
        }

        // Non-copyable to avoid accidental double-release of GPU resources
        Texture() = default;
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&&) = default;
        Texture& operator=(Texture&&) = default;
    };

} 
