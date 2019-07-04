CC:=gcc

# Directories
L_SRC_DIR         := ../../lib
SRC_DIR           := ../../src
COMMON_SRC_DIR    := $(SRC_DIR)/common
UTILS_SRC_DIR     := $(SRC_DIR)/io_utils
SCODA_SRC_DIR     := $(SRC_DIR)/scoda
FA2_SRC_DIR       := $(SRC_DIR)/FA2
LO_EVAL_SRC_DIR   := $(SRC_DIR)/layouteval
OBJ_DIR           := obj
COMMON_OBJ_DIR    := $(OBJ_DIR)/common
UTILS_OBJ_DIR     := $(OBJ_DIR)/io_utils
SCODA_OBJ_DIR     := $(OBJ_DIR)/scoda
FA2_OBJ_DIR       := $(OBJ_DIR)/FA2
LO_EVAL_OBJ_DIR   := $(OBJ_DIR)/layouteval

# Compile with CUDA support by default
CUDA_SUPPORT ?= 1
ifeq ($(CUDA_SUPPORT), 1)
	CC:=nvcc
	MY_CXXFLAGS:=-Xcompiler -Wextra -Xcompiler -Wall
else
	MY_CXXFLAGS:=-Wextra -Wall
endif

# Debug compilation, disable by default
DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CXXFLAGS:=-pg -g
	NVCCFLAGS:=-G
	O_LVL:=-O0
else
	NVCCFLAGS:=-D NDEBUG
	O_LVL:=-O3
endif

# C preprocessor flags
CPPFLAGS :=-D NO_FREETYPE

# C/C++ compiler flags
CFLAGS   :=$(O_LVL)
CXXFLAGS :=$(CXXFLAGS) $(O_LVL) -std=c++11

# Linker flags
LDFLAGS  :=-lc -lm -lstdc++ -lpng

