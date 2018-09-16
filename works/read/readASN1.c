/************************************************************************
 *                                                                      *
 * myASN1 ASN1 dump tool: readASN1                                      *
 *                                                                      *
 * Written by: Kiyoshi Watanabe                                         *
 * Last Modified: Time-stamp: <01/05/02 01:59:10 kiyoshi>               * 
 *                                                                      *
 * compile:                                                             *
 *   gcc -o readASN1 -Wall -W -Wtraditional -ansi sstream.o myASN1.o    *
 *   readTag.o readLength.o getTLHeader.c readShortInteger.c readASN1.c * 
 *                                                                      *
 * run:                                                                 *          
 *   readASN1 test/sequence.binary                                      *
 *                                                                      *
 ************************************************************************/

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include "myASN1.h"
#include "readASN1.h"

static long totalASN1Number = 0;  /* The total number of ASN1 object */
static int hextotal = 0;          /* HEX representation of totalASN1Number */
static int indentation = 0;       /* flag to do indentation */
static int dump = 0;              /* flag to dump strings */

char *tagTypeName[] = {
 NULL,
 "BOOLEAN",                       
 "INTEGER",
 "BITSTRING",
 "OCTETSTRING",
 "NULL",
 "OBJECTIDENTIFIER", 
 "OBJECTDESCRIPTOR",
 "EXTERNAL",
 "REAL",
 "ENUMERATED",
 "EMBEDDED_PDY",
 "UTF8STRING",
 NULL,
 NULL,
 NULL,
 "SEQUENCE",
 "SET",
 "NUMERICSTRING",
 "PRINTABLESTRING",
 "T61STRING",
 "VIDEOTEXSTRING",
 "IA5STRING",
 "UTCTIME",
 "GENERALIZEDTIME",
 "GRAPHICSTRING",
 "VISIBLESTRING",
 "GENERALSTRING",
 "UNIVERSALSTRING",
 NULL,
 "BMPSTRING"
};

static void printUsage( void );
static void dumpHeader( ASN1_OBJECT *asn1obj, char *type );

#define printContextSpecific(asn1obj,type) dumpHeader(asn1obj,type) 
#define printConstructed(asn1obj,type) dumpHeader(asn1obj,type)
#define printPrimitive(asn1obj,type) dumpHeader(asn1obj,type)

/********************************************************************

  MAIN function
*********************************************************************/
int main(int argc, char *argv[])
{

  FILE *fin; 
  STREAM stream;
  char *filename;
  int c;

  if( argc < 1 ){
    printUsage();
    exit(1);
  }
  
  while( --argc > 0 ){
    if ((*++argv)[0] == '-' ){
      while((c = *++argv[0])){
	switch( c ){
	case 'i':
	  indentation = 1;
	  break;
	case 'x':
	  hextotal = 1;
	  break;
	case 'd':
	  dump = 1;
	  break;
	case 'h':
	  printUsage();
	default:
	  fprintf(stderr, "find: illegal option %c\n", c);
	  printUsage();
	}
      }
    }
    else{
      filename = *argv;
    }
  }


  /* open a file to read a binary */
  fin = fopen(filename,"rb");
  if(fin == NULL){
    fprintf(OUTPUT, "File cannot open. An error occurs\n"); 
    exit(1);
  }
  stream.fileptr = fin;
  readASN1( &stream );
  fclose(fin);
  exit(0);
}

/********************************************************************

  readASN1 function
*********************************************************************/
void readASN1( STREAM *stream )
{

 int buffer;
      
  /* read the binary data */
  while(( buffer = fgetc(stream->fileptr)) != EOF ){
    readASN1Object( stream, buffer );
  }
}

/********************************************************************

  readASN1Object function
*********************************************************************/
static int indentCounter = 1;
static long cons[100];
static long from[100];

