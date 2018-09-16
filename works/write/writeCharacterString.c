/********************************************************************

  writeCharacterString function
  Last Modified: 15th Sunday October, 2000
*********************************************************************/
/* This function can be used with writeOctetString, writeNumericString,
   writeVisibleString, writeIA5String, writePrintableString. 

   This function encodes only the V value of TLV structure in the
   above charaster-based string. You should refer to the other
   function for this usage.

*/

#include <stdio.h>
#include <limits.h>
#include <assert.h>

#include "myASN1.h"

void writeCharacterString(STREAM *stream, BYTE *charString, int length);

int main(int argc, char *argv[])
{
  
  STREAM *stream;
  assert(argc == 0 || argc>1);
  writeCharacterString(stream, argv[1], strlen(argv[1]));
  exit(0);

}

void writeCharacterString(STREAM *stream, BYTE *charString, int length)
{

  int i;
  s2fwrite( stream, charString, length );
  for(i=0;i<length;i++)
    bit_print_char(charString[i]); 

}








