# https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
# 'make	depend'	uses makedepend	to automatically generate dependencies 
#				(dependencies are added	to end of Makefile)
# 'make'		build executable file 'mycc'
# 'make	clean'	removes	all	.o and executable files
#

# define the C compiler	to use
CC = g++
CFLAGS = -Wall -g

INCLUDES = -I/.	-I/home/newhall/include	 -I../include

#LFLAGS	= -L/home/newhall/lib  -L../lib
LFLAGS = 

LIBS = -lpthread

SRCS_FILES = BlogManagerApp.cpp	RESTServices.cpp

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
