#include "static_serial_test.hpp"

///
/// Single skipped fields
///

struct PartialSerializable {
    int a;
    [[=stse::skip]] int* b;
    bool operator==(const PartialSerializable&) const = default;
};

static_assert(std::is_trivially_copyable_v<PartialSerializable>);
static_assert(stse::is_serializable_v<PartialSerializable>);

static constexpr PartialSerializable ps{1, nullptr};
static_assert(stse::test::test_round_trip<ps>());
static_assert(stse::serial_size_v<PartialSerializable> == sizeof(int));

///
/// Multiple skipped fields
///

struct MultiSkip {
    int a;
    [[=stse::skip]] int* b;
    int c;
    [[=stse::skip]] int* d;
    bool operator==(const MultiSkip&) const = default;
};

static_assert(stse::is_serializable_v<MultiSkip>);
static_assert(stse::serial_size_v<MultiSkip> == sizeof(int) * 2);

static constexpr MultiSkip ms{1, nullptr, 2, nullptr};
static_assert(stse::test::test_round_trip<ms>());

///
/// Skip on inherited field
///

struct SkipBase {
    int a;
    [[=stse::skip]] int* ptr;
    bool operator==(const SkipBase&) const = default;
};

struct SkipDerived : SkipBase {
    int b;
    bool operator==(const SkipDerived&) const = default;
};

static_assert(stse::is_serializable_v<SkipBase>);
static_assert(stse::is_serializable_v<SkipDerived>);
static_assert(stse::serial_size_v<SkipBase>    == sizeof(int));
static_assert(stse::serial_size_v<SkipDerived> == sizeof(int) * 2);

static constexpr SkipDerived sd{SkipBase{1, nullptr}, 2};
static_assert(stse::test::test_round_trip<sd>());
