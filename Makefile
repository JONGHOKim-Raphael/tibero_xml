SRCS            = $(wildcard *.c)
OBJS            = $(SRCS:.c=.o)
LIBS            = -lexpat -L$(TB_HOME)/client/lib -ltbcli
TARGET          = tibero_xml

#
DEBUG_TARGET    = $(TARGET)-debug
DEBUG_FLAGS     = -DDEBUG

all: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

$(OBJS): $(SRCS)
	$(CC) -c $(SRCS)

debug: $(SRCS)
	$(CC) -c $(SRCS) $(DEBUG_FLAGS)
	$(CC) -o $(DEBUG_TARGET) $(OBJS) $(LIBS)

clean:
	rm -f $(TARGET) $(DEBUG_TARGET)
	rm -f *.o
