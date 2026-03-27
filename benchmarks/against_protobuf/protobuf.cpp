#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

// Generated from protoc
#include "market_data.pb.h" 
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

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

void populate_proto_snapshot(MarketSnapshot* msg, const char* symbol) {
    static thread_local std::mt19937_64 engine(std::random_device{}());
    std::uniform_int_distribution<std::uint64_t> ts_dist(1711540000000000000ULL, 1711550000000000000ULL);
    std::uniform_int_distribution<std::int64_t> price_dist(10000, 50000);

    msg->set_timestamp_ns(ts_dist(engine));
    msg->set_symbol(symbol);

    std::int64_t mid_price = price_dist(engine);

    for (int i = 0; i < 5; ++i) {
        auto* b = msg->add_bids();
        b->mutable_price()->set_mantissa(mid_price - (i + 1));
        b->mutable_price()->set_exponent(-2);
        b->set_quantity(100);
        b->set_order_count(5);

        auto* a = msg->add_asks();
        a->mutable_price()->set_mantissa(mid_price + (i + 1));
        a->mutable_price()->set_exponent(-2);
        a->set_quantity(100);
        a->set_order_count(5);
    }
}

int main() {
    static constexpr std::size_t N = 1'000'000;
    // Note: PB size is variable. We'll over-allocate for the benchmark.
    static constexpr std::size_t EST_BUFFER_SIZE = N * 512; 

    std::vector<MarketSnapshot> entries(N);
    for(auto& m : entries) populate_proto_snapshot(&m, "AAPL");

    // SERIALIZATION BENCHMARK
    {
        file_guard file("pb_data.bin", EST_BUFFER_SIZE);
        mmap_guard addr(file(), EST_BUFFER_SIZE);
        
        google::protobuf::io::ArrayOutputStream array_stream(addr(), EST_BUFFER_SIZE);
        google::protobuf::io::CodedOutputStream coded_stream(&array_stream);

        for (const auto& msg : entries) {
            // Write size tag first (required for delimited/streaming messages)
            coded_stream.WriteVarint32(msg.ByteSizeLong());
            msg.SerializeToCodedStream(&coded_stream);
        }
    }

    // DESERIALIZATION BENCHMARK
    {
        file_guard file("pb_data.bin");
        mmap_guard addr(file(), EST_BUFFER_SIZE);

        google::protobuf::io::ArrayInputStream array_stream(addr(), EST_BUFFER_SIZE);
        google::protobuf::io::CodedInputStream coded_stream(&array_stream);

        for (std::size_t i = 0; i < N; ++i) {
            uint32_t size;
            if (!coded_stream.ReadVarint32(&size)) break;

            MarketSnapshot restored;
            auto limit = coded_stream.PushLimit(size);
            restored.ParseFromCodedStream(&coded_stream);
            coded_stream.PopLimit(limit);
            
            // assert(restored.timestamp_ns() == entries[i].timestamp_ns());
        }
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
