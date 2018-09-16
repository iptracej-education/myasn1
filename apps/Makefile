#------------------------------------------------------------------#
#         Makefile for Unix Systems                                #
#         using a GNU C complier                                   #
#                                                                  #
#         tag: rule                                                #
#              execute                                             #
#                                                                  # 
#         Use TAB for space                                        #
#------------------------------------------------------------------#

CC=gcc
CFLAGS=	-W -Wall -ansi -I. -I../
OBJS=../smemory.o ../sstream.o ../serror.o ../myASN1.o

readasn1:	readasn1.c readasn1.h $(OBJS)
		$(CC) -o readasn1 $(CFLAGS) readasn1.c $(OBJS) 


rmapp:		
		rm -f readasn1

clean:  
		rm -f *~ $(OBJS2)
		rm -f *.o



