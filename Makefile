SRCS            = $(wildcard *.c)
OBJS            = $(SRCS:.c=.o)
LIBS            = -lexpat -L$(TB_HOME)/client/lib -ltbcli
TARGET          = tibero_xml

all: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

$(OBJS): $(SRCS)
	$(CC) -c $(SRCS)

clean:
	rm -f $(TARGET)
	rm -f *.o
