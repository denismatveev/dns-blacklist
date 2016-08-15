#include"config_parser.h"
#include"blist.h"
#include <arpa/inet.h>
#define INIT_SIZE 1024
#define IPLEN 16
/*  disclaimer
My the simpliest config parser
this parser does not check if domain is valid
if allocation memory has been occured the program fails
There is no if ip address is valid
disson@yandex.ru

*/

size_t remove_LF(char* str)
{
  size_t getstrlen=strlen(str);
  if(str[getstrlen-1] == '\n')
    *(str+getstrlen - 1)=0;
  return getstrlen-=1;
}

int check_if_ip_valid(char* ip)
{

  struct sockaddr_in sa;
  return !inet_pton(AF_INET, ip, &(sa.sin_addr));
}

config parse_config(FILE* f)
{

  config cfg;
  blist blacklist;
  char* getstr;
  long curpos;
  size_t getstrlen;


  if((cfg=(config)malloc(sizeof(struct __config))) == NULL)
    return NULL;
  if((cfg->forward=(char*)calloc(IPLEN,sizeof(char))) == NULL)
    return NULL;

  blacklist=blist_init();
  cfg->blacklist=blacklist;

  if((getstr=(char*)calloc(INIT_SIZE,sizeof(char))) == NULL)
    return NULL;




  while((fgets(getstr,INIT_SIZE,f)) != NULL)
    {
      getstrlen=remove_LF(getstr);

      if(!(strcmp(getstr,"[forward]")))
        while((fgets(getstr,INIT_SIZE,f)) != NULL)
          {
            getstrlen=remove_LF(getstr);
            if(!check_if_ip_valid(getstr))
              {
                strncpy(cfg->forward,getstr,IPLEN);
                break;
              }
          }
      if(!(strcmp(getstr,"[blacklist]")))
        {
          curpos = ftell(f);
          while((fgets(getstr,INIT_SIZE,f)) != NULL)
            {
              getstrlen=remove_LF(getstr);

              if(!strcmp(getstr,"[forward]"))
                {
                  fseek(f,curpos,SEEK_SET);
                  break;
                }
              if(getstrlen)
                if((blist_add_to(blacklist,getstr)) == -1)
                  return NULL;

            }
        }
    }


  if(!feof(f) && ferror(f))
    return NULL;
  if(!strlen(cfg->forward)) // if forward server is not set
    return NULL;



  free(getstr);

  return cfg;

}


void remove_config(config cfg)
{

  free(cfg->forward);
  free(cfg->blacklist);
  free(cfg);
}
