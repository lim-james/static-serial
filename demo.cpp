#include <cstdint>
#include <array>
#include <print>

#include "static_serial.hpp"

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

int main() {
    constexpr MarketSnapshot snapshot {
        .timestamp_ns = 1'711'900'000'000'000'000ULL,
        .symbol       = make_ticker("AAPL"),
        .bids = {{
            { .price = {.mantissa = 17250, .exponent = -2}, .quantity = 500,  .order_count = 3 },
            { .price = {.mantissa = 17245, .exponent = -2}, .quantity = 1200, .order_count = 7 },
            { .price = {.mantissa = 17240, .exponent = -2}, .quantity = 800,  .order_count = 4 },
            { .price = {.mantissa = 17235, .exponent = -2}, .quantity = 300,  .order_count = 2 },
            { .price = {.mantissa = 17230, .exponent = -2}, .quantity = 950,  .order_count = 5 },
        }},
        .asks = {{
            { .price = {.mantissa = 17255, .exponent = -2}, .quantity = 400,  .order_count = 2 },
            { .price = {.mantissa = 17260, .exponent = -2}, .quantity = 750,  .order_count = 6 },
            { .price = {.mantissa = 17265, .exponent = -2}, .quantity = 1100, .order_count = 8 },
            { .price = {.mantissa = 17270, .exponent = -2}, .quantity = 200,  .order_count = 1 },
            { .price = {.mantissa = 17275, .exponent = -2}, .quantity = 600,  .order_count = 4 },
        }},
    };

    auto raw_bytes = std::array<std::byte, 1024>{};
    auto moved_ptr = stse::serialize_into(snapshot, raw_bytes);
    
    auto restored_snapshot = stse::deserialize<MarketSnapshot>(raw_bytes);
    assert(snapshot == restored_snapshot);

    std::string schema = stse::schema<MarketSnapshot>();
    std::print("{}", schema);
}

