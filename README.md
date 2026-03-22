# Static Serial

`C++26` `Reflections` `GCC16`

Zero-overhead binary serialization w/ C++26 Reflection support

## Example

```cpp
#include "static_serial.hpp"

auto bid = OrderBookLevel{
    .price       = 17250, // int64  - 8B
    .quantity    = 500,   // uint64 - 8B
    .order_count = 3      // uint32 - 4B
};

auto raw_bytes = stse::serialize(bid);
auto restored  = stse::deserialize<OrderBookLevel>(raw_bytes);

assert(bid == restored);
assert(raw_bytes.size() != sizeof(OrderBookLevel));
assert(raw_bytes.size() == 20); // excludes padding
```

### Supported Types
> Intend to better define this in a future patch


| Serializable | Non-Serializable |
| --- | --- |
| Scalar types | Pointers (& std::nullptr_t) |
| std::array with trivially copyable types | std::vector (any dynamically sized container |
| Aggregated structs | |
| Nested structs | |


## Zero-overhead Verification

Compiled with GCC 16 `-O2`


| Metric          | `-O0`  | `-O2` |
|-----------------|--------|-------|
| Function calls  | 94     | 3     |
| Loop labels     | 76     | 1     |
| Assembly lines  | 2494   | 446   |


## Motivation

I have been hearing a lot about C++26's reflections and wanted to check it out.
But I was primarily inspired by [Barry Revzin's Practical Reflection at CppCon25](https://www.youtube.com/watch?v=ZX_z6wzEOG0) and his challenge to see how we can use reflections to solve problems.

## Resources
- [P2996R12 Paper](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2025/p2996r12.html#selecting-members) 
- GCC 16's header files

> As of writing this project, I couldn't find any available resources but that's
> fine with me because I want to attempt a new form of learning.

## C++26 features explored

1. `template for` expansion statements, complie-time iteration
2. `std::meta::nonstatic_data_members_of` extract all nonstatic members of given
   type
3. `^^` reflection operator - compute reflection value
4. `[: refl :]` splicers - produce grammatical elements from reflections (copied
   from P2996
5. `std::define_static_array` - takes a range and materialize a span for compile
   time (consteval). Needed this for `template for` 
6. `std::meta::size_of` why this over `sizeof`? Simply because sizeof
   accomodates for padded bytes whilst meta::size_of is raw number of bytes

## Difficulties faced

In this section, I will be documenting problems I have faced whilst attempting
to learn.

1. Compiler & P-Paper interface mismatch. p-paper describes `member_of(info
   r)` whilst the compiler takes in an additional parameter `access_context`
   - Overcomed by referrencing header files directly
2. `members_of` returns a vector which is heap allocated with new. Doesn't work
   directly at compile time esp template for. 
    - Use define_static_array which takes in a range and extracts compile time
    information
3. Using a recursive serializer leads to some really ugly error messages when an
   invalid type is deeply nested in a type.
   - Fixed it by introducing `is_serializable` check at the start of serializing
4. Static assert messages are a little vague
    - Waiting for constexpr std::format [P3391](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p3391r0.html)

## Known issues

1. std::tuple & std::pair support
