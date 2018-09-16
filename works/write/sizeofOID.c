/********************************************************************

sizeofOID function
Last Modified: 6th October 2000
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <assert.h>

/*
int main(int argc,char *argv[])
{
  assert(argc == 0 || argc > 1);

  printf("%d\n",sizeofOID(argv[1]));
  exit(0);
}
*/

int sizeofOID(char *oid)
{

  int TYPE = 1;
  int LENGTH = 1;
  int length;

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













