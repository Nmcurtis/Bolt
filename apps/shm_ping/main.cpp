#include <multiproc/Producer.hpp>
#include <multiproc/SharedMemoryRingBuffer.hpp>

#include <string>

using namespace multiproc;

namespace {
    struct Ping {
        char message[5] = "ping";
    };
}

int main() {
    // Map a buffer
    std::string shm_name = "shm_ping";
    SharedMemoryRingBuffer buffer = SharedMemoryRingBuffer::create(shm_name);

    // Create a producer
    Producer producer(buffer);

    // Write data
    Ping ping;
    producer.write(&ping);
}