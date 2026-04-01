#include <GopherEngine/Core/FileLoader.hpp>

#include <fstream>
#include <stdexcept>

namespace GopherEngine
{

    LoadHandle FileLoader::load_file_async(std::filesystem::path path)
    {
        std::future<FileData> future = std::async(std::launch::async, &FileLoader::load_file, std::move(path));

        return LoadHandle{ future.share() };
    }

    FileData FileLoader::load_file(std::filesystem::path path) {
        FileData result;
        result.path_ = path;

        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file) {
            result.ok_    = false;
            result.error_ = "Could not open file: " + path.string();
            return result;
        }

        const auto size = file.tellg();
        if (size <= 0) {
            result.ok_    = false;
            result.error_ = "File empty or unreadable: " + path.string();
            return result;
        }

        result.bytes_.resize(static_cast<std::size_t>(size));
        file.seekg(0);
        file.read(reinterpret_cast<char*>(result.bytes_.data()), size);

        if (!file) {
            result.ok_    = false;
            result.error_ = "Read failed: " + path.string();
            return result;
        }

        result.ok_ = true;
        return result;
    }
   
}   