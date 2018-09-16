/********************************************************************

  PROGRAM: strbit2i
     
  DESCRIPTION: This program input the bit strings and convert
               it to integer representation

  Last Modified: by Kiyoshi Watanabe 30th September 2000

  NOTE: I am not satisfied with the treatment of negative
        integer. This is because the negative number was out of 
	scope when I started. I will fix this soon.

*********************************************************************/

#include <stdio.h>

int strbit2i(char *strbit, int octet);
int twoSquare(int power);
int NEGATIVE = 0;

int main(int argc, char *argv[])
{
  int i,n;
  int value = 0;

  if(argc == 1 || argc > 5){
    printf("Usage: strbit2i (4-octet bits)\n");
    exit(1);
  }      
  
  n = argc - 1;
  printf("argc:%d n:%d\n",argc,n);
  for(i=1;i<argc;i++){
    printf("argv:%s\n",argv[i]);
    value += strbit2i(argv[i],n);
    printf("value:%d\n",value);
    n--;
  }

  if(NEGATIVE){
    printf("i  :%d\n",(~value+1)-1);
    printf("hex:%x\n",(~value+1)-1);
  }
  else{
    printf("i  :%d\n",value);
    printf("hex:%x\n",value);
  }
  exit(0);
}

int strbit2i(char *strbit, int octet){

  int integer = 0;
  int N = 8;
  int power = octet * N;
  int i;
  
  /* NEGATIVE FLAG should be global */
  if(NEGATIVE || (octet == 4) && strbit[N-1] == '1'){
    for(i=0; i<N; i++){
      if((strbit[i])== '1')
	strbit[i] = '0';
      else
	strbit[i] = '1';
    }
    NEGATIVE = 1;
  }
  
  /* determine the upper octets's values */
  if(octet>1){
    for(i=0; i<N; i++){
      if((strbit[i]) == '1'){
	integer = integer + twoSquare(power);
      }
      power--;
    }
  }
  /* determine the last octet value */
  if(octet==1){
    /* determine the value[0] - [6] */
    for(i=0; i<N-1; i++){
      if((strbit[i]) == '1'){
	integer = integer + twoSquare(power);
      }
      power--;
    }
    /* determine the value[7] */
    if(strbit[N-1] == '1'){
      integer += 1;
    }
  }

  return integer;
}

int twoSquare(int power){
  
  int i=1;
  int integer=1;
  while(i<power){
    integer *= 2;
    i++;
  }
  return integer;
}
  



