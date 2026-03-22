#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <array>
#include <bit>
#include <span>
#include <tuple>
#include <type_traits>
#include <cassert>
#include <algorithm>

#include <meta>

namespace stse {

namespace {

struct BigEndian    { static constexpr std::endian endian = std::endian::big; };
struct LittleEndian { static constexpr std::endian endian = std::endian::little; };
struct NativeEndian { static constexpr std::endian endian = std::endian::native; };

template<typename T>
concept EndianType = requires {
    { T::endian } -> std::convertible_to<std::endian>;
};

template<size_t T> struct uint_of_size;
template<> struct uint_of_size<1> { using type = std::uint8_t; };
template<> struct uint_of_size<2> { using type = std::uint16_t; };
template<> struct uint_of_size<4> { using type = std::uint32_t; };
template<> struct uint_of_size<8> { using type = std::uint64_t; };

template<typename T>
using uint_of_size_t = typename uint_of_size<sizeof(T)>::type; 

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

template<SerializableScalar T>    consteval std::size_t size_of();
template<SerializableStdArray T>  consteval std::size_t size_of();
template<SerializableAggregate T> consteval std::size_t size_of();

template<SerializableScalar T, EndianType Endian>
constexpr std::span<std::byte> serialize(std::span<std::byte>, const T&, Endian);
template<SerializableStdArray T, EndianType Endian>
constexpr std::span<std::byte> serialize(std::span<std::byte>, const T&, Endian);
template<SerializableAggregate T, EndianType Endian>
constexpr std::span<std::byte> serialize(std::span<std::byte>, const T&, Endian);
template<NotSerializable T, EndianType Endian>
constexpr std::span<std::byte> serialize(std::span<std::byte>, const T&, Endian) 
    = delete("Type not supported for static serialization");

template<SerializableScalar T, EndianType Endian>
constexpr std::span<const std::byte> deserialize(T&, std::span<const std::byte>, Endian);
template<SerializableStdArray T, EndianType Endian>
constexpr std::span<const std::byte> deserialize(T&, std::span<const std::byte>, Endian);
template<SerializableAggregate T, EndianType Endian>
constexpr std::span<const std::byte> deserialize(T&, std::span<const std::byte>, Endian);
template<NotSerializable T, EndianType Endian>
constexpr std::span<const std::byte> deserialize(T&, std::span<const std::byte>, Endian)
    = delete("Type not supported for static serialization");

template<SerializableScalar T>
consteval std::size_t size_of() { return std::meta::size_of(^^T); }

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

template<SerializableStdArray T>
consteval std::size_t size_of() { 
    return std::tuple_size_v<T> * size_of<typename T::value_type>();
}

template<typename T>
inline constexpr std::size_t raw_size = size_of<T>();

template<SerializableScalar T, EndianType Endian>
constexpr std::span<std::byte> serialize(
    std::span<std::byte> destination, 
    const T& source,
    [[maybe_unused]] Endian endianness
) {
    static constexpr std::size_t value_byte_count = raw_size<T>;
    using value_buffer_t = std::array<std::byte, value_byte_count>;

    auto bytes = [&source]() {
        if constexpr (Endian::endian == NativeEndian::endian) {
            return std::bit_cast<value_buffer_t>(source);
        } else if constexpr (std::is_integral_v<T>) {
            return std::bit_cast<value_buffer_t>(std::byteswap(source));
        } else {
            using U = uint_of_size_t<T>;
            auto byte_buffer = std::bit_cast<U>(source);
            return std::bit_cast<value_buffer_t>(std::byteswap(byte_buffer));
        }
    }();

    for (std::size_t i{}; i < value_byte_count; ++i) {
        destination[i] = bytes[i];
    }

    return destination.subspan(value_byte_count);
}

template<SerializableAggregate T, EndianType Endian>
constexpr std::span<std::byte> serialize(
    std::span<std::byte> destination, 
    const T& source,
    Endian endianness
) {
    static constexpr auto data_members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())
    );

    template for (constexpr auto member : data_members) {
        destination = serialize(destination, source.[:member:], endianness);
    }

    return destination;
}

template<SerializableStdArray T, EndianType Endian>
constexpr std::span<std::byte> serialize(
    std::span<std::byte> destination, 
    const T& source,
    Endian endianness
) {
    for (const auto& item: source) destination = serialize(destination, item, endianness);
    return destination;
}

template<SerializableScalar T, EndianType Endian>
constexpr std::span<const std::byte> deserialize(
    T& destination, 
    std::span<const std::byte> source,
    [[maybe_unused]] Endian endianness
) {
    static constexpr std::size_t value_byte_count = raw_size<T>;
    using value_buffer_t = std::array<std::byte, value_byte_count>;
    value_buffer_t buffer;

    auto bytes = source.first(value_byte_count);
    for (std::size_t i{}; i < value_byte_count; ++i) {
        buffer[i] = bytes[i];
    }

    destination = [&buffer]() {
        if constexpr (Endian::endian == NativeEndian::endian) {
            return std::bit_cast<T>(buffer);
        } else if constexpr (std::is_integral_v<T>) {
            return std::byteswap(std::bit_cast<T>(buffer));
        } else {
            using U = uint_of_size_t<T>;
            auto byte_buffer = std::bit_cast<U>(buffer);
            return std::bit_cast<T>(std::byteswap(byte_buffer));
        }
    }();

    return source.subspan(value_byte_count);
}

template<SerializableAggregate T, EndianType Endian>
constexpr std::span<const std::byte> deserialize(
    T& destination, 
    std::span<const std::byte> source,
    Endian endianness
) {
    static constexpr auto data_members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())
    );

    template for (constexpr auto member: data_members) {
        source = deserialize(destination.[:member:], source, endianness);
    }

    return source;
}

template<SerializableStdArray T, EndianType Endian>
constexpr std::span<const std::byte> deserialize(
    T& destination, 
    std::span<const std::byte> source,
    Endian endianness
) {
    for (auto& item: destination) source = deserialize(item, source, endianness);
    return source;
}
}

inline constexpr BigEndian    big_endian{};
inline constexpr LittleEndian little_endian{};
inline constexpr NativeEndian native_endian{};

template<typename T>
[[nodiscard]] consteval bool is_serializable() {
    if constexpr (SerializableScalar<T>) {
        return !NotSerializable<T>;
    } else if constexpr (SerializableAggregate<T>) {
        static constexpr auto data_members = std::define_static_array(
            std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())
        );
        template for (constexpr auto member: data_members) {
            if (!is_serializable<typename[:std::meta::type_of(member):]>()) {
                return false;
            }
        }
        return true;
    } else if constexpr (SerializableStdArray<T>) {
        return is_serializable<typename T::value_type>();
    }
    return false;
}

template<typename T, EndianType Endian = NativeEndian> 
[[nodiscard]] constexpr auto serialize(
    const T& data, 
    Endian endianness = {}
) -> std::array<std::byte, raw_size<T>> {
    if constexpr (is_serializable<T>()) {
        auto buffer = std::array<std::byte, raw_size<T>>{};
        serialize(buffer, data, endianness);
        return buffer;
    } else {
        static_assert(is_serializable<T>(), "Type not serializable.");
    }
}

template<typename T, EndianType Endian = NativeEndian>
[[nodiscard]] constexpr T deserialize(
    std::span<const std::byte> data, 
    Endian endianness = {}
) {
    if constexpr (is_serializable<T>()) {
        assert(data.size() >= raw_size<T>);

        T parsed;
        deserialize(parsed, data, endianness);
        return parsed;
    } else {
        static_assert(is_serializable<T>(), "Type not deserializable.");
    }
}

}
