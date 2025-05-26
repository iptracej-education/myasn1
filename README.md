# myasn1
# Time-stamp: <2018-09-15 22:15:21 kiyoshi>

This project is aimed to build an fast ASN1 encoding and decoding code example on
Unix platform (developed at LINUX Redhat 5.2). It provides
basic functions in C to encode and decode ASN1 objects in
BER or DER format. It will cover almost all
ASN1 tags including BMP and UTF8 strings necessary to encode and decode 
all the ASN1 objects. 

I hope that you enjoy it!


OVERVIEW
--------

myASN1 tool includes:

 1) write functions. These will encode the data into ASN1 format
 2) read functions.  These will cedocde the ASN1 data into internal 
    format or dump them out in c
 3) sstream functions. These are file related functions 
 4) smemory functions. THere are memory functions (Malloc,Free...)


HOW TO USE  
-------

This is basically not an application so that you will never have to 
install. I am preparing a sample program to understand
how to incorporate the functions in C source code. If you
want to run the program, just type:

	make

This creates the executable file called samplepkcs7. When you
run this program, type:

	samplepkcs7

This produces the ASN1 encoded binary file, tmp.pk7. If you want
to make sure whether the file are correctly encoded or not, type:

	apps/readasn1 tmp.pk7

That's it.

If you cannot believe what I have done, type

	openssl asn1parse -inform DER -in tmp.pk7

and see the results.


TO DO 
-----

Of course, A documentation should be written clealy and 
libary will be eventually developed.

Error Handling is almost nothing but stupid. This is not
oragnized at all. I will work on this ASAP.


KNOWN BUG
---------

Even thought the long-form length can be read, the size of length
is evaluated as the short form length.


BUG REPORT
----------

I am very pleased to have any information related to bug or 
any suggestion for improvement. Please send the message to
kiwatana@hotmail.co.jp
kiwatanag@gmail.com


