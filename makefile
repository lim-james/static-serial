.PHONY: all test_debug test_release test_aubsan benchmark demo clean

# Configure + build + run tests (debug)
test_debug:
	cmake --preset test_debug
	cmake --build --preset test_debug
	./build/test_debug/tests

# Configure + build + run tests (release)
test_release:
	cmake --preset test_release
	cmake --build --preset test_release
	./build/test_release/tests

# Configure + build + run tests (ASan + UBSan)
test_aubsan:
	cmake --preset test_aubsan
	cmake --build --preset test_aubsan
	ASAN_OPTIONS=halt_on_error=1:abort_on_error=1 \
	UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
	./build/test_aubsan/tests

# Configure + build benchmarks
benchmark:
	cmake --preset benchmark
	cmake --build --preset benchmark

# Configure + build demo
demo:
	cmake --preset demo
	cmake --build --preset demo
	./build/demo/demo

# Clean all build directories
clean:
	rm -rf build/

# Default: run all test presets
all: test_debug test_release test_aubsan
