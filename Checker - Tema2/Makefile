CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = search_index
SRCS = /home/essio/Facultate/An1/Sem2/SDA/tema2/main.c
OBJS = main.o

all: build

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $(SRCS) -o $(OBJS)

run: build
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all build clean run