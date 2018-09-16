/********************************************************************

  printString function

  Written By Kiyoshi Watanabe <kiyoshi@tg.rim.or.jp> 
  Last Modified: Time-stamp: <01/04/22 23:07:52 kiyoshi>

  Compile:
   Make printString
  Run:
   printOctetstring test/string

  NOTE: This does not implement CER decoding.
  
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include "myASN1.h"
#include "readASN1.h"

#define I        0x01 /* The Range of the IA5 String, which includes I, V, P, and N. */
#define V        0x02 /* The Range of the Visible String, which includes V, P, and N. */
#define P        0x03 /* The Range of the Printable String, which includes P and N.*/
#define N        0x04 /* The Range of the Numeric String, which only includes N. */

/*                         0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
static int checkChar[] = { I , I , I , I , I , I , I , I , I , I , I , I , I , I , I , I,
                           I , I , I , I , I , I , I , I , I , I , I , I , I , I , I , I,
                           N , V , V , V , V , V , V , P , P , P , V , P , P , P , P , P,
                           N , N , N , N , N , N , N , N , N , N , V , V , V , P , V , P,
                           V , P , P , P , P , P , P , P , P , P , P , P , P , P , P , P,
                           P , P , P , P , P , P , P , P , P , P , P , V , V , V , V , V,
                           V , P , P , P , P , P , P , P , P , P , P , P , P , P , P , P,
                           P , P , P , P , P , P , P , P , P , P , P , V , V , V , V , I };

#ifdef MAIN
int main( int argc, char *argv[] )
{

  FILE *fin; 
  STREAM stream; 
  ASN1_OBJECT asn1obj;
  int byte;

  if( argc > 2 || argc < 2 ){
    printf( "Usage: readString filename (binary file) \n" );
    exit(1);
  }

  /* File operations */
  fin = fopen( argv[1],"rb" );
  if(fin == NULL){
    printf( "File cannot open. An error occurs\n" ); 
    exit( 1 );
  }

  stream.fileptr = fin;

  byte = f2sgetc( &stream );
  getTLHeader( &stream, byte, &asn1obj );
  printf( "readString(): ");
  if(printNumericString( &stream, &asn1obj ) <= 0 ){
    fprintf( stderr, "error occured in readString function\n" );
  } 

  fclose( fin );
  exit( 0 );
}
#endif

/********************************************************************

  dumpByte function: get bytes and print them.
*********************************************************************/
void dumpByte_wCheck( STREAM *stream, ASN1_OBJECT *asn1obj, int (*fn)( int ) ){

  int i;
  int byte;
  int check;

  assert( stream != NULL );
  assert( stream->fileptr != NULL );

  /* read the binary data */
  for( i = 0; i < asn1obj->lengthNumber; i++ ){
    byte = fgetc(stream->fileptr);
    if( byte == EOF ){
      fprintf( stderr, "EOF ERROR: reading files \n");
      exit( EXIT_FAILURE );
    }
    if( ( check = (*fn)( byte ) ) <= 0 )
      fprintf( stderr, "UNACCEPTABLE VALUE in %X\n", asn1obj->tagNumber );
    fputc( byte, OUTPUT );

  }
  fputc( '\n', OUTPUT );

}

/********************************************************************

  isNumeric function: check the numeric or not
*********************************************************************/
int isNumeric( int ch )
{
  /* check whether the char is within the numberic char values */
  if( ch >= 128 || checkChar[ch] < N)
    return FALSE;
  return TRUE;
}

/********************************************************************

  isPrintable function check the printable or not
*********************************************************************/
int isPrintable( int ch )
{
  if( ch >= 128 || checkChar[ch] < P)
    return FALSE;
  return TRUE;  
}

/********************************************************************

  isVisible function check the printable or not
*********************************************************************/
int isVisible( int ch )
{
  if( ch >= 128 || checkChar[ch] < V)
    return FALSE;
  return TRUE;  
}

/********************************************************************

  isIA5 function check the printable or not
*********************************************************************/
int isIA5( int ch )
{
  if( ch >= 128 || checkChar[ch] < I)
    return FALSE;
  return TRUE; 
}

/********************************************************************

  printNumericString function: read numeric strings and print them
*********************************************************************/
void printNumericString( STREAM *stream, ASN1_OBJECT *asn1obj )
{

  assert( stream != NULL );
  assert( stream->fileptr != NULL );

  dumpByte_wCheck( stream, asn1obj, isNumeric );

}

/********************************************************************

  printPrintableString function:
*********************************************************************/
void printPrintableString( STREAM *stream, ASN1_OBJECT *asn1obj )
{
  dumpByte_wCheck( stream, asn1obj, isPrintable ); 
}

/********************************************************************

  printT61String function:
*********************************************************************/

void printT61String( STREAM *stream, ASN1_OBJECT *asn1obj )
{
  dumpByte( stream, asn1obj ); 
}

/********************************************************************

  printIA5String function:
*********************************************************************/
void printIA5String( STREAM *stream, ASN1_OBJECT *asn1obj )
{
  dumpByte_wCheck( stream, asn1obj, isIA5 ); 
}

/********************************************************************

  printVisibleString function:
*********************************************************************/
void printVisibleString( STREAM *stream, ASN1_OBJECT *asn1obj )
{
  dumpByte_wCheck( stream, asn1obj, isVisible );
}





