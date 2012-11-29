OBJS=xml.o xml_parser.o uniqueTags.o
CFLAGS=-g -I. -Wall -Wextra -lfuse -D_FILE_OFFSET_BITS=64 libroxml/libroxml.so.0
#DEFINES=-D_FILE_OFFSET_BITS=64
BIN=xml
CC=gcc

%.o:%.c
	$(CC) $(CFLAGS) $(DEFINES) -o $@ -c $<

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(DEFINES) -o $(BIN) $^

clean:
	rm $(BIN) $(OBJS)
