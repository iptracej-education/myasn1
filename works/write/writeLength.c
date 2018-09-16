#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include "myASN1.h"

/********************************************************************

  writeLength function
  Last Modified: Novemver 22 2000

*********************************************************************/
/*
  The Length(L) part comes immediately after the Type(T) part in every 
  encording and has at least one octet.

  The length part represents the number of octets for the Value part.
  But not always...

  The length part has three variants, "short", "long", and "indefinite".
  The small and long represents what I mentioned earlier, the number
  of octets in the Value part. The indefinite puts the speccial
  mark at the end of the octets. (You can have the value part 
  without any knowledge of the number of length.)

  The short variant is used for Values with the less-than 127 octets.
  (off course, no negative value)

  8 7 6 5 4 3 2 1   
  0 L L L L L L L  ( 0 represents less-than 127 and L represents the
                      length)

  The long variant is used for Values larger than 128 octets.

  8 7 6 5 4 3 2 1  8 7 6 5 4 3 2 1    8 7 6 5 4 3 2 1 
  1 (0 < n < 127)  L L L L L L L L ...L L L L L L L L 
  
          1                2              N + 1

  The 1st octet represents the length of length with the most significant
  bit to 1. The 2nd to N+1 octets represent the number of length.

  The indefinite variant is used for any constructed encoding. 

  This starts with  8 7 6 5 4 3 2 1
                    1 0 0 0 0 0 0 0

		    ...values..
  
  and ends with  8 7 6 5 4 3 2 1  8 7 6 5 4 3 2 1
                 0 0 0 0 0 0 0 0  0 0 0 0 0 0 0 0    
              
 */ 

/*
int main(int argc, char *argv[])
{

  assert(argc == 2);
  writeLength(NULL,atol(argv[1]));
  exit(0);
}
*/

/********************************************************************
  Assume that the length will not be over 2147483647 (octets), 
  which means the 215M bytes for the Content Value. 

  This leads to the 4 octets max for the length of length in design.
  
*********************************************************************/
void writeLength(STREAM *stream, const long length)
{
  
  BYTE buffer[5];
  int len=0;
  int i;

  /* length is less than 128 */
  if(length < 0x00000080L){
    buffer[len] = (BYTE)length;
    len++;
  }
  /* When length is more than 255, determine the value of the 
     length of length */
  if(length > 0x00FFFFFFL)                  /* N > 16777215 (four octets */
    buffer[len++] = (BYTE)0x84L;             
  else if(length > 0x0000FFFFL)             /* 65535 < N < 16777215 (three octets) */
    buffer[len++] = (BYTE)0x83L;
  else if(length > 0x000000FFL)             /* 255 < N < 65535 (two octets) */
    buffer[len++] = (BYTE)0x82L;
  else if(length >= 0x00000080L)            /* 127 < N < 255 (one octet) */
    buffer[len++] = (BYTE)0x81L;

  /* Determine the number of bytes to encode the length
     and encode it into a temporary buffer */

  if(length > 0x00FFFFFFL)                  
    buffer[len++] = (BYTE)(length >> 24);
  if(length > 0x0000FFFFL)                  
    buffer[len++] = (BYTE)(length >> 16);
  if(length > 0x000000FFL)                  
    buffer[len++] = (BYTE)(length >> 8);
  if(length >= 0x00000080L)                 
    buffer[len++] = (BYTE)length;

  for(i =0; i<len; i++){
    bit_print_char(buffer[i]);
  }
  // s2fwrite(stream, buffer, len);
  
 
}







