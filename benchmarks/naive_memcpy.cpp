#include <cstring>
#include <chrono>
#include <iterator>
#include <cassert>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <vector> 
#include <algorithm>
#include <print>

#include "market_data.h"
#include "common.h"

using entry_t = native::MarketSnapshot;

template<typename T>
std::span<std::byte> serialize(std::span<std::byte>, const T&);

template<typename T>
std::span<const std::byte> deserialize(std::span<const std::byte>, T&);

template<>
std::span<std::byte> serialize<entry_t>(std::span<std::byte> buffer, const entry_t& entry) {
    auto entry_size = sizeof(entry_t);
    std::memcpy(buffer.data(), &entry, entry_size);
    return buffer.subspan(entry_size);
}

template<>
std::span<const std::byte> deserialize<entry_t>(std::span<const std::byte> buffer, entry_t& entry) {
    auto entry_size = sizeof(entry_t);
    std::memcpy(&entry, buffer.data(), entry_size);
    return buffer.subspan(entry_size);
}

int main() {
    static constexpr std::size_t ENTRY_SIZE = sizeof(entry_t);
    static constexpr std::size_t BUFFER_SIZE = NUMBER_OF_ENTRIES * ENTRY_SIZE;

    std::vector<entry_t> entries;
    entries.reserve(NUMBER_OF_ENTRIES);
    std::generate_n(std::back_inserter(entries), NUMBER_OF_ENTRIES, []() {
        return native::generate_random_snapshot("APPL");
    });

    {
        auto file = file_guard("naive_memcpy_market_data.bin", BUFFER_SIZE);
        auto addr = mmap_guard(file(), BUFFER_SIZE);
        auto buffer = std::span<std::byte>(static_cast<std::byte*>(addr()), BUFFER_SIZE);

        const auto start_timer = std::chrono::steady_clock::now();
        for (const auto& entry: entries) buffer = serialize(buffer, entry);
        const auto end_timer = std::chrono::steady_clock::now();

        const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_timer - start_timer);
        std::println("Serialize: {}", duration_ms);
    }

    {
        auto file = file_guard("naive_memcpy_market_data.bin", BUFFER_SIZE);
        auto addr = mmap_guard(file(), BUFFER_SIZE);
        auto buffer = std::span<const std::byte>(static_cast<std::byte*>(addr()), BUFFER_SIZE);

        std::size_t correct{};
        entry_t restored;
        const auto start_timer = std::chrono::steady_clock::now();
        for (const auto& entry: entries) {
            buffer = deserialize(buffer, restored);
            correct += static_cast<std::size_t>(entry == restored);
        }
        const auto end_timer = std::chrono::steady_clock::now();

        const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_timer - start_timer);
        std::println("Deserialize: {}", duration_ms);
        std::println("Correct: {}/{}", correct, NUMBER_OF_ENTRIES);
    }
}
