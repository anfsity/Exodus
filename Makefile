# Makefile

BUILD_DIR = build

.PHONY: all debug release clean

all: release

debug:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=23
	cmake --build $(BUILD_DIR) -j $(shell nproc)

release:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17
	cmake --build $(BUILD_DIR) -j $(shell nproc)

clean:
	rm -rf $(BUILD_DIR)
