#include"blist.h"
#define ALLOC_ERROR -1
#define INIT_SIZE 1024
// my simple iplementation of black list using an array


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

  //printf("a=%s,b=%s\n",((blist_t)a)->value,((blist_t)b)->value);
  //return strcasecmp(((blist_t)a)->value,((blist_t)b)->value);
  return z;
}

int blist_add_to(blist a,char* s)
{
  blist_t b;

  if((b=(blist_t)malloc(sizeof(_blist_t))) == NULL)
    return ALLOC_ERROR;
  b->value=s;

  // search if element already is in the list
  if((bsearch(&b,a->array,a->q,sizeof(blist_t),mycmp)) != NULL)
    {
      free(b);
      return 2;// if not found
    }

  if(a->capacity == 0)
    return -2;
  if(a->array == NULL)
    *(a->array)=b;
  else
    if(a->q < a->capacity)
      *(a->array+a->q++)=b;
    else
      {
        blist_t newptr;
        if((newptr=realloc(a->array,sizeof(blist_t)+INIT_SIZE)) == NULL)
          return ALLOC_ERROR;
        *(a->array+a->q++)=b;
        a->capacity+=INIT_SIZE;
      }


  //sort

  qsort(a->array,a->q,sizeof(blist_t),mycmp);

  return 0;


}

int blist_check(blist bl,char*str)
{

  // dynamically creates an element
  blist_t b;
  if((b=(blist_t)malloc(sizeof(struct _blist_t))) == NULL)
    return ALLOC_ERROR;
  b->value=str;
  //binary search
  int ret;
  if((bsearch(&b,bl->array,bl->q,sizeof(blist_t),mycmp)) != NULL)
    ret=0;//found
  else
    ret=1; //no found
  free(b);
  return ret;
}


void blist_delete(blist bl)
{
  size_t i;
  for(i=0;i<bl->q;i++)
    free(bl->array[i]);

  free(bl);
}
