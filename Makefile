# If the FREEBSD headers path is not set we will try to use the relative path
ifeq ($(BSD_INC),)
BSD_INC := ../freebsd-headers
endif

# Project name
PROJ_NAME := OniFramework

# C++ compiler
CPPC	:=	g++

# C compiler
CC		:=	gcc

# Archiver
AS		:=	ar

# Objcopy
OBJCOPY	:=	objcopy

# Output directory
ifeq ($(OUT_DIR),)
OUT_DIR	:=	build
endif

# Source directory
SRC_DIR	:=	src

# Include directory paths
I_DIRS	:=	-I. -Iinclude -I$(SRC_DIR) -I$(BSD_INC) -Idepends/include

# Library directory paths
L_DIRS	:=	-L.	-Llib

# Included libraries
LIBS	:= 

# C++ Flags
CFLAGS	:= $(I_DIRS) -D_KERNEL -D_DEBUG -std=c11 -O2 -fno-builtin -nostartfiles -nodefaultlibs -nostdlib -nostdinc -fcheck-new -ffreestanding -fno-strict-aliasing -fno-exceptions -fno-asynchronous-unwind-tables -Wall -m64 -fPIC -Werror -Wno-unknown-pragmas

# Assembly flags
SFLAGS	:= -nostartfiles -nodefaultlibs -nostdlib -fPIC

# Linker flags
LFLAGS	:= $(L_DIRS) -nostartfiles -nodefaultlibs -nostdlib -fPIC -Xlinker -T link.x -Wl,--build-id=none

# Calculate the listing of all file paths
CFILES	:=	$(wildcard $(SRC_DIR)/*.c)
CPPFILES :=	$(wildcard $(SRC_DIR)/*.cpp)
SFILES	:=	$(wildcard $(SRC_DIR)/*.s)
OBJS	:=	$(patsubst $(SRC_DIR)/%.c, $(OUT_DIR)/$(SRC_DIR)/%.o, $(CFILES)) $(patsubst $(SRC_DIR)/%.s, $(OUT_DIR)/$(SRC_DIR)/%.o, $(SFILES)) $(patsubst $(SRC_DIR)/%.cpp, $(OUT_DIR)/$(SRC_DIR)/%.o, $(CPPFILES))

ALL_CPP := $(shell find $(SRC_DIR)/ -type f -name '*.cpp')
ALL_C	:= $(shell find $(SRC_DIR)/ -type f -name '*.c')
ALL_S	:= $(shell find $(SRC_DIR)/ -type f -name '*.s')

ALL_SOURCES :=  $(ALL_S) $(ALL_C) $(ALL_CPP)
#ALL_OBJ := $(addprefix $(OUT_DIR)/, $(SOURCES:%.s=%.0)) $(addprefix $(OUT_DIR)/, $(SOURCES:%.c=%.o)) $(addprefix $(OUT_DIR)/, $(SOURCES:%.cpp=%.o))
TO_BUILD := $(ALL_S:$(SRC_DIR)%=$(OUT_DIR)/$(SRC_DIR)%) $(ALL_C:$(SRC_DIR)%=$(OUT_DIR)/$(SRC_DIR)%) $(ALL_CPP:$(SRC_DIR)%=$(OUT_DIR)/$(SRC_DIR)%)
ALL_OBJ_CPP := $(TO_BUILD:.cpp=.o)
ALL_OBJ_C := $(ALL_OBJ_CPP:.c=.o)
ALL_OBJ := $(ALL_OBJ_C:.s=.o)

# Target name
TARGET = $(PROJ_NAME).a

$(TARGET): $(ALL_OBJ)
	@echo Compiling $(PROJ_NAME)...
	@$(AS) rcs $(TARGET) $(ALL_OBJ)

$(OUT_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $< ..."
	@$(CC) $(CFLAGS) $(IDIRS) -c $< -o $@

$(OUT_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp	
	@echo "Compiling $< ..."
	@$(CPPC) $(CFLAGS) $(IDIRS) -c $< -o $@

$(OUT_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.s
	@echo "Compiling $< ..."
	@$(CC) -c -o $@ $< $(SFLAGS)

.PHONY: clean

clean:
	@echo "Cleaning project..."
	@rm -f $(TARGET) $(PAYLOAD) $(shell find $(OUT_DIR)/ -type f -name '*.o')

create:
	@echo "Creating directories..."
	@mkdir -p $(shell find '$(SRC_DIR)/' -type d -printf '$(OUT_DIR)/%p\n')