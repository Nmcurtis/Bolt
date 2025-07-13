#pragma once

#include <cstdint>
#include <string_view>

#include <multiproc/Layout.hpp>

namespace multiproc {

class RingBuffer {
public:
    ~RingBuffer() noexcept;

    // Copying disallowed
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;

    // Moving allowed
    RingBuffer(RingBuffer&& other) noexcept;
    RingBuffer& operator=(RingBuffer&& other) noexcept;

    [[nodiscard]] static RingBuffer create(std::string_view shm_name);
    [[nodiscard]] static RingBuffer attach(std::string_view shm_name);

private:
    struct SharedMemoryState {
        static constexpr size_t INVALID_SIZE_BYTES = 0;
        static constexpr int INVALID_FILE_DESCRIPTOR = -1;

        size_t shm_size_bytes = INVALID_SIZE_BYTES;
        int shm_file_descriptor = INVALID_FILE_DESCRIPTOR;
    };

private:
    RingBuffer(SharedMemoryRingBuffer* buffer, size_t shm_size_bytes, int shm_file_descriptor);

    void detach();

private:
    SharedMemoryRingBuffer* buffer_ = nullptr;
    SharedMemoryState state_;
};

} // namespace multiproc
