/**********************************************************************
 *                                                                    *
 *    myASN1 ASN1 encoding and decoding functions                     *
 *                                                                    *
 *    Written by: Kiyoshi Watanabe   cert.bbcert.org                  *
 *    Last modified: Time-stamp: <01/05/02 22:35:18 kiyoshi>          *
 *                                                                    *
 **********************************************************************/

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "myASN1.h"

/********************************************************************

  myASN1 _writeNumeric functioin
  last modified: 9th October 2000
*********************************************************************/

static void _writeNumeric(STREAM *stream, const long integer)
{
  
  BOOLEAN needsZERO = TRUE;
  BYTE buffer[5];
  int length = 0;

  /* The values are defined because minus number can not be represented 
     as hex decimals like 0xFF0000000L. When you compile with -Wall -W options,
     it will complain that you cannot compare the signed and unsigned values. */

  const long MINUS_24BITINTEGER = -16777216;  /* 11111111 00000000 00000000 00000000 */ 
  const long MINUS_16BITINTEGER = -65536;     /* 11111111 11111111 00000000 00000000 */
  const long MINUS_8BITINTEGER = -256;        /* 11111111 11111111 11111111 00000000 */

  /* Determine the number of bytes necessary to encode the integer and 
     encode it into a temporary buffer */

  /* In case of negative integer */
  if(integer < 0){
        
    if(integer < MINUS_24BITINTEGER) /* Modifiled by Kiyoshi @ 12/19/2000 */
      buffer[length++] = (BYTE)(integer >> 24);
    if(integer <= MINUS_16BITINTEGER) /* Modifiled by Kiyoshi @ 12/19/2000 */
      buffer[length++] = (BYTE)(integer >> 16);
    if(integer <= MINUS_8BITINTEGER) /* Modifiled by Kiyoshi @ 12/19/2000 */
      buffer[length++] = (BYTE)(integer >> 8);
    buffer[length++] = (BYTE)integer;
  }
  else{ 
    if(integer > 0x00FFFFFFL)
      {
	buffer[length++] = (BYTE)(integer >> 24);
	needsZERO = FALSE;
      }
    if(integer >= 0x00800000L && needsZERO)
      buffer[length++] = 0;
    if(integer > 0x0000FFFFL)
      {
	buffer[length++] = (BYTE)(integer >> 16);
	needsZERO = FALSE;
      }
    if(integer >= 0x00008000L && needsZERO)
      buffer[length++] = 0;
    if(integer > 0x000000FFL)
      {
	buffer[length++] = (BYTE)(integer >> 8);
	needsZERO = FALSE;
      }
    if( integer >= 0x00000080L && needsZERO )
      buffer[length++] = 0;
    
    buffer[length++] = (BYTE)integer;
  }

  /* Write the LENGTH and VALUE */
  writeLength( stream, length ); 
  s2fwrite( stream, buffer, length ); 
  
}


/********************************************************************

  myASN1 writeSequence function
  Last Modified: Monday 2nd October 2000
*********************************************************************/
/* This function writes the ASN.1 SEQUENCE TYPE. This function takes
   two variables. You must obtain the length of the sequence
   for this, by calling the sizeof... functions beforehand. */

void writeSequence( STREAM *stream, int length )
{

  writeTag( stream, SEQUENCE_TYPE_W_CONSBIT, 'U');
  writeLength( stream, length );
  
}


/********************************************************************

  myASN1 writeSet function
  Last Modified: Monday 2nd October 2000
*********************************************************************/
/* This function writes the ASN.1 SET TYPE. This function takes
   two variables. You must obtain the length of the sequence
   for this, by calling the sizeof... functions beforehand. */

void writeSet( STREAM *stream, int length )
{

  writeTag( stream, SET_TYPE_W_CONSBIT, 'U');
  writeLength( stream, length );
  
}


