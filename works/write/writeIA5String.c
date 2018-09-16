/********************************************************************

  writeIA5String function
  Last Modified: 15th October 2000
*********************************************************************/
/* The IA5 values of string is passed to this function. 
   This function takes 3 arguments and simply assign them to the 
   other fuctnions to encode.

*/

void writeIA5String(STREAM *stream, BYTE *octetString, int length)
{
  writeTAG( stream, IA5STRING_TYPE, 'U');
  writeLength( stream, length );
  writeCharacterString( stream, octetString, length );
}
