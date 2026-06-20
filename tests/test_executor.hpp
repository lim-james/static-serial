#pragma once

#include <utility>
#include <string_view>
#include <print>

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

    TestExecutor(
        std::string_view test_label, 
        fn_t&& test_fn, 
        FailureStrategy failure_strategy = FailureStrategy::CONTINUE_ON_FAIL
    )
        : failure_strategy_(failure_strategy)
        , test_label_(test_label)
        , test_fn_(std::forward<fn_t&&>(test_fn))
    {
        std::println("[TEST][INIT] Initialising '{}' suite", test_label_);
    }

    constexpr TestExecutor(
        fn_t&& test_fn, 
        FailureStrategy failure_strategy = FailureStrategy::CONTINUE_ON_FAIL
    )
        : failure_strategy_(failure_strategy)
        , test_fn_(std::forward<fn_t&&>(test_fn))
    {}

    constexpr ~TestExecutor() noexcept {
        if not consteval {
            std::println(
                "[TEST][CLOSE] Wrapping up '{}' --- {} passed | {} failed",
                test_label_,
                total_passed_, 
                total_ran_ - total_passed_
            );
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
            std::println(
                "      [SINGLE] Running tests {}/{} --- Result: {}",
                total_passed_, total_ran_,
                passed ? "PASS" : "FAIL"
            );
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

        if not consteval {
            std::println("      [SEQUENCE] Running tests for '{}' --- 0/{}", test_label_, test_count);
        }   

        std::size_t ran_tests{};
        switch (failure_strategy_) {
        case FailureStrategy::EXIT_ON_FAIL:
        {
            (... && [&] {
                auto passed = invoke_one(packets);
                ++ran_tests;
                total_passed_ += static_cast<std::size_t>(passed);
                if not consteval {
                    std::println(
                        "      [IN PROGRESS] Running {}/{} --- Result: {}",
                        ran_tests, test_count,
                        passed ? "PASS" : "FAIL (exiting now)"
                    );
                }
                return passed;
            }());

            if not consteval {
                std::println(
                    "      [SUMMARY] Ran sequence on '{}' --- {} passed | {} failed",
                    test_label_,
                    ran_tests, 
                    test_count - ran_tests
                );
            }
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
                    std::println(
                        "      [IN PROGRESS] Running {}/{} --- result: {}",
                        ran_tests, test_count,
                        passed ? "PASS" : "FAIL (carrying on)"
                    );
                }
            }());

            if not consteval {
                std::println(
                    "      [SUMMARY] Ran sequence on '{}' --- {} passed | {} failed",
                    test_label_,
                    passing, 
                    test_count - passing
                );
            }
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
    std::size_t total_passed_{}, total_ran_{};
    [[no_unique_address]] fn_t test_fn_;

};

template<typename... Args>
TestExecutor(std::string_view, bool(*)(Args...), FailureStrategy) -> TestExecutor<bool(Args...)>;

template<typename... Args>
TestExecutor(std::string_view, bool(*)(Args...)) -> TestExecutor<bool(Args...)>;

template<typename... Args>
TestExecutor(bool(*)(Args...)) -> TestExecutor<bool(Args...)>;

template<typename... Args>
TestExecutor(bool(*)(Args...), FailureStrategy) -> TestExecutor<bool(Args...)>;
