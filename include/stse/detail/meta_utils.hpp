#pragma once

#include <meta>
#include <vector>
#include <ranges>
#include <algorithm>

namespace stse::detail {

template<typename Annotation>
consteval bool has_annotation(std::meta::info info) {
    auto annotations = std::meta::annotations_of(info) 
                     | std::views::transform(std::meta::type_of)
                     | std::views::transform(std::meta::remove_cv);

    auto match_annotation = [](auto a) { return a == ^^Annotation; };

    return std::ranges::any_of(annotations, match_annotation);
}

template<typename T>
consteval auto get_all_data_members_of() {
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
consteval std::size_t size_of();

template<typename T>
inline constexpr std::size_t raw_size = size_of<T>();

template<Serializable T>
consteval std::size_t size_of() { 
    if constexpr (Scalar<T>) {
        return  std::meta::size_of(^^T); 
    } else if constexpr (StaticContainer<T>) {
        return std::tuple_size_v<T> * raw_size<typename T::value_type>;
    } else if constexpr (Aggregate<T>) {
        std::size_t total = 0;
        template for (constexpr auto member : serializable_members_of<T>) {
            total += raw_size<typename[:std::meta::type_of(member):]>;
        }
        return total;
    } else {
        std::unreachable();
    }
}

}
