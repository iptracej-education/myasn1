/********************************************************************

  The Program: writeTag

  The Description of Program:

        This write a CLASS type and encode it to a buffer

  Author: Kiyoshi Watanabe
  
  Last Modified: Monday 2nd October 2000

*********************************************************************/

/* Define the ASN.1 Type. Here is the definition for the
   four classes */

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
#define SEQUENCE_TYPE                      0x10 
#define SEQUENCE_OF_TYPE                   0x10 
#define SEQUENCE_TYPE_W_CONSBIT            0x30 
#define SEQUENCE_OF_TYPE_W_CONSBIT         0x30 

/* SET and SET OF (UNIVERSAL 17) are representaed 
   as 00110001 */
#define SET_TYPE                           0x11 
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
#define GENERALSTRING_TYPE                 0x1B /* 26 */
#define UNIVERSALSTRING_TYPE               0x1C /* 28 */
#define BMPSTRING_TYPE                     0x1E /* 30 */

/* UNIVERSAL 31- Reserved for addenda to this
   Recommendation International Standard */


#include <stdio.h>
#include "myASN1.h"

void writeTag(STREAM *stream, int tag, char asn1class)
{

  BYTE buffer[5];
  int length = 0;
  int mask_application = 0x40;
  int mask_private = 0xC0;
  int i;

  /* currently assume that the the number will be less than 31 */
  if(asn1class == 'U' || asn1class == 'u')
    buffer[length++] = (BYTE)tag;
  else if(asn1class == 'A' || asn1class == 'a')
    buffer[length++] = (BYTE)(tag | mask_application);
  else if(asn1class == 'P' || asn1class == 'p')
    buffer[length++] = (BYTE)(tag | mask_private);
  
  /* The following function will be called separately, since the context specific
     tag will need an additnional parameter (primitive/construcuted). #normally
     the universal tag already assign the parameter when the tag called. (11/17/2000)

     else if(asn1class == 'C' || asn1class == 'c')
     writeContext(buffer, length); 
  */
  
  else{
    printf("error: You specify the incorrect flag. Should be u,a,p,c\n");
    exit(1);
  }
  s2fwrite(stream, buffer, length);
}







