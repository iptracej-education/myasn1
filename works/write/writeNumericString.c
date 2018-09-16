/********************************************************************

  writeNumericString function
  Last Modified: 15th October 2000
*********************************************************************/
/* The numeric values of string is passed to this function. 
   This function takes 3 arguments and simply assign them to the 
   other fuctnions to encode.

*/

void writeNumericString(STREAM *stream, BYTE *numericString, int length)
{
  writeTAG( stream, NUMERICSTRING_TYPE, 'U');
  writeLength( stream, length );
  writeCharacterString( stream, numericString, length );
}
