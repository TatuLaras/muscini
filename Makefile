NAME = muscini
BUILD_DIR = build
BUILD_DIR_TESTS = build/tests
BUILD_DIR_SCENES = build/scenes
SRC_DIR = src
EXTERNAL_SRC_DIR = external/src
SRC_DIR_SCENES = scene_src
SRC_DIR_TESTS = test
UNITY_DIR = external/unity

INCLUDE = -Iexternal/include -Isrc

CC = gcc

PACKAGES = $(shell pkg-config --libs raylib) -lm -ldl
SANITIZE = -fsanitize=address
CFLAGS = $(PACKAGES) $(INCLUDE) -Wall -Wextra -Wshadow -pedantic -Wstrict-prototypes -march=native

CFLAGS_TEST = $(PACKAGES) -DTEST -I$(UNITY_DIR) -I$(SRC_DIR) $(INCLUDE) -ggdb $(SANITIZE)
CFLAGS_DEBUG = $(CFLAGS) -DDEBUG -ggdb
CFLAGS_ASAN = $(CFLAGS) -DDEBUG $(SANITIZE) -g
CFLAGS_RELEASE = $(CFLAGS) -DNDEBUG -Ofast

CFLAGS_SCENE = $(PACKAGES) $(INCLUDE) -Wall -Wextra -Wshadow -pedantic -Wstrict-prototypes -march=native -c -fpic -g

# Arguments to append to the program run with "make run"
ARGS = 

# Build program

SRC = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(LIBEBB)/src/*.c) $(wildcard $(EXTERNAL_SRC_DIR)/*.c)

debug: $(BUILD_DIR) $(BUILD_DIR)/debug
release: $(BUILD_DIR) $(BUILD_DIR)/release
asan: $(BUILD_DIR) $(BUILD_DIR)/asan

install: release
	rm -rf /usr/share/noble
	cp resources /usr/share/noble -r
	cp $(BUILD_DIR)/release /usr/bin/$(NAME)

run: $(BUILD_DIR) $(BUILD_DIR)/debug
	@echo "WARNING: no address sanitation enabled, consider running with 'make run_asan' when developing."
	$(BUILD_DIR)/debug $(ARGS)

run_asan: $(BUILD_DIR) $(BUILD_DIR)/asan
	$(BUILD_DIR)/asan $(ARGS)


$(BUILD_DIR)/debug: $(SRC)
	@echo "INFO: Building debug build"
	$(CC) -o $@ $^ $(CFLAGS_DEBUG)

$(BUILD_DIR)/release: $(SRC)
	@echo "INFO: Building release build"
	$(CC) -o $@ $^ $(CFLAGS_RELEASE)

$(BUILD_DIR)/asan: $(SRC)
	@echo "INFO: Building address sanitation build"
	$(CC) -o $@ $^ $(CFLAGS_ASAN)



# Build scenes

OBJS_SCENES = $(patsubst $(SRC_DIR_SCENES)/%.c, $(BUILD_DIR_SCENES)/%.so, $(wildcard $(SRC_DIR_SCENES)/*.c))

scenes: $(BUILD_DIR_SCENES) $(OBJS_SCENES)

$(BUILD_DIR_SCENES)/%.o: $(SRC_DIR_SCENES)/%.c
	$(CC) $(CFLAGS_SCENE) $< -o $@

$(BUILD_DIR_SCENES)/%.so: $(BUILD_DIR_SCENES)/%.o
	$(CC) -shared $< -o $@


# Dirs

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR_TESTS):
	mkdir -p $(BUILD_DIR_TESTS)

$(BUILD_DIR_SCENES):
	mkdir -p $(BUILD_DIR_SCENES)


# Build and run tests

TEST_IGNORE = $(SRC_DIR)/main.c
SRC_FOR_TESTS = $(filter-out $(TEST_IGNORE), $(SRC)) $(wildcard $(UNITY_DIR)/*.c)
OBJS_TESTS = $(patsubst $(SRC_DIR_TESTS)/%.c, $(BUILD_DIR_TESTS)/%.o, $(wildcard $(SRC_DIR_TESTS)/test_*.c))

test: $(BUILD_DIR_TESTS) run_tests
	@echo

NOOP=
SPACE = $(NOOP) $(NOOP)

run_tests: $(OBJS_TESTS)
	@echo -e "\n\n--------------\n Test results\n--------------\n"
	@$(subst $(SPACE), && echo -e "\n" && ,$^)

$(OBJS_TESTS): $(BUILD_DIR_TESTS)/%.o: $(SRC_DIR_TESTS)/%.c $(SRC_FOR_TESTS)
	@echo -e "\nBuilding $@"
	$(CC) -o $@ $^ $(CFLAGS_TEST)

clean:
	rm -rf $(BUILD_DIR)