void readASN1Object( STREAM *stream, int buffer )
{
  int i, currentCounter;
  ASN1_OBJECT asn1obj;

  assert(stream != NULL);
  assert(stream->fileptr != NULL);
  assert(buffer);

  getTLHeader( stream, buffer, &asn1obj ); 

  if( asn1obj.tagClass == APPLICATION ){
    ;
  }
  else if( asn1obj.tagClass == PRIVATE ){
    ;
  }

  else if( asn1obj.tagClass == CONTEXT_SPECIFIC ){
    switch( asn1obj.tagForm ){
    case CONSTRUCTED:
      printContextSpecific( &asn1obj, "CONTEXT" ); 
      break;
    case PRIMITIVE:
      ;
    default:
      break;
    }
  }     
  else if( asn1obj.tagClass == UNIVERSAL ){ 
    switch( asn1obj.tagNumber ){
    case SEQUENCE_TYPE:
    case SET_TYPE:
      printConstructed( &asn1obj, tagTypeName[asn1obj.tagNumber] );
      break;
    case BOOLEAN_TYPE:
      printPrimitive( &asn1obj, tagTypeName[asn1obj.tagNumber] );
      fprintf( OUTPUT, "%s\n", readBoolean( stream ) );
      break;
    case INTEGER_TYPE:
      printPrimitive( &asn1obj, tagTypeName[asn1obj.tagNumber] );
      fprintf( OUTPUT, "%ld\n",readShortInteger( stream, asn1obj.lengthNumber ) );
      break;
    case BITSTRING_TYPE:
    case OCTETSTRING_TYPE:
      printPrimitive( &asn1obj, tagTypeName[asn1obj.tagNumber] );
      fputc('\n', OUTPUT);
      if(dump){
	(asn1obj.tagNumber == BITSTRING_TYPE) ? printBitString( stream, &asn1obj ) 
	  : printOctetString( stream, &asn1obj );
      }
      else{
	fseek(stream->fileptr, asn1obj.lengthNumber, SEEK_CUR);
      }
      break;
    case NULL_TYPE:
      printPrimitive( &asn1obj, tagTypeName[asn1obj.tagNumber] );
      fputc('\n', OUTPUT);
      if( asn1obj.lengthNumber != 0 )
	fprintf( OUTPUT, "NULL has more than or less than 0 values\n");
      break;
    case OBJECT_IDENTIFIER_TYPE:
      printPrimitive( &asn1obj, tagTypeName[asn1obj.tagNumber] );
      printOID( stream, asn1obj.lengthNumber );
      break;
    case OBJECT_DESCRIPTOR_TYPE:
    case EXTERNAL_TYPE_AND_INSTANCE_OF_TYPE:
    case REAL_TYPE:
    case EMBEDDED_PDY_TYPE:
      printPrimitive( &asn1obj, tagTypeName[asn1obj.tagNumber] );
      fputc('\n', OUTPUT);
      fseek(stream->fileptr, asn1obj.lengthNumber, SEEK_CUR);
      break;
    case ENUMERATED_TYPE:
      printPrimitive( &asn1obj, tagTypeName[asn1obj.tagNumber] );
      printf( "%ld\n",readShortInteger( stream, asn1obj.lengthNumber ) );
      break;
    case NUMERICSTRING_TYPE:
      printPrimitive( &asn1obj, tagTypeName[asn1obj.tagNumber] );
      printNumericString( stream, &asn1obj );
      break;
    case PRINTABLESTRING_TYPE:
      printPrimitive( &asn1obj, tagTypeName[asn1obj.tagNumber] );
      printPrintableString( stream, &asn1obj );
      break;
     case IA5STRING_TYPE:
      printPrimitive( &asn1obj, tagTypeName[asn1obj.tagNumber] );
      printIA5String( stream, &asn1obj );
      break;
    case UTCTIME_TYPE:
    case GENERALIZEDTIME_TYPE:
      printPrimitive( &asn1obj, tagTypeName[asn1obj.tagNumber] );
      printVisibleString( stream, &asn1obj );
      break;
    case VISIBLESTRING_TYPE:
      printPrimitive( &asn1obj, tagTypeName[asn1obj.tagNumber] );
      printVisibleString( stream, &asn1obj );
      break;
    case T61STRING_TYPE:
    case GRAPHICSTRING_TYPE:
    case VIDEOTEXSTRING_TYPE:
    case GENERALSTRING_TYPE:
    case UNIVERSALSTRING_TYPE:
    case UTF8STRING_TYPE:
    case BMPSTRING_TYPE:
      printPrimitive( &asn1obj, tagTypeName[asn1obj.tagNumber] );
      fputc('\n', OUTPUT);
      printOctetString( stream, &asn1obj );
      break;
    default: 
      break; 
    }
  }
  else{
    fprintf(OUTPUT, "Invalid Tag Number is specified\n");
    exit( EXIT_FAILURE );
  }

  /* calculate the total ASN1 number */
  if( asn1obj.tagForm == 0x01 ){
    totalASN1Number += (asn1obj.tagLength + asn1obj.lengthLength);
  }else{
    totalASN1Number += asn1obj.TLVLength;
  }
  
  if(indentation){
  /* do the indentation. 
      1) cons[indentCounter] contains the length of the SEQUENCE or SET values. 
      2) from[indentCounter] contains the total number of the ASN1 objects that
         just was read
  */
    if(asn1obj.tagNumber == SEQUENCE_TYPE || asn1obj.tagNumber == SET_TYPE){
      cons[indentCounter] = asn1obj.lengthNumber;
      from[indentCounter] = totalASN1Number;
      /* printf("cons[%d]:%ld from[%d]:%ld\n",indentCounter, 
	 cons[indentCounter],indentCounter,from[indentCounter]);  */
      indentCounter++;
    }

    /* set back to the original position 
       This can be achieved by substructing from[] from the current total number. 

     */
    currentCounter = indentCounter;
    for(i = 1; i <= currentCounter; i++){
      /* printf("cons[%d]:%ld <= totalN:%ld from[%d]:%ld %ld\n", 
	 (currentCounter - i), cons[currentCounter - i], totalASN1Number, 
	 (currentCounter - i),from[currentCounter - i], 
	 (totalASN1Number - from[currentCounter - i])); */
      if(cons[currentCounter - i] == (totalASN1Number - from[currentCounter - i])){
	indentCounter--;
      }
    }
  }
}

