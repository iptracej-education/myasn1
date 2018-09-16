/**********************************************************************
 *                                                                    *
 *    myASN1 readLength function                                      *
 *                                                                    *
 *    Written by: Kiyoshi Watanabe                                    *
 *    Last modified: Time-stamp: <01/05/02 00:17:49 kiyoshi>          *
 *                                                                    *
 **********************************************************************/

/* This function takes the FILE pointer as input and reads one byte
   at the beginning. When the MSB is Zero, this byte has the short
   form, which has the length value in the subsequent bits. When
   the MSB is One, this byte represents the two alternatives, one is
   the long form and the other is the indefinite. How to distinguish?
   If the subsequent bits represent more than 1, the next and more
   bytes represent the Length value. If the subsequent bits represent
   ZERO, the long form comes until it reaches at the two ZERO bytes
   as delimitor.

   @Input Parameter:  STREAM *stream 
   @Return Value   :  Long value, otherwise -1 for error
   @Restriction    :  The Long Form is limited 127 bytes at max.
                      The Length Value is limited 2^32 at max.
                      The indefinite Value is not supported currently.

   Compile to create an object file:
   gcc -c -Wall -W -Wtraditional -ansi -pedantic readLength.c

   Compile to create the executive
   gcc -o readLength -DMAIN -Wall -W -Wtraditional -ansi -pedantic sstream.o readLength.c

 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "myASN1.h"

unsigned long readLength( STREAM *stream );

#ifdef MAIN
int main( int argc, char *argv[] )
{

  FILE *fin; 
  STREAM stream; 

  if( argc > 2 || argc < 2 ){
    printf( "Usage: readLength filename (binary file) \n" );
    exit(1);
  }

  /* File operations */
  fin = fopen( argv[1],"rb" );
  if(fin == NULL){
    printf( "File cannot open. An error occurs\n" ); 
    exit( 1 );
  }

  stream.fileptr = fin;
  printf( "readLength: %lu\n", readLength( &stream ) );
  
  fclose( fin );
  exit( 0 );
}
#endif

unsigned long readLength( STREAM *stream )
{

  int first_byte;           
  unsigned long remaining_byte;
  unsigned long length = 0;

  int checkMSB = 0;        /* Check the most significant bit */
  int checkREST = 0;       /* Check the remaining bits */
  int long_form_byte_n = 0;

  assert( stream != NULL );
  assert( stream->fileptr != NULL );

  /* checking the first bit and the remaining bits in order to 
     decide what to do next */
  first_byte = f2sgetc( stream );
  checkMSB = (int)( first_byte >> 7 ) & 0x01;

  if( checkMSB == 0x00 ){ 
    /* the short form is processed. This is just to insert the first bit into
       the length value */
    length = first_byte;
  }
  else if( checkMSB == 0x01 ){ 
    /* the long form or the indefinite form is processed. In order to distinguish
       the two forms, it it necessary to check the remaining bits */

    checkREST = (int)( ((BYTE)first_byte ^ (BYTE)0x80) );
    long_form_byte_n = checkREST;

    if( checkREST >= 0x01 ){ 
      /* When the remaining bits represent more than one, the long form is used.
	 the loop function puts the remaining byte(s) into long tyep variable 
	 and shift them to approprite position(s). */
      
      long_form_byte_n--;
      while( long_form_byte_n >= 0x00 ){
	remaining_byte = f2sgetc( stream );
	if(long_form_byte_n == 4 && remaining_byte > 0x7F){
	  fprintf( OUTPUT, "ERROR: The length of value exceeds the max of long type\n");
	  exit( EXIT_FAILURE );
	}
	length += remaining_byte << ( 8 * long_form_byte_n );
	long_form_byte_n--;
      }
    }
    else{ 
      /* When the remaining bits represents ZERO, the indefinite form is used.
	 The first loop obtains the remaining byte(s) and put them into temporary 
         buffer array until the two successive bytes has ZERO values. The second 
	 loop puts the remaining byte(s) into the long variable and 
	 shift them into the appropriate postion(s). 
	 */ 
      fprintf( OUTPUT, "The indefinite form is not supported currently.\n");
      return -1;
    }

  }
  else{
    fprintf( OUTPUT, "ERROR: check function does not work properly.\n");
    return -1;
  }

  return length;
}


