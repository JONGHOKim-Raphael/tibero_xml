SRCS            = $(wildcard *.c)
OBJS            = $(SRCS:.c=.o)
LIBS            = -lexpat
TARGET          = tibero_xml

all: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

$(OBJS): $(SRCS)
	$(CC) -c $(SRCS)

clean:
	rm -f $(TARGET)
	rm -f *.o
