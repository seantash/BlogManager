CC = g++
CFLAGS = -Wall -g

INCLUDES = -I/.	

#L../lib
LFLAGS = 

LIBS = -lpthread

SRCS_FILES = BlogManagerApp.cpp

OBJS = $(SRCS_FILES:.cpp=.o)

# define the executable	file 
MAIN = BlogManagerApp

.PHONY:	depend clean

all: $(MAIN)
	./BlogManagerApp

$(MAIN): $(OBJS) 
		$(CC) $(CFLAGS)	$(INCLUDES)	-o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.cpp.o:
		$(CC) $(CFLAGS)	$(INCLUDES)	-c $<  -o $@

clean:
		$(RM) -rf ./*.o	*~ $(MAIN)

depend:	$(SRCS_FILES)
		makedepend $(INCLUDES) $^

# End of Makefile
