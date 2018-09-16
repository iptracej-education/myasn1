/********************************************************************

  writeBitString function
  Last Modified: Time-stamp: <01/04/14 23:46:46 kiyoshi>
*********************************************************************/
/* The VALUE of the BIT STRING is encoded as follows:
   The first octet is to set the unused bit. This value is between 0 to 7. 
   The next and sub sequent octets contain the bit values. The first
   bit is mapped to the most significant bit and next bit is mapped to
   the second most significant bit. If the unused bit is there, mapped
   0 to them. 

   Assume that the Input value as char string. I thought that
   is there any solution for the larger bytes bigger than 32 bits.
   The solution is to use GMP library. This takes almost non-limited
   bigger strings of integer. So I decide to assmue that the BIT
   STRING will be created from char string instead of 32 bit string
   especially when you use the RSA encryption

   Compile:

	gcc -o writeBit -Wall -W -traditional -ansi -pedantic smemory.o writeBitString.c

   */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include "myASN1.h"

void writeBitString(STREAM *stream, char *bit, const int length);

int main(int argc, char *argv[])
{

  STREAM *stream;
  assert(argc == 0 || argc>1);
  //writeBit(stream, argv[1], strlen(argv[1]));
  writeBitString(stream, argv[1], strlen(argv[1]));
  exit(0);
}

void writeBitString(STREAM *stream, BYTE *bit, const int length)
{

  BYTE *buffer;
  int number;
  int input;
  int unusedbit;
  int i, j = 0;
  int SHIFT = 7;
  int tmp = length;

  unusedbit = ((length % 8) > 0)? (8 - (length % 8)) : 0;
  number = (length % 8 == 0)? tmp/8+1:tmp/8+2;
  buffer = MALLOC( char, number);
  
  buffer[j++]=(BYTE)unusedbit;

  for(i=0;bit[i] != '\0';i++){
    input = bit[i] - '0';
    buffer[j] |= ((BYTE)input << SHIFT);
    SHIFT --;


    if(SHIFT < 0){

      buffer[j] |= (BYTE)input;
      SHIFT = 7;
      j++;
    }
  }
  printf("\n");

  for(i=0;i<number;i++){
    bit_print_char(buffer[i]);
    printf("%x\n", (unsigned char)buffer[i]);
  }
  FREE( buffer );  
  DEREF( buffer );

}

void bit_print_char(char a)
{
  int i;
  char mask;
  int n = CHAR_BIT;
  mask = 1 << (n-1);

  for(i=1;i<=n;++i){
    putchar(((a & mask) == 0) ? '0':'1');
    a <<=1;
    if(i % CHAR_BIT == 0 && i < n)
      putchar(' ');
  }
  putchar('\n');
}

