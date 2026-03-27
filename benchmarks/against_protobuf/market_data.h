#pragma once

#include <cstdint>
#include <array>
#include <random>

namespace native {

struct Price {
    std::int64_t mantissa;
    std::int8_t  exponent;

    bool operator==(const Price&) const = default;
};

struct OrderBookLevel {
    Price         price;
    std::uint64_t quantity;
    std::uint32_t order_count;

    bool operator==(const OrderBookLevel&) const = default;
};

using Ticker = std::array<char, 8>;

struct MarketSnapshot {
    std::uint64_t                 timestamp_ns;
    Ticker                        symbol;
    std::array<OrderBookLevel, 5> bids;
    std::array<OrderBookLevel, 5> asks;

    bool operator==(const MarketSnapshot&) const = default;
};

constexpr Ticker make_ticker(const char* s) {
    Ticker t{};
    for (std::size_t i = 0; i < 8 && s[i]; ++i) t[i] = s[i];
    return t;
}

inline MarketSnapshot generate_random_snapshot(const char* symbol_str) {
    static thread_local std::mt19937_64 engine(std::random_device{}());

    std::uniform_int_distribution<std::uint64_t> ts_dist(1711540000000000000ULL, 1711550000000000000ULL);
    std::uniform_int_distribution<std::int64_t>  price_dist(10000, 50000);
    std::uniform_int_distribution<std::uint64_t> qty_dist(1, 1000);
    std::uniform_int_distribution<std::uint32_t> count_dist(1, 20);

    MarketSnapshot snapshot;
    snapshot.timestamp_ns = ts_dist(engine);
    snapshot.symbol = make_ticker(symbol_str);

    std::int64_t mid_price      = price_dist(engine);
    std::int8_t  common_exponent = -2;

    for (std::size_t i = 0; i < 5; ++i) {
        snapshot.bids[i] = {
            .price       = {mid_price - static_cast<std::int64_t>(i + 1), common_exponent},
            .quantity    = qty_dist(engine),
            .order_count = count_dist(engine)
        };
        snapshot.asks[i] = {
            .price       = {mid_price + static_cast<std::int64_t>(i + 1), common_exponent},
            .quantity    = qty_dist(engine),
            .order_count = count_dist(engine)
        };
    }

    return snapshot;
}

} // namespace native
