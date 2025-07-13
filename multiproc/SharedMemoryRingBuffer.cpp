#include "SharedMemoryRingBuffer.hpp"

#include <cstring>
#include <fcntl.h>
#include <stdexcept>
#include <sys/mman.h>
#include <unistd.h>

namespace multiproc {

// Permissions for shared memory files: rw-rw-rw-
//
// This allows any process to read/write the shared memory
constexpr int SHM_PERMISSIONS = 0666;

constexpr off_t MMAP_OFFSET = 0;

constexpr int FTRUNCATE_FAILURE_INDICATOR = -1;
constexpr int SHM_UNLINK_SUCCESS_INDICATOR = 0;


SharedMemoryRingBuffer::~SharedMemoryRingBuffer() noexcept {
    detach();
}

SharedMemoryRingBuffer::SharedMemoryRingBuffer(SharedMemoryRingBuffer&& other) noexcept
    : buffer_{other.buffer_}
    , state_{other.state_} {

    other.buffer_ = nullptr;
    other.state_ = SharedMemoryState{};
}

SharedMemoryRingBuffer& SharedMemoryRingBuffer::operator=(SharedMemoryRingBuffer&& other) noexcept {
    if (this != &other) {
        detach();

        // Setup this
        buffer_ = other.buffer_;
        state_ = other.state_;

        // Nullify other
        other.buffer_ = nullptr;
        other.state_ = SharedMemoryState{};
    }
    return *this;
}

SharedMemoryRingBuffer SharedMemoryRingBuffer::create(std::string_view shm_name) {
    // Create or open a shared memory region with read/write permissions
    int shm_file_descriptor = shm_open(shm_name.data(), O_CREAT | O_RDWR, SHM_PERMISSIONS);

    if (shm_file_descriptor == SharedMemoryState::INVALID_FILE_DESCRIPTOR) {
        throw std::runtime_error("Call to shm_open failed when creating shared memory");
    }

    // Resize the shared memory region to the size (in bytes) of the buffer
    //
    // Without this call to ftruncate, mmap would fail because the shared memory region
    // has a size of zero by default
    const size_t shm_size_bytes = sizeof(RingBuffer);

    if (ftruncate(shm_file_descriptor, shm_size_bytes) == FTRUNCATE_FAILURE_INDICATOR) {
        close(shm_file_descriptor);
        throw std::runtime_error("Call to ftruncate failed when creating shared memory");
    }

    // Map the shared memory region to this process's address space
    void* buffer_address = mmap(
        nullptr, shm_size_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, shm_file_descriptor, MMAP_OFFSET);

    if (buffer_address == MAP_FAILED) {
        close(shm_file_descriptor);
        throw std::runtime_error("Call to mmap failed when creating shared memory");
    }

    // For safety, placement new the buffer
    new (buffer_address) RingBuffer{};

    return SharedMemoryRingBuffer(
        static_cast<RingBuffer*>(buffer_address),
        shm_size_bytes, shm_file_descriptor);
}

SharedMemoryRingBuffer SharedMemoryRingBuffer::attach(std::string_view shm_name) {
    // Open an existing shared memory region
    int shm_file_descriptor = shm_open(shm_name.data(), O_RDWR, SHM_PERMISSIONS);

    if (shm_file_descriptor == SharedMemoryState::INVALID_FILE_DESCRIPTOR) {
        throw std::runtime_error("shm_open failed when attaching to shared memory");
    }

    // Map the shared memory into this process's address space
    const size_t shm_size_bytes = sizeof(RingBuffer);

    void* buffer_address = mmap(
        nullptr, shm_size_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, shm_file_descriptor, MMAP_OFFSET);

    if (buffer_address == MAP_FAILED) {
        close(shm_file_descriptor);
        throw std::runtime_error("mmap failed when attaching to shared memory");
    }

    return SharedMemoryRingBuffer(
        static_cast<RingBuffer*>(buffer_address),
        shm_size_bytes,
        shm_file_descriptor
    );
}

void SharedMemoryRingBuffer::destroy(std::string_view shm_name) {
    if (shm_unlink(shm_name.data()) != SHM_UNLINK_SUCCESS_INDICATOR) {
        throw std::runtime_error("shm_unlink failed when destroying shared memory");
    }
}

SharedMemoryRingBuffer::SharedMemoryRingBuffer(RingBuffer* buffer, size_t shm_size_bytes, int shm_file_descriptor)
    : buffer_{buffer}
    , state_{.shm_size_bytes = shm_size_bytes, .shm_file_descriptor = shm_file_descriptor} {}

void SharedMemoryRingBuffer::detach() {
    if (buffer_ != nullptr) {
        // Unmap buffer memory from this process
        munmap(static_cast<void*>(buffer_), state_.shm_size_bytes);
    }
    if (state_.shm_file_descriptor != SharedMemoryState::INVALID_FILE_DESCRIPTOR) {
        // Close buffer shm file descriptor
        close(state_.shm_file_descriptor);
    }
}

} // namespace multiproc
