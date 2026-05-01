#pragma once

#include <concepts>
#include <cstddef>
#include <cstring>
#include <cstdint>
#include <array>
#include <bit>
#include <span>
#include <tuple>
#include <type_traits>
#include <string>
#include <format>
#include <ranges>
#include <algorithm>
#include <utility>

#include <meta>

namespace stse {

namespace detail {

struct BigEndian    { static constexpr std::endian endian = std::endian::big; };
struct LittleEndian { static constexpr std::endian endian = std::endian::little; };
struct NativeEndian { static constexpr std::endian endian = std::endian::native; };

template<typename T>
concept EndianType = requires {
    { T::endian } -> std::convertible_to<std::endian>;
};

struct skipserialization {};

template<size_t T> struct uint_of_size;
template<> struct uint_of_size<1> { using type = std::uint8_t; };
template<> struct uint_of_size<2> { using type = std::uint16_t; };
template<> struct uint_of_size<4> { using type = std::uint32_t; };
template<> struct uint_of_size<8> { using type = std::uint64_t; };

template<typename T>
using uint_of_size_t = typename uint_of_size<sizeof(T)>::type; 

template<typename T>
concept NotSerializable = std::is_pointer_v<T> 
                       || std::is_null_pointer_v<T> 
                       || std::is_polymorphic_v<T>;

template<typename T>
concept Scalar = std::is_scalar_v<T>;

template<typename T>
concept StaticContainer = requires {
    typename T::value_type;
    typename std::tuple_size<T>::type;
} && std::is_trivially_copyable_v<typename T::value_type>;

template<typename T>
concept Aggregate = !Scalar<T> &&
                    !StaticContainer<T> &&
                    !NotSerializable<T>;

template<typename T>
concept Serializable = (Scalar<T> 
                    || StaticContainer<T> 
                    || Aggregate<T>)
                    && !NotSerializable<T>;

template<Serializable T> consteval std::size_t size_of();

template<Scalar T, EndianType Endian>
constexpr std::span<std::byte> serialize_scalar(std::span<std::byte>, const T&, Endian);
template<StaticContainer T, EndianType Endian>
constexpr std::span<std::byte> serialize_static_container(std::span<std::byte>, const T&, Endian);
template<Aggregate T, EndianType Endian>
constexpr std::span<std::byte> serialize_aggregate(std::span<std::byte>, const T&, Endian);

template<Serializable T, EndianType Endian>
constexpr std::span<std::byte> serialize(std::span<std::byte>, const T&, Endian);
template<NotSerializable T, EndianType Endian>
constexpr std::span<std::byte> serialize(std::span<std::byte>, const T&, Endian) 
    = delete("Type not supported for static serialization");

template<Scalar T, EndianType Endian>
constexpr std::span<const std::byte> deserialize_scalar(T&, std::span<const std::byte>, Endian);
template<StaticContainer T, EndianType Endian>
constexpr std::span<const std::byte> deserialize_static_container(T&, std::span<const std::byte>, Endian);
template<Aggregate T, EndianType Endian>
constexpr std::span<const std::byte> deserialize_aggregate(T&, std::span<const std::byte>, Endian);

template<Serializable T, EndianType Endian>
constexpr std::span<const std::byte> deserialize(T&, std::span<const std::byte>, Endian);
template<NotSerializable T, EndianType Endian>
constexpr std::span<const std::byte> deserialize(T&, std::span<const std::byte>, Endian)
    = delete("Type not supported for static serialization");


template<Scalar T, std::uint8_t depth> std::string generate_schema();
template<StaticContainer T, std::uint8_t depth> std::string generate_schema();
template<Aggregate T, std::uint8_t depth> std::string generate_schema();

template<NotSerializable T, std::uint8_t depth>
std::string generate_schema() = delete("Type not supported for static serialization");


template<typename Annotation>
consteval bool has_annotation(std::meta::info info) {
    auto annotations = std::meta::annotations_of(info) 
                     | std::views::transform(std::meta::type_of)
                     | std::views::transform(std::meta::remove_cv);

    auto match_annotation = [](auto a) { return a == ^^Annotation; };

    return std::ranges::any_of(annotations, match_annotation);
}

template<typename T>
constexpr auto get_all_data_members_of() {
    auto all_data_members = std::vector<std::meta::info>{};

    static constexpr auto bases = std::define_static_array(
        std::meta::bases_of(^^T, std::meta::access_context::unchecked())
    );

    template for (constexpr auto base: bases) {
        auto parent_members = get_all_data_members_of<typename[:std::meta::type_of(base):]>();
        for (auto member: parent_members) all_data_members.push_back(member);
    }

    static constexpr auto data_members = std::define_static_array(
        std::meta::nonstatic_data_members_of(
            ^^T, 
            std::meta::access_context::unchecked()
        )
    );

    template for (constexpr auto member: data_members) all_data_members.push_back(member);

    return all_data_members;
}

template<typename T>
consteval auto get_serializable_members_of() {
    static constexpr auto skip_serialization = std::views::filter([](auto info) { 
        return !has_annotation<skipserialization>(info); 
    });

    return std::define_static_array(get_all_data_members_of<T>() | skip_serialization);
}

template<typename T>
inline constexpr auto serializable_members_of = get_serializable_members_of<T>();

template<Serializable T>
consteval std::size_t size_of() { 
    if constexpr (Scalar<T>) {
        return std::meta::size_of(^^T); 
    } else if constexpr (StaticContainer<T>) {
        return std::tuple_size_v<T> * size_of<typename T::value_type>(); 
    } else if constexpr (Aggregate<T>) {
        std::size_t total = 0;
        template for (constexpr auto member : serializable_members_of<T>) {
            total += size_of<typename[:std::meta::type_of(member):]>();
        }
        return total;
    } else {
        std::unreachable();
    }
}

template<typename T>
inline constexpr std::size_t raw_size = size_of<T>();

template<Scalar T, EndianType Endian>
constexpr std::span<std::byte> serialize_scalar(
    std::span<std::byte> destination, 
    const T& source,
    [[maybe_unused]] Endian endianness
) {
    static constexpr std::size_t value_byte_count = raw_size<T>;
    using value_buffer_t = std::array<std::byte, value_byte_count>;
    value_buffer_t bytes;

    if constexpr (Endian::endian == NativeEndian::endian) {
        bytes = std::bit_cast<value_buffer_t>(source);
    } else if constexpr (std::is_integral_v<T>) {
        bytes = std::bit_cast<value_buffer_t>(std::byteswap(source));
    } else {
        using U = uint_of_size_t<T>;
        auto byte_buffer = std::bit_cast<U>(source);
        bytes = std::bit_cast<value_buffer_t>(std::byteswap(byte_buffer));
    }

    if consteval {
        for (std::size_t i = 0; i < value_byte_count; ++i) {
            destination[i] = bytes[i];
        }
    } else {
        std::memcpy(destination.data(), bytes.data(), value_byte_count);
    }

    return destination.subspan(value_byte_count);
}

template<StaticContainer T, EndianType Endian>
constexpr std::span<std::byte> serialize_static_container(
    std::span<std::byte> destination, 
    const T& source,
    Endian endianness
) {
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        ((destination = serialize(destination, source[Is], endianness)), ...);
    }(std::make_index_sequence<std::tuple_size_v<T>>{});
    return destination;
}

template<Aggregate T, EndianType Endian>
constexpr std::span<std::byte> serialize_aggregate(
    std::span<std::byte> destination, 
    const T& source,
    Endian endianness
) {
    template for (constexpr auto member : serializable_members_of<T>) {
        destination = serialize(destination, source.[:member:], endianness);
    }

    return destination;
}

template<Serializable T, EndianType Endian> 
constexpr std::span<std::byte> serialize(
    std::span<std::byte> destination, 
    const T& source,
    Endian endianness
) { 
    if constexpr (Scalar<T>) {
        return serialize_scalar(destination, source, endianness); 
    } else if constexpr (StaticContainer<T>) {
        return serialize_static_container(destination, source, endianness); 
    } else if constexpr (Aggregate<T>) {
        return serialize_aggregate(destination, source, endianness); 
    } else {
        std::unreachable();
    }
}

template<Scalar T, EndianType Endian>
constexpr std::span<const std::byte> deserialize_scalar(
    T& destination, 
    std::span<const std::byte> source,
    [[maybe_unused]] Endian endianness
) {
    static constexpr std::size_t value_byte_count = raw_size<T>;
    using value_buffer_t = std::array<std::byte, value_byte_count>;
    value_buffer_t buffer;

    auto bytes = source.first(value_byte_count);
    if consteval {
        for (std::size_t i = 0; i < value_byte_count; ++i) {
            buffer[i] = bytes[i];
        }
    } else {
        std::memcpy(buffer.data(), bytes.data(), value_byte_count);
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

template<StaticContainer T, EndianType Endian>
constexpr std::span<const std::byte> deserialize_static_container(
    T& destination, 
    std::span<const std::byte> source,
    Endian endianness
) {
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        ((source = deserialize(destination[Is], source, endianness)), ...);
    }(std::make_index_sequence<std::tuple_size_v<T>>{});
    return source;
}

template<Aggregate T, EndianType Endian>
constexpr std::span<const std::byte> deserialize_aggregate(
    T& destination, 
    std::span<const std::byte> source,
    Endian endianness
) {
    template for (constexpr auto member: serializable_members_of<T>) {
        source = deserialize(destination.[:member:], source, endianness);
    }

    return source;
}

template<Serializable T, EndianType Endian> 
constexpr std::span<const std::byte> deserialize(
    T& destination, 
    std::span<const std::byte> source,
    Endian endianness
) { 
    if constexpr (Scalar<T>) {
        return deserialize_scalar(destination, source, endianness); 
    } else if constexpr (StaticContainer<T>) {
        return deserialize_static_container(destination, source, endianness); 
    } else if constexpr (Aggregate<T>) {
        return deserialize_aggregate(destination, source, endianness); 
    } else {
        std::unreachable();
    }
}

template<std::uint8_t depth>
constexpr std::string_view pad() {
    static_assert(depth <= 127, "Schema nesting depth exceeds maximum supported depth");

    if constexpr (depth == 0) {
        using namespace std::literals;
        return ""sv;
    } else {
        static constexpr auto padding = [] {
            std::array<char, depth * 2> padding{};
            padding.fill(' ');
            return padding;
        }();
        return {padding.data(), padding.size()};
    }

}

template<typename T>
std::string type_header() {
    constexpr auto type = std::meta::display_string_of(^^T);
    return std::format("[{} :: {} bytes]", type, raw_size<T>);
}

template<Scalar T, std::uint8_t depth>
std::string generate_schema() {
    return type_header<T>();
}

template<StaticContainer T, std::uint8_t depth>
std::string generate_schema() {
    auto schema_out = type_header<T>();

    using child_type = typename T::value_type;
    constexpr auto child_info  = std::meta::dealias(^^child_type);
    constexpr auto identifier  = std::meta::display_string_of(child_info);
    constexpr auto child_count = std::tuple_size_v<T>;
    auto meta_data  = generate_schema<child_type, depth + 1>();

    schema_out += std::format(
        "\n{}{}[{}]: {}",
        pad<depth + 1>(),
        identifier,
        child_count,
        meta_data 
    );

    return schema_out;
}

template<Aggregate T, std::uint8_t depth>
std::string generate_schema() {
    static constexpr auto data_members = serializable_members_of<T>;

    constexpr auto fields = data_members.size();
    std::string schema_out = std::format("{} [{} fields]", type_header<T>(), fields);

    template for (constexpr auto member: data_members) {
        constexpr auto identifier = std::meta::identifier_of(member);
        auto meta_data  = generate_schema<typename[:std::meta::type_of(member):], depth + 1>();
        schema_out += std::format(
            "\n{}{}: {}",
            pad<depth + 1>(),
            identifier,
            meta_data 
        );
    }

    return schema_out;
}

}

inline constexpr detail::BigEndian    big_endian{};
inline constexpr detail::LittleEndian little_endian{};
inline constexpr detail::NativeEndian native_endian{};

inline constexpr auto skip = detail::skipserialization{};

template<typename T>
[[nodiscard]] consteval bool is_serializable();

template<typename T>
inline constexpr bool is_serializable_v = is_serializable<T>();

template<typename T>
inline constexpr std::size_t serial_size_v = detail::raw_size<T>;

template<typename T>
[[nodiscard]] consteval bool is_serializable() {
    using T_NORM = std::decay_t<T>;
    
    if constexpr (detail::NotSerializable<T_NORM>) {
        return false;
    } else if constexpr (detail::Scalar<T_NORM>) {
        return true;
    } else if constexpr (detail::StaticContainer<T_NORM>) {
        return is_serializable_v<typename T_NORM::value_type>;
    } else if constexpr (detail::Aggregate<T_NORM>) {
        template for (constexpr auto member: detail::serializable_members_of<T_NORM>) {
            if (!is_serializable_v<typename[:std::meta::type_of(member):]>) {
                return false;
            }
        }
        return true;
    } else {
        std::unreachable(); 
    }   
}


template<typename T, detail::EndianType Endian = detail::NativeEndian> 
[[nodiscard]] constexpr auto serialize(
    const T& data, 
    Endian endianness = {}
) -> std::array<std::byte, serial_size_v<T>> {
    if constexpr (is_serializable_v<T>) {
        auto buffer = std::array<std::byte, serial_size_v<T>>{};
        detail::serialize(buffer, data, endianness);
        return buffer;
    } else {
        static_assert(is_serializable_v<T>, "Type not serializable.");
    }
}

template<typename T, detail::EndianType Endian = detail::NativeEndian> 
constexpr auto serialize_advance(
    const T& data, 
    const std::span<std::byte> destination,
    Endian endianness = {}
) -> std::span<std::byte> 
    pre(destination.size() >= serial_size_v<T>)
    // post(out: out.size() >= destination.size() - serial_size_v<T>)
{
    if constexpr (is_serializable_v<T>) {
        return detail::serialize(destination, data, endianness);
    } else {
        static_assert(is_serializable_v<T>, "Type not serializable.");
    }
}

template<typename T>
struct DeserializeResult {
    T object;
    std::span<const std::byte> remaining;
};

template<typename T, detail::EndianType Endian = detail::NativeEndian>
[[nodiscard]] constexpr auto deserialize(
    const std::span<const std::byte> data, 
    Endian endianness = {}
) -> DeserializeResult<T> 
    pre(data.size() >= serial_size_v<T>)
    // post(out: out.remaining.size() >= data.size() - serial_size_v<T>)
{
    if constexpr (is_serializable_v<T>) {
        auto parsed = T{};
        auto remaining_ptr = detail::deserialize(parsed, data, endianness);
        return DeserializeResult{
            .object = parsed, 
            .remaining = remaining_ptr
        };
    } else {
        static_assert(is_serializable_v<T>, "Type not deserializable.");
    }
}

template<typename T, detail::EndianType Endian = detail::NativeEndian>
constexpr auto deserialize_advance(
    T& parsed,
    const std::span<const std::byte> data, 
    Endian endianness = {}
) -> std::span<const std::byte> 
    pre(data.size() >= serial_size_v<T>)
    // post(out: out.size() >= data.size() - serial_size_v<T>)
{
    if constexpr (is_serializable_v<T>) {
        return detail::deserialize(parsed, data, endianness);
    } else {
        static_assert(is_serializable_v<T>, "Type not deserializable.");
    }
}

template<typename T>
[[nodiscard]] std::string schema() {
    if constexpr (is_serializable_v<T>) {
        return detail::generate_schema<T, 0>();
    } else {
        static_assert(is_serializable_v<T>, "Type not serializable.");
    }
}

}
