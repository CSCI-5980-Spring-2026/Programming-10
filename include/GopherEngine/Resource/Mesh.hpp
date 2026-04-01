#pragma once

#include <GopherEngine/Resource/Geometry.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace GopherEngine {
    struct Mesh : public Geometry {
    
        Mesh() { 
            type_ = GeometryType::Mesh; 

            // 3 positions, 3 normals, 4 colors, 2 uvs = 12 floats per vertex
            array_stride_ = 12; 
        }

    };
}
