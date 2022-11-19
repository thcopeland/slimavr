SRC				= src
OBJ 			= obj
TEST			= tests
DEFS			= # -DDEBUG
# for some reason, -finline-small-functions destroys performance
CFLAGS			= -O2 -fno-inline-small-functions -finline-functions -finline -flto -Wall -Wextra
LDLIBS			= -lm

src_models 		= $(wildcard $(SRC)/models/*.c)
src_models_o 	= $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(src_models))
src		 		= $(wildcard $(SRC)/*.c)
src_o 			= $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(src))

all: objdirs libslimavr.a

objdirs:
	mkdir -p $(OBJ)/models

libslimavr.a: $(src_models_o) $(src_o)
	$(AR) rcs $@ $^

$(OBJ)/models/%.o: $(SRC)/models/%.c
	$(CC) $(DEFS) $(CFLAGS) $^ -c -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(DEFS) $(CFLAGS) $^ -c -o $@

test: libslimavr.a .FORCE
	$(MAKE) -j4 -C $(TEST) run

clean:
	rm -f $(src_models_o) $(src_o)
	$(MAKE) -C $(TEST) clean

.FORCE:
