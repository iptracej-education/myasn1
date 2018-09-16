/********************************************************************

  printOctetString function

  Written By Kiyoshi Watanabe <kiyoshi@tg.rim.or.jp> 
  Last Modified: Time-stamp: <01/04/30 04:21:04 kiyoshi>

  Compile:
   Make printOctetString
  Run:
   printOctetstring test/octetstring

  NOTE: This does not implement CER decoding
  
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include "myASN1.h"
#include "readASN1.h"

#ifdef MAIN
int main( int argc, char *argv[] )
{

  FILE *fin; 
  STREAM stream; 
  ASN1_OBJECT asn1obj;
  int bit;

  if( argc > 2 || argc < 2 ){
    printf( "Usage: printOctetString filename (binary file) \n" );
    exit(1);
  }

  /* File operations */
  fin = fopen( argv[1],"rb" );
  if(fin == NULL){
    printf( "File cannot open. An error occurs\n" ); 
    exit( 1 );
  }

  stream.fileptr = fin;

  bit = f2sgetc( &stream );
  getTLHeader( &stream, bit, &asn1obj );

  printf( "printOctetString(): \n");
  printOctetString( &stream, &asn1obj );

  fclose( fin );
  exit( 0 );
}

void printIndent( int counter )
{
  int i;
  for (i = 0; i <= counter; i++)
    putchar(' ');

}

#endif

/********************************************************************

  dumpByte function: get bytes and print them.
*********************************************************************/
void dumpByte( STREAM *stream, ASN1_OBJECT *asn1obj ){

  int i;
  int byte;
  char buffer[16]; /* ASCII representation */

  assert( stream != NULL );
  assert( stream->fileptr != NULL );

  memset(buffer,'\0',strlen(buffer));

  /* dump the octet data */
  for( i = 0; i < asn1obj->lengthNumber; i++ ){
    byte = fgetc(stream->fileptr);
    if( byte == EOF ){
      fprintf( stderr, "EOF ERROR: reading files \n");
      exit( EXIT_FAILURE );
    }

    if( (i % 16) == 0 ){
      if( i > 0 ){
	/* if you want to adjust the space here, you have
	   to change the spaces below indicated by the number */
	fprintf(OUTPUT, "  %s",buffer); /* [1] */
	fprintf(OUTPUT, "\n");
	memset(buffer,'\0',strlen(buffer));
      }
      fprintf(OUTPUT, "  %3X: ", i);
    }
    else{
      if( (i % 8) == 0 && i > 0 )
	fputc(' ', OUTPUT);
    }

    if(!isprint( byte )){
      byte = '.';
    }
    buffer[(i % 16)] = byte;

    if( (byte >= 0 && byte <= 15 ) ) 
      fprintf(OUTPUT, "0%X ", byte );
    else
      fprintf(OUTPUT, "%X ", byte );
  }

  /* display the remaining octet values. When more than 8 values exist,
     the indentation must be minus one, since there 2 spaces used between
     7th and 8th values. */
  if( (i % 16) > 8 )
      printIndent( ((16 - (i % 16) ) * 3) -1 ); 
  else
  /* this is the case when the remaing octet values is the exact 16.
     do no indentation here */
    if( (i % 16) == 0 )
	;
    /* otherwise do indentation to align the text to the currect positions */
    else
      printIndent( ((16 - (i % 16) ) * 3) ); 
  fprintf(OUTPUT, "  %s",buffer);   /* [2] */
  fputc( '\n', OUTPUT );

}



/********************************************************************

  printOctetString function: read octet strings and print them
*********************************************************************/
void printOctetString( STREAM *stream, ASN1_OBJECT *asn1obj )
{

  assert( stream != NULL );
  assert( stream->fileptr != NULL );

  dumpByte( stream, asn1obj );

}


