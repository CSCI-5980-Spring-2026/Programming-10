#pragma once

#include <cassert>

namespace GopherEngine
{
    template <typename T>
    class Service
    {
        public:
            static void provide(T* instance)
            {
                assert(instance_ == nullptr && "Service already registered");
                instance_ = instance;
            }

            static T& get()
            {
                assert(instance_ != nullptr && "Service not registered");
                return *instance_;
            }

            // Delete copy and move constructors and assignment operators
            Service(const Service&)            = delete;
            Service& operator=(const Service&) = delete;
            Service(Service&&)                 = delete;
            Service& operator=(Service&&)      = delete;

        protected:
            Service() = default;
            ~Service() = default;

        private:
            static inline T* instance_ = nullptr;
    };
}