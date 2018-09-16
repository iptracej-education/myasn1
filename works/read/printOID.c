/**********************************************************************
 *                                                                    *
 *    myASN1 printOID function                                        *
 *                                                                    *
 *    Written by: Kiyoshi Watanabe                                    *
 *    Last modified: Time-stamp: <01/04/25 01:02:49 kiyoshi>          *
 *                                                                    *
 **********************************************************************/

/* 

   Description: To read the ASN1 object of OID value and convert it
                into the char.

   Input Parameter:  STREAM *stream  - file pointer 
                     long length,    - ASN.1 object Length
                     char *oid       - pointer for storing the OID data
   Return Value   :  None
   Restriction    :  

   To compile:
   gcc -o printOID -Wall -W -Wtraditional -pedantic myASN1.o smemory.o
            readLength.o readTag.o sstream.o getTLHeader.o printOID.c -DMAIN

   To run:
   printOID test/readoid.binary 

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "myASN1.h"

void printOID( STREAM *stream, long length );
#define getTag( stream ) f2sgetc( stream )

#ifdef MAIN
int main( int argc, char *argv[] )
{

  FILE *fin; 
  STREAM stream; 
  ASN1_OBJECT asn1obj;
  int buffer;

  if( argc > 2 || argc < 2 ){
    printf( "Usage: printOID filename (binary file) \n" );
    exit(1);
  }

  /* File operations */
  fin = fopen( argv[1],"rb" );
  if(fin == NULL){
    printf( "File cannot open. An error occurs\n" ); 
    exit( 1 );
  }

  stream.fileptr = fin;

  buffer = getTag( &stream );
  getTLHeader( &stream, buffer, &asn1obj );
  printf("printOID(): ");
  printOID( &stream, asn1obj.lengthNumber );

  fclose( fin );
  exit( 0 );
}
#endif

void printOID( STREAM *stream, long length )
{
  
  int buffer[20]; /* the total arcs are limited to 20 here */
  int firstByte;
  int nextByte;
  int value = 0;
  
  int MSB_check;
  int i = 0;
  int loopMax, lastArrayNum;

  assert( stream != NULL );
  assert( stream->fileptr != NULL ) ;

  /* get the first byte and obtain the first and second
     values of the OID */        
  
  firstByte = fgetc( stream->fileptr );
  
  buffer[i++] = firstByte / 40;                        /* the top arc */
  buffer[i++] = firstByte % 40;                        /* the second arc */

  /* This is the main routine to obtain the sequence of OID values
     in bytes. Until the MSB is euqal to Zero, add the byte to the value. */
  
  while( length > 1 ){

    do{
      nextByte = fgetc( stream->fileptr );
      MSB_check = nextByte >> 7;

      if( MSB_check != 0x00 ){
        value |= ( nextByte ^ 0x80 );      /* cut off the MSB and add it to the value */
        value <<= 7;                       /* shift the value to the correct position */
      }
      else{
        value |= nextByte;          
        buffer[i] = value;           /* insert the final value */
	i++;
      }
    length--;

    }while( MSB_check != 0x00 );

    value = 0;                         /* for next oid component */
  }

  /* print out the OID */
  loopMax = i; 
  lastArrayNum = loopMax - 1;

  for(i = 0; i < loopMax; i++ ){
    if( i == lastArrayNum )
      fprintf(OUTPUT, "%d",buffer[i]);
    else
      fprintf(OUTPUT, "%d.",buffer[i]);
  }
  fprintf(OUTPUT, "\n");

}

