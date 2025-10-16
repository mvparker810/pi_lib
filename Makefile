# === Compiler & Flags ===
CC      := cc
CFLAGS  := -Wall -Wextra -std=c11
TARGETS := block_blast space_invaders fluid_sim snake

# === Default target ===
all: $(TARGETS)

# === Link each executable ===
block_blast: block_blast.o pilib_impl.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

space_invaders: space_invaders.o pilib_impl.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

fluid_sim: fluid_sim.o pilib_impl.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

snake: snake.o pilib_impl.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

# === Compile object files ===
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

pilib_impl.o: pilib_impl.c pilib_impl.h
	$(CC) $(CFLAGS) -c pilib_impl.c 

# === Run targets ===
run-%: %
	./$<

# Shortcut: `make run-block_blast`
#          	`make run-space_invaders`
#          	`make run-fluid_sim`
#          	`make run-snake`

# === Clean ===
clean:
	rm -f *.o $(TARGETS)

# === Test all ===
test: all
	@echo "Build complete. Run individual games with make run-<name>"

.PHONY: all clean test run-%
