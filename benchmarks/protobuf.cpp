#include <vector>
#include <algorithm>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cassert>
#include <chrono>
#include <print>

#include "market_data.pb.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/arena.h>

#include "market_data.h"
#include "common.h"

static constexpr size_t ARENA_INITIAL_BLOCK = 4096;

bool operator==(const Price& lhs, const Price& rhs) {
    return lhs.mantissa() == rhs.mantissa() 
        && lhs.exponent() == rhs.exponent();
}

bool operator==(const OrderBookLevel& lhs, const OrderBookLevel& rhs) {
    return lhs.price()       == rhs.price() 
        && lhs.quantity()    == rhs.quantity()
        && lhs.order_count() == rhs.order_count();
}

bool operator==(const MarketSnapshot& lhs, const MarketSnapshot& rhs) {
    return lhs.timestamp_ns() == rhs.timestamp_ns() 
        && lhs.symbol()       == rhs.symbol()
        && lhs.bids()[0]      == rhs.bids()[0]
        && lhs.bids()[1]      == rhs.bids()[1]
        && lhs.bids()[2]      == rhs.bids()[2]
        && lhs.bids()[3]      == rhs.bids()[3]
        && lhs.bids()[4]      == rhs.bids()[4]
        && lhs.asks()[0]      == rhs.asks()[0]
        && lhs.asks()[1]      == rhs.asks()[1]
        && lhs.asks()[2]      == rhs.asks()[2]
        && lhs.asks()[3]      == rhs.asks()[3]
        && lhs.asks()[4]      == rhs.asks()[4];
}

google::protobuf::ArenaOptions make_arena_opts() {
    google::protobuf::ArenaOptions opts;
    opts.start_block_size = ARENA_INITIAL_BLOCK;
    opts.max_block_size   = ARENA_INITIAL_BLOCK * 4;
    return opts;
}

MarketSnapshot* to_proto_arena(
    google::protobuf::Arena& arena,
    const native::MarketSnapshot& src
) {
    auto* msg = google::protobuf::Arena::CreateMessage<MarketSnapshot>(&arena);
    msg->set_timestamp_ns(src.timestamp_ns);
    msg->set_symbol(std::string(src.symbol.data(), src.symbol.size()));

    for (const auto& lvl : src.bids) {
        auto* b = msg->add_bids();
        b->mutable_price()->set_mantissa(lvl.price.mantissa);
        b->mutable_price()->set_exponent(lvl.price.exponent);
        b->set_quantity(lvl.quantity);
        b->set_order_count(lvl.order_count);
    }

    for (const auto& lvl : src.asks) {
        auto* a = msg->add_asks();
        a->mutable_price()->set_mantissa(lvl.price.mantissa);
        a->mutable_price()->set_exponent(lvl.price.exponent);
        a->set_quantity(lvl.quantity);
        a->set_order_count(lvl.order_count);
    }

    return msg;
}

int main() {
    static constexpr std::size_t EST_BUFFER_SIZE = NUMBER_OF_ENTRIES * 512;

    std::vector<native::MarketSnapshot> native_entries;
    native_entries.reserve(NUMBER_OF_ENTRIES);
    std::generate_n(std::back_inserter(native_entries), NUMBER_OF_ENTRIES, []() {
        return native::generate_random_snapshot("AAPL");
    });

    google::protobuf::Arena serialize_arena(make_arena_opts());
    std::vector<MarketSnapshot*> proto_entries;
    proto_entries.reserve(NUMBER_OF_ENTRIES);
    for (const auto& s : native_entries)
        proto_entries.push_back(to_proto_arena(serialize_arena, s));

    {
        file_guard file("protobuf_market_data.bin", EST_BUFFER_SIZE);
        mmap_guard addr(file(), EST_BUFFER_SIZE);

        google::protobuf::io::ArrayOutputStream array_stream(addr(), EST_BUFFER_SIZE);
        google::protobuf::io::CodedOutputStream coded_stream(&array_stream);

        const auto start_timer = std::chrono::steady_clock::now();
        for (const auto* msg : proto_entries) {
            coded_stream.WriteVarint32(msg->ByteSizeLong());
            msg->SerializeToCodedStream(&coded_stream);
        }
        const auto end_timer = std::chrono::steady_clock::now();

        std::println("Serialize: {}",
            std::chrono::duration_cast<std::chrono::milliseconds>(end_timer - start_timer));
        std::println("Wire size: {}MB", coded_stream.ByteCount() / 1024 / 1024);
    }

    {
        file_guard file("protobuf_market_data.bin");
        mmap_guard addr(file(), EST_BUFFER_SIZE);

        google::protobuf::io::ArrayInputStream array_stream(addr(), EST_BUFFER_SIZE);
        google::protobuf::io::CodedInputStream coded_stream(&array_stream);

        google::protobuf::Arena deser_arena(make_arena_opts());

        std::size_t correct{};
        const auto start_timer = std::chrono::steady_clock::now();
        for (auto entry: proto_entries) {
            deser_arena.Reset();

            uint32_t size;
            if (!coded_stream.ReadVarint32(&size)) break;

            auto* restored = google::protobuf::Arena::CreateMessage<MarketSnapshot>(&deser_arena);
            auto limit = coded_stream.PushLimit(size);
            restored->ParseFromCodedStream(&coded_stream);
            coded_stream.PopLimit(limit);

            correct += static_cast<std::size_t>(*entry == *restored);

        }
        const auto end_timer = std::chrono::steady_clock::now();

        std::println("Deserialize: {}",
            std::chrono::duration_cast<std::chrono::milliseconds>(end_timer - start_timer));
        std::println("Correct: {}/{}", correct, NUMBER_OF_ENTRIES);
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
