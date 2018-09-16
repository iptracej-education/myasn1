/********************************************************************

sizeofInteger function
Last Modified: 6th October 2000
*********************************************************************/

#include <stdio.h>
#include <assert.h>

/*
int main(int argc,char *argv[])
{
  assert(argc > 1);
  printf("%d\n",sizeofInteger(atoi(argv[1])));
  exit(0);
}
*/

int sizeofInteger(int length)
{
  /* Assume that the Universal CLASS INTEGER TYPE is used. If you want 
     to specify the APPLICATION CLASS, or PRIVATE CLASS, you probably
     neet to define the C strucutre to check the CLASS. Context-Specific
     class is not clear to me now (10.6.2000)*/

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



