#include <cstdint>
#include <cstddef>
#include <array>
#include <bit>
#include <span>

#include <meta>

struct Vec3 {
    float x;
    float y;
    float z;

    bool operator==(const Vec3&) const = default;
};

struct Player {
    uint32_t id;
    Vec3 pos;
    std::array<int, 4> inventory;

    bool operator==(const Player&) const = default;
};

constexpr std::span<std::byte> constexpr_memcpy(std::span<std::byte> destination, const auto& source) {
    constexpr std::size_t value_byte_count = sizeof(source);
    using value_buffer_t = std::array<std::byte, value_byte_count>;
    auto bytes = std::bit_cast<value_buffer_t>(source);

    for (std::size_t i{}; i < value_byte_count; ++i) {
        destination[i] = bytes[i];
    }

    return destination.subspan(value_byte_count);
}

template<typename T>
constexpr std::span<const std::byte> constexpr_memcpy(T& destination, std::span<const std::byte> source) {
    constexpr std::size_t value_byte_count = sizeof(T);

    using value_buffer_t = std::array<std::byte, value_byte_count>;
    value_buffer_t buffer;

    auto bytes = source.first(value_byte_count);
    for (std::size_t i{}; i < value_byte_count; ++i) {
        buffer[i] = bytes[i];
    }

    destination = std::bit_cast<T>(buffer);

    return source.subspan(value_byte_count);
}

template<typename T> // constraint
constexpr auto serialize(const T& data) -> std::array<std::byte, sizeof(T)> {
    static constexpr auto data_members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())
    );

    auto buffer = std::array<std::byte, sizeof(T)>{};
    std::span<std::byte> buffer_ptr = buffer;

    template for (constexpr auto member : data_members) {
        buffer_ptr = constexpr_memcpy(buffer_ptr, data.[:member:]);
    }

    return buffer;
}

template<typename T> // constraint
constexpr T deserialize(std::span<const std::byte> data) {
    static constexpr auto data_members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())
    );
    
    T parsed;

    template for (constexpr auto member: data_members) {
        data = constexpr_memcpy(parsed.[:member:], data);
    }

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

    volatile bool result = test_back_and_forth_runtime(Vec3{.x = 0.1, .y = 0.2, .z = 0.3});

    static constexpr Player player{
        .id = 0, 
        .pos = Vec3{.x = 0.1, .y = 0.2, .z = 0.3},
        .inventory = {1, 2, 3, 4}
    };
}

