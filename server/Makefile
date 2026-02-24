# Default to gcc if no CROSS_COMPILE is provided
ifeq ($(CROSS_COMPILE),)
	CC = gcc
else
	CC = $(CROSS_COMPILE)gcc
endif

CFLAGS = -g -Wall -Werror
TARGET = aesdsocket
LDFLAGS = -lpthread

all: $(TARGET)
default: $(TARGET)

$(TARGET): aesdsocket.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o
