/**********************************************************************
 *                                                                    *
 *    myASN1 readTag function                                         *
 *                                                                    *
 *    Written by: Kiyoshi Watanabe                                    *
 *    Last modified: Time-stamp: <01/04/21 17:52:12 kiyoshi>          *
 *                                                                    *
 **********************************************************************/

/* This function takes the FILE pointer as input and reads the
   bytes one bye one until the first bit comes at ZERO at MSB. So the 
   function, for instance, has to read the first two 
   bytes, but do not read furtehr!                                   

   ----- TAG -------  -LENGTH-  --VALUE--
   10000001 00000000  00000001  100100100


   @Input Parameter:  STREAM *stream 
   @Return Value   :  int variable
   @Restriction    :  

   Compile to create an object file:
   gcc -c -Wall -W -Wtraditional -ansi -pedantic readTag.c

   Compile to create the executive
   gcc -o readTag -DMAIN -Wall -W -Wtraditional -ansi -pedantic sstream.o readTag.c


 */

#include <stdio.h>
#include <assert.h>
#include "myASN1.h"

int readTag( STREAM *stream );

#ifdef MAIN
int main( int argc, char *argv[] )
{

  FILE *fin; 
  STREAM stream; 

  if( argc > 2 || argc < 2 ){
    printf( "Usage: readTag filename (binary file) \n" );
    exit( 1 );
  }
  /* File operations */
  fin = fopen( argv[1],"rb" );
  if(fin == NULL){
    printf("File cannot open. An error occurs\n"); 
    exit(1);
  }
  stream.fileptr = fin;
  printf( "%d\n",readTag(&stream) );
  
  fclose( fin );
  exit( 0 );
}

#endif

int readTag( STREAM *stream )
{

  int tagNumber = 0;
  int buffer[3]; /* Assume that there are 4 octets at max for Tag numbers */
  int MSB_check;
  int tmp;
  int i = 0, j = 0; 
  
  assert(stream != NULL);
  assert( stream->fileptr != NULL );
  
  do{
    if( i > 3 ){
      fprintf( OUTPUT, "Tag Number too large to process\n" );
      return FALSE;
    }
    buffer[i] = f2sgetc( stream ); 
    MSB_check = buffer[i] >> 7;
    i++;
  } while( MSB_check == 1 );
  
  /* decrease one to fit the algorithm below */
  i--;

  /* cut off the MSB value, then add the shift-to-the-left value */
  while( i >= 0 ){
    tmp = ( i > 0 ) ? buffer[j] ^ 0x80 : buffer[j];
    tagNumber |= ( tmp << (i * 7) );
    i--; j++;
  }
  return tagNumber;
}


