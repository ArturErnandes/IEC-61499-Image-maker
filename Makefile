CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I./fbsvg -I./pugixml
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

LIB_NAME = libfbsvg.a

TARGET_SVG = $(BIN_DIR)/fb2svg.exe
TARGET_PNG = $(BIN_DIR)/fb2png.exe

FB_SOURCES = $(SRC_DIR)/fb_parser.cpp $(SRC_DIR)/svg_renderer.cpp $(SRC_DIR)/png_renderer.cpp
FB_OBJECTS = $(FB_SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

PUGI_SRC = pugixml/pugixml.cpp
PUGI_OBJ = $(BUILD_DIR)/pugixml.o

EXAMPLE_SRC = example/main.cpp
EXAMPLE_OBJ = $(BUILD_DIR)/main.o

FB2PNG_SRC = $(SRC_DIR)/fb2png.cpp
FB2PNG_OBJ = $(BUILD_DIR)/fb2png.o

PKG_CONFIG_CFLAGS := $(shell pkg-config --cflags cairo librsvg-2.0)
PKG_CONFIG_LIBS := $(shell pkg-config --libs cairo librsvg-2.0)
CXXFLAGS += $(PKG_CONFIG_CFLAGS)
LDFLAGS = $(PKG_CONFIG_LIBS)

all: $(TARGET_SVG) $(TARGET_PNG)

$(TARGET_SVG): $(LIB_NAME) $(EXAMPLE_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(EXAMPLE_OBJ) $(LIB_NAME) $(LDFLAGS)

$(TARGET_PNG): $(LIB_NAME) $(FB2PNG_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(FB2PNG_OBJ) $(LIB_NAME) $(LDFLAGS)

$(LIB_NAME): $(FB_OBJECTS) $(PUGI_OBJ)
	ar rcs $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/pugixml.o: $(PUGI_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: $(EXAMPLE_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/fb2png.o: $(FB2PNG_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(LIB_NAME) $(TARGET_SVG) $(TARGET_PNG)

.PHONY: all clean