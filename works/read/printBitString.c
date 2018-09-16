/********************************************************************

  printBitString function

  Written By Kiyoshi Watanabe <kiyoshi@tg.rim.or.jp> 
  Last Modified: Time-stamp: <01/04/22 02:11:29 kiyoshi>

  Compile:
   Make printBitString
  Run:
   printBitstring test/bitstring

  NOTE: This does not implement CER decoding
  
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
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
    printf( "Usage: printBitString filename (binary file) \n" );
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

  printf( "printBitString():\n");
  printBitString( &stream, &asn1obj );

  fclose( fin );
  exit( 0 );
}
#endif

/********************************************************************

  dumpBit function:
*********************************************************************/
void dumpBit( STREAM *stream, ASN1_OBJECT *asn1obj )
{
  int i, j;
  int unusedbit;
  int bits;

  int MaxChar = CHAR_BIT;
  const char bitMASK = 1 << (MaxChar - 1);

  long length = asn1obj->lengthNumber - 1;

  assert( stream != NULL );
  assert( stream->fileptr != NULL );

  /* get the unused bit of the Bit String first */
  unusedbit = fgetc( stream->fileptr );
  if( unusedbit < 0 || 0 > 7 ){
    fprintf( OUTPUT, "illegal bit numbers for the unusedbit %d: will set 0 instead.\n", unusedbit );
    unusedbit = 0;
  }

  printf("0%d:",unusedbit);
  /* dump the bit string */
  for( i = 0; i < length; i++ ){
    bits = fgetc(stream->fileptr);
    if( bits == EOF ){
      fprintf( stderr, "EOF ERROR: reading files in dumpBit function \n");
      exit( EXIT_FAILURE );
    }

    /* the last bit should be the number substructed by unusedbit */
    if( (i + 1) == length )
      MaxChar -= unusedbit; 

    /* print a bit by bit */
    for( j = 1; j <= MaxChar; ++j ){
      fputc((( (unsigned char)bits & bitMASK ) == 0 ) ? '0':'1', OUTPUT );
      bits <<= 1;
      if( j % CHAR_BIT == 0 )
	fputc( ' ', OUTPUT);
    }
    if( (i+1) % 8 == 0 && i > 0 ){
      fprintf( OUTPUT, "\n" );
      fprintf( OUTPUT, "   " );
    }
  }

  fputc( '\n', OUTPUT );

}

/********************************************************************

  readBitString function: read bit strings and print them
*********************************************************************/
void printBitString( STREAM *stream, ASN1_OBJECT *asn1obj )
{

  assert( stream != NULL );
  assert( stream->fileptr != NULL );

  dumpBit( stream, asn1obj );

}
