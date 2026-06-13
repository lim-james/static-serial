.PHONY: release debug bench test clean

release:
	cmake --preset release && cmake --build --preset release

debug:
	cmake --preset debug && cmake --build --preset debug

bench:
	cmake --preset bench && cmake --build --preset bench

test: debug
	./build/debug/tests

clean:
	rm -rf build
