CC = g++
CFLAGS = -Wall -g

INCLUDES +=	\
	-I.	\
	-I./BM_RESTServices	\
	-I./BM_HTTPClient \

LFLAGS = 
LDFLAGS  = -shared

LIBS = -lpthread
	
SRCS_FILES += \
	BM_RESTServices/PostInfo.cpp \
	BM_RESTServices/PostList.cpp \
	BM_RESTServices/PostServices.cpp \
	BM_HTTPClient/BlogManHTTPClient.cpp	\
	BM_HTTPClient/HTTPClientMsg.cpp	\
	BlogManagerApp.cpp \

OBJS = $(SRCS_FILES:.cpp=.o)
LIB_OBJS = $(LIB_SRCS_FILES:.cpp=.o)

# define the executable	file 
MAIN = BlogManagerApp

.PHONY:	depend clean

noLib: $(MAIN)
	./BlogManagerApp

$(MAIN): $(OBJS) 
		$(CC) $(CFLAGS)	$(INCLUDES)	-o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.cpp.o:
		$(CC) $(CFLAGS)	$(INCLUDES)	-c $<  -o $@

clean:
		$(RM) ./*.o	*~ $(MAIN) $(TARGET_LIB) ./BM_RESTServices/*.o	./BM_HTTPClient/*.o  ./*.so.* BlogManagerApp ./libs/*.* ./libs/*

depend:	$(SRCS_FILES)
		makedepend $(INCLUDES) $^
				
# End of Makefile
