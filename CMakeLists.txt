cmake_minimum_required(VERSION 3.28)
project(GopherEngine LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
    glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG glew-cmake-2.3.1
)
FetchContent_MakeAvailable(glew)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 8d1fd52e5ab5590e2c81768ace50c72bae28f2ed #refs/tags/1.0.3
)
FetchContent_MakeAvailable(glm)

# Ensure GLVND is used for Bullet on Linux to avoid conflicts with SFML's OpenGL dependencies
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()
set(OpenGL_GL_PREFERENCE GLVND)

FetchContent_Declare(bullet
    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
    GIT_TAG d1a4256b3a019117f2bb6cb8c63d6367aaf512e2
)
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
set(BUILD_BULLET3 OFF CACHE BOOL "" FORCE)
set(BUILD_PYBULLET OFF CACHE BOOL "" FORCE)
set(BUILD_ENET OFF CACHE BOOL "" FORCE)
set(BUILD_CLSOCKET OFF CACHE BOOL "" FORCE)
set(INSTALL_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(bullet)

# Core library
add_library(GopherEngineCore)
target_compile_features(GopherEngineCore PUBLIC cxx_std_20)
target_include_directories(GopherEngineCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE GOPHERENGINECORE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GopherEngine/Core/*.cpp
)
target_sources(GopherEngineCore PRIVATE ${GOPHERENGINECORE_SOURCES})
target_link_libraries(GopherEngineCore PRIVATE SFML::Window)
target_link_libraries(GopherEngineCore PRIVATE libglew_static)
target_link_libraries(GopherEngineCore PRIVATE glm::glm)

# Platform independence library
add_library(GopherEnginePlatform)
target_compile_features(GopherEnginePlatform PUBLIC cxx_std_20)
target_include_directories(GopherEnginePlatform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE GOPHERENGINEPLATFORM_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GopherEngine/Platform/*.cpp
)
target_sources(GopherEnginePlatform PRIVATE ${GOPHERENGINEPLATFORM_SOURCES})
target_link_libraries(GopherEnginePlatform PRIVATE SFML::Window)
target_link_libraries(GopherEnginePlatform PRIVATE glm::glm)

# Renderer library
add_library(GopherEngineRenderer)
target_compile_features(GopherEngineRenderer PUBLIC cxx_std_20)
target_include_directories(GopherEngineRenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE GOPHERENGINERENDERER_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GopherEngine/Renderer/*.cpp
)
target_sources(GopherEngineRenderer PRIVATE ${GOPHERENGINERENDERER_SOURCES})
target_link_libraries(GopherEngineRenderer PRIVATE SFML::Window)
target_link_libraries(GopherEngineRenderer PRIVATE libglew_static)
target_link_libraries(GopherEngineRenderer PRIVATE glm::glm)

# Shaders
function(embed_shader TARGET SHADER_FILE VAR_NAME)
    file(READ ${SHADER_FILE} SHADER_CONTENT)
    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/shader_template.h.in
        ${CMAKE_BINARY_DIR}/generated/${VAR_NAME}.h
        @ONLY
    )
    target_include_directories(${TARGET} PRIVATE ${CMAKE_BINARY_DIR}/generated)
    # Tell CMake to reconfigure when the shader file changes
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SHADER_FILE})
endfunction()
embed_shader(GopherEngineRenderer ${CMAKE_CURRENT_SOURCE_DIR}/src/GopherEngine/Renderer/shaders/unlit.vert UNLIT_VERT_SHADER)
embed_shader(GopherEngineRenderer ${CMAKE_CURRENT_SOURCE_DIR}/src/GopherEngine/Renderer/shaders/unlit.frag UNLIT_FRAG_SHADER)
embed_shader(GopherEngineRenderer ${CMAKE_CURRENT_SOURCE_DIR}/src/GopherEngine/Renderer/shaders/blinnphong.vert BLINN_PHONG_VERT_SHADER)
embed_shader(GopherEngineRenderer ${CMAKE_CURRENT_SOURCE_DIR}/src/GopherEngine/Renderer/shaders/blinnphong.frag BLINN_PHONG_FRAG_SHADER)

# Resource library
add_library(GopherEngineResource)
target_compile_features(GopherEngineResource PUBLIC cxx_std_20)
target_include_directories(GopherEngineResource PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE GOPHERENGINE_RESOURCE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GopherEngine/Resource/*.cpp
)
target_sources(GopherEngineResource PRIVATE ${GOPHERENGINE_RESOURCE_SOURCES})
target_link_libraries(GopherEngineResource PRIVATE GopherEngineCore)
target_link_libraries(GopherEngineResource PRIVATE glm::glm)

# Physics library
add_library(GopherEnginePhysics)
target_compile_features(GopherEnginePhysics PUBLIC cxx_std_20)
target_include_directories(GopherEnginePhysics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE GOPHERENGINEPHYSICS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GopherEngine/Physics/*.cpp
)
target_sources(GopherEnginePhysics PRIVATE ${GOPHERENGINEPHYSICS_SOURCES})
target_link_libraries(GopherEnginePhysics PRIVATE BulletDynamics BulletCollision LinearMath)
target_link_libraries(GopherEnginePhysics PRIVATE glm::glm)
target_include_directories(GopherEnginePhysics SYSTEM PRIVATE ${bullet_SOURCE_DIR}/src)

# Test executable
add_executable(MainLoopTest)
target_compile_features(MainLoopTest PRIVATE cxx_std_20)
target_include_directories(MainLoopTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE TESTAPP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MainLoopTest/*.cpp
)
target_sources(MainLoopTest PRIVATE ${TESTAPP_SOURCES})
target_link_libraries(MainLoopTest PRIVATE GopherEngineCore)
target_link_libraries(MainLoopTest PRIVATE GopherEnginePlatform)
target_link_libraries(MainLoopTest PRIVATE GopherEngineRenderer)
target_link_libraries(MainLoopTest PRIVATE GopherEngineResource)
target_link_libraries(MainLoopTest PRIVATE GopherEnginePhysics)
target_link_libraries(MainLoopTest PRIVATE SFML::Window)
target_link_libraries(MainLoopTest PRIVATE libglew_static)
target_link_libraries(MainLoopTest PRIVATE glm::glm)
