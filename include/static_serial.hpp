#pragma once

#include <cstddef>
#include <array>
#include <bit>
#include <span>
#include <tuple>
#include <type_traits>
#include <cassert>

#include <meta>

namespace stse {
namespace {

template<typename T>
concept SerializableScalar = std::is_scalar_v<T>;

template<typename T>
concept SerializableStdArray = requires {
    typename T::value_type;
    typename std::tuple_size<T>::type;
} && std::is_trivially_copyable_v<typename T::value_type>;

template<typename T>
concept SerializableAggregate = std::is_trivially_copyable_v<T> && 
                                std::is_aggregate_v<T> &&
                                !SerializableScalar<T> &&
                                !SerializableStdArray<T>;

template<typename T>
concept NotSerializable = std::is_pointer_v<T> || std::is_null_pointer_v<T>;

template<SerializableScalar T>
consteval std::size_t size_of() { return std::meta::size_of(^^T); }

template<SerializableStdArray T>
consteval std::size_t size_of() { 
    return std::tuple_size_v<T> * size_of<typename T::value_type>();
}

template<SerializableAggregate T>
consteval std::size_t size_of() { 
    static constexpr auto data_members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())
    );

    std::size_t total = 0;
    template for (constexpr auto member : data_members) {
        total += size_of<typename[:std::meta::type_of(member):]>();
    }
    return total;
}

template<typename T>
inline constexpr std::size_t raw_size = size_of<T>();


template<NotSerializable T>
constexpr std::span<std::byte> serialize(std::span<std::byte>, const T&) = delete("Type not supported for static serialization");

template<SerializableScalar T>
constexpr std::span<std::byte> serialize(std::span<std::byte> destination, const T& source) {
    static constexpr std::size_t value_byte_count = raw_size<T>;
    using value_buffer_t = std::array<std::byte, value_byte_count>;
    auto bytes = std::bit_cast<value_buffer_t>(source);

    for (std::size_t i{}; i < value_byte_count; ++i) {
        destination[i] = bytes[i];
    }

    return destination.subspan(value_byte_count);
}

template<SerializableStdArray T>
constexpr std::span<std::byte> serialize(std::span<std::byte> destination, const T& source) {
    for (const auto& item: source) destination = serialize(destination, item);
    return destination;
}

template<SerializableAggregate T>
constexpr std::span<std::byte> serialize(std::span<std::byte> destination, const T& source) {
    static constexpr auto data_members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())
    );

    template for (constexpr auto member : data_members) {
        destination = serialize(destination, source.[:member:]);
    }

    return destination;
}

template<SerializableScalar T>
constexpr std::span<const std::byte> deserialize(T& destination, std::span<const std::byte> source) {
    static constexpr std::size_t value_byte_count = raw_size<T>;
    using value_buffer_t = std::array<std::byte, value_byte_count>;
    value_buffer_t buffer;

    auto bytes = source.first(value_byte_count);
    for (std::size_t i{}; i < value_byte_count; ++i) {
        buffer[i] = bytes[i];
    }

    destination = std::bit_cast<T>(buffer);

    return source.subspan(value_byte_count);
}


template<SerializableStdArray T>
constexpr std::span<const std::byte> deserialize(T& destination, std::span<const std::byte> source) {
    for (auto& item: destination) source = deserialize(item, source);
    return source;
}

template<SerializableAggregate T>
constexpr std::span<const std::byte> deserialize(T& destination, std::span<const std::byte> source) {
    static constexpr auto data_members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())
    );

    template for (constexpr auto member: data_members) {
        source = deserialize(destination.[:member:], source);
    }

    return source;
}

}

template<typename T> 
[[nodiscard]] constexpr auto serialize(const T& data) -> std::array<std::byte, raw_size<T>> {
    auto buffer = std::array<std::byte, raw_size<T>>{};
    serialize(buffer, data);
    return buffer;
}

template<typename T>
[[nodiscard]] constexpr T deserialize(std::span<const std::byte> data) 
{
    assert(data.size() >= raw_size<T>);

    T parsed;
    deserialize(parsed, data);
    return parsed;
}

}
