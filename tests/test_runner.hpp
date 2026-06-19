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

    TestExecutor(std::string_view test_label, fn_t&& test_fn, FailureStrategy failure_strategy)
        : failure_strategy_(failure_strategy)
        , test_label_(test_label)
        , test_fn_(test_fn)
    {}

    template<typename... TestPacket>
    bool operator()(TestPacket... test_suite) {
        static constexpr std::size_t test_count = sizeof...(TestPacket);
        std::println("[TEST][INIT] Running tests for '{}' --- 0/{}", test_label_, test_count);

        std::size_t ran_tests{};

        switch (failure_strategy_) {
        case FailureStrategy::EXIT_ON_FAIL:
        {
            template for (auto [...test]: {test_suite...}) {
                auto test_passed = test_fn_(test...);
                ++ran_tests;
                std::println(
                    "[TEST][IN PROGRESS] Running {}/{} --- Result: {}",
                    ran_tests, test_count,
                    test_passed ? "PASS" : "FAIL (exiting now)"
                );
                if (!test_passed) {
                    std::println(
                        "[TEST][SUMMARY] Early exiting on '{}' --- Failed at {} --- {}/{} passed.",
                        test_label_,
                        ran_tests,
                        ran_tests - 1, test_count
                    );
                    return false;
                }
            }
            
            std::println(
                "[TEST][SUMMARY] Ran tests on '{}' --- All {}/{} passed!",
                test_label_,
                ran_tests, test_count
            );
            return true;
        }
        case FailureStrategy::CONTINUE_ON_FAIL:
        {
            std::size_t passing_tests{};
            template for (auto [...test]: {test_suite...}) {
                auto test_passed = test_fn_(test...);
                passing_tests += static_cast<std::size_t>(test_passed);
                ++ran_tests;
                std::println(
                    "[TEST][IN PROGRESS] Running {}/{} --- result: {}",
                    ran_tests, test_count,
                    test_passed ? "PASS" : "FAIL (carrying on)"
                );
            }

            std::println(
                "[TEST][SUMMARY] Ran tests on '{}' --- {} passed | {} failed",
                test_label_,
                ran_tests, 
                test_count - ran_tests
            );
            return passing_tests == ran_tests;
        }
        default: std::unreachable();
        }
    }

private:

    FailureStrategy failure_strategy_;
    std::string_view test_label_;
    [[no_unique_address]] fn_t test_fn_;

};

template<typename... Args>
TestExecutor(std::string_view, bool(*)(Args...), FailureStrategy) -> TestExecutor<bool(Args...)>;
