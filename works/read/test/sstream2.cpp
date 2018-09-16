/**********************************************************************
 *                                                                    *
 *    myASN1 sstream functions                                        *
 *                                                                    *
 *    Written by: Kiyoshi Watanabe                                    *
 *    Last modified: Time-stamp: <00/12/23 02:25:20 kiyoshi>          *
 *                                                                    *
 **********************************************************************/

/* This is the collection of the FILE-processing functions. All of
   the functions first check the input parameters with assert function.
   Also the error checking is processed as best as I can. In this way, 
   we can assure that the correct behavior will be performed. */

#include <assert.h>
#include "myASN1.h"

/********************************************************************

  s2fwrite function
  Last Modified: 2000/10/27
*********************************************************************/

/* This function write the content of the "buffer", which points to
   with the number of the "length" to a file, which is pointed
   by the "stream" pointer. The function is really straight forward.

  @ Input parameters: STREAM *stream, BYTE *buffer, int length
  @ Retrun          : None
  @ Restriction     : 
  */

void s2fwrite(STREAM *stream, BYTE *buffer, int length)
{
  int size;

  assert(stream != NULL);
  assert(buffer != NULL);
  assert(stream->fileptr != NULL);
  assert(length > 0);

  size = sizeof(char);
  if(fwrite(buffer,size,length,stream->fileptr) != (size_t)length){
    printf("An error occurs in s2fwrite function\n");
    exit(0);
  }
}

/********************************************************************

  f2sgetc function
  Last Modified: 2000/10/27
*********************************************************************/

/* This function will get a character from a file, which is pointed
   by the "stream" pointer and retrun a char value.

  @ Input parameters: STREAM *stream
  @ Retrun          : Char 
  @ Restriction     : 
  */


int f2sgetc( STREAM *stream )
{
  int bit;

  assert( stream != NULL );
  assert( stream->fileptr != NULL );

  bit = fgetc(stream->fileptr);
  if( bit == EOF )
    fprintf( OUTPUT, "ERROR: EOF Error occurs when reading files at f2sgetc()\n" );
  return bit;
}

