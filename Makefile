CC = cc

PNAME = andai

INSTALL_PATH = /usr/local/bin

SRC_DIR := src
BIN := build
OBJ_DIR := $(BIN)/obj

SRCS := $(shell find $(SRC_DIR) -name '*.c')

EXEC = $(BIN)/$(PNAME)

##########
# COMMON #
##########

INCFLAGS  = -Isrc/

WARNINGS  = -Wall
WARNINGS += -Wextra
WARNINGS += -Wshadow
WARNINGS += -Wconversion
WARNINGS += -pedantic
WARNINGS += -Wno-error=padded
WARNINGS += -Wno-error=unused-parameter
WARNINGS += -Wno-error=incompatible-pointer-types
WARNINGS += -Wpadded
WARNINGS += -fanalyzer

STD := -std=c11

COMMON_CFLAGS := $(STD) $(WARNINGS) $(INCFLAGS)

COMMON_LDFLAGS  = $(INCFLAGS)
COMMON_LDFLAGS += -lpthread
COMMON_LDFLAGS += -lSDL2
COMMON_LDFLAGS += -lSDL2_image
COMMON_LDFLAGS += -lSDL2_ttf
COMMON_LDFLAGS += -lraylib
COMMON_LDFLAGS += -lrt
COMMON_LDFLAGS += -lm

#########
# DEBUG #
#########

DEBUG_OBJ := $(BIN)/obj/debug
DEBUG_BIN := $(BIN)/$(PNAME)_debug

DEBUG_CFLAGS := \
	 -ggdb \
	 -O0 \
	 -fsanitize=address,leak \
	 -fsanitize=undefined \
	 -fsanitize=bounds \
	 -fsanitize=bounds-strict \
	 -fsanitize=alignment \
	 -fsanitize=bool \
	 -fsanitize=pointer-overflow \
	 -fstack-protector-strong \
	 -fstack-check \
	 -fno-omit-frame-pointer \

DEBUG_LDFLAGS := \
	 -g \
	 -fsanitize=address,leak \
	 -fsanitize=undefined \
	 -fsanitize=bounds \
	 -fsanitize=bounds-strict \
	 -fsanitize=alignment \
	 -fsanitize=bool \
	 -fsanitize=pointer-overflow \
	 -fstack-protector-strong \
	 -fstack-check \
	 -fno-omit-frame-pointer

##################
# COVERAGE BUILD #
##################

COV_OBJ := $(BIN)/obj/coverage
COV_BIN := $(BIN)/$(PNAME)_coverage

COV_CFLAGS := \
	 -ggdb \
	 -O0 \
	 --coverage \
	 -fprofile-arcs -ftest-coverage
COV_LDFLAGS := \
	 -g \
	 --coverage \
	 -fprofile-arcs -ftest-coverage \
	 -lgcov

COV_BIN := $(BIN)/$(PNAME)_cov

###################
# PROFILING BUILD #
###################
# gprof

PROF_OBJ := $(BIN)/obj/profiling
PROF_BIN := $(BIN)/$(PNAME)_profiling

PROF_CFLAGS := \
	 -ggdb \
	 -O0 \
	 -pg

PROF_LDFLAGS := \
	 -pg

###########
# RELEASE #
###########

REL_OBJ := $(BIN)/obj/release
REL_BIN := $(BIN)/$(PNAME)

RELEASE_CFLAGS := \
	 -O3 \
	 -march=native \
	 -flto \
	 -DNDEBUG

RELEASE_LDFLAGS := \
	 -flto

#==============#
# OBJECT LISTS #
#==============#

DEBUG_OBJS := $(SRCS:$(SRC_DIR)/%.c=$(DEBUG_OBJ)/%.o)
COV_OBJS   := $(SRCS:$(SRC_DIR)/%.c=$(COV_OBJ)/%.o)
PROF_OBJS  := $(SRCS:$(SRC_DIR)/%.c=$(PROF_OBJ)/%.o)
REL_OBJS   := $(SRCS:$(SRC_DIR)/%.c=$(REL_OBJ)/%.o)

#================#
# DEFAULT TARGET #
#================#

.PHONY: all
all: release debug

#===============#
#  BUILD RULES  #
#===============#

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN):
	mkdir -p $(BIN)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(COMMON_CFLAGS) $(CFLAGS) -c $< -o $@

#======================#
#  DIRECTORY CREATION  #
#======================#

define make_obj_rule
$1/%.o: $(SRC_DIR)/%.c
	mkdir -p $$(dir $$@)
	$$(CC) $$(COMMON_CFLAGS) $2 -c $$< -o $$@
endef

$(eval $(call make_obj_rule,$(DEBUG_OBJ),$(DEBUG_CFLAGS)))
$(eval $(call make_obj_rule,$(COV_OBJ),  $(COV_CFLAGS)))
$(eval $(call make_obj_rule,$(PROF_OBJ), $(PROF_CFLAGS)))
$(eval $(call make_obj_rule,$(REL_OBJ),  $(REL_CFLAGS)))

#==============#
#  LINK RULES  #
#==============#

$(DEBUG_BIN): $(DEBUG_OBJS)
	mkdir -p $(dir $@)
	$(CC) $^ $(COMMON_LDFLAGS) $(DEBUG_LDFLAGS) -o $@

$(COV_BIN): $(COV_OBJS)
	mkdir -p $(dir $@)
	$(CC) $^ $(COMMON_LDFLAGS) $(COV_LDFLAGS) -o $@

$(PROF_BIN): $(PROF_OBJS)
	mkdir -p $(dir $@)
	$(CC) $^ $(COMMON_LDFLAGS) $(PROF_LDFLAGS) -o $@

$(REL_BIN): $(REL_OBJS)
	mkdir -p $(dir $@)
	$(CC) $^ $(COMMON_LDFLAGS) $(REL_LDFLAGS) -o $@

#==================#
#  PUBLIC TARGETS  #
#==================#

.PHONY: debug coverage profile release

debug:   $(DEBUG_BIN)
coverage:$(COV_BIN)
profile: $(PROF_BIN)
release: $(REL_BIN)


#=========#
#  TOOLS  #
#=========#

.PHONY: cov_run
coverage: coverage
	@echo "Running program to generate coverage data..."
	./$(COV_BIN) || true
	gcov -o $(COV_OBJ) $(SRCS)

.PHONY: profile_run
profile: debug
	@echo "Running program to generate gprof data..."
	./$(PROF_BIN) || true
	gprof $(PROF_BIN) gmon.out > $(BIN)/gprof.txt
	@echo "Profile written to build/gprof.txt"

.PHONY: valgrind
valgrind: debug
	valgrind \
		--tool=memcheck \
		--leak-check=full -s \
		--track-origins=yes \
		./$(DEBUG_BIN)

#=========#
#  CLEAN  #
#=========#

.PHONY: clean
clean:
	rm -rf $(BIN) logs.txt

#=======#
#  RUN  #
#=======#

.PHONY: run
run: release
	./$(REL_BIN)

#===========#
#  INSTALL  #
#===========#

install: release
	cp $(BIN)/$(PNAME) $(INSTALL_PATH)

