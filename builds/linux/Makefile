CC:=gcc

# Directories
L_SRC_DIR         := ../../lib
SRC_DIR           := ../../src
COMMON_SRC_DIR    := $(SRC_DIR)/common
UTILS_SRC_DIR     := $(SRC_DIR)/utils
IO_SRC_DIR        := $(UTILS_SRC_DIR)/IO
GV_SRC_DIR        := $(SRC_DIR)/graph_viewer
SCODA_SRC_DIR     := $(SRC_DIR)/scoda
FA2_SRC_DIR       := $(SRC_DIR)/FA2
LO_EVAL_SRC_DIR   := $(SRC_DIR)/layouteval

OBJ_DIR           := obj
COMMON_OBJ_DIR    := $(OBJ_DIR)/common
UTILS_OBJ_DIR     := $(OBJ_DIR)/utils
IO_OBJ_DIR        := $(UTILS_OBJ_DIR)/IO
GV_OBJ_DIR        := $(OBJ_DIR)/graph_viewer
SCODA_OBJ_DIR     := $(OBJ_DIR)/scoda
FA2_OBJ_DIR       := $(OBJ_DIR)/FA2
LO_EVAL_OBJ_DIR   := $(OBJ_DIR)/layouteval

#######################################################
################# BEGIN CUDA SUPPORT FLAGS ############
#######################################################
# Compile with CUDA support by default
CUDA_SUPPORT ?= 1
ifeq ($(CUDA_SUPPORT), 1)
	CC:=nvcc
	MY_CXXFLAGS:=-Xcompiler -Wextra -Xcompiler -Wall
	# Debug compilation, disable by default
	DEBUG ?= 0
	ifeq ($(DEBUG), 1)
		MY_CXXFLAGS:=$(MY_CXXFLAGS) -Xcompiler -pg
		CXXFLAGS:=-pg -g
		NVCCFLAGS:=-G
		O_LVL:=-O0
	else
		NVCCFLAGS:=-D NDEBUG
		O_LVL:=-O3
	endif

	PROFILE ?= 0
	ifeq ($(PROFILE), 1)
		# TODO: Do we need Xcompiler here? Does this even work with nvcc?
		CXXFLAGS:=$(CXXFLAGS) -fprofile-arcs -ftest-coverage
		LDFLAGS:=-lgcov --coverage
	endif
########################################################
################# END CUDA SUPPORT FLAGS ###############
########################################################

###### NO CUDA SUPPORT FLAGS
else
	MY_CXXFLAGS:=-Wextra -Wall
	# Debug compilation, disable by default
	DEBUG ?= 0
	ifeq ($(DEBUG), 1)
		CXXFLAGS:=-pg -g
		O_LVL:=-O0
	else
		O_LVL:=-O3
	endif

	PROFILE ?= 0
	ifeq ($(PROFILE), 1)
		CXXFLAGS:=$(CXXFLAGS) -fprofile-arcs -ftest-coverage
		LDFLAGS:=-lgcov --coverage
	endif
endif
###### END NO CUDA SUPPORT FLAGS

###### SHARED FLAGS
# Override -O3 optimization if requested.
FAST ?= 0
ifeq ($(FAST), 1)
	O_LVL:=-O0
endif

# C preprocessor flags
CPPFLAGS :=-D NO_FREETYPE

# C/C++ compiler flags
CFLAGS   :=$(O_LVL)
CXXFLAGS :=$(CXXFLAGS) $(O_LVL) -std=c++11

# Linker flags
LDFLAGS  :=-lc -lm -lstdc++ -lpng $(LDFLAGS)
###### END SHARED FLAGS

