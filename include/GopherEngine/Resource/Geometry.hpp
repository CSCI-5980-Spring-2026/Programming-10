#pragma once

#include <GopherEngine/Resource/Resource.hpp>

#include <functional>

namespace GopherEngine {

    enum class GeometryType {
        None,
        Mesh
    };

    struct Geometry: public Resource {

        // Type of geometry resource, used for safe downcasting in the renderer
        GeometryType type_{GeometryType::None};

        // CPU-side data (generic flat buffers)
        std::vector<float> array_buffer_;
        std::vector<unsigned int> element_buffer_;

        // Number of array elements per vertex, used for interleaving vertex attributes into a single buffer
        unsigned int array_stride_{0};

        // GPU-side handles (mutable: populated lazily by the renderer)
        mutable unsigned int vao_{0};
        mutable unsigned int vbo_{0};
        mutable unsigned int ebo_{0};
        mutable std::function<void()> gpu_release_{0};
        mutable bool pending_upload_{false};

        // Destructor to clean up GPU resources if they were allocated
        ~Geometry()
        {
            if(gpu_release_) gpu_release_();
        }

        // Make the resource non-copyable but moveable to avoid accidental double release of GPU resources
        Geometry() = default;
        Geometry(const Geometry&) = delete;
        Geometry& operator=(const Geometry&) = delete;
        Geometry(Geometry&&) = default;
        Geometry& operator=(Geometry&&) = default;

    };
}