/********************************************************************

  myASN1 writeTag function
  Last Modified: Monday 2nd October 2000
*********************************************************************/
void writeTag(STREAM *stream, int tag, char asn1class)
{

  BYTE buffer[5];
  int length = 0;
  int mask_application = 0x40;
  int mask_private = 0xC0;
  int mask_context = 0xA0;

  /* currently assume that the the number will be less than 31 */
  if(asn1class == 'U' || asn1class == 'u')
    buffer[length++] = (BYTE)tag;
  else if(asn1class == 'A' || asn1class == 'a')
    buffer[length++] = (BYTE)(tag | mask_application);
  else if(asn1class == 'P' || asn1class == 'p')
    buffer[length++] = (BYTE)(tag | mask_private);
  else
    buffer[length++] = (BYTE)(tag | mask_context);

  s2fwrite(stream, buffer, length);
}


/********************************************************************

  myASN1 writeContext Function
  Last Modified: 11/17 Friday 2000 
*********************************************************************/
/* 
   This is the function to write a context-specific tag, i.e. [0]...[N]
   The N is virtually infinite. However since the number of context-specific
   tag will not be over 2 bytes range (16383), I will process only the two
   identifier octets only. Normally the tag number is less than 127, but once 
   saw 999 for the tag! So the specification will be:

   In case the tag number is [0] and [30], the tag would be:
   
     8 7 6 5 4 3 2 1   
     1 0 1 . . . . .  (constructed)
   
     8 7 6 5 4 3 2 1   
     1 0 0 . . . . .  (primitive)

   In case the tag is [31] and [16383], the tag would be:

     8 7 6 5 4 3 2 1   
     1 0 1 1 1 1 1 1  (flag)

     8 7 6 5 4 3 2 1     
     1 . . . . . . .     2  

     8 7 6 5 4 3 2 1     
     0 . . . . . . .     N
    

   */

void writeContext( STREAM *stream, unsigned int tagNumber, char formParameter, int tagLength )
{

  BYTE buffer[2];
  int length = 0;
  int mask_contextSpecific = 0x80;
  int mask_constructedForm = 0x20;
  int mask_multileBytes = 0x1F;

  buffer[0] = (BYTE)0;
  buffer[length] |= mask_contextSpecific;

  /* When the formParameter is "Constructed" */
  if( formParameter == 'c' || formParameter == 'C' ){
    buffer[length] |= mask_constructedForm;
  }
  
  /* When the Tag number is 30 (which requires multiple bytes) */
  if ( tagNumber > 0x1E ){
    buffer[length] |= mask_multileBytes;
  }

  /* ----------------------------------------------------------------
   Here is the code to encode the number into the buffer along with 
   the ASN.1 specification. 
  ------------------------------------------------------------------*/

  if( tagNumber > 0x3FFF ){  /* over 16383 */
    printf("The number you specified is too big or too samll to encode!\n");
    exit(1);
  }
  if( tagNumber > 0x7F ){    /* over 127   */ 
    length++;
    buffer[length] = (BYTE)( tagNumber >> 7 ) | 0x80;
  }   
  if( tagNumber > 0x1E ){   /*  over 30    */
    length++;
    buffer[length] = (BYTE)~(~( tagNumber ) | 0x80 );
  }
  else{                     /* less than 30 */
    buffer[length] |= (BYTE)tagNumber; 
  }

  length++;

  s2fwrite( stream, buffer, length );   /* write this tag */
  writeLength( stream, tagLength );     /* write following any content */
 
}


/********************************************************************

  myASN1 writeLength function
  Last Modified: Monday 2nd October 2000
*********************************************************************/
void writeLength(STREAM *stream, const long length){

  BYTE buffer[5];
  int len=0;

  /* length is less than 255 */
  if(length < 0x00000080L){
    buffer[len] = (BYTE)length;
    len++;
  }
  /* When length is more than 255, determine the value of the 
     length of length */
  if(length > 0x00FFFFFFL)
    buffer[len++] = (BYTE)0x84;
  else if(length > 0x0000FFFFL)
    buffer[len++] = (BYTE)0x83;
  else if(length > 0x000000FFL)
    buffer[len++] = (BYTE)0x82;
  else if(length >= 0x00000080L)
    buffer[len++] = (BYTE)0x81;

  /* Determine the number of bytes to encode the lenght
     and encode it into a temporary buffer */

  if(length > 0x00FFFFFFL)
    buffer[len++] = (BYTE)(length >> 24);
  if(length > 0x0000FFFFL)
    buffer[len++] = (BYTE)(length >> 16);
  if(length > 0x000000FFL)
    buffer[len++] = (BYTE)(length >> 8);
  if(length >= 0x00000080L)
    buffer[len++] = (BYTE)length;

  s2fwrite(stream, buffer, len);

}

