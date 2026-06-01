#pragma once

#include "detail/types.hpp"
#include "detail/concepts.hpp"
#include "detail/serialize.hpp"
#include "detail/deserialize.hpp"
#include "detail/schema.hpp"

#include <cstddef>
#include <cstring>
#include <type_traits>
#include <utility>

#include <meta>

namespace stse {

template<typename T>
concept Serializable = detail::Serializable<T>;

inline constexpr detail::BigEndian    big_endian{};
inline constexpr detail::LittleEndian little_endian{};
inline constexpr detail::NativeEndian native_endian{};

inline constexpr auto skip = detail::skipserialization{};

template<typename... Args>
inline constexpr std::size_t serial_size_v = (detail::raw_size<Args> + ...);

template<detail::EndianType Endian, detail::Serializable... Args> 
[[nodiscard]] constexpr auto serialize(
    Endian endianness,
    const Args&... data
) -> std::array<std::byte, serial_size_v<Args...>> {
    auto buffer = std::array<std::byte, serial_size_v<Args...>>{};
    std::span<std::byte> write_ptr = buffer;
    ((write_ptr = detail::serialize(write_ptr, data, endianness)), ...);
    return buffer;
}

template<detail::Serializable... Args> 
[[nodiscard]] constexpr auto serialize(
    const Args&... data
) -> std::array<std::byte, serial_size_v<Args...>> {
    auto buffer = std::array<std::byte, serial_size_v<Args...>>{};
    std::span<std::byte> write_ptr = buffer;
    ((write_ptr = detail::serialize_flat(write_ptr, data)), ...);
    return buffer;
}


template<detail::EndianType Endian, detail::Serializable... Args> 
constexpr auto serialize_advance(
    Endian endianness,
    const std::span<std::byte> destination,
    const Args&... data
) -> std::span<std::byte> 
    pre(destination.size() >= serial_size_v<Args...>)
    // post(out: out.size() >= destination.size() - serial_size_v<T>)
{
    auto write_ptr = destination;
    return ((write_ptr = detail::serialize(write_ptr, data, endianness)), ...);
}

template<detail::Serializable... Args> 
constexpr auto serialize_advance(
    const std::span<std::byte> destination,
    const Args&... data
) -> std::span<std::byte> 
    pre(destination.size() >= serial_size_v<Args...>)
    // post(out: out.size() >= destination.size() - serial_size_v<T>)
{
    auto write_ptr = destination;
    return ((write_ptr = detail::serialize_flat(write_ptr, data)), ...);
    // return ((write_ptr = detail::serialize(write_ptr, data, native_endian)), ...);
}

template<detail::Serializable... Args>
struct DeserializeResult {
    std::tuple<Args...> objects;
    std::span<const std::byte> remaining;
};

template<detail::Serializable... Args, detail::EndianType Endian>
[[nodiscard]] constexpr auto deserialize(
    Endian endianness,
    const std::span<const std::byte> data
) -> DeserializeResult<Args...> 
    pre(data.size() >= serial_size_v<Args...>)
    // post(out: out.remaining.size() >= data.size() - serial_size_v<T>)
{
    std::tuple<Args...> parsed_objects{};
    auto& [...parsed] = parsed_objects;
    
    auto remaining_ptr = data;
    ((remaining_ptr = detail::deserialize(parsed, remaining_ptr, endianness)), ...);

    return DeserializeResult{
        .objects   = parsed_objects, 
        .remaining = remaining_ptr
    };
}

template<detail::Serializable... Args>
[[nodiscard]] constexpr auto deserialize(
    const std::span<const std::byte> data
) -> DeserializeResult<Args...> 
    pre(data.size() >= serial_size_v<Args...>)
    // post(out: out.remaining.size() >= data.size() - serial_size_v<T>)
{
    std::tuple<Args...> parsed_objects{};
    auto& [...parsed] = parsed_objects;
    
    auto remaining_ptr = data;
    ((remaining_ptr = detail::deserialize_flat(parsed, remaining_ptr)), ...);

    return DeserializeResult{
        .objects   = parsed_objects, 
        .remaining = remaining_ptr
    };
}


template<detail::Serializable... Args, detail::EndianType Endian>
constexpr auto deserialize_advance(
    Endian endianness,
    const std::span<const std::byte> data, 
    Args&... parsed
) -> std::span<const std::byte> 
    pre(data.size() >= serial_size_v<Args...>)
    // post(out: out.size() >= data.size() - serial_size_v<T>)
{
    auto read_ptr = data;
    return ((read_ptr = detail::deserialize(parsed, read_ptr, endianness)), ...);
}

template<detail::Serializable... Args>
constexpr auto deserialize_advance(
    const std::span<const std::byte> data, 
    Args&... parsed
) -> std::span<const std::byte> 
    pre(data.size() >= serial_size_v<Args...>)
    // post(out: out.size() >= data.size() - serial_size_v<T>)
{
    auto read_ptr = data;
    return ((read_ptr = detail::deserialize_flat(parsed, read_ptr)), ...);
}


template<detail::Serializable T>
[[nodiscard]] std::string schema() {
    return detail::generate_schema<T, 0>();
}

}
