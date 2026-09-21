CXX=clang++
CFLAGS=-Wall -std=c++23
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

$(TARGET):
	$(CXX) $(CFLAGS) $(SRC_DIR)/*.cc $(INCLUDE) $(LIBS) -o $@ $(LDFLAGS)
