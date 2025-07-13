#pragma once

#include <cstdint>
#include <string_view>

#include <multiproc/Layout.hpp>

namespace multiproc {

class SharedMemoryRingBuffer {
public:
    ~SharedMemoryRingBuffer() noexcept;

    // Copying disallowed
    SharedMemoryRingBuffer(const SharedMemoryRingBuffer&) = delete;
    SharedMemoryRingBuffer& operator=(const SharedMemoryRingBuffer&) = delete;

    // Moving allowed
    SharedMemoryRingBuffer(SharedMemoryRingBuffer&& other) noexcept;
    SharedMemoryRingBuffer& operator=(SharedMemoryRingBuffer&& other) noexcept;

    RingBuffer* buffer() { return buffer_; }

    size_t size_in_bytes() const { return state_.shm_size_bytes; }

    [[nodiscard]] static SharedMemoryRingBuffer create(std::string_view shm_name);

    [[nodiscard]] static SharedMemoryRingBuffer attach(std::string_view shm_name);
    
    static void destroy(std::string_view shm_name);

private:
    struct SharedMemoryState {
        static constexpr size_t INVALID_SIZE_BYTES = 0;
        static constexpr int INVALID_FILE_DESCRIPTOR = -1;

        size_t shm_size_bytes = INVALID_SIZE_BYTES;
        int shm_file_descriptor = INVALID_FILE_DESCRIPTOR;
    };

private:
    SharedMemoryRingBuffer(RingBuffer* buffer, size_t shm_size_bytes, int shm_file_descriptor);

    void detach();

private:
    RingBuffer* buffer_ = nullptr;
    SharedMemoryState state_;
};

} // namespace multiproc
