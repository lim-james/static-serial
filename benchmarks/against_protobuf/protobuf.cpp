#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cassert>
#include <chrono>
#include <print>

// Generated from protoc — defines ::Price, ::OrderBookLevel, ::MarketSnapshot
#include "market_data.pb.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

// Native structs — now in namespace native:: to avoid collision
#include "market_data.h"

class file_guard {
public:
    file_guard(
        std::filesystem::path filepath,
        std::optional<std::size_t> filesize = std::nullopt,
        int flags = O_RDWR | O_CREAT
    ) {
        fd_ = open(filepath.c_str(), flags, 0644);
        if (filesize) ftruncate(fd_, *filesize);
    }

    ~file_guard() { if (fd_ != -1) close(fd_); }
    int operator()() const { return fd_; }
private:
    int fd_ = -1;
};

class mmap_guard {
public:
    mmap_guard(int fd, std::size_t size) : size_(size) {
        addr_ = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    }

    ~mmap_guard() { if (addr_ != MAP_FAILED) munmap(addr_, size_); }
    void* operator()() const { return addr_; }

private:
    std::size_t size_;
    void* addr_ = MAP_FAILED;
};

// Convert native struct to proto message.
// Both benchmarks generate from native::generate_random_snapshot so the
// underlying data is identical.
MarketSnapshot to_proto(const native::MarketSnapshot& src) {
    MarketSnapshot msg;
    msg.set_timestamp_ns(src.timestamp_ns);
    msg.set_symbol(std::string(src.symbol.data(), src.symbol.size()));

    for (const auto& lvl : src.bids) {
        auto* b = msg.add_bids();
        b->mutable_price()->set_mantissa(lvl.price.mantissa);
        b->mutable_price()->set_exponent(lvl.price.exponent);
        b->set_quantity(lvl.quantity);
        b->set_order_count(lvl.order_count);
    }

    for (const auto& lvl : src.asks) {
        auto* a = msg.add_asks();
        a->mutable_price()->set_mantissa(lvl.price.mantissa);
        a->mutable_price()->set_exponent(lvl.price.exponent);
        a->set_quantity(lvl.quantity);
        a->set_order_count(lvl.order_count);
    }

    return msg;
}

int main() {
    static constexpr std::size_t N               = 1'000'000;
    static constexpr std::size_t EST_BUFFER_SIZE = N * 512;

    // --- Data generation (outside benchmark timing) ---
    std::vector<native::MarketSnapshot> native_entries;
    native_entries.reserve(N);
    std::generate_n(std::back_inserter(native_entries), N, []() {
        return native::generate_random_snapshot("AAPL");
    });

    std::vector<MarketSnapshot> proto_entries;
    proto_entries.reserve(N);
    for (const auto& s : native_entries)
        proto_entries.push_back(to_proto(s));

    // --- Serialization ---
    {
        file_guard file("pb_data.bin", EST_BUFFER_SIZE);
        mmap_guard addr(file(), EST_BUFFER_SIZE);

        google::protobuf::io::ArrayOutputStream  array_stream(addr(), EST_BUFFER_SIZE);
        google::protobuf::io::CodedOutputStream  coded_stream(&array_stream);

        const auto start_timer = std::chrono::steady_clock::now();
        for (const auto& msg : proto_entries) {
            coded_stream.WriteVarint32(msg.ByteSizeLong());
            msg.SerializeToCodedStream(&coded_stream);
        }

        const auto end_timer = std::chrono::steady_clock::now();
        const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_timer - start_timer);
        std::println("Serialize: {}", duration_ms);
    }

    // --- Deserialization ---
    {
        file_guard file("pb_data.bin");
        mmap_guard addr(file(), EST_BUFFER_SIZE);

        google::protobuf::io::ArrayInputStream  array_stream(addr(), EST_BUFFER_SIZE);
        google::protobuf::io::CodedInputStream  coded_stream(&array_stream);

        const auto start_timer = std::chrono::steady_clock::now();
        for (std::size_t i = 0; i < N; ++i) {
            uint32_t size;
            if (!coded_stream.ReadVarint32(&size)) break;

            MarketSnapshot restored;
            auto limit = coded_stream.PushLimit(size);
            restored.ParseFromCodedStream(&coded_stream);
            coded_stream.PopLimit(limit);

            assert(restored.timestamp_ns() == proto_entries[i].timestamp_ns());
            assert(restored.symbol()       == proto_entries[i].symbol());
        }

        const auto end_timer = std::chrono::steady_clock::now();
        const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_timer - start_timer);
        std::println("Deserialize: {}", duration_ms);
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
