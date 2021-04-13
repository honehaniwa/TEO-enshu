CC		=	gcc
INSTALL	=	/usr/bin/install
TEODIR	=	/usr/local
CFLAGS	=	-O2 -I $(TEODIR)/include -Wall -DTEO_DEBUG_ALL
LDFLAGS	=	-L $(TEODIR)/lib

LIBS	=	-lteo -lm
DEST	=	$(HOME)/bin
SRCS	=	copy.c
OBJS	=	$(SRCS:.c=.o)
PROGRAM	=	teo_copy
all:		$(PROGRAM)
$(PROGRAM):	$(OBJS)
			$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)
clean:;		rm -f *.o *~ $(PROGRAM)
install:	$(PROGRAM)
			$(INSTALL) -s $(PROGRAM) $(DEST)
			strip $(DEST)/$(PROGRAM)