# inspiration: https://devhints.io/makefile
# more inspiration: https://gist.github.com/isaacs/62a2d1825d04437c6f08
# src./obj. files
COMMON_SRCS       := $(wildcard $(COMMON_SRC_DIR)/*.cpp)
COMMON_OBJS       := $(COMMON_SRCS:$(COMMON_SRC_DIR)/%.cpp=$(COMMON_OBJ_DIR)/%.o)

UTILS_SRCS        := $(wildcard $(UTILS_SRC_DIR)/*.cpp)
UTILS_OBJS        := $(UTILS_SRCS:$(UTILS_SRC_DIR)/%.cpp=$(UTILS_OBJ_DIR)/%.o)

GV_SRCS           := $(wildcard $(GV_SRC_DIR)/*.cpp)
GV_OBJS           := $(GV_SRCS:$(GV_SRC_DIR)/%.cpp=$(GV_OBJ_DIR)/%.o)

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

IO_SRCS           := $(wildcard $(IO_SRC_DIR)/*.cpp)
IO_OBJS           := $(IO_SRCS:$(IO_SRC_DIR)/%.cpp=$(IO_OBJ_DIR)/%.o)
IO_OBJS           := $(IO_OBJS) $(PNGWRITER_OBJS)

SCODA_EXE_SRC     := $(SRC_DIR)/scoda_exec.cpp

GV_EXE_SRC        := $(SRC_DIR)/graph_viewer_exec.cpp

STRESS_EXE_SRC    := $(SRC_DIR)/stress_exec.cpp

STRESS_RAND_SRC   := $(SRC_DIR)/stress_rand_sampling.cpp

ALL_PAIRS_EXE_SRC := $(SRC_DIR)/all_pairs_exec.cpp

CPP_SRCS          := $(COMMON_SRCS) $(UTILS_SRCS) $(IO_SRCS) $(SCODA_SRCS) \
					 $(FA2_SRCS) $(LO_EVAL_SRCS) $(GV_SRCS) $(SCODA_EXE_SRC) \
					 $(GV_EXE_SRC) $(STRESS_EXE_SRC) $(PNGWRITER_SRCS) $(ALL_PAIRS_EXE_SRC)
CUDA_SRCS         := $(CUDA_FA2_SRCS)
SOURCES           := $(C_SRC) $(CPP_SRCS)
GV_EXE_OBJS       := $(COMMON_OBJS) $(IO_OBJS) $(GV_OBJS) $(SCODA_OBJS) $(FA2_OBJS)
SCODA_EXE_OBJS    := $(COMMON_OBJS) $(SCODA_OBJS) $(IO_OBJS)
STRESS_EXE_OBJS   := $(COMMON_OBJS) $(LO_EVAL_OBJS) $(IO_OBJS)
STRESS_RAND_OBJS  := $(COMMON_OBJS) $(LO_EVAL_OBJS) $(IO_OBJS) # Might want to have an extra directory for ogdf.
ALL_PAIRS_EXE_OBJS:= $(COMMON_OBJS) $(LO_EVAL_OBJS)
CUDA_OBJS         := $(CUDA_FA2_OBJS)

ifeq ($(CUDA_SUPPORT), 1)
SOURCES           := $(SOURCES) $(CUDA_SRCS)
GV_EXE_OBJS       := $(GV_EXE_OBJS) $(CUDA_FA2_OBJS)
endif

# Generate dependency (.h, .hpp)
# ala http://stackoverflow.com/questions/2394609/makefile-header-dependencies
depend: .depend
.depend: $(SOURCES)
	rm -f .depend
	$(CC) $(CXXFLAGS) $(CPPFLAGS) -M $^ >> .depend

include ./.depend

all: scoda_exec graph_viewer_exec stress_exec stress_rand_sampling

scoda_exec: $(SCODA_EXE_OBJS) $(SCODA_EXE_SRC)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) $(LDFLAGS) -o $@ $^

graph_viewer_exec: $(GV_EXE_OBJS) $(GV_EXE_SRC)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) $(LDFLAGS) -o $@ $^

stress_exec: $(STRESS_EXE_OBJS) $(STRESS_EXE_SRC)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) $(LDFLAGS) -o $@ $^

stress_rand_sampling: $(STRESS_RAND_OBJS) $(STRESS_RAND_SRC)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) $(LDFLAGS) -o $@ $^

all_pairs_exec: $(ALL_PAIRS_EXE_OBJS) $(ALL_PAIRS_EXE_SRC)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) $(LDFLAGS) -o $@ $^

$(COMMON_OBJS): $(COMMON_SRCS)
	@mkdir -p $(@D)
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) -o $@ $(@:$(COMMON_OBJ_DIR)/%.o=$(COMMON_SRC_DIR)/%.cpp)

$(UTILS_OBJS): $(UTILS_SRCS)
	@mkdir -p $(@D)
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) -o $@ $(@:$(UTILS_OBJ_DIR)/%.o=$(UTILS_SRC_DIR)/%.cpp)

$(IO_OBJS): $(IO_SRCS)
	@mkdir -p $(@D)
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) -o $@ $(@:$(IO_OBJ_DIR)/%.o=$(IO_SRC_DIR)/%.cpp)

$(GV_OBJS): $(GV_SRCS)
	@mkdir -p $(@D)
	$(CC) -c $(CPPFLAGS) $(CXXFLAGS) $(MY_CXXFLAGS) -o $@ $(@:$(GV_OBJ_DIR)/%.o=$(GV_SRC_DIR)/%.cpp)

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
	rm -rf scoda_exec graph_viewer_exec stress_exec all_pairs_exec $(OBJ_DIR)/* ./.depend

.PHONY: all clean depend
