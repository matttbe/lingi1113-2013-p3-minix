CFLAGS+=-Wall -Werror -g
LDFLAGS+= -L.
SOURCES=test.c
SOURCES_BASH=test_fragment_file.sh
OBJECTS=$(SOURCES:.c=.o)
OUT=test
# not gcc!!
CC = cc

all: $(SOURCES_BASH) $(OUT)
	bash $(SOURCES_BASH)
	./$(OUT)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.PHONY: clean mrproper

clean:
	@rm $(OBJECTS)

mrproper: clean
	@rm $(EXEC)
