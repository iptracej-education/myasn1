/********************************************************************

  writeBoolean function
  Last Modified: October 9th 2000
*********************************************************************/
/* Boolean type takes only two logical data, which are TRUE and FALSE.
   The FALSE value should be '00'H, but The TRUE value should be
   anything except '00'H. I will use '01'H for the TRUE.

   */

void writeBoolean(STREAM *stream, int boolean)
{
  BYTE buffer;
  int length = 1;

  buffer = (BYTE)boolean;

  writeTag( stream, BOOLEAN_TYPE, 'U');
  writeLength( stream, length );
  s2fwrite( stream, buffer, length );

}
