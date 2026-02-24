# 設定預設編譯器，若有傳入 CROSS_COMPILE 變數則加上前綴
CC = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -Werror -g

# 預設目標
all: writer

writer: writer.c
	$(CC) $(CFLAGS) -o writer writer.c

# 清除目標
clean:
	rm -f writer *.o