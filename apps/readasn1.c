/************************************************************************
 *                                                                      *
 * myASN1 ASN1 dump tool: readasn1                                      *
 *                                                                      *
 * Written by: Kiyoshi Watanabe                                         *
 * Last Modified: Time-stamp: <01/05/02 22:29:38 kiyoshi>               * 
 *                                                                      *
 * compile:                                                             *
 *   make                                                               *
 *                                                                      *
 * run:                                                                 *          
 *   readasn1 [options] binary_file                                     *
 *                                                                      *
 ************************************************************************/

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "myASN1.h"
#include "readasn1.h"

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

static void printUsage( void );
static void dumpHeader( ASN1_OBJECT *asn1obj, char *type );

#define printContextSpecific(asn1obj,type) dumpHeader(asn1obj,type) 
#define printConstructed(asn1obj,type) dumpHeader(asn1obj,type)
#define printPrimitive(asn1obj,type) dumpHeader(asn1obj,type)

/********************************************************************

  myASN1 MAIN function
*********************************************************************/
int main(int argc, char *argv[])
{

  FILE *fin; 
  STREAM stream;
  char *filename;
  char *programname;
  int buffer;
  int c;
  ASN1_OBJECT asn1obj;

  programname = argv[0];

  if( argc < 1 ){
    printUsage();
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
	  error_warn("find: illegal option %c", c);
	  printUsage();
	}
      }
    }
    else{
      filename = *argv;
    }
  }

  fin = fopen(filename,"rb");
  if(fin == NULL){
    error_exit("%s: The specified file cannot be opened:", programname);
  }
  stream.fileptr = fin;
  stream.filename =filename; 

  /* Should test whether any readable bytes are there */
  if(!(buffer = f2sgetc( &stream )))
    error_exit("%s EOF: Nothing to read", stream.filename);
  /* another checking on the T and L value */
  if(!(getTLHeader( &stream, buffer, &asn1obj ))){
    error_exit("%s: The bad ASN1 header", stream.filename);
  }
  fseek( stream.fileptr, 0, SEEK_SET );

  /* Now start to read the ASN1 objects */
  while(( buffer = fgetc(stream.fileptr)) != EOF ){
    readASN1Object( &stream, buffer );
  }
  fclose(fin);
  exit(0);
}

/********************************************************************

  myASN1 readASN1Object function
  Written by: Kiyoshi Watanabe                                   
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

  if(!(getTLHeader( stream, buffer, &asn1obj ))){
    error_exit("%s: The bad ASN1 header", stream->filename);
  }

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


/**********************************************************************
                                                                     
  myASN1 getTLHeader function                                     
  Written by: Kiyoshi Watanabe                                    
 **********************************************************************/
/* This function inserts the T(Tag) information into several variables. The
   tagClass is for Universal, Application, Context_specific, and Private.
   The tagForm is for constructed or primitive. The tagNumber is for the
   specific number assigned by the tag class. 

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

   Input Parameter:  STREAM *stream int buffer ASN1_OBJECT *object
   Return Value   :  TRUE otherwise FALSE
   NOTE           :  calls readTag and readLength 
   */

int getTLHeader( STREAM *stream, const int buffer, ASN1_OBJECT *asn1obj )
{

  int checkTagNumber;
  int ret = TRUE;

  assert(stream != NULL);
  assert(stream->fileptr != NULL);

  /* read the T part of TLV */
  asn1obj->tagClass = ( buffer & 0xC0 ) >> 6;        /* 11000000 */
  asn1obj->tagForm = ( buffer & 0x20 ) >> 5;         /* 00100000 */

  checkTagNumber = ( buffer & 0x1F );                /* 00011111 */

  /* check the Tag Number constraints */
  if( checkTagNumber <= 0x00 ){  /* 0 or minus number */
    /* Right now only the context-specific number can contain the Zero or
       negative number. */
    if( asn1obj->tagClass == 0x02 && asn1obj->tagForm == 0x01 ){
      asn1obj->tagNumber = checkTagNumber;
    }
    else{
      error_warn("Currently the negative Tag number is not supported");
      ret = FALSE;
    }
  }
  else if( checkTagNumber >= 0x1F ){ /* larger than one octet value */                            
    if( !(asn1obj->tagNumber = readTag( stream )))
      error_exit("Too large to process the Tag number");
  }
  else{
    asn1obj->tagNumber = checkTagNumber;
  }

  /* read the L part of TLV */
  asn1obj->lengthNumber = readLength( stream );
  if( feof(stream->fileptr) || ferror(stream->fileptr))
    error_exit("%s EOF: Nothing to read for Length Value", stream->filename);

  if( asn1obj->lengthNumber == 0 ){
    if( asn1obj->tagClass != 0x00 || asn1obj->tagNumber != 0x05 )
      error_exit("%s: 0 length value is not allowed except in NULL", stream->filename);
  }
  else if( asn1obj->lengthNumber == -1L )
    error_exit("%s EOF: Nothing to read for Length Value", stream->filename);
  else if( asn1obj->lengthNumber == -2L )
    error_exit("%s: The length exceeds the max of long type", stream->filename);
  else if( asn1obj->lengthNumber == -3L )
    error_exit("%s The indefinite form is not supported currently", stream->filename);
  else if( asn1obj->lengthNumber <= -4L)
    ret = FALSE;

  asn1obj->tagLength = sizeofTag( asn1obj->tagNumber );
  asn1obj->lengthLength = sizeofLength( asn1obj->lengthNumber ); 
  asn1obj->TLVLength = asn1obj->tagLength
                       + asn1obj->lengthLength 
                       + asn1obj->lengthNumber;
  return ret;

}


