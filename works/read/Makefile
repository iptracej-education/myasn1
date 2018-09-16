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
CFLAGS=	-W -Wall -ansi -I../../ -I.
OBJS=../../smemory.o ../../sstream.o ../../serror.o ../../myASN1.o
OBJS2=printOID.o printBitString.o printOctetString.o printString.o

readasn1:	$(OBJS) $(OBJS2)
		$(CC) -o readasn1 $(CFLAGS) readASN1.c $(OBJS) $(OBJS2)

readShortInteger.o:	readShortInteger.c readASN1.h

printOID.o:		printOID.c readASN1.h

printBitString.o:	printBitString.c readASN1.h

printOctetString.o:	printOctetString.c readASN1.h

printString.o:		printString.c readASN1.h

printString:
		$(CC) -o printString $(CFLAGS) printString.c $(OBJS) printOctetString.o -DMAIN

printOctetString:
		$(CC) -o printOctetString $(CFLAGS) printOctetString.c $(OBJS) -DMAIN

printBitString:
		$(CC) -o printBitString $(CFLAGS) printBitString.c $(OBJS) -DMAIN

rm:		
		rm -f readasn1

clean:  
		rm -f *~ $(OBJS2) *.o
		rm -f readasn1 printOctetString printBitString printString
                


