CC		=	g++
INSTALL		=	/usr/bin/install
TEODIR		=	/usr/local/lab64
CFLAGS		=	-O2 `pkg-config --cflags teo_draw teo` -Wall
LDFLAGS		=	-L$(TEODIR)/lib64 -I/usr/local/include/eigen3 `pkg-config --libs teo_draw teo`
LIBS		=	`pkg-config --libs teo_draw teo`
DEST		=	$(HOME)/bin
SRCS		=	panorama.cpp
OBJS		=	$(SRCS:.c=.o)
PROGRAM		=	panorama
all:		$(PROGRAM)
$(PROGRAM):  $(OBJS)
		$(CC) $(OBJS) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)
clean:;		rm -f *.o *~ $(PROGRAM)
install:	$(PROGRAM) $(INSTALL) -s $(PROGRAM) $(DEST)
		strip $(DEST)/$(PROGRAM)
