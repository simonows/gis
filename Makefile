APP_NAME=GIS
BUILD_DIR=build
SOURCES=src/*.cc
DEBUG_FLAGS=-g
INCLUDES=include
FLAGS=-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused

.PHONY: all debug release clean run

release:
	@mkdir -p $(BUILD_DIR)/release
	@echo "Make a Release"
	@g++ $(FLAGS) -I $(INCLUDES) $(SOURCES) -o $(BUILD_DIR)/release/$(APP_NAME)
	@echo "Compile done"

debug:
	@mkdir -p $(BUILD_DIR)/debug
	@echo "Make a Debug"
	@g++ $(DEBUG_FLAGS) -I $(INCLUDES) $(SOURCES) -o $(BUILD_DIR)/debug/$(APP_NAME)
	@echo "Compile done"

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f test/output01.txt

run:
	./build/release/GIS test/database test/script01.txt test/output01.txt

run_debug: debug
	gdb ./build/debug/GIS

