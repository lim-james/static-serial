#include "stse/stse.hpp"

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
    auto write = [&]<typename T>(const T& member) {
        auto member_size = sizeof(T);
        std::memcpy(buffer.data(), &member, member_size);
        buffer = buffer.subspan(member_size);
    };

    write(entry.timestamp_ns);
    write(entry.symbol);

    for (const auto& bid: entry.bids) {
        write(bid.price.mantissa);
        write(bid.price.exponent);
        write(bid.quantity);
        write(bid.order_count);
    }

    for (const auto& ask: entry.asks) {
        write(ask.price.mantissa);
        write(ask.price.exponent);
        write(ask.quantity);
        write(ask.order_count);
    }

    return buffer;
}

template<>
std::span<const std::byte> deserialize<entry_t>(std::span<const std::byte> buffer, entry_t& entry) {
    auto read = [&]<typename T>(T& member) {
        auto member_size = sizeof(T);
        std::memcpy(&member, buffer.data(), member_size);
        buffer = buffer.subspan(member_size);
    };

    read(entry.timestamp_ns);
    read(entry.symbol);

    for (auto& bid: entry.bids) {
        read(bid.price.mantissa);
        read(bid.price.exponent);
        read(bid.quantity);
        read(bid.order_count);
    }

    for (auto& ask: entry.asks) {
        read(ask.price.mantissa);
        read(ask.price.exponent);
        read(ask.quantity);
        read(ask.order_count);
    }

    return buffer;
}

int main() {
    static constexpr std::size_t ENTRY_SIZE = stse::serial_size_v<entry_t>;
    static constexpr std::size_t BUFFER_SIZE = NUMBER_OF_ENTRIES * ENTRY_SIZE;

    std::vector<entry_t> entries;
    entries.reserve(NUMBER_OF_ENTRIES);
    std::generate_n(std::back_inserter(entries), NUMBER_OF_ENTRIES, []() {
        return native::generate_random_snapshot("APPL");
    });

    {
        auto file = file_guard("handrolled_market_data.bin", BUFFER_SIZE);
        auto addr = mmap_guard(file(), BUFFER_SIZE);
        auto buffer = std::span<std::byte>(static_cast<std::byte*>(addr()), BUFFER_SIZE);

        const auto start_timer = std::chrono::steady_clock::now();
        for (const auto& entry: entries) buffer = serialize(buffer, entry);
        const auto end_timer = std::chrono::steady_clock::now();

        const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_timer - start_timer);
        std::println("Serialize: {}", duration_ms);
    }

    {
        auto file = file_guard("handrolled_market_data.bin", BUFFER_SIZE);
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
