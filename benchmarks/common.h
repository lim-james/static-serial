#pragma once

#include <optional>
#include <filesystem>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

class file_guard {
public:
    file_guard(
        std::filesystem::path filepath, 
        std::optional<std::size_t> filesize = std::nullopt,
        int flags = O_RDWR | O_CREAT
    ) {
        fd_ = open(filepath.c_str(), flags, 0644);
        if (filesize) auto _ = ftruncate(fd_, filesize.value());
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

static constexpr std::size_t NUMBER_OF_ENTRIES = 1'000'000;
