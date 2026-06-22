#pragma once

#include <utility>
#include <cstdint>
#include <string_view>
#include <print>

namespace stse::tests {

template<typename T>
class TestExecutor;

enum class FailureStrategy {
    EXIT_ON_FAIL,
    CONTINUE_ON_FAIL
};

template<typename... Args>
class TestExecutor<bool(Args...)> {
    
    using fn_t = bool(*)(Args...);

public:

    constexpr TestExecutor(
        std::string_view test_label, 
        fn_t test_fn, 
        FailureStrategy failure_strategy = FailureStrategy::CONTINUE_ON_FAIL
    )
        : failure_strategy_(failure_strategy)
        , test_label_(test_label)
        , test_fn_(std::forward<fn_t>(test_fn))
    {}

    constexpr ~TestExecutor() noexcept {
        if not consteval {
            const bool all_passed = total_ran_ == total_passed_;
            auto outcome = all_passed ? "ALL PASSED" :  std::format(
                "{} passed | {} failed", 
                total_passed_, total_ran_ - total_passed_
            );
            log_line("Complete!", outcome);
        }
    }

    TestExecutor(const TestExecutor&) = delete;
    void operator=(const TestExecutor&) = delete;

    TestExecutor(TestExecutor&&) = delete;
    void operator=(TestExecutor&&) = delete;

    template<typename... TestArgs>
    constexpr TestExecutor& run_single(TestArgs&&... args) {
        ++total_ran_;

        auto passed = test_fn_(std::forward<TestArgs&&>(args)...);
        total_passed_ += static_cast<std::size_t>(passed);

        if not consteval {
            if (!passed) {
                log_line(
                    "Failed single", 
                    std::format("{}/{}", total_passed_, total_ran_)
                );
            }
        } 

        return *this;
    }

    template<typename... TestPacket>
    constexpr TestExecutor& run_sequence(TestPacket&&... packets) {
        static constexpr std::size_t test_count = sizeof...(TestPacket);
        total_ran_ += test_count;

        auto invoke_one = [this](auto&& packet) -> bool {
            if constexpr (sizeof...(Args) == 1) {
                return test_fn_(std::forward<decltype(packet)>(packet));
            } else {
                return std::apply(test_fn_, std::forward<decltype(packet)>(packet));
            }
        };

        std::size_t ran_tests{};
        switch (failure_strategy_) {
        case FailureStrategy::EXIT_ON_FAIL:
        {
            (... && [&] {
                auto passed = invoke_one(packets);
                ++ran_tests;
                total_passed_ += static_cast<std::size_t>(passed);
                if not consteval {
                    if (!passed) {
                        log_line(
                            "Failed sequence",
                            std::format("{}/{}", ran_tests, test_count)
                        );
                    }
                }
                return passed;
            }());
            break;
        }
        case FailureStrategy::CONTINUE_ON_FAIL:
        {
            std::size_t passing{};
            (..., [&] {
                auto passed = invoke_one(packets);
                passing += static_cast<std::size_t>(passed);
                total_passed_ += static_cast<std::size_t>(passed);
                ++ran_tests;
                if not consteval {
                    if (!passed) {
                        log_line(
                            "Failed sequence",
                            std::format("{}/{}", ran_tests, test_count)
                        );
                    }
                }
            }());
            break;
        }
        default: std::unreachable();
        }

        return *this;
    }

    constexpr operator bool() const {
        return total_ran_ == total_passed_;
    }

private:

    FailureStrategy failure_strategy_;
    std::string_view test_label_;
    std::uint32_t total_passed_{}, total_ran_{};
    [[no_unique_address]] fn_t test_fn_;

    void log_line(
        std::string_view description, 
        std::string_view result
    ) {
        auto label_column = std::format("{} ", test_label_);
        auto description_column = std::format(" {}", description);
        std::println(
            "[TEST] {:-<48}{:->16} --- {}", 
            label_column, description_column, result
        );
    }
};

template<typename... Args>
TestExecutor(std::string_view, bool(*)(Args...), FailureStrategy) -> TestExecutor<bool(Args...)>;

template<typename... Args>
TestExecutor(std::string_view, bool(*)(Args...)) -> TestExecutor<bool(Args...)>;

}