/********************************************************************
  
  myASN1 writeShortInteger function
  Last Modified: 2nd October 2000
*********************************************************************/
/* This is expected to be called when the ASN.1 INTEGER TYPE
   is BER/DER encoded. This function takes both positive and
   negative integers. A temporary buffer arrary is allocated
   for 5 bytes, since ASN.1 Complete Book recommends that 
   ASN.1 INTEGER TYPE can expect 4 octets as max. */

void writeShortInteger( STREAM *stream, const long integer )
{
  BOOLEAN needsZERO = TRUE;
  BYTE buffer[5];
  int length = 0;
  
  /* The const values are defined because minus number can not be represented 
     as hex decimals like 0xFF0000000L. When you compile with -Wall -W options,
     it will complain that you cannot compare the signed and unsigned values. */

  const long MINUS_24BITINTEGER = -16777216;  /* 11111111 00000000 00000000 00000000 */ 
  const long MINUS_16BITINTEGER = -65536;     /* 11111111 11111111 00000000 00000000 */
  const long MINUS_8BITINTEGER = -256;        /* 11111111 11111111 11111111 00000000 */

  /* Determine the number of bytes necessary to encode the integer and 
     encode it into a temporary buffer */

  if(integer < 0){   /* In case of negative integer */
    
    if(integer < MINUS_24BITINTEGER) /* Modifiled by Kiyoshi @ 12/19/2000 */
      buffer[length++] = (BYTE)(integer >> 24);
    if(integer <= MINUS_16BITINTEGER) /* Modifiled by Kiyoshi @ 12/19/2000 */
      buffer[length++] = (BYTE)(integer >> 16);
    if(integer <= MINUS_8BITINTEGER) /* Modifiled by Kiyoshi @ 12/19/2000 */
      buffer[length++] = (BYTE)(integer >> 8);
    buffer[length++] = (BYTE)integer;
  }
  else{ 
    if(integer > 0x00FFFFFFL)
      {
	buffer[length++] = (BYTE)(integer >> 24);
	needsZERO = FALSE;
      }
    if(integer >= 0x00800000L && needsZERO)
      buffer[length++] = 0;
    if(integer > 0x0000FFFFL)
      {
	buffer[length++] = (BYTE)(integer >> 16);
	needsZERO = FALSE;
      }
    if(integer >= 0x00008000L && needsZERO)
      buffer[length++] = 0;
    if(integer > 0x000000FFL)
      {
	buffer[length++] = (BYTE)(integer >> 8);
	needsZERO = FALSE;
      }
    if( integer >= 0x00000080L && needsZERO )
      buffer[length++] = 0;
    
    buffer[length++] = (BYTE)integer;
  }

  /* Write the length and integer */
  writeTag( stream, INTEGER_TYPE, 'U');
  writeLength( stream, length );
  s2fwrite( stream, buffer, length );

} /* writeShortInteger function */

/********************************************************************

  myASN1 writeOID function
  Last Modified: 8th October 2000
*********************************************************************/
/* Assume that the OID is input as a sequence of char, delimited
   by the ".(comma)". For example, 1.2.2888.1..34 . The first 
   and second value of OID is encoded as (first value) x 40 + 
   (second value). The sub OID value is encoded using the 7 bits
   and the most significant value is used for the flag. If the
   0 is flagged, the value is ended. For example 4401, the bit
   representation is 10001 00110001, but encoded as 
   1010001 00110001.
   f [   ] f[     ]
   l       l
   a       a
   g       g

*/

