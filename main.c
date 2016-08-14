#include <stdio.h>
#include<stdlib.h>
#include<getopt.h>
#include"blist.h"

/*
 * Написать DNS прокси-сервер с поддержкой "черного" списка доменных имен.
 1. Для параметров используется конфигурационный файл, считывающийся при запуске сервера;
 2. "Черный" список доменных имен находится в конфигурационном файле;
 3. Адрес вышестоящего сервера также находится в конфигурационном файле;
 4. Сервер принимает запросы DNS-клиентов на стандартном порту;
 5. Если запрос содержит доменное имя, включенное в "черный" список, сервер возвращает клиенту ответ, заданный
конфигурационным файлом (варианты: not resolved, адрес в локальной сети, ...).
 6. Если запрос содержит доменное имя, не входящее в "черный" список, сервер перенаправляет запрос вышестоящему
серверу, дожидается ответа и возвращает его клиенту.
Язык разработки: Python/C
Использование готовых библиотек: без ограничений
Остальные условия/допущения, не затронутые в тестовом задании - на усмотрение кандидата.
*/


int config_parser(char**, FILE*);



int main(int argc, char *argv[])
{

  char *help_message;
  char* config_file;

  help_message="Usage: %s -c <path to config file> - Start dns proxy with config file.\n"
               "\t\t-h\t\t\t - Print help message and exit.\n";
  char *program_name=argv[0];
  char oc;
/*
  if(argc < 2)
    {
      fprintf(stderr, help_message, program_name);
      exit(EXIT_FAILURE);
    }
  while((oc=getopt(argc, argv, ":hc:")) != -1)
    {
      switch (oc)
        {
        case  'h':
          fprintf(stderr, help_message, program_name);
          exit(EXIT_FAILURE);

        case 'c':
          config_file=optarg;
          break;
        case '?':
          fprintf(stderr,help_message,program_name);
          exit(EXIT_FAILURE);
        case ':':
          fprintf(stderr, "Option -%c requires config filename\n",optopt);
          exit(EXIT_FAILURE);
        default:
          fprintf(stderr,help_message,program_name);
          exit(EXIT_FAILURE);
        }
    }
  if (optind != argc)
    {
      fprintf(stderr,help_message,program_name);
      exit(EXIT_FAILURE);
    }

*/

blist m;
m=blist_init();

blist_add_to(m,"aaa");
blist_add_to(m,"bbb");
blist_add_to(m,"aaa");
blist_add_to(m,"ccc");
int i=0;
for(i=0;i < m->q;i++)
  printf("%s\n",(m->array[i])->value);

blist_delete(m);



  return 0;
}
