TARGET = writer

CROSS_COMPILE ?=
CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -Werror
LDFLAGS = -static
all: $(TARGET)

$(TARGET): writer.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) writer.c

clean:
	rm -f $(TARGET) *.o

