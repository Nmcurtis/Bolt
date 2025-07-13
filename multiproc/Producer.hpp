#pragma once

#include <atomic>
#include <concepts>
#include <cstring>

#include <multiproc/SharedMemoryRingBuffer.hpp>

namespace multiproc {

template <typename T>
concept WriteableType = std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>;


inline constexpr RawIndex_t WRAP_AROUND_WRITE_OFFSET = 0;

struct alignas(8) MessageHeader {
    std::uint64_t absolute_write_index;
    std::uint32_t payload_size_in_bytes;
} __attribute__((packed));

class Producer {
public:
    explicit Producer(SharedMemoryRingBuffer& ring_buffer)
        : buffer_(ring_buffer.buffer()) {}

    template <WriteableType T>
    void write(const T* data) {
        // The singular producer is the only writer of the write index, so no ordering guarantees required
        const auto absolute_write_index = buffer_->state_.absolute_write_index.load(std::memory_order_relaxed);

        // Compute write offset
        auto write_offset = absolute_write_index % RingBuffer::BUFFER_SIZE_BYTES;
        constexpr auto message_size_in_bytes = sizeof(MessageHeader) + sizeof(T);

        if (write_offset + message_size_in_bytes > RingBuffer::BUFFER_SIZE_BYTES) {
            // Wrap
            write_offset = WRAP_AROUND_WRITE_OFFSET;
        }

        // Prepare header
        MessageHeader header{
            .absolute_write_index = absolute_write_index,
            .payload_size_in_bytes = sizeof(T)
        };

        // Write header and payload
        std::memcpy(buffer_->raw_buffer_ + write_offset, &header, sizeof(header));
        std::memcpy(buffer_->raw_buffer_ + write_offset + sizeof(header), data, sizeof(T));

        // Need to flush the above buffer writes before updating the write index, hence memory order release
        buffer_->state_.absolute_write_index.store(absolute_write_index + message_size_in_bytes, std::memory_order_release);
    }

private:
    RingBuffer* buffer_;

};

static_assert(sizeof(MessageHeader) == 16, "MessageHeader must be 16 bytes");

} // namespace multiproc