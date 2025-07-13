#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

namespace multiproc {

using RawIndex_t = std::uint64_t;
using AtomicIndex_t = std::atomic<RawIndex_t>;

inline constexpr std::uint64_t CACHE_LINE_SIZE_BYTES = 64;
inline constexpr RawIndex_t INITIAL_INDEX = 0;


struct alignas(CACHE_LINE_SIZE_BYTES) ConsumerState {
    AtomicIndex_t read_index = INITIAL_INDEX;
    std::byte padding[CACHE_LINE_SIZE_BYTES - sizeof(AtomicIndex_t)]; // Pad to prevent false sharing
};

static_assert(sizeof(ConsumerState) == CACHE_LINE_SIZE_BYTES, "ConsumerState must be 64 bytes");


struct alignas(CACHE_LINE_SIZE_BYTES) ProducerState {
    AtomicIndex_t write_index = INITIAL_INDEX;
    std::byte padding[CACHE_LINE_SIZE_BYTES - sizeof(AtomicIndex_t)]; // Pad to prevent false sharing
};

static_assert(sizeof(ProducerState) == CACHE_LINE_SIZE_BYTES, "ProducerState must be 64 bytes");


class alignas(CACHE_LINE_SIZE_BYTES) SharedMemoryRingBuffer {
public:
    static constexpr std::uint64_t MAX_CONSUMERS = 64;
    static constexpr std::uint64_t BUFFER_SIZE_BYTES = 1024;

    SharedMemoryRingBuffer() = default;

    SharedMemoryRingBuffer(const SharedMemoryRingBuffer&) = delete;
    SharedMemoryRingBuffer(SharedMemoryRingBuffer&&) = delete;

    SharedMemoryRingBuffer& operator=(const SharedMemoryRingBuffer&) = delete;
    SharedMemoryRingBuffer& operator=(SharedMemoryRingBuffer&&) = delete;

private:
    ProducerState state_;
    ConsumerState consumers_[MAX_CONSUMERS];
    std::byte raw_buffer_[BUFFER_SIZE_BYTES];
};

} // namespace multiproc
