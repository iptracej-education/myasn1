/********************************************************************

  writeContext Function
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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "myASN1.h"

int main( int argc, char *argv[] )
{

  char test[]="kiyoshi";

  /* assert( argc > 2 ); */
  writeContext( NULL, 1, 'c', strlen(test) );
  exit(0);

}

void writeContext( STREAM *stream, unsigned int tagNumber, char formParameter, int tagLength )
{

  BYTE buffer[2];
  int length = 0;
  int mask_contextSpecific = 0x80;
  int mask_constructedForm = 0x20;
  int mask_multileBytes = 0x1F;
  int i;

  buffer[0]=0;

  buffer[length] |= mask_contextSpecific;
  
  /* When the formParameter is "Constructed" */
  if( formParameter == 'c' || formParameter == 'C'){
    buffer[length] |= mask_constructedForm;
  }
  
  /* When the Tag number is 30 (which requires multiple bytes) */
  if ( tagNumber > 0x1E ){
    buffer[length] |= mask_multileBytes;
  }

 /**************************************************************** 
   Here is the code to encode the number into the buffer along with 
   the ASN.1 specification. 
  ***************************************************************/

  if( tagNumber > 0x3FFF ){  /* over 16383 */
    printf("The number you specified is too big or small to encode!\n");
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

  /* The following is the code to display the bit string */
  length++;  
  for(i=0;i<length;i++)
    bit_print_char(buffer[i]);
  /* end */

  //s2fwrite( stream, buffer, length ); /* write this tag */
  //writeLength( stream, tagLength );   /* write following by any*/
 
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
