# STSE vs Protobuf

Compiled with `-O3 -march=native`, 1M `MarketSnapshot` entries.

| | Naive memcpy | Hand-rolled | **STSE** | Protobuf |
| --- | --- | --- | --- | --- |
| Serialization | 271ms | 188ms | **188ms** | 533ms |
| Deserialization | 54ms | 47ms | **48ms** | 830ms |
| Wire size | 321MB | 216MB | **216MB** | 247MB |

* Matches hand-rolled exactly 
* **2.8× faster to serialize** & **17.3× faster to deserialize** than protobuf
* Naive memcpy copies padding bytes, producing a 49% larger wire format than STSE and running slower due to the extra memory bandwidth.

## `MarketSnapshot` Definition

```cpp
struct Price {
    std::int64_t mantissa;
    std::int8_t  exponent;
};

struct OrderBookLevel {
    Price         price;
    std::uint64_t quantity;
    std::uint32_t order_count;
};

using Ticker = std::array<char, 8>;

struct MarketSnapshot {
    std::uint64_t                 timestamp_ns;
    Ticker                        symbol;
    std::array<OrderBookLevel, 5> bids;
    std::array<OrderBookLevel, 5> asks;
};
```

## Trade-offs

- Fixed schema only — no forward/backward compatibility or schema evolution
- Fixed-layout protocols only (NASDAQ ITCH, SBE, similar) — not a general-purpose serialiser
