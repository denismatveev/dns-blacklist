#include"config_parser.h"
#include"dnsproxy.h"
#include"blist.h"
#include <stdio.h>
#include<stdlib.h>
#include<getopt.h>




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


int main(int argc, char *argv[])
{

  /* For config and its parsing */

  int fd;
  char *help_message;
  char* config_file;

  config cfg;
  FILE *fp;

  help_message="Usage: %s -c <path to config file> - Start dns proxy with config file.\n"
               "\t\t-h\t\t\t - Print help message and exit.\n";
  char *program_name=argv[0];
  char oc;

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



  if((fp=fopen(config_file,"r")) == NULL)
    {
      perror(config_file);
      exit(1);
    }

  if((cfg=parse_config(fp)) == NULL)
    return 1;

/*
  pid_t pid = fork();
  if(pid < 0) {
      perror("fork");
      return -1;
    }
  if(pid != 0)
    exit(0);
  pid = setsid();
  if(pid < -1) {
      perror("setsid");
      return -1;
    }
  chdir("/");
  fd = open ("/dev/null", O_RDWR, 0);
  if(fd != -1) {
      dup2 (fd, 0);
      dup2 (fd, 1);
      dup2 (fd, 2);
      if(fd > 2)
        close (fd);
    }
  umask(0);

  signal(SIGPIPE, SIG_IGN); // signal - ANSI C signal handling,SIG_IGN - the signal is ignored. SIGPIPE - Broken pipe: write to pipe with no readers(if TCP connection fails);
*/
  dnsproxy(cfg->forward);

  remove_config(cfg);

  return 0;

}
