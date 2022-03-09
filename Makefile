SRC				= src
OBJ 			= obj
TEST			= tests
DEFS			= -D CHECKS=1
CFLAGS			= -O2 -flto -Wall -Wextra

src_models 		= $(wildcard $(SRC)/models/*.c)
src_models_o 	= $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(src_models))
src		 		= $(wildcard $(SRC)/*.c)
src_o 			= $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(src))


all: objdirs slimavr.a

objdirs:
	mkdir -p $(OBJ)/models

slimavr.a: $(src_models_o) $(src_o)
	ar rcs libslimavr.a $^

$(OBJ)/models/%.o: $(SRC)/models/%.c
	$(CC) $(DEFS) $(CFLAGS) $^ -c -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(DEFS) $(CFLAGS) $^ -c -o $@

test: slimavr.a
	make -C $(TEST)

clean:
	rm -f $(src_models_o) $(src_o)
	make -C $(TEST) clean
