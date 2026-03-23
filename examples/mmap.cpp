#include "static_serial.hpp"

#include <expected>
#include <string_view>
#include <span>
#include <cassert>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

struct Price {
    std::int64_t mantissa;
    std::int8_t  exponent;
    
    bool operator==(const Price&) const = default;
};

class file_guard {
public:
    file_guard(std::filesystem::path filepath, int flags = O_RDWR | O_CREAT) {
        fd_ = open(filepath.c_str(), flags, 0644);
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
    static constexpr std::size_t message_size = 64;

    auto price_data = Price{.mantissa=12345, .exponent=67};

    {
        auto file = file_guard("market_data.bin");
        ftruncate(file(), message_size);
        auto addr = mmap_guard(file(), message_size);

        auto buffer = std::span<std::byte>(
            static_cast<std::byte*>(addr()),
            message_size
        );

        stse::serialize_into(price_data, buffer);
    }

    {
        auto file = file_guard("market_data.bin");
        ftruncate(file(), message_size);
        auto addr = mmap_guard(file(), message_size);

        auto buffer = std::span<std::byte>(
            static_cast<std::byte*>(addr()),
            message_size
        );
        auto restored = stse::deserialize<Price>(buffer);
        assert(price_data == restored);
    }
}
