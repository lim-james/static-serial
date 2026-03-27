#pragma once 

#include <cstdint>
#include <array>

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
