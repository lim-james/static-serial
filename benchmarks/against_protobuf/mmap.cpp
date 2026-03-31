#include "static_serial.hpp"

#include <chrono>
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
#include <print>

#include "market_data.h"

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
    mmap_guard(int fd, std::size_t size):  size_(size) {
        addr_ = mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    }

    ~mmap_guard() { if (addr_ != MAP_FAILED) munmap(addr_, size_); }
    void* operator()() const { return addr_; }

private:
    const std::size_t size_;
    void* addr_ = MAP_FAILED;
};

int main() {
    using entry_t = native::MarketSnapshot;

    static constexpr std::size_t NUMBER_OF_ENTRIES = 1'000'000;
    static constexpr std::size_t ENTRY_SIZE = sizeof(entry_t);
    static constexpr std::size_t BUFFER_SIZE = NUMBER_OF_ENTRIES * ENTRY_SIZE;

    std::vector<entry_t> entries;
    entries.reserve(NUMBER_OF_ENTRIES);
    std::generate_n(std::back_inserter(entries), NUMBER_OF_ENTRIES, []() {
        return native::generate_random_snapshot("APPL");
    });

    {
        auto file = file_guard("market_data.bin", BUFFER_SIZE);
        auto addr = mmap_guard(file(), BUFFER_SIZE);
        auto buffer = std::span<std::byte>(static_cast<std::byte*>(addr()), BUFFER_SIZE);

        const auto start_timer = std::chrono::steady_clock::now();
        for (const auto& entry: entries) buffer = stse::serialize_into(entry, buffer);
        const auto end_timer = std::chrono::steady_clock::now();

        const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_timer - start_timer);
        std::println("Serialize: {}", duration_ms);
    }

    {

        auto file = file_guard("market_data.bin", BUFFER_SIZE);
        auto addr = mmap_guard(file(), BUFFER_SIZE);
        auto buffer = std::span<const std::byte>(static_cast<std::byte*>(addr()), BUFFER_SIZE);

        const auto start_timer = std::chrono::steady_clock::now();
        for (const auto& entry: entries) {
            auto [restored, offset_buffer] = stse::deserialize<entry_t>(buffer);
            buffer = offset_buffer;
            assert(entry == restored);
        }
        const auto end_timer = std::chrono::steady_clock::now();

        const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_timer - start_timer);
        std::println("Deserialize: {}", duration_ms);
    }
}
