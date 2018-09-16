/********************************************************************

  writeOID function
  Last modified: 6th October, 2000
*********************************************************************/

/* Assume that the OID is input as a sequence of char, delimited
   by the ".(comma)". For example, 1.2.2888.1.34. The first 
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

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include "myASN1.h"

void writeOID(STREAM *stream, char *oid);

int main(int argc, char *argv[])
{

  STREAM *stream;
  assert(argc>1);
  writeOID(stream, argv[1]);
  exit(0);
}
  

void writeOID(STREAM *stream, char *oid)
{
  BYTE buffer[50]; /* 5 octets x 10 */
  BYTE last;
  int length;     
  long value[10];  /* the sub sequece is limited in 10 */
  char tmp[20];    /* char oid is roughtly limited in 20 */
  int i = 0;int j = 0;int k = 0;int l;

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

  for(l = 0; l<k ;l++)
    printf(" oid %d\n", value[l]);


  /* Now BER encode the OID values */
  length = 0;j = 0;
  buffer[length++] = (BYTE)(value[0]*40 + value[1]); 
  j=2;

  while(j < k){

    printf("--%d\n",value[j]);

    if(value[j] > 0x00FFFFFFFL){ /* 00001111 11111111 11111111 11111111  (268435455) */
      buffer[length++] = (BYTE)(value[j] >> 28) | 0x80;
    }  
    if(value[j] > 0x0001FFFFFL){ /* 00000000 00001111 11111111 11111111 (2097151) */
      buffer[length++] = (BYTE)(value[j] >> 21) | 0x80;
    }  
    if(value[j] > 0x000003FFFL){ /* 00000000 00000000 11111111 11111111 (16383) */
      buffer[length++] = (BYTE)(value[j] >> 14) | 0x80;
    }
    if(value[j] > 0x00000007FL){ /* 00000000 00000000 00000000 11111111 (127) */
      buffer[length++] = (BYTE)(value[j] >> 7) | 0x80;

      /* This is not graceful to obtain the last number,
	 but it works. */
      last = (BYTE)(value[j] << 1);
      buffer[length++] = (BYTE) ~(~(last >> 1) | 0x80);

      /*
	if(value[j] >= 0x80 && value[j] <= 0xFF){
	printf("dd\n");
	buffer[length++] = (BYTE) ~(~(last >> 1) | 0x80);
	}
	else
	buffer[length++] = (BYTE)(last >> 1);
	*/
    }  
    else{
      buffer[length++] = (BYTE)(value[j]);
    }

    j++;
  }

  printf("length:%d\n",length);
  for(i=0;i<length;i++)
    bit_print_char(buffer[i]);

  /* write TLV of the OBJECTIDENTIFIER  */
  //writeTag( stream, OBJECT_IDENTIFIER_TYPE, 'U');
  //writeLength( stream, length );
  //s2fwrite( stream, buffer, length );

}



void bit_print_char(char a)
{
  int i;
  char mask;
  int n = CHAR_BIT;
  mask = 1 << (n-1);

  for(i=1;i<=n;++i){
    putchar(((a & mask) == 0) ? '0':'1');
    a <<=1;
    if(i % CHAR_BIT == 0 && i < n)
      putchar(' ');
  }
  putchar('\n');
}



