#include <stdio.h>      
#include <stdlib.h>      
#include <sys/stat.h>


int
main(int argc, char *argv[])
{
   int         i;
   struct stat   buf;
   char      *tekst;

   for (i = 1; i < argc; i++) {
      printf("%s: \n", argv[i]);
      if (lstat(argv[i], &buf) < 0) {
         printf("lstat error");
         continue;
      }
       switch (buf.st_mode & S_IFMT) {
           case S_IFDIR:  printf("directory\n");               break;
           case S_IFLNK:  printf("symlink\n");                 break;
           case S_IFREG:  printf("regular file\n");            break;
           default:       printf("unknown?\n");                break;
        }
   }
   exit(0);
}