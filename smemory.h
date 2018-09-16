/**********************************************************************
 *                                                                    *
 *    smemory function declarations                                   *
 *                                                                    *
 *    Written by: Kiyoshi Watanabe                                    *
 *    Last modified: Time-stamp: <01/01/21 18:27:03 kiyoshi>          *
 *                                                                    *
 **********************************************************************/

#ifndef SMEMORY_H
#define SMEMORY_H

/* Definitions for a less error-prone memory functions. */

#include <stdlib.h> 
#define malloc     
#define MALLOC(type,num) (type *)MEM_alloc((num)*sizeof(type), __FILE__, __LINE__)
#define REALLOC(ptr,type,nbytes) (type *)MEM_realloc((ptr), (nbytes)*sizeof(type), __FILE__, __LINE__)
#define FREE(ptr) ((void)MEM_free((ptr), __FILE__, __LINE__), (ptr)=NULL)
#define DEREF(ptr) ((ptr)=NULL)
#define RAISE(e) Except_raise(&(e), __FILE__, __LINE__)

typedef struct Except_T {
  char *reason;
}EXCEPT_T;

extern void *MEM_alloc( size_t size, const char *file, int line );
extern void *MEM_realloc( void *currentp, size_t nbytes, const char *file, int line );
extern void MEM_free( void *ptr, const char *file, int line );
extern void Except_raise( const EXCEPT_T *e, const char *file, int line );

#endif
