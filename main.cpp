#include <cstdint>
#include <cstddef>
#include <array>
#include <bit>
#include <span>
#include <tuple>
#include <type_traits>

#include <meta>

struct Vec3 {
    float x;
    float y;
    float z;

    bool operator==(const Vec3&) const = default;
};

struct PaddedStruct {
    char a;
    std::uint64_t b;

    bool operator==(const PaddedStruct&) const = default;
};

struct Player {
    uint32_t id;
    Vec3 pos;
    std::array<int, 4> inventory;

    bool operator==(const Player&) const = default;
};

template<typename T>
concept SerializableScalar = std::is_scalar_v<T>;

template<typename T>
concept SerializableCArray = std::is_array_v<T> &&
                             std::is_trivially_copyable_v<std::remove_all_extents_t<T>>;

template<typename T>
concept SerializableStdArray = requires {
    typename T::value_type;
    typename std::tuple_size<T>::type;
} && std::is_trivially_copyable_v<typename T::value_type>;

template<typename T>
concept SerializableArray = SerializableCArray<T> || SerializableStdArray<T>;

template<typename T>
concept SerializableAggregate = std::is_trivially_copyable_v<T> && 
                                std::is_aggregate_v<T> &&
                                !SerializableScalar<T> &&
                                !SerializableArray<T>;

template<SerializableScalar T>
consteval std::size_t size_of() { return std::meta::size_of(^^T); }

template<SerializableCArray T>
consteval std::size_t size_of() { 
    return std::extent_v<T> * size_of<std::remove_all_extents_t<T>>();
}

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

template<SerializableArray T>
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

template<typename T> 
[[nodiscard]] constexpr auto serialize(const T& data) -> std::array<std::byte, raw_size<T>> {
    auto buffer = std::array<std::byte, raw_size<T>>{};
    serialize(buffer, data);
    return buffer;
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


template<SerializableArray T>
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

template<typename T>
[[nodiscard]] constexpr T deserialize(std::span<const std::byte> data) {
    T parsed;
    deserialize(parsed, data);
    return parsed;
}

template<auto data>
constexpr bool test_back_and_forth() {
    using T = decltype(data);
    auto bytes    = serialize(data);
    auto restored = deserialize<T>(bytes);
    return data == restored;
}

template<typename T>
bool test_back_and_forth_runtime(const T& data) {
    auto bytes    = serialize(data);
    auto restored = deserialize<T>(bytes);
    return data == restored;
}

int main() {
    static constexpr Vec3 position{.x = 0.1, .y = 0.2, .z = 0.3};
    static_assert(test_back_and_forth<position>(), "Back-&-Forth serialization failed");

    static constexpr std::array arr{1, 2, 3, 4, 5};
    static_assert(test_back_and_forth<arr>(), "Back-&-Forth serialization failed");
    
    static constexpr PaddedStruct padded{'a', 0};
    constexpr auto bytes = serialize(padded);
    static_assert(bytes.size() != sizeof(PaddedStruct));
    static_assert(bytes.size() == 9);
    static_assert(deserialize<PaddedStruct>(bytes) == padded);

    static constexpr Player player{
        .id = 0, 
        .pos = Vec3{.x = 0.1, .y = 0.2, .z = 0.3},
        .inventory = {1, 2, 3, 4}
    };
    static_assert(test_back_and_forth<player>(), "Back-&-Forth serialization failed");
}

