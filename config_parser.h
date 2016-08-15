#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP
#include"blist.h"

typedef struct __config
{
  char* forward;
  blist blacklist;

} __config;
typedef __config* config;
config parse_config(FILE*);

void remove_config(config);



#endif // CONFIG_PARSER_HPP
