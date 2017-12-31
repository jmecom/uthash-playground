PROG=uthash-playground

CC=clang
CFLAGS=-Wall -Werror -Wno-pragma-once-outside-header -g

BUILD_DIR=obj/
SOURCE_DIR=src/

SRC=$(wildcard $(SOURCE_DIR)*.c)
OBJ=$(addprefix $(BUILD_DIR),$(notdir $(SRC:.c=.o)))

all: $(PROG)

obj/%.o: $(SOURCE_DIR)%.c
	$(CC) -c $(INCLUDE) -o $@ $< $(CFLAGS)

$(PROG): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

clean:
	rm -f $(BUILD_DIR)*
	rm -f ./$(PROG)

.PHONY: all
.PHONY: clean
