/**********************************************************************
 *                                                                    *
 *    myASN1 function declarations                                    *
 *                                                                    *
 *    Written by: Kiyoshi Watanabe                                    *
 *    Last modified: Time-stamp: <01/05/02 14:00:21 kiyoshi>          *
 *                                                                    *
 **********************************************************************/

#ifndef MYASN1_H
#define MYASN1_H

#include <stdio.h> 
#include "smemory.h"
#include "serror.h"

/* myASN1 uses the three type definition, BYTE, BOOLEAN, STREAM
   These help understand the source codes and ASN1 semantics. 
   The STREAM might be added later to have some status checking.
   */

typedef char BYTE;
typedef int BOOLEAN;
typedef struct{
  BYTE *value;
  FILE *fileptr;
  char *filename;
}STREAM;

typedef struct asn1_object{
  int tagClass;         /* Universal, Application, Private, or Context-Specific */
  int tagForm;          /* Primitive or Constructed                             */
  int tagNumber;        /* T value, Tag Number                                  */
  long lengthNumber;    /* L value, Value Length Number                         */
  long tagLength;       /* T Length                                             */
  long lengthLength;    /* L Length                                             */
  long TLVLength;       /* a whole ASN1 object length                           */
  char *value_p;        /* Pointer to V value                                   */
} ASN1_OBJECT;

#define TRUE 1
#define FALSE 0

/* This is used for the "sstream" functions in order to determine 
   where the output (mostly error status) should be displayed.*/

#ifndef OUTPUT
 #define OUTPUT stdout
#endif

/* These are myASN1 "write" functions declarations */

void writeContext( STREAM *stream, unsigned int tagNumber, char formParameter, int tagLength );
void writeCharacterString(STREAM *stream, BYTE *charString, int length);
void writeNumericString(STREAM *stream, BYTE *numericString, int length);
void writePrintableString(STREAM *stream, BYTE *octetString, int length);
void writeVisibleString(STREAM *stream, BYTE *octetString, int length);
void writeIA5String(STREAM *stream, BYTE *octetString, int length);
void writeOctetString(STREAM *stream, BYTE *octetString, int length);
void writeBitString(STREAM *stream, char *bit, const int length);
void writeEnumerated(STREAM *stream, const long number);
void writeShortInteger( STREAM *stream, const long integer );
void writeOID( STREAM *stream, char *oid );
void writeTag( STREAM *stream, int tag, char asn1class );
void writeLength( STREAM *stream, const long length );
void writeSet( STREAM *stream, int length );
void writeSequence( STREAM *stream, int length );

int sizeofInteger( int length );
int sizeofOID( char *oid );
long sizeofObject( const long valueLength );
long sizeofContextObject( const long tagNumber, const long valueLength );
int sizeofLength( const long valueLength );
int sizeofTag( const long tagNumber );


/* These are myASN1 "read" functions declarations */

int readTag( STREAM *stream );
long readLength( STREAM *stream );
long readShortInteger( STREAM *stream, const long integer );

/* file write and read function declaration */

void s2fwrite(STREAM *stream, BYTE *buffer, int length);
int f2sgetc( STREAM *stream );


/* debug function declaration */

void bit_print_int(int a);
void bit_print_char(char a); 


/* The ASN.1 Type definition, covering all the four classes */

#define BOOLEAN_TYPE                       0x01 /* 01 */
#define INTEGER_TYPE                       0x02 /* 02 */
#define BITSTRING_TYPE                     0x03 /* 03 */
#define OCTETSTRING_TYPE                   0x04 /* 04 */
#define NULL_TYPE                          0x05 /* 05 */
#define OBJECT_IDENTIFIER_TYPE             0x06 /* 06 */
#define OBJECT_DESCRIPTOR_TYPE             0x07 /* 07 */
#define EXTERNAL_TYPE_AND_INSTANCE_OF_TYPE 0x08 /* 08 */
#define REAL_TYPE                          0x09 /* 09 */
#define ENUMERATED_TYPE                    0x0A /* 10 */
#define EMBEDDED_PDY_TYPE                  0x0B /* 11 */
#define UTF8STRING_TYPE                    0x0C /* 12 */

/* UNIVERSAL 13 - 15 is reserved for future
   editions for this Recommendation International
   Standard
   */

/* SEQUENCE and SEQUENCE_OF (Universal 16) are 
   represented as 00110000 bit sequence  */

#define SEQUENCE_TYPE                     0x10 
#define SEQUENCE_OF_TYPE                  0x10 
#define SEQUENCE_TYPE_W_CONSBIT           0x30 
#define SEQUENCE_OF_TYPE_W_CONSBIT        0x30 

/* SET and SET OF (UNIVERSAL 17) are representaed 
   as 00110001 */

#define SET_TYPE	                   0x11 
#define SET_OF_TYPE                        0x11 
#define SET_TYPE_W_CONSBIT                 0x31 
#define SET_OF_TYPE_W_CONSBIT              0x31

/* UNIVERSAL 18-22 Character string type */
#define NUMERICSTRING_TYPE                 0x12 /* 18 */
#define PRINTABLESTRING_TYPE               0x13 /* 19 */
#define T61STRING_TYPE                     0x14 /* 20 */
#define VIDEOTEXSTRING_TYPE                0x15 /* 21 */
#define IA5STRING_TYPE                     0x16 /* 22 */

/* UNIVERSAL 23-24 Time Types */
#define UTCTIME_TYPE                       0x17 /* 23 */
#define GENERALIZEDTIME_TYPE               0x18 /* 24 */

/* UNIVERSAL 20-30 More CHaracter string types */
#define GRAPHICSTRING_TYPE                 0x19 /* 25 */
#define VISIBLESTRING_TYPE                 0x1A /* 26 */
#define GENERALSTRING_TYPE                 0x1B /* 27 */
#define UNIVERSALSTRING_TYPE               0x1C /* 28 */
#define BMPSTRING_TYPE                     0x1E /* 30 */

/* UNIVERSAL 31- Reserved for addenda to this
   Recommendation International Standard */


#endif /* _MYASN1_H */








