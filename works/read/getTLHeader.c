/**********************************************************************
 *                                                                    *
 *    myASN1 getTLHeader function                                     *
 *                                                                    *
 *    Written by: Kiyoshi Watanabe                                    *
 *    Last modified: Time-stamp: <01/04/24 00:12:22 kiyoshi>          *
 *                                                                    *
 **********************************************************************/

/* This function inserts the T(Tag) information into several variables. The
   tagClass is for Universal, Application, Context_specific, and Private.
   The tagForm is for constructed or primitive. The tagNumber is for the
   number. 

   BIT 8   BIT 7   CLASS
   0       0       UNIVERSAL
   0       1       APPLICATION
   1       0       CONTEXT-SPECIFIC
   1       1       PRIVATE

   BIT 6           FORM
   0               Primitive
   1               Constructed

   BIT 5 - 1       tagNumber
   if BIT [5 - 1] represents 31, this should be longer tagNumber!!

   @Input Parameter:  STREAM *stream int buffer ASN1_OBJECT *object
   @Return Value   :  TRUE otherwise FALSE
   @NOTE           :  calls readTag and readLength 

   To compile to create an object file:
   gcc -c -Wall -W -Wtraditional -ansi -pedantic getTLHeaderc.

   To compile to create the executive:
   gcc -o getTLHeader -Wall -W -Wtraditional -ansi -pedantic sstream.o readTag.o 
   readLength.o myASN1.o getTLHeader.c -DMAIN

   To run:
   getTLHeader test/gettlheadertest.binary


*/

#include <stdio.h>
#include <assert.h>
#include "myASN1.h"

#ifdef MAIN

int getTLHeader( STREAM *stream, const int buffer, ASN1_OBJECT *asn1obj );

int main(int argc, char *argv[])
{

  FILE *fin; 
  STREAM stream; 
  ASN1_OBJECT obj;
  int buffer;

  if( argc > 2 || argc < 2 ){
    printf( "Usage: readTag filename (binary file) \n" );
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
  getTLHeader( &stream, buffer, &obj );

  printf( "getTLHeader() tagClass     : %d\n", obj.tagClass );
  printf( "getTLHeader() tagForm      : %d\n", obj.tagForm );
  printf( "getTLHeader() tagNumber    : %d\n", obj.tagNumber );
  printf( "getTLHeader() lengthNumber : %ld\n", obj.lengthNumber );
  printf( "getTLHeader() tagLength    : %ld\n", obj.tagLength );
  printf( "getTLHeader() lengthLength : %ld\n", obj.lengthLength );
  printf( "getTLHeader() TLVLength    : %ld\n", obj.TLVLength );

  fclose( fin );
  exit( 0 );

}

#endif

int getTLHeader( STREAM *stream, const int buffer, ASN1_OBJECT *asn1obj )
{

  int checkTagNumber;

  assert(stream != NULL);
  assert(stream->fileptr != NULL);

  /* read the T part of TLV */
  asn1obj->tagClass = ( buffer & 0xC0 ) >> 6;        /* 11000000 */
  asn1obj->tagForm = ( buffer & 0x20 ) >> 5;         /* 00100000 */

  checkTagNumber = ( buffer & 0x1F );                /* 00011111 */

  /* check the Tag Number constraints */
  if( checkTagNumber <= 0x00 ){  /* 0 or minus number */
    if( asn1obj->tagClass == 0x02 && asn1obj->tagForm == 0x01 ){
      asn1obj->tagNumber = checkTagNumber;
    }
    else
      return FALSE;
  }
  else if( checkTagNumber >= 0x1F ){ /* larger than one octet value */                            
    if( !(asn1obj->tagNumber = readTag( stream )))
      return FALSE;
  }
  else{
    asn1obj->tagNumber = checkTagNumber;
  }
  
  /* read the L part of TLV */

  if( (asn1obj->lengthNumber = readLength( stream )) < 0L ){
    return FALSE;
  }
  asn1obj->tagLength = sizeofTag( asn1obj->tagNumber );
  asn1obj->lengthLength = sizeofLength( asn1obj->lengthNumber ); 
  asn1obj->TLVLength = asn1obj->tagLength
                       + asn1obj->lengthLength 
                       + asn1obj->lengthNumber;

  return TRUE;

}


