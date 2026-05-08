CC = gcc

CFLAGS = -Wall -Iinclude

SRC = src/Ptracer.c src/SysCalls.c src/TUI.c src/logs.c main.c shell.c

OBJ = $(SRC:.c=.o)

TARGET = mtrace.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

src/Ptracer.o: src/Ptracer.c include/Ptracer.h include/SysCalls.h include/Logs.h
	$(CC) $(CFLAGS) -c src/Ptracer.c -o src/Ptracer.o

src/SysCalls.o: src/SysCalls.c include/SysCalls.h
	$(CC) $(CFLAGS) -c src/SysCalls.c -o src/SysCalls.o

src/TUI.o: src/TUI.c include/TUI.h
	$(CC) $(CFLAGS) -c src/TUI.c -o src/TUI.o

src/logs.o: src/logs.c include/Logs.h
	$(CC) $(CFLAGS) -c src/logs.c -o src/logs.o

main.o: main.c include/Shell.h
	$(CC) $(CFLAGS) -c main.c -o main.o

shell.o: shell.c include/Shell.h include/Ptracer.h include/TUI.h
	$(CC) $(CFLAGS) -c shell.c -o shell.o

clean:
	rm -f src/*.o *.o $(TARGET)