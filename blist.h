#ifndef BLIST_HPP
#define BLIST_HPP
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#define INIT_SIZE 1024
#define STR_SIZE 1024
#define ALLOC_ERROR -1
typedef struct _blist_t{
   char *value;
} _blist_t;
typedef _blist_t* blist_t;

typedef struct _blist{
  blist_t* array;
  size_t capacity;
  size_t q; // current number of elements

} _blist;
typedef _blist* blist;

blist blist_init(void); // return pointer to the first element
int blist_add_to(blist,const char*);
int blist_check(blist,const char*);
void blist_delete(blist);


#endif
