CXX = clang++
COMMON_FLAGS = -Wall -std=c++23
INCLUDE = -Isrc -Ipkg/include
LIBS = -Lpkg/lib
LDFLAGS = -limgui -lglad -lglfw3dll -lstdc++exp

SRC_DIR = src
SRCS = $(shell find $(SRC_DIR) -name "*.cc")

DEBUG_DIR = build/debug
RELEASE_DIR = build/release

DEBUG_OBJS = $(patsubst $(SRC_DIR)/%.cc,$(DEBUG_DIR)/obj/%.o,$(SRCS))
RELEASE_OBJS = $(patsubst $(SRC_DIR)/%.cc,$(RELEASE_DIR)/obj/%.o,$(SRCS)) \
               $(RELEASE_DIR)/resources.res.o

.PHONY: all debug release clean
all: debug

debug: $(DEBUG_DIR)/app.exe
	cp pkg/lib/*.dll $(DEBUG_DIR)
	./$<

release: $(RELEASE_DIR)/app.exe

$(DEBUG_DIR)/app.exe: $(DEBUG_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(COMMON_FLAGS) -O0 -g $^ $(INCLUDE) $(LIBS) -o $@ $(LDFLAGS)

$(RELEASE_DIR)/app.exe: $(RELEASE_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(COMMON_FLAGS) -O2 $^ $(INCLUDE) $(LIBS) -o $@ $(LDFLAGS)

$(DEBUG_DIR)/obj/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(COMMON_FLAGS) -O0 -g $(INCLUDE) -c $< -o $@

$(RELEASE_DIR)/obj/%.o: $(SRC_DIR)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(COMMON_FLAGS) -O2 $(INCLUDE) -c $< -o $@

$(RELEASE_DIR)/resources.res.o: resources.rc
	@mkdir -p $(dir $@)
	windres $< -o $@

clean:
	rm -rf build