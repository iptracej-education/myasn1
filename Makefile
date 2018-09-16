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
CFLAGS=-g -Wall -W -ansi 
OBJ= myASN1.o sstream.o smemory.o serror.o utils.o main.o

#Complier flags:
#   -g         -- Enable debugging
#   -Wall      -- Print all Extral warning messages 
#   -pedantic  -- Issue all the warnings demanded by strict
#                 ASNI C
#   -traditional -- Check non-traditional or non-ANSI
#   -ansi      -- Support All ANSI Standard C Program 


all:	samplepkcs7 readasn1

samplepkcs7:	$(OBJ)
		$(CC) $(CFLAGS) -o samplepkcs7 $(OBJ)


myASN1.o:	myASN1.c myASN1.h

sstream.o:	sstream.c myASN1.h

smemory.o:	smemory.c smemory.h

serror.o:	serror.c serror.h

utils.o:	utils.c myASN1.h

main.o:		main.c myASN1.h

readasn1:	
		(cd apps;$(MAKE) readasn1)

clean:  
		rm -f samplepkcs7 $(OBJ)
		rm -f *~

