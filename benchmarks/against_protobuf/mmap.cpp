#include "static_serial.hpp"

#include <iterator>
#include <optional>
#include <span>
#include <cassert>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <vector> 
#include <random>
#include <algorithm>
#include <string_view>

#include "market_data.h"

MarketSnapshot generate_random_snapshot(const char* symbol_str) {
    // Use thread_local to avoid re-seeding and for thread safety in concurrent environments
    static thread_local std::mt19937_64 engine(std::random_device{}());
    
    // Define distributions for various fields
    std::uniform_int_distribution<std::uint64_t> ts_dist(1711540000000000000ULL, 1711550000000000000ULL);
    std::uniform_int_distribution<std::int64_t> price_dist(10000, 50000); // e.g., $100.00 to $500.00
    std::uniform_int_distribution<std::uint64_t> qty_dist(1, 1000);
    std::uniform_int_distribution<std::uint32_t> count_dist(1, 20);

    MarketSnapshot snapshot;
    snapshot.timestamp_ns = ts_dist(engine);
    snapshot.symbol = make_ticker(symbol_str);

    // Starting midpoint for a realistic spread
    std::int64_t mid_price = price_dist(engine);
    std::int8_t common_exponent = -2; // Standard 2 decimal places

    for (std::size_t i = 0; i < 5; ++i) {
        // Bids: Prices go down as we move deeper into the book
        snapshot.bids[i] = {
            .price = {mid_price - static_cast<std::int64_t>(i + 1), common_exponent},
            .quantity = qty_dist(engine),
            .order_count = count_dist(engine)
        };

        // Asks: Prices go up as we move deeper into the book
        snapshot.asks[i] = {
            .price = {mid_price + static_cast<std::int64_t>(i + 1), common_exponent},
            .quantity = qty_dist(engine),
            .order_count = count_dist(engine)
        };
    }

    return snapshot;
}

class file_guard {
public:
    file_guard(
        std::filesystem::path filepath, 
        std::optional<std::size_t> filesize = std::nullopt,
        int flags = O_RDWR | O_CREAT
    ) {
        fd_ = open(filepath.c_str(), flags, 0644);
        if (filesize) ftruncate(fd_, filesize.value());
    }

    ~file_guard() { if (fd_ != -1) close(fd_); }
    int operator()() const { return fd_; }
private:
    int fd_ = -1;
};

class mmap_guard {
public:
    mmap_guard(int fd, std::size_t size) {
        addr_ = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    }

    ~mmap_guard() { if (addr_ != MAP_FAILED) munmap(addr_, size_); }
    void* operator()() const { return addr_; }

private:
    std::size_t size_ = 0;
    void* addr_ = MAP_FAILED;
};

int main() {
    using entry_t = MarketSnapshot;

    static constexpr std::size_t NUMBER_OF_ENTRIES = 1'000'000;
    static constexpr std::size_t ENTRY_SIZE = sizeof(entry_t);
    static constexpr std::size_t BUFFER_SIZE = NUMBER_OF_ENTRIES * ENTRY_SIZE;

    std::vector<entry_t> entries;
    entries.reserve(NUMBER_OF_ENTRIES);
    std::generate_n(std::back_inserter(entries), NUMBER_OF_ENTRIES, []() {
        return generate_random_snapshot("APPL");
    });

    {
        auto file = file_guard("market_data.bin", BUFFER_SIZE);
        auto addr = mmap_guard(file(), BUFFER_SIZE);
        auto buffer = std::span<std::byte>(static_cast<std::byte*>(addr()), BUFFER_SIZE);

        for (const auto& entry: entries) buffer = stse::serialize_into(entry, buffer);
    }

    {

        auto file = file_guard("market_data.bin", BUFFER_SIZE);
        auto addr = mmap_guard(file(), BUFFER_SIZE);
        auto buffer = std::span<const std::byte>(static_cast<std::byte*>(addr()), BUFFER_SIZE);

        for (const auto& entry: entries) {
            auto [restored, offset_buffer] = stse::deserialize<entry_t>(buffer);
            buffer = offset_buffer;
            assert(entry == restored);
        }
    }
}
