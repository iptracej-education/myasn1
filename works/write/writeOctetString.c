/********************************************************************

  writeOctetString function
  Last Modified: 15th October 2000
*********************************************************************/
/* The octet values of string is passed to this function. 

   This function takes 3 arguments and simply assign them to the 
   other fuctnions to encode.

*/

void writeOctetString(STREAM *stream, BYTE *octetString, int length)
{
  writeTAG( stream, OCTETSTRING_TYPE, 'U');
  writeLength( stream, length );
  writeCharacterString( stream, octetString, length );
}
