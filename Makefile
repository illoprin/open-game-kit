CXX=clang++
CFLAGS=-Wall -std=c++23 -O2
LDFLAGS=-lglad -lglfw3dll -lstdc++exp

INCLUDE=-Isrc -Ipkg/include
LIBS=-Lpkg/lib

SRC_DIR=src
SRCS = $(shell find $(SRC_DIR) -name "*.cc")
OBJS = $(patsubst $(SRC_DIR)/%.cc, $(OBJ_DIR)/%.o, $(SRCS))

BUILD_DIR=build
OBJ_DIR=$(BUILD_DIR)/obj

TARGET=$(BUILD_DIR)/app.exe

all: $(TARGET)
	cp pkg/lib/*.dll $(BUILD_DIR)
	./$<

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $^ $(INCLUDE) $(LIBS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(INCLUDE) -c $< -o $@