/********************************************************************

  dumpHeader function: to print ASN1 Header information
*********************************************************************/
static void dumpHeader( ASN1_OBJECT *asn1obj, char *type )
{
  /* display the total number of the All objects read in Decimal or Hex */
  (hextotal)?
    fprintf(OUTPUT, " %4lX: ", totalASN1Number ):fprintf(OUTPUT, " %4ld: ", totalASN1Number );

  /* display the tagClass and tagNumber */
  if(asn1obj->tagClass == 0x00)
    fputc('u', OUTPUT);
  else if(asn1obj->tagClass == 0x01)
    fputc('a', OUTPUT);
  else if(asn1obj->tagClass == 0x02)
    fputc('c', OUTPUT);
  else
    fputc('p', OUTPUT);
  fprintf(OUTPUT, "=%2d: ", asn1obj->tagNumber );

  /* display the TLHeader Length */
  fprintf(OUTPUT, "hl=%2lu: ", asn1obj->tagLength + asn1obj->lengthLength );

  /* display the Length of Value */
  fprintf(OUTPUT, "l=%4lu: ", asn1obj->lengthNumber );

  /* display how depth are generated */
  /* fprintf(OUTPUT, "i=%2d: ", indentCounter ); */
  printIndent( indentCounter ); 

  /* display the TAG name */
  if(!strcmp( type, "CONTEXT" )){
    fprintf(OUTPUT, "[ %d ]", asn1obj->tagNumber );
  }
  else{
    fprintf(OUTPUT, "%s", type);
  }
  
  /* align the text values to the certain position. 
     change the value of 23 if you want to align them in different postions */
  if(asn1obj->tagForm != 0x00)
    puts("");
  else
    printIndent( 23 - (strlen(type) + indentCounter) );
}

/********************************************************************

  printIndent function: to print space
*********************************************************************/
void printIndent( int counter )
{
  int i;
  for (i = 0; i <= counter; i++)
    putchar(' ');

}

/********************************************************************

  printUsage function: to print usage and exit eventually
*********************************************************************/
void printUsage(){

  puts("usage: readASN1 [options] binary_file");
  puts(" -i              - do indentation");
  puts(" -d              - dump BIT and OCTET stinrg");
  puts(" -h              - help");
  puts(" -x              - print HEX number of total ASN1 ojbect");
  puts("");
  puts("readASN1: ASN1 dump tool written by Kiyoshi Watanabe");
  exit(2);
}
