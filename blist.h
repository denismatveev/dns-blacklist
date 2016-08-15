#ifndef BLIST_HPP
#define BLIST_HPP
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

typedef struct _blist_t {
   char *value;
} _blist_t;
typedef _blist_t* blist_t;

typedef struct{
  blist_t* array;
  size_t capacity;
  size_t q; // current number of elements

} _blist;
typedef _blist* blist;

blist blist_init(void); // return pointer to the first element
int blist_add_to(blist,char*);
int blist_check(blist,char*);
void blist_delete(blist);


#endif
