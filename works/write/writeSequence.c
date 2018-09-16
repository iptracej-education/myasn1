#include <stdio.h>
#include "myASN1.h"

/********************************************************************



                                               writeSequence function
*********************************************************************/
/* This function writes the ASN.1 SEQUENCE TYPE. This function takes
   two variables. You must obtain the length of the sequence
   for this, by calling the sizeof... functions beforehand. */

void writeSequence( STREAM *stream, int length )
{

  writeTag( stream, SEQUENCE_TYPE_W_CONSBIT, 'U');
  writeLength( stream, length );
  
}