void writeOID(STREAM *stream, char *oid)
{
  BYTE buffer[50]; /* 5 octets x 10 */
  BYTE last;
  int length;     
  long value[10];  /* the sub sequence is limited in 10 */
  char tmp[20];    /* char oid is roughtly limited in 20 */
  int i = 0;int j = 0;int k = 0;

  while(oid[i] != '\0'){
    if(oid[i] == '.'){
      tmp[j] = '\0';               /* Insert the NULL delimiter */
      value[k] = atol(tmp);        /* and convert the char to i */
      j = 0;memset(tmp,'\0',20);   /* Re-initialize the tmp array */
      i++;                         /* Skip the "." into the next array */
      k++;                         /* use the next array */
    }
    tmp[j] = oid[i];               /* copy the oid value */
    i++;j++;                       /* increment by one in both array */
  }
  value[k] = atol(tmp);            /* convert the last char array to i */
  k++;                             /* use this as the senital in the
				      encoding */

  /* Now BER encode the OID values */
  length = 0;j = 0;
  buffer[length++] = (BYTE)(value[0]*40 + value[1]); 
  j=2;

  while(j < k){

    if(value[j] > 0x00FFFFFFFL){
      buffer[length++] = (BYTE)(value[j] >> 28) | 0x80;
    }  
    if(value[j] > 0x0001FFFFFL){
      buffer[length++] = (BYTE)(value[j] >> 21) | 0x80;
    }  
    if(value[j] > 0x000003FFFL){
      buffer[length++] = (BYTE)(value[j] >> 14) | 0x80;
    }  
    if(value[j] > 0x00000007FL){
      buffer[length++] = (BYTE)(value[j] >> 7) | 0x80;

      /* This is not graceful to obtain the last number,
	 but it works. */
      last = (BYTE)(value[j] << 1);
      buffer[length++] = (BYTE) ~(~(last >> 1) | 0x80);
    }  
    else{
      buffer[length++] = (BYTE)(value[j]);
    }

    j++;
  }

  /* write TLV of the OBJECTIDENTIFIER  */
  writeTag( stream, OBJECT_IDENTIFIER_TYPE, 'U');
  writeLength( stream, length );
  s2fwrite( stream, buffer, length );

}


/********************************************************************

  myASN1 writeEnumerated function
  Last Modified: 9th October 2000
*********************************************************************/
/* ENUMERATED TYPE can take the similar function as in INTEGER TYPE,
   so this function call the writeNumeric, which create a INTEGER-TYPE
   value, but encode as ENUMERATED TYPE.

   */
void writeEnumerated(STREAM *stream, const long number)
{
  writeTag( stream, ENUMERATED_TYPE, 'U' );
  _writeNumeric( stream, number );
}	   


/********************************************************************

  myASN1 writeCharacterString function
  Last Modified: 15th Sunday October, 2000
*********************************************************************/
/* This function can be used with writeOctetString, writeNumericString,
   writeVisibleString, writeIA5String, writePrintableString. 

   This function encodes only the V value of TLV structure in the
   above charaster-based string. You should refer to the other
   function for this usage.

*/

void writeCharacterString(STREAM *stream, BYTE *charString, int length)
{

  s2fwrite( stream, charString, length );

}


/********************************************************************

  myASN1 writeNumericString function
  Last Modified: 15th October 2000
*********************************************************************/
/* The numeric values of string is passed to this function. 
   This function takes 3 arguments and simply assign them to the 
   other fuctnions to encode.

*/

void writeNumericString(STREAM *stream, BYTE *numericString, int length)
{
  writeTag( stream, NUMERICSTRING_TYPE, 'U');
  writeLength( stream, length );
  writeCharacterString( stream, numericString, length );
}


/********************************************************************

  myASN1 writePrintableString function
  Last Modified: 15th October 2000
*********************************************************************/
/* The printable values of string is passed to this function. 
   This function takes 3 arguments and simply assign them to the 
   other fuctnions to encode.

*/

void writePrintableString(STREAM *stream, BYTE *octetString, int length)
{
  writeTag( stream, PRINTABLESTRING_TYPE, 'U');
  writeLength( stream, length );
  writeCharacterString( stream, octetString, length );
}


/********************************************************************

  myASN1 writeVisibleString function
  Last Modified: 15th October 2000
*********************************************************************/
/* The visible values of string is passed to this function. 
   This function takes 3 arguments and simply assign them to the 
   other fuctnions to encode.

*/

void writeVisibleString(STREAM *stream, BYTE *octetString, int length)
{
  writeTag( stream, VISIBLESTRING_TYPE, 'U');
  writeLength( stream, length );
  writeCharacterString( stream, octetString, length );
}


/********************************************************************

  myASN1 writeIA5String function
  Last Modified: 15th October 2000
*********************************************************************/
/* The IA5 values of string is passed to this function. 
   This function takes 3 arguments and simply assign them to the 
   other fuctnions to encode.

*/

void writeIA5String(STREAM *stream, BYTE *octetString, int length)
{
  writeTag( stream, IA5STRING_TYPE, 'U');
  writeLength( stream, length );
  writeCharacterString( stream, octetString, length );
}


/********************************************************************

  myASN1 writeOctetString function
  Last Modified: 15th October 2000
*********************************************************************/
/* The octet values of string is passed to this function. 

   This function takes 3 arguments and simply assign them to the 
   other fuctnions to encode.

*/

void writeOctetString(STREAM *stream, BYTE *octetString, int length)
{
  writeTag( stream, OCTETSTRING_TYPE, 'U');
  writeLength( stream, length );
  writeCharacterString( stream, octetString, length );
}


/********************************************************************

  myASN1 writeBitString function
  Last Modified: 11th October 2000
*********************************************************************/
/* The VALUE of the BIT STRING is encoded as follows:
   The first octet is to set the unused bit. This value is between 0 to 7. 
   The next and sub sequent octets contain the bit values. The first
   bit is mapped to the most significant bit and next bit is mapped to
   the second most significant bit. If the unused bit is there, mapped
   0 to them. 

   Assume that the Input value as char string. I thought that
   is there any solution for the larger bytes bigger than 32 bits.
   The solution is to use GMP library. This takes almost non-limited
   bigger strings of integer. So I decide to assmue that the BIT
   STRING will be created from char string instead of 32 bit string
   especially when you use the RSA encryption

   Complie:
   gcc -o writeBit sstream.o writeBitString.c

   Run: (like)
   kiyoshi#>writeBit 1111000011110000111101
   
   This should be: 02F0F0F4

   */

void writeBitString(STREAM *stream, BYTE *bit, const int length)
{

  BYTE *buffer;
  int number;
  int input;
  int unusedbit;
  int i, j;
  int SHIFT = 7;
  int tmp = length;

  unusedbit = ((length % 8) > 0)? (8 - (length % 8)) : 0;
  number = (length % 8 == 0)? tmp/8+1:tmp/8+2;
  buffer = MALLOC( char, number ); 

  j = 0;
  buffer[j++]=(BYTE)unusedbit;

  for(i=0;bit[i] != '\0';i++){
    input = bit[i] - '0';
    buffer[j] |= ((BYTE)input << SHIFT);
    SHIFT --;

    if(SHIFT < 0){
      buffer[j] |= (BYTE)input;
      SHIFT = 7;
      j++;
    }
  }

  writeTag( stream, BITSTRING_TYPE, 'U');
  writeLength( stream, number );
  s2fwrite( stream, buffer, number );

  free(buffer);  

}

/********************************************************************

  myASN1 sizeofInteger function
  Last Modified: 6th October 2000
*********************************************************************/
/* Assume that the Universal CLASS INTEGER TYPE is used. If you want 
   to specify the APPLICATION CLASS, or PRIVATE CLASS, you probably
   neet to define the Strucutre to check the CLASS. Context-Specific
   class is not clear now (10.6.2000)*/

int sizeofInteger(int length)
{

  int TYPE = 1;
  int LENGTH = 1;

  if(length > 8388608 || length < -16777216)
    return 4 + TYPE + LENGTH;
  else if(length >= 32767 || length <= -65536) 
    return 3 + TYPE + LENGTH;
  else if(length >= 128 || length <= -256) 
    return 2 + TYPE + LENGTH;
  else
    return 1 + TYPE + LENGTH;
}


/********************************************************************

  myASN1 sizeofOID function
  Last Modified: 8th October 2000
*********************************************************************/
int sizeofOID(char *oid)
{

  int length;
  int TYPE = 1;
  int LENGTH = 1;

  long value[10];  /* the sub sequece is limited in 10 */
  char tmp[20];    /* char oid is roughtly limited in 20 */
  int i = 0;int j = 0;int k = 0;

  while(oid[i] != '\0'){
    if(oid[i] == '.'){
      tmp[j] = '\0';               /* Insert the NULL delimiter */
      value[k] = atol(tmp);        /* and convert the char to i */
      j = 0;memset(tmp,'\0',20);   /* Re-initialize the tmp array */
      i++;                         /* Skip the "." into the next array */
      k++;                         /* use the next array of value[] */
    }
  tmp[j] = oid[i];               /* copy the oid value */
  i++;j++;                       /* increment by one in both array */
  }
  value[k] = atol(tmp);            /* convert the last char array to i */
  k++;
  
  length=1;
  
  j=2;
  while(j < k){

    if(value[j] > 0x00FFFFFFFL){
      length++;
    }  
    if(value[j] > 0x0001FFFFFL){
      length++;
    }  
    if(value[j] > 0x000003FFFL){
      length++;
    }  
    if(value[j] > 0x00000007FL){
      length++;
    }
    length++;

    j++;
  }
  return length + TYPE + LENGTH;

}


/********************************************************************

  myASN1 sizeofObject function
  Last Modified: 19th November 2000
*********************************************************************/
/* This function returns the size of char[] to determine
   the length of VALUE */

long sizeofObject( const long valueLength )
{
  if (valueLength < 0)
    return -1; /* used for error */
  else
    return sizeof(BYTE) + sizeofLength(valueLength) + valueLength;
}


/********************************************************************

  myASN1 sizeofContextObject function
  Last Modified: 19th November 2000
*********************************************************************/
long sizeofContextObject( const long tagNumber, const long valueLength )
{
  if (tagNumber < 0 || valueLength < 0)
    return -1; /* used for error */
  else
    return sizeofTag(tagNumber) + sizeofLength(valueLength) + valueLength;
}


/********************************************************************

  myASN1 sizeofLength function
  Last Modified: 19th November 2000
*********************************************************************/
int sizeofLength( const long valueLength )
{
  if(valueLength < 0)
    return -1; /* used for error checking */
  else if (valueLength > 0x00FFFFFFL)
    return 5;
  else if (valueLength > 0x0000FFFFL)
    return 4;
  else if (valueLength > 0x000000FFL)
    return 3;
  else if (valueLength > 0x0000007FL)
    return 2;
  else
    return 1;
}

/********************************************************************

  myASN1 sizeofTag function
  Last Modified: 19th November 2000
********************************************************************/
/*  This function determine the size of Tag Length. I think that
    most of Tag would be 1, i.e. sizeof(BYTE). However, when you
    think the case of Context-Specific, we need to find a way to
    calcualte the value 

    */

int sizeofTag( const long tagNumber )
{

  if(tagNumber < 0)
    return -1; /* used for error checking */
  else if(tagNumber > 0x0000007FL)
    return 3;
  else if(tagNumber > 0x0000001EL)
    return 2;
  else
    return 1;
}


/**********************************************************************
                                                                     
  myASN1 readTag function                                         
  Written by: Kiyoshi Watanabe                                    
 **********************************************************************/
/* This function takes the FILE pointer as input and reads the
   bytes one bye one until the first bit comes at ZERO at MSB. So the 
   function, for instance, has to read the first two 
   bytes, but do not read furtehr!                                   

   ----- TAG -------  -LENGTH-  --VALUE--
   10000001 00000000  00000001  100100100


   Input Parameter:  STREAM *stream 
   Return Value   :  int variable, if the octets more than 4, return FALSE
   Restriction    :  

 */

int readTag( STREAM *stream )
{

  int tagNumber = 0;
  int buffer[3]; /* Assume that there are 4 octets at max for Tag numbers */
  int MSB_check;
  int tmp;
  int i = 0, j = 0; 
  
  assert(stream != NULL);
  assert( stream->fileptr != NULL );
  
  do{
    /* too large to process the tag number */
    if( i > 3 ){
      return FALSE;
    }
    buffer[i] = f2sgetc( stream ); 
    MSB_check = buffer[i] >> 7;
    i++;
  } while( MSB_check == 1 );
  
  /* decrease one to fit the algorithm below */
  i--;

  /* cut off the MSB value, then add the shift-to-the-left value */
  while( i >= 0 ){
    tmp = ( i > 0 ) ? buffer[j] ^ 0x80 : buffer[j];
    tagNumber |= ( tmp << (i * 7) );
    i--; j++;
  }
  return tagNumber;
}

/*********************************************************************

  myASN1 readLength function                                      
  Written by: Kiyoshi Watanabe                                    
 *********************************************************************/
/* This function takes the FILE pointer as input and reads one byte
   at the beginning. When the MSB is Zero, this byte has the short
   form, which has the length value in the subsequent bits. When
   the MSB is One, this byte represents the two alternatives, one is
   the long form and the other is the indefinite. How to distinguish?
   If the subsequent bits represent more than 1, the next and more
   bytes represent the Length value. If the subsequent bits represent
   ZERO, the long form comes until it reaches at the two ZERO bytes
   as delimitor.

   Input Parameter:  STREAM *stream 
   Return Value   :  Long value, otherwise -1 for nothing to read
                                           -2 for exceeding max length
                                           -3 for indefinite error

   Restriction    :  The Long Form is limited 127 bytes at max.
                     The Length Value is limited 2^32 at max.
                     The indefinite Value is not supported currently.

 */
long readLength( STREAM *stream )
{

  int first_byte;           
  long remaining_byte;
  long length = 0;

  int checkMSB = 0;        /* Check the most significant bit */
  int checkREST = 0;       /* Check the remaining bits */
  int long_form_byte_n = 0;

  assert( stream != NULL );
  assert( stream->fileptr != NULL );

  /* checking the first bit and the remaining bits in order to 
     decide what to do next */
  first_byte = f2sgetc( stream );
  if(first_byte < 0){
    return -1;
  }
  checkMSB = (int)( first_byte >> 7 ) & 0x01;

  if( checkMSB == 0x00 ){ 
    /* the short form is processed. This is just to insert the first bit into
       the length value */
    length = first_byte;
  }
  else{
    /* the long form or the indefinite form is processed. In order to distinguish
       the two forms, it it necessary to check the remaining bits */

    checkREST = (int)( ((BYTE)first_byte ^ (BYTE)0x80) );
    long_form_byte_n = checkREST;

    if( checkREST >= 0x01 ){ 
      /* When the remaining bits represent more than one, the long form is used.
	 the loop function puts the remaining byte(s) into long tyep variable 
	 and shift them to approprite position(s). */
      
      long_form_byte_n--;
      while( long_form_byte_n >= 0x00 ){

	remaining_byte = f2sgetc( stream );
	if(remaining_byte < 0){
	  return -1;
	}
	if(long_form_byte_n == 4 && remaining_byte > 0x7F){
	  /*
	  fprintf( OUTPUT, "ERROR: The length of value exceeds the max of long type\n");
	  exit( EXIT_FAILURE );
	  */
	  return -2;
	}
	
	length += remaining_byte << ( 8 * long_form_byte_n );
	long_form_byte_n--;
      }
    }
    else{ 
      /* When the remaining bits represents ZERO, the indefinite form is used.
	 The first loop obtains the remaining byte(s) and put them into temporary 
         buffer array until the two successive bytes has ZERO values. The second 
	 loop puts the remaining byte(s) into the long variable and 
	 shift them into the appropriate postion(s). 
	 */ 
      return -3;
    }
  }

  return length;
}


/*********************************************************************

  myASN1 readShortInteger function                                
  Written by: Kiyoshi Watanabe                                   
 **********************************************************************/
/* 

   @Input Parameter:  STREAM *stream (file related operations)
		      const long integer (actual length of the integer bits)
   @Return Value   :  Long type
   @Restriction    :  Assume the Integer should be within 4 octets.
                      (-2147483648 ~ 2147483647)
 */
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

  assert(stream != NULL);
  
  if(length < 1){
    return FALSE;
  }
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

  /* when negative, insert the 0xFFs first and then shift the value til the end */
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


