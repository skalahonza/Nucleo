
CC:=ccache $(CC)
CFLAGS+=-O2 -std=gnu99 -g
CFLAGS+=-pedantic -Wall
LDFLAGS+=-lpthread

OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

TARGET=program

program: $(TARGET)

$(OBJS): %.o: %.c
	$(CC) -c $< $(CFLAGS) $(CPPFLAGS) -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

clean:
	$(RM) $(OBJS) $(TARGET)

test1:
	./program /dev/ttyACM0

test2:
	./program /dev/ttyACM0 commands.txt

run: 
	./program