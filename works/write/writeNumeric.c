#include <stdio.h>
#include <limits.h>
#include "myASN1.h"

void writeNumeric(STREAM *stream, const long integer)
{
  
  BOOLEAN needsZERO = TRUE;
  BYTE buffer[5];
  int length = 0;
  const long MINUS_24BITINTEGER = -16777216;
  const long MINUS_16BITINTEGER = -65536;
  const long MINUS_8BITINTEGER = -256;

  /* Determine the number of bytes necessary to encode 
     the integer and encode it into a temporary buffer */

  /* In case of negative integer */
  if(integer < 0){
    
    if(integer < MINUS_24BITINTEGER) /* Modifiled by Kiyoshi @ 12/19/2000 */
      buffer[length++] = (BYTE)(integer >> 24);
    if(integer <= MINUS_16BITINTEGER) /* Modifiled by Kiyoshi @ 12/19/2000 */
      buffer[length++] = (BYTE)(integer >> 16);
    if(integer <= MINUS_8BITINTEGER) /* Modifiled by Kiyoshi @ 12/19/2000 */
      buffer[length++] = (BYTE)(integer >> 8);
    buffer[length++] = (BYTE)integer;

  } /* In case of positive integer */
  else{ 
    if(integer > 0x00FFFFFFL)
      {
	buffer[length++] = (BYTE)(integer >> 24);
	needsZERO = FALSE;
      }
    if(integer >= 0x00800000L && needsZERO)
      buffer[length++] = 0;
    if(integer > 0x0000FFFFL)
      {
	buffer[length++] = (BYTE)(integer >> 16);
	needsZERO = FALSE;
      }
    if(integer >= 0x00008000L && needsZERO)
      buffer[length++] = 0;
    if(integer > 0x000000FFL)
      {
	buffer[length++] = (BYTE)(integer >> 8);
	needsZERO = FALSE;
      }
    if( integer >= 0x00000080L && needsZERO )
      buffer[length++] = 0;
    
    buffer[length++] = (BYTE)integer;
  }

  /* Write the length and integer */
  writeLength( stream, length ); 
  s2fwrite( stream, buffer, length ); 

}
