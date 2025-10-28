# MPI compiler and flags
MPICC ?= mpicc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -D_POSIX_C_SOURCE=200809L -I./src
LDFLAGS ?= -lm

# Target binary executable
TARGET := bin/CMsketch

# Source files
SRC := src/main.c src/cms.c src/file_io.c

# Objects and dependency files
OBJ := $(SRC:.c=.o)
DEPS := $(OBJ:.o=.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	$(MPICC) $(LDFLAGS) -o $@ $(OBJ)

# Generic rule: compile .c -> .o and generate deps
%.o: %.c
	$(MPICC) $(CFLAGS) -c $< -o $@ -MMD -MP

-include $(DEPS)

clean:
	rm -f $(OBJ) $(DEPS) $(TARGET)
