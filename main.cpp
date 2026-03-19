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

template<typename T> // constraint
constexpr auto serialize(const T& data) -> std::array<std::byte, sizeof(T)> {
    static constexpr auto data_members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())
    );

    auto buffer = std::array<std::byte, sizeof(T)>{};
    std::size_t ptr_offset = 0;
    template for (constexpr auto member : data_members) {
        auto value = data.[:member:];
        constexpr std::size_t value_byte_count = sizeof(value);

        using value_buffer_t = std::array<std::byte, value_byte_count>;
        auto bytes = std::bit_cast<value_buffer_t>(value);

        for (std::size_t i{}; i < value_byte_count; ++i) {
            buffer[ptr_offset++] = bytes[i];
        }
    }

    return buffer;
}

template<typename T> // constraint
constexpr T deserialize(std::span<std::byte> data) {
    static constexpr auto data_members = std::define_static_array(
        std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current())
    );
    
    T parsed;

    template for (constexpr auto member: data_members) {
        using member_t = decltype(parsed.[:member:]);
        constexpr std::size_t member_size = sizeof(member_t);

        auto bytes = data.first(member_size);

        using value_buffer_t = std::array<std::byte, member_size>;
        value_buffer_t buffer;

        for (std::size_t i{}; i < member_size; ++i) {
            buffer[i] = bytes[i];
        }

        parsed.[:member:] = std::bit_cast<member_t>(buffer);

        data = data.subspan(member_size);
    }

    return parsed;
}

template<auto Data>
constexpr bool test_back_and_forth() {
    using T = decltype(Data);
    auto bytes    = serialize(Data);
    auto restored = deserialize<T>(bytes);
    return Data == restored;
}

int main() {
    static constexpr Vec3 position{.x = 0.1, .y = 0.2, .z = 0.3};
    static constexpr auto result = test_back_and_forth<position>();
    static_assert(result, "Back-&-Forth serialization failed");

    static constexpr Player player{
        .id = 0, 
        .pos = Vec3{.x = 0.1, .y = 0.2, .z = 0.3},
        .inventory = {1, 2, 3, 4}
    };
}
