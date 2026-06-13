# Static Serial

`C++26` `Reflections` `GCC16`

Reflection-based binary serializer for fixed-size wire formats.

## Example

```cpp
#include "stse/stse.hpp"

auto bid = OrderBookLevel{
    .price       = 17250, // int64  - 8B
    .quantity    = 500,   // uint64 - 8B
    .order_count = 3      // uint32 - 4B
};

auto raw_bytes = stse::serialize(bid);
auto restored  = stse::deserialize<OrderBookLevel>(raw_bytes).object;

assert(bid == restored);
assert(raw_bytes.size() != sizeof(OrderBookLevel));
assert(raw_bytes.size() == 20); // excludes padding

assert(stse::is_serializable_v<OrderBookLevel>);
assert(stse::serial_size_v<OrderBookLevel> != sizeof(OrderBookLevel));
```

## Getting Started

### Requirements
- GCC 16: Required for C++26 reflection and contracts support
- CMake 3.28+

```bash
# Ubuntu
sudo apt install gcc-16 g++-16
```

### Integration
Add the [`include/stse`](https://github.com/lim-james/static-serial/blob/main/include/stse) directory to your project and include the top-level header:

```cpp
#include "stse/stse.hpp"
```

> No module support yet.

### Build
```bash
git clone https://github.com/lim-james/static-serial.git
cd static-serial
make release # or: make debug
```

### Build manually
```bash
g++-16 -std=c++26 -freflection -fcontracts -fcontract-evaluation-semantic=observe \
    -I path/to/include your_file.cpp -o your_file.out
```

### Compile Flags

| Flag | Purpose |
|------|---------|
| `-std=c++26` | Required |
| `-freflection` | P2996 static reflection |
| `-fcontracts` | Contract annotations |
| `-fcontract-evaluation-semantic=observe` | Recommended — logs violations without requiring a custom handler. See [Known Issues](#known-issues). |

## Public Interface

**`serialize`** - serializes one or more objects into a stack-allocated byte array
```cpp
template<detail::EndianType Endian, detail::Serializable... Args> 
[[nodiscard]] constexpr auto serialize(
    Endian endianness,
    const Args&... data
) -> std::array<std::byte, serial_size_v<Args...>>;

template<detail::Serializable... Args> 
[[nodiscard]] constexpr auto serialize(
    const Args&... data
) -> std::array<std::byte, serial_size_v<Args...>>;
```

**`serialize_advance`** - serializes into an existing span, returning the 
remaining span after the written bytes.
```cpp
template<detail::EndianType Endian, detail::Serializable... Args> 
constexpr auto serialize_advance(
    Endian endianness,
    const std::span<std::byte> destination,
    const Args&... data
) -> std::span<std::byte>;

template<detail::Serializable... Args> 
constexpr auto serialize_advance(
    const std::span<std::byte> destination,
    const Args&... data
) -> std::span<std::byte>;
```

**`deserialize`** - deserializes one or more objects from a byte span, returning
results and remaining span.
```cpp
[[nodiscard]] constexpr auto deserialize(
    Endian endianness,
    const std::span<const std::byte> data
) -> DeserializeResult<Args...> 
    pre(data.size() >= serial_size_v<Args...>);

template<detail::Serializable... Args>
[[nodiscard]] constexpr auto deserialize(
    const std::span<const std::byte> data
) -> DeserializeResult<Args...> 
    pre(data.size() >= serial_size_v<Args...>);
```

**`deserialize_advance`** — deserializes into existing objects, returning 
remaining span.
```cpp
template<detail::Serializable... Args, detail::EndianType Endian>
constexpr auto deserialize_advance(
    Endian endianness,
    const std::span<const std::byte> data, 
    Args&... parsed
) -> std::span<const std::byte> 
    pre(data.size() >= serial_size_v<Args...>)

template<detail::Serializable... Args>
constexpr auto deserialize_advance(
    const std::span<const std::byte> data, 
    Args&... parsed
) -> std::span<const std::byte> 
    pre(data.size() >= serial_size_v<Args...>)
```

**Skip & Ignore Member Annotation**
Rule of thumb, skip will totally be ignored from the wire. Whereas ignored
members exist on the wire as garbage bytes.
```cpp
inline constexpr auto skip = skipserialization{};
inline constexpr auto ignore = ignoreserialization{};
// e.g. [[=stse::skip]] int* skip_member;
// e.g. [[=stse::ignore]] int* ignore_member;
```

**Properties**
```cpp
template<typename T>
inline constexpr bool is_serializable_v;

template<typename T>
inline constexpr std::size_t serial_size_v;
```

**Endian Specifiers**
```cpp
inline constexpr BigEndian    big_endian{};
inline constexpr LittleEndian little_endian{};
inline constexpr NativeEndian native_endian{};
```

**Return Schema**
```cpp
template<typename T>
[[nodiscard]] std::string schema();
```

**Deserialize Return Type**
```cpp
template<typename T>
struct DeserializeResult {
    T object;
    std::span<const std::byte> remaining;
};
```

### Serializable Types

A rule-of-thumb is serializable objects should have no heap owning members/pointers, and should not inherit from a polymorphic class

- Decided to mark polymorphic classes as not serializable because of the existence of vptr

| Serializable | Non-Serializable |
| --- | --- |
| Scalar types | Pointers (& std::nullptr_t) |
| std::array with trivially copyable types | std::vector (any dynamically sized container) |
| Aggregated structs | std::tuple |
| Nested structs  | std::string | 
| Inheritted structs (non-virtual) | std::pair |


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
7. `std::meta::annotations_of` for option to omit a field from serialization
8. `std::meta::bases_of` fetch parent classes to recursively get member
9. `contracts` included precondition for buffer size checks
10. `structured binding pack` for static container operations

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
5. `nonstatic_data_members_of` does not pick up private members
    - use unchecked access context
6. Long build times
    - use `-ftime-report` to breakdown build times. Reduce number of function
overloads to cut build times by using `if constexpr`
7. `std::meta::nonstatic_data_members_of` of `std::vector` yields nothing
    - use `bases_of` to recursively fetch parent members (std::vector likely had its main mmeber in a subclass)

## Known issues

1. [ICE] varialble template disallowed in contract postcondition (submitted bug
   report for GCC16)
2. `constexpr` public API is incompatible with C++26 Contracts — contract conditions 
   cannot be constant-evaluated; documented under compile flags as a known limitation
3. **bit fields** the api now struggles with serializing bit fields, meta
   provides some useful methods such as is_bit_field or bit_size_of which I
   attempted to use. however, attempts at supporting this revealed that I will
   need to rework my current serializer to offer more fine-grained packing at bit
   level rather than the current byte level packing. there are plans of
supporting this in a future version as bitfields do show up in binary formats.

## License
MIT © James Lim
