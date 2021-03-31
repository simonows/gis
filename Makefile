APP_NAME=GIS
BUILD_DIR=build
SOURCES=src/*.cxx
DEBUG_FLAGS=-g
INCLUDES=include
FLAGS=

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

run:
	./build/release/GIS

run_debug: debug
	gdb ./build/debug/GIS

