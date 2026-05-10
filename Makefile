CC     = gcc
CFLAGS = -Wall -Iinclude
LDFLAGS = -lncurses

SRC = src/Ptracer.c \
      src/SysCalls.c \
      src/TUI.c \
      src/logs.c \
      src/Filter.c \
      main.c \
      shell.c

OBJ = $(SRC:.c=.o)
TARGET = mtrace.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

src/Ptracer.o: src/Ptracer.c include/Ptracer.h include/SysCalls.h include/Logs.h include/Filter.h
	$(CC) $(CFLAGS) -c src/Ptracer.c -o src/Ptracer.o

src/SysCalls.o: src/SysCalls.c include/SysCalls.h
	$(CC) $(CFLAGS) -c src/SysCalls.c -o src/SysCalls.o

src/TUI.o: src/TUI.c include/TUI.h include/Filter.h
	$(CC) $(CFLAGS) -c src/TUI.c -o src/TUI.o

src/logs.o: src/logs.c include/Logs.h
	$(CC) $(CFLAGS) -c src/logs.c -o src/logs.o

src/filter.o: src/filter.c include/Filter.h
	$(CC) $(CFLAGS) -c src/filter.c -o src/filter.o

main.o: main.c include/Shell.h
	$(CC) $(CFLAGS) -c main.c -o main.o

shell.o: shell.c include/Shell.h include/Ptracer.h include/TUI.h include/Filter.h
	$(CC) $(CFLAGS) -c shell.c -o shell.o

clean:
	rm -f src/*.o *.o