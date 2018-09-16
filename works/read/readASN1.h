/********************************************************************

  myASN1 ASN1 dump tool: readASN1
  
  Written by: Kiyoshi Watanabe
  Last Modified: Time-stamp: <01/04/30 04:30:22 kiyoshi>

  compile:
  gcc -o readASN1 -Wall -W -Wtraditional -ansi sstream.o myASN1.o 
  readTag.o readLength.o getTLHeader.c readShortInteger.c readASN1.c 

  run:
  readASN1 test/sequence.binary

*********************************************************************/

#ifndef READASN1_H
#define READASN1_H

#define getTag(fileptr) f2sgetc(fileptr)
#define readBoolean(stream) ((f2sgetc(stream)==1)?"TRUE":"FALSE")

void readASN1( STREAM *stream );
void readASN1Object( STREAM *stream, int buffer );
long readShortInteger( STREAM *stream, const long integer );

void printOID( STREAM *stream, long length );
void printOctetString( STREAM *stream, ASN1_OBJECT *asn1obj );
void printBitString( STREAM *stream, ASN1_OBJECT *asn1obj );
void printNumericString( STREAM *stream, ASN1_OBJECT *asn1obj );
void printPrintableString( STREAM *stream, ASN1_OBJECT *asn1obj );
void printT61String( STREAM *stream, ASN1_OBJECT *asn1obj );
void printIA5String( STREAM *stream, ASN1_OBJECT *asn1obj );
void printVisibleString( STREAM *stream, ASN1_OBJECT *asn1obj );

int getTLHeader( STREAM *stream, const int buffer, ASN1_OBJECT *asn1obj );

void dumpByte( STREAM *stream, ASN1_OBJECT *asn1obj );
void dumpBit( STREAM *stream, ASN1_OBJECT *asn1obj );
void dumpByte_wCheck( STREAM *stream, ASN1_OBJECT *asn1obj, int (*fn)( int ) );

int isNumeric( int ch );
int isPrintable( int ch );
int isIA5( int ch );
int isVisible( int ch );

void printContextSpecific( ASN1_OBJECT *asn1obj );
void printConstructed( ASN1_OBJECT *asn1obj, char *type );
void printPrimitive( ASN1_OBJECT *asn1obj, char *type );
void printIndent( int Counter );

enum{
    CONSTRUCTED = 0x01,
    PRIMITIVE   = 0x00
    };

enum{
    UNIVERSAL =        0x00, 
    APPLICATION =      0x01, 
    CONTEXT_SPECIFIC = 0x02, 
    PRIVATE =          0x03 
    };

#endif /* READASN1_H */
