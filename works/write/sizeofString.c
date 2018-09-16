#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "myASN1.h"

int main(int argc, char *argv[])
{
  int length;
  
  assert(argc < 1 || argc > 3);
  length = sizeofObject(atol(argv[1]));
  printf("Length of sizeofObject is : %d\n", length);
  length = sizeofContextObject(atol(argv[2]), atol(argv[3]));
  printf("Length of sizeofContextObject is : %d\n", length);
  exit(0);
}

/********************************************************************

  sizeofObject function
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

  sizeofContextObject function
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

  sizeofLength function
  Last Modified: 19th November 2000

*********************************************************************/

static int sizeofLength( const long valueLength )
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

  sizeofTag function
  Last Modified: 19th November 2000

/********************************************************************
/*  This function determine the size of Tag Length. I think that
    most of Tag would be 1, i.e. sizeof(BYTE). However, when you
    think the case of Context-Specific, we need to find a way to
    calcualte the value 

    */

static int sizeofTag( const long tagNumber )
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


