#include <stdio.h>
#include "myASN1.h"
#include "serror.h"

#define dmpfile "tmp.pk7"

/*
  This program writes the simple PKCS #7 ASN.1 codes and output to a file
  called "tmp.pk7" and display the result to STDOUT. 

 */


int main(int argc, char *argv[])
{

  FILE *fout,*fin; 
  char x;
  int size = sizeof(char);
  STREAM stream; 

  /****************************************************************** 
    7. PKCS #7 General Syntax 
    
    ContentInfo :: = SEQUENCE {
                 contentType ContentType,
                 content
		   [0] EXPLICIT ANY DEFINED By contentType OPTIONAL}
    ContentType :: = OBJECT IDENTIFIER
   
    8. Data content type
    Data :: = OCTET STRING

    14. Object idenfifier

    pkcs-7 OBJECT IDENTIFIER :: = 
    { iso(1) member-body(2) US(840) usadsi(113459) pkcs(1) 7}

    data OBJECT IDENTIFIER :: = {pkcs-7 1}

  *******************************************************************/
  
  /* a simple PKCS 7 data value */
  char PKCS_dataType_OID[] = "1.2.840.113459.7.1";
  char test[] = "welcome to myASN1 ASN1 tool";
  char test2[] = "1111000011110000111101";
  int size_Data2 = strlen(test2);
  int size_Data = strlen(test);                         /* V of OCTET STRING */
  int size_Content = sizeofObject(size_Data);           /* V of [0], which is EXPLICIT ANY */
  int size_ContentInfo = sizeofOID(PKCS_dataType_OID)   /* V of SEQUECE */
                     + sizeofContextObject(0,size_Content); 

  fout = fopen(dmpfile,"wb");
  if(fout == NULL){
    error_exit("%s:File cannot open:", argv[0]);
  }
  stream.fileptr = fout;

 /* The above syntax will be constructed as follows, */	
     writeSequence(&stream, size_ContentInfo);
         writeOID( &stream, PKCS_dataType_OID );
         writeContext( &stream, 0, 'C', size_Content ); 
         writeOctetString( &stream, test, size_Data );
     writeBitString( &stream, test2, size_Data2 );
	 
  fclose(fout);
  
  fin = fopen(dmpfile,"rb");
  if(fin == NULL){
    printf("File cannot open. An error occurs\n");
    exit(1);
  }
  while(fread(&x,size,1,fin)>0)
    printf("--%x\n",x);
  fclose(fin);

  exit(0);
}




