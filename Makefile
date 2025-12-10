BUILD_DIR:=build
BIN_DIR:=$(BUILD_DIR)/bin
SHADERS_BIN_DIR:=$(BIN_DIR)/shaders
TARGET:=$(BIN_DIR)/fractales_generator

# Compilation stuff
CXX=clang++
#Debug
CXXFLAGS:=-std=c++20 -g -O0
#Release
# CXXFLAGS:=-std=c++20 -O3
INCLUDES:=-Isrc/header
LDFLAGS:=
#LIBS:=-lGL -lGLEW -lglfw
LIBS:=-lEGL -lGLESv2 -lX11

SOURCES_CPP:=$(shell find src -name '*.cpp' -print)
OBJECTS_CPP:=$(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES_CPP))

SRC_SHADERS:=$(shell find shaders -type f)
TARGET_SHADERS:=$(patsubst %,$(BIN_DIR)/%,$(SRC_SHADERS))

.PHONY: all clean
all: $(TARGET) $(TARGET_SHADERS) | $(BUILD_DIR) $(BIN_DIR)

# Link

$(TARGET): $(OBJECTS_CPP) $(BUILD_DIR)/main.o | $(BUILD_DIR) $(BIN_DIR)
	@echo Linking $@
	@$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

# Compile

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo Compiling $@
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/main.o: main.cpp | $(BUILD_DIR)
	@echo Compiling $@
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Directories

$(BUILD_DIR):
	@mkdir -p $@

$(BIN_DIR):
	@mkdir -p $@

$(SHADERS_BIN_DIR):
	@mkdir -p $@

# Install shaders

$(SHADERS_BIN_DIR)/%: shaders/% | $(SHADERS_BIN_DIR)
	@echo Installing shader $@
	@cp $< $@

# Other

clean:
	rm -rf build
