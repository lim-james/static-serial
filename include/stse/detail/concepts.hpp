#pragma once

#include <type_traits>
#include <array>

namespace stse::detail {

template<typename T>
concept EndianType = requires {
    { T::endian } -> std::convertible_to<std::endian>;
};

template<typename T>
concept NotSerializable = std::is_pointer_v<T> 
                       || std::is_null_pointer_v<T> 
                       || std::is_polymorphic_v<T>
                       || !std::is_trivially_copyable_v<T>
                       || EndianType<T>;

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

}
