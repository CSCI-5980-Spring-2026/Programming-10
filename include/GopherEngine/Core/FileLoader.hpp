#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <future>
#include <string>
#include <vector>
#include <functional>

namespace GopherEngine
{
    // -------------------------------------------------------------------------
    // FileData
    //   Plain data bag returned when a load completes.  Intentionally simple.
    //   Upper layers are responsible for interpreting the bytes.
    // -------------------------------------------------------------------------
    struct FileData {
        std::filesystem::path path_;         // original request path
        std::vector<std::uint8_t> bytes_;    // raw file contents
        bool ok_{false};                     // false on any I/O error
        std::string error_;                  // human-readable message on failure
    };


    // -------------------------------------------------------------------------
    // LoadHandle
    //   Returned immediately by FileLoader::startLoad().  The caller keeps this
    //   alive, calls isReady() each frame (cheap -- non-blocking), and calls
    //   get() once to take ownership of the FileData on the main thread.
    //
    //   Built on std::shared_future so handles can be freely copied and passed
    //   to multiple systems without either one consuming the result.
    // -------------------------------------------------------------------------
    class LoadHandle {
    public:
        using Callback = std::function<void(const FileData&)>;

        LoadHandle() = default;

        explicit LoadHandle(std::shared_future<FileData> future) : future_(std::move(future)) {}

        // Register a callback to be invoked on the main thread after the load completes.
        // Fluent API: FileLoader::load_file_async(...).on_complete([](auto& d){ ... });
        LoadHandle& on_complete(Callback cb) {
            callback_ = std::move(cb);
            return *this;
        }

        // Returns true if the background thread has finished (success OR error).
        // Safe to call every frame -- never blocks.
        bool is_ready() const {
            return future_.valid() && future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        }

        // Call on the main thread once is_ready() returns true.
        // Invokes the registered callback (if any) with the completed FileData.
        void fire_callback() {
            if (!callback_) 
                return;
            
            callback_(future_.get());
        }

        // Returns true if this handle refers to an in-flight or completed load.
        bool valid() const { 
            return future_.valid(); 
        }

        // Call on the main thread once is_ready() returns true.
        // May be called multiple times (shared_future -- result is not consumed).
        FileData get() const { 
            return future_.get(); 
        }

    private:
        std::shared_future<FileData> future_;
        Callback callback_;
    };
    
    class FileLoader 
    {
        public:

            // Start an asynchronous binary read.  Returns immediately.
            // The returned handle becomes ready when I/O completes (or fails).
            static LoadHandle load_file_async(std::filesystem::path path);

            // Blocking call to read a file on the current thread.
            // Used internally by load_file_async.
            static FileData load_file(std::filesystem::path path);
                
    };
}