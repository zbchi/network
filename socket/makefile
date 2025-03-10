
SRCS := $(wildcard *.cc)
OBJS := $(SRCS:.cpp=.o)
TARGET := out

CC := g++
CFLAGS := -g -Wall -Wextra 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

%.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
