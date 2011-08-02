#
# Defaults

CC			= gcc
CFLAGS		= -Iinclude -Wall -O2 -DNDEBUG
LDFLAGS		= -lpthread

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.out: %.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) $<

#
# Default target

all: obj

#
# Objects

OBJS		=	src/common.o

TEST_OBJS	=	test/test_hash.out \
				test/test_vector.out

obj: $(OBJS)

test: obj $(TEST_OBJS)

clean:
	find . -name '*.o' -exec rm {} \;
	find . -name '*.out' -exec rm {} \;

