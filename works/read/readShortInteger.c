/*********************************************************************
 *                                                                    *
 *    myASN1 readShortInteger function                                *
 *                                                                    *
 *    Written by: Kiyoshi Watanabe                                    *
 *    Last modified: Time-stamp: <01/05/02 00:15:49 kiyoshi>          *
 *                                                                    *
 **********************************************************************/

/* 

   @Input Parameter:  STREAM *stream (file related operations)
		      const long integer (actual length of the integer bits)
   @Return Value   :  Long type
   @Restriction    :  Assume the Integer should be within 4 octets.
                      (-2147483648 ~ 2147483647)

   Compile to create an object file:
   gcc -c -Wall -W -Wtraditional -ansi -pedantic readInteger.c

   Compile to create the executive
   gcc -o readShortInteger -DMAIN -Wall -W -Wtraditional -ansi -pedantic sstream.o getTLHeader.o 
   readTag.o readLength.o myASN1.o readShortInteger.c


 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "myASN1.h"
#include "readASN1.h"

long readShortInteger( STREAM *stream, const long integer );

#ifdef MAIN
int main( int argc, char *argv[] )
{

  FILE *fin; 
  STREAM stream; 
  ASN1_OBJECT asn1obj;
  int buffer;

  if( argc > 2 || argc < 2 ){
    printf( "Usage: readInteger filename (binary file) \n" );
    exit(1);
  }

  /* File operations */
  fin = fopen( argv[1],"rb" );
  if(fin == NULL){
    printf( "File cannot open. An error occurs\n" ); 
    exit( 1 );
  }

  stream.fileptr = fin;

  buffer = f2sgetc( &stream );
  getTLHeader( &stream, buffer, &asn1obj );
  printf( "readShortInteger(): %ld\n", readShortInteger( &stream, asn1obj.lengthNumber ) );
  
  fclose( fin );
  exit( 0 );
}
#endif

  /* The shortest possible integer is specified. What does it mean? This means 
     that the most significant bit 1 indicates the minus number. 

          0                                                      00000000        
        127                                                      01111111        
        128(0x80)                                       00000000 10000000        
        255(0xFF)                                       00000000 11111111        
         
        256                                             00000001 00000000        
        32767                                           01111111 11111111     
	32768(0x8000)                          00000000 10000000 00000000
        65535(0xFFFF)                          00000000 11111111 11111111

        65536                                  00000001 00000000 00000000
        8388607                                01111111 11111111 11111111
        8388608(0x800000)             00000000 10000000 00000000 00000000
        16777215(0xFFFFFF)            00000000 11111111 11111111 11111111

        16777216                      00000001 00000000 00000000 00000000 
        2147483647                    01111111 11111111 11111111 11111111


          -1                                                     11111111
        -128(0xFFFFFF80)                                         10000000 
	-129(0xFFFFFF7F)                                11111111 01111111
	-256(0xFFFFFF00)                                11111111 00000000

        -257                                            11111110 11111111
        -32768(0xFFFF8000)                              10000000 00000000
        -32769(0xFFFF7FFF)                     11111111 01111111 11111111
        -65536(0xFFFF0000)                     11111111 00000000 00000000

        -65537                                 11111110 11111111 11111111
        -8388608(0xFF800000)                   10000000 00000000 00000000
        -8388609(0xFF7FFFFF)         111111111 01111111 11111111 11111111
        -16777216(0xFF00000)         111111111 00000000 00000000 00000000

        -16777217                    111111110 11111111 11111111 11111111
        -2147483648                  100000000 00000000 00000000 00000000

    */


long readShortInteger( STREAM *stream, const long length  )
{
  
  long integerBuffer = 0;
  long checkPlus;         /* check whether non negative */
  int shiftCounter;       
  int allZeroCounter = 3; /* used to insert the ZEROs when the value is negative */
  int i;                  /* For statemet loop counter */


  /* obtain the FIRST byte and create a flag to check non-negative */
  integerBuffer = f2sgetc( stream );
  checkPlus = ( integerBuffer & 0x80 ) >> 7;

  /* prepare for the shift for the FIRST byte and then shift it */
  shiftCounter = length - 1; 
  integerBuffer <<= ( 8 * shiftCounter ); 

  shiftCounter--;

  /* check non-negative and shift the value until the Integer values end. */
  if(checkPlus == 0x00000000L && length > 1 ){
    for( i = 1; i < length; i++ ){
      integerBuffer += f2sgetc( stream ) << ( 8 * shiftCounter-- ) ;
    }
  }

  /* when negative, insert the 0xFFs first and then shift the value until
     the Integer values end */
  if(checkPlus == 0x00000001L){
    for( i = 4; i > length; i-- ){
      integerBuffer += 0xFF << ( 8 * allZeroCounter-- );
    }
    for( i = 1; i < length; i++ ){
      integerBuffer += ( f2sgetc( stream ) ) << ( 8 * shiftCounter-- );
    }
  }
  return integerBuffer;

}