# inspiration: https://devhints.io/makefile
# more inspiration: https://gist.github.com/isaacs/62a2d1825d04437c6f08
# src./obj. files
COMMON_SRCS       := $(wildcard $(COMMON_SRC_DIR)/*.cpp)
COMMON_OBJS       := $(COMMON_SRCS:$(COMMON_SRC_DIR)/%.cpp=$(COMMON_OBJ_DIR)/%.o)

UTILS_SRCS        := $(wildcard $(UTILS_SRC_DIR)/*.cpp)
UTILS_OBJS        := $(UTILS_SRCS:$(UTILS_SRC_DIR)/%.cpp=$(UTILS_OBJ_DIR)/%.o)

SCODA_SRCS        := $(wildcard $(SCODA_SRC_DIR)/*.cpp)
SCODA_OBJS        := $(SCODA_SRCS:$(SCODA_SRC_DIR)/%.cpp=$(SCODA_OBJ_DIR)/%.o)

FA2_SRCS          := $(wildcard $(FA2_SRC_DIR)/*.cpp)
FA2_OBJS          := $(FA2_SRCS:$(FA2_SRC_DIR)/%.cpp=$(FA2_OBJ_DIR)/%.o)

CUDA_FA2_SRCS     := $(wildcard $(FA2_SRC_DIR)/*.cu)
CUDA_FA2_OBJS     := $(CUDA_FA2_SRCS:$(FA2_SRC_DIR)/%.cu=$(FA2_OBJ_DIR)/%.o)
CUDA_FA2_DEPS     := $(wildcard $(FA2_SRC_DIR)/*.cuh)

LO_EVAL_SRCS      := $(wildcard $(LO_EVAL_SRC_DIR)/*.cpp)
LO_EVAL_OBJS      := $(LO_EVAL_SRCS:$(LO_EVAL_SRC_DIR)/%.cpp=$(LO_EVAL_OBJ_DIR)/%.o)

PNGWRITER_SRCS    := $(L_SRC_DIR)/pngwriter/src/pngwriter.cc
PNGWRITER_OBJS    := $(PNGWRITER_SRCS:$(L_SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)

SCODA_EXE_SRC     := $(SRC_DIR)/scoda_exec.cpp

GRAPH_VIEWER_SRC  := $(SRC_DIR)/graph_viewer.cpp

STRESS_EXE_SRC    := $(SRC_DIR)/stress_exec.cpp

CPP_SRCS          := $(COMMON_SRCS) $(UTILS_SRCS) $(SCODA_SRCS) $(FA2_SRCS) $(LO_EVAL_SRCS) $(SCODA_EXE_SRC) $(GRAPH_VIEWER_SRC) $(PNGWRITER_SRCS)
CUDA_SRCS         := $(CUDA_FA2_SRCS)
SOURCES           := $(C_SRC) $(CPP_SRCS)
GV_OBJECTS        := $(PNGWRITER_OBJS) $(COMMON_OBJS) $(UTILS_OBJS) $(SCODA_OBJS) $(FA2_OBJS)
SCODA_EXE_OBJS    := $(COMMON_OBJS) $(SCODA_OBJS)
STRESS_EXE_OBJS   := $(COMMON_OBJS) $(LO_EVAL_OBJS)
CUDA_OBJS         := $(CUDA_FA2_OBJS)

ifeq ($(CUDA_SUPPORT), 1)
SOURCES           := $(SOURCES) $(CUDA_SRCS)
GV_OBJECTS        := $(GV_OBJECTS) $(CUDA_FA2_OBJS)
endif

# Generate dependency (.h, .hpp)
# ala http://stackoverflow.com/questions/2394609/makefile-header-dependencies
depend: .depend
.depend: $(SOURCES)
	rm -f .depend
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -M $^ >> .depend

include ./.depend

all: scoda_exec graph_viewer stress_exec

scoda_exec: $(SCODA_EXE_OBJS) $(SCODA_EXE_SRC)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) $(LDFLAGS) -o $@ $^

graph_viewer: $(GV_OBJECTS) $(GRAPH_VIEWER_SRC)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) $(LDFLAGS) -o $@ $^

stress_exec: $(STRESS_EXE_OBJS) $(STRESS_EXE_SRC)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) $(LDFLAGS) -o $@ $^

$(COMMON_OBJS): $(COMMON_SRCS)
	@mkdir -p $(@D)
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) -o $@ $(@:$(COMMON_OBJ_DIR)/%.o=$(COMMON_SRC_DIR)/%.cpp)

$(UTILS_OBJS): $(UTILS_SRCS)
	@mkdir -p $(@D)
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) -o $@ $(@:$(UTILS_OBJ_DIR)/%.o=$(UTILS_SRC_DIR)/%.cpp)

$(SCODA_OBJS): $(SCODA_SRCS)
	@mkdir -p $(@D)
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) -o $@ $(@:$(SCODA_OBJ_DIR)/%.o=$(SCODA_SRC_DIR)/%.cpp)

$(LO_EVAL_OBJS): $(LO_EVAL_SRCS)
	@mkdir -p $(@D)
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) -o $@ $(@:$(LO_EVAL_OBJ_DIR)/%.o=$(LO_EVAL_SRC_DIR)/%.cpp)

$(FA2_OBJS): $(FA2_SRCS)
	@mkdir -p $(@D)
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $(@:$(FA2_OBJ_DIR)/%.o=$(FA2_SRC_DIR)/%.cpp)

$(CUDA_FA2_OBJS): $(CUDA_FA2_SRCS) $(CUDA_FA2_DEPS)
	@mkdir -p $(@D)
	nvcc --device-c $(NVCCFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $(@:$(OBJ_DIR)/%.o=$(SRC_DIR)/%.cu)

$(PNGWRITER_OBJS): $(PNGWRITER_SRCS)
	@mkdir -p $(@D)
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $(@:$(OBJ_DIR)/%.o=$(L_SRC_DIR)/%.cc)

clean:
	rm -rf scoda_exec graph_viewer stress_exec $(OBJ_DIR)/* ./.depend

.PHONY: all clean depend
