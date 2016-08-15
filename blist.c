#include"blist.h"
#define ALLOC_ERROR -1
#define INIT_SIZE 1024
/* disclaimer */
/* my simple iplementation of black list using an array,qsort and bsearch */
/* it hasn't size limitation */
/* if allocation memory has been occured the program fails */
/* disson@yandex.ru */


blist blist_init(void)
{
  blist a;
  if((a=(blist)malloc(sizeof(_blist))) == NULL)
    return NULL;
  if((a->array=(blist_t*)calloc(INIT_SIZE,sizeof(blist_t))) == NULL)
    return NULL;
  a->capacity=INIT_SIZE;
  a->q=0; // current number of elements
  return a;

}


int mycmp(const void* a,const void* b)
{
  int z=strcasecmp(((blist_t)a)->value,((blist_t)b)->value);
  return z;
}

int blist_add_to(blist bl,char* s)
{
  blist_t blt, *newptr;

  if((blt=(blist_t)malloc(sizeof(_blist_t))) == NULL)
    return ALLOC_ERROR;
  if((blt->value=(char*)calloc(INIT_SIZE,sizeof(char))) == NULL)
    return ALLOC_ERROR;


  // search if element already is in the list
  //printf("a=%s\nb=%s\n",&blt->value,bl->array->value);
  strncpy(blt->value,s,INIT_SIZE);
  if(((bsearch(&blt,bl->array,bl->q,sizeof(blist_t),mycmp)) != NULL))
    {
      free(blt);
      free(blt->value);
      return 0;// if not found
    }


  if(bl->capacity == 0)
    return -2;

  if(bl->q < bl->capacity)
    {
      *(bl->array+bl->q)=blt;
      bl->q++;
    }
  else
    {
      if((newptr=(blist_t*)realloc(bl->array,sizeof(blist_t)*(bl->capacity+INIT_SIZE))) == NULL)
        return ALLOC_ERROR;
      bl->array=newptr;
      *(bl->array+bl->q)=blt;
      bl->q++;
      bl->capacity+=INIT_SIZE;
    }

  //sort

  qsort(bl->array,bl->q,sizeof(blist_t),mycmp);

  return 0;


}

int blist_check(blist bl,char* str)
{

  // dynamically creates an element
  blist_t b;
  int ret;
  if((b=(blist_t)malloc(sizeof(struct _blist_t))) == NULL)
    return ALLOC_ERROR;

  if((b->value=(char*)calloc(INIT_SIZE,sizeof(char))) == NULL)
    return ALLOC_ERROR;
  strncpy(b->value,str,INIT_SIZE);

  //binary search

  if((bsearch(&b,bl->array,bl->q,sizeof(blist_t),mycmp)) != NULL)
    ret=0;//found
  else
    ret=1; //not found
  free(b);
  free(b->value);
  return ret;
}


void blist_delete(blist bl)
{
  size_t i;
  for(i=0;i<bl->q;i++)
    free(bl->array[i]);

  free(bl);
}
