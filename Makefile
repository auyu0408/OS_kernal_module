CC := gcc
override CFLAGS += -O3 -Wall

SOURCE := app.c
BINARY := app

GIT_HOOKS := .git/hooks/applied
all: $(GIT_HOOKS) $(BINARY) mod ins_mod

debug: CFLAGS += -DDEBUG -g
debug: $(GIT_HOOKS) $(BINARY)

$(GIT_HOOKS):
	scripts/install-git-hooks

$(BINARY): $(SOURCE) $(patsubst %.c, %.h, $(SOURCE))
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f *.o $(BINARY)
	cd module && make clean
	cd ..

mod:
	cd module && make
	cd ..

ins_mod:
	cd module && make ins
	cd ..

rm_mod:
	cd module && make rm
	cd ..

del_app:
	rm -f *.o $(BINARY)

del_mod:
	cd module && make clean