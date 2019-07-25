SHELL	= /bin/sh
# To enable SSE, erase '#' below
CC     = gcc #-DUSE_SSE 

#---optimize---
# To disable the optimizing of gcc, comment out below line by '#'.
CFLAGS += -O3

# To enable SSE, erase '#' below
LDFLAGS = -lm #-mavx
DEFS	= 

CSRCS	= main.c init.c util.c move.c bit_util.c get_feature.c cnn_calc.c 
# ===========================================================================

OBJS	= $(CSRCS:.c=.o)
TARGET  = deltaGo.exe

.PHONY: clean
.SUFFIXES: .c .o

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $(DEFS) $(INCDIR) -o $@ $< $(LDFLAGS)

clean:
	$(RM) $(OBJS) $(TARGET) core *.o *~ 
