#include <stdio.h>
#include "myASN1.h"

/********************************************************************

                                                    writeSet function
*********************************************************************/
/* This function writes the ASN.1 SET TYPE. This function takes
   two variables. You must obtain the length of the sequence
   for this, by calling the sizeof... functions beforehand. */

void writeSet( STREAM *stream, int length )
{

  writeTag( stream, SET_TYPE_W_CONSBIT, 'U');
  writeLength( stream, length );
  
}