/********************************************************************

  myASN1 dumpHeader function: to print ASN1 Header information
  Written by: Kiyoshi Watanabe                                   
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

  myASN1 printIndent function: to print space
  Written by: Kiyoshi Watanabe                                   
*********************************************************************/
void printIndent( int counter )
{
  int i;
  for (i = 0; i <= counter; i++)
    putchar(' ');

}

/********************************************************************

  myASN1 printUsage function: to print usage and exit eventually
  Written by: Kiyoshi Watanabe                                   
*********************************************************************/
void printUsage(){

  puts("Usage: readasn1 [options] binary_file");
  puts("Example: readasn1 -di cert.der");
  puts("");
  puts(" -i              - do indentation");
  puts(" -d              - dump BIT and OCTET stinrg");
  puts(" -h              - help");
  puts(" -x              - print HEX number of total ASN1 object");
  puts("");
  puts("ASN1 dump tool written by Kiyoshi Watanabe 2001 -2003.");
  puts("Report bugs to <kiyoshi@tg.rim.or.jp>.");
  exit(2);
}


/********************************************************************

  myASN1 dumpBit function:
  Written by: Kiyoshi Watanabe                                   
*********************************************************************/
void dumpBit( STREAM *stream, ASN1_OBJECT *asn1obj )
{
  int i, j;
  int unusedbit;
  int bits;

  int MaxChar = CHAR_BIT;
  const char bitMASK = 1 << (MaxChar - 1);
  long length = asn1obj->lengthNumber - 1;

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

  myASN1 printBitString function: read bit strings and print them
  Written by: Kiyoshi Watanabe                                   
*********************************************************************/
void printBitString( STREAM *stream, ASN1_OBJECT *asn1obj )
{

  assert( stream != NULL );
  assert( stream->fileptr != NULL );

  dumpBit( stream, asn1obj );

}


/********************************************************************

  myASN1 dumpByte function: get bytes and print them.
  Written by: Kiyoshi Watanabe                                   
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

  myASN1 printOctetString function: read octet strings and print them
  Written by: Kiyoshi Watanabe                                   
*********************************************************************/
void printOctetString( STREAM *stream, ASN1_OBJECT *asn1obj )
{

  assert( stream != NULL );
  assert( stream->fileptr != NULL );

  dumpByte( stream, asn1obj );

}


/**********************************************************************
                                                                     
  myASN1 printOID function                                        
  Written by: Kiyoshi Watanabe                                    
 **********************************************************************/
/* 
   Description: To read the ASN1 object of OID value and convert it
                into the char.

   Input Parameter:  STREAM *stream  - file pointer 
                     long length,    - ASN.1 object Length
                     char *oid       - pointer for storing the OID data
   Return Value   :  None
   Restriction    :  
 */

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


/********************************************************************

  myASN1 dumpByte function: get bytes and print them.
  Written by: Kiyoshi Watanabe
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

  myASN1 isNumeric function: check the numeric or not
  Written by: Kiyoshi Watanabe
*********************************************************************/
int isNumeric( int ch )
{
  /* check whether the char is within the numberic char values */
  if( ch >= 128 || checkChar[ch] < N)
    return FALSE;
  return TRUE;
}


/********************************************************************

  myASN1 isPrintable function: check the printable or not
  Written by: Kiyoshi Watanabe
*********************************************************************/
int isPrintable( int ch )
{
  if( ch >= 128 || checkChar[ch] < P)
    return FALSE;
  return TRUE;  
}


/********************************************************************

  myASN1 isVisible function: check the printable or not
  Written by: Kiyoshi Watanabe
*********************************************************************/
int isVisible( int ch )
{
  if( ch >= 128 || checkChar[ch] < V)
    return FALSE;
  return TRUE;  
}


/********************************************************************

  myASN1 isIA5 function: check the printable or not
  Written by: Kiyoshi Watanabe
*********************************************************************/
int isIA5( int ch )
{
  if( ch >= 128 || checkChar[ch] < I)
    return FALSE;
  return TRUE; 
}


/********************************************************************

  myASN1 printNumericString function: read numeric strings and print them
  Written by: Kiyoshi Watanabe
*********************************************************************/
void printNumericString( STREAM *stream, ASN1_OBJECT *asn1obj )
{

  assert( stream != NULL );
  assert( stream->fileptr != NULL );

  dumpByte_wCheck( stream, asn1obj, isNumeric );

}


/********************************************************************

  myASN1 printPrintableString function:
  Written by: Kiyoshi Watanabe
*********************************************************************/
void printPrintableString( STREAM *stream, ASN1_OBJECT *asn1obj )
{
  dumpByte_wCheck( stream, asn1obj, isPrintable ); 
}


/********************************************************************

  myASN1 printT61String function:
  Written by: Kiyoshi Watanabe
*********************************************************************/
void printT61String( STREAM *stream, ASN1_OBJECT *asn1obj )
{
  dumpByte( stream, asn1obj ); 
}


/********************************************************************

  myASN1 printIA5String function:
  Written by: Kiyoshi Watanabe
*********************************************************************/
void printIA5String( STREAM *stream, ASN1_OBJECT *asn1obj )
{
  dumpByte_wCheck( stream, asn1obj, isIA5 ); 
}


/********************************************************************

  myASN1 printVisibleString function:
  Written by: Kiyoshi Watanabe
*********************************************************************/
void printVisibleString( STREAM *stream, ASN1_OBJECT *asn1obj )
{
  dumpByte_wCheck( stream, asn1obj, isVisible );
}





