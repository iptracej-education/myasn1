/********************************************************************

  writeEnumerated function
  Last Modified: 9th October 2000
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>       /* atol function */
#include <assert.h>       /* assert function */
#include <limits.h>       /* CHAR_BIT */
#include "myASN1.h"

/* 
int main(int argc, char *argv[])
{

  STREAM *stream;
  assert(argc>1);
  writeEnumerated(stream, atol(argv[1]));
  exit(0);
}
*/

void writeEnumerated(STREAM *stream, const long number)
{
  writeTag( stream, ENUMERATED_TYPE, 'U' );
  writeNumeric( stream, number );
}	   


