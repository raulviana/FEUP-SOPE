

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char* argv[], char* envp[]){
    
 /*   Thus, to test for a regular file (for example), one could write:

           stat(pathname, &sb);
           if ((sb.st_mode & S_IFMT) == S_IFREG) {
               /* Handle regular file */
   /*        }

       Because tests of the above form are common, additional macros are
       defined by POSIX to allow the test of the file type in st_mode to be
       written more concisely:

           S_ISREG(m)  is it a regular file?

           S_ISDIR(m)  directory?

           S_ISCHR(m)  character device?

           S_ISBLK(m)  block device?

           S_ISFIFO(m) FIFO (named pipe)?

           S_ISLNK(m)  symbolic link?  (Not in POSIX.1-1996.)

           S_ISSOCK(m) socket?  (Not in POSIX.1-1996.)

       The preceding code snippet could thus be rewritten as:

           stat(pathname, &sb);
           if (S_ISREG(sb.st_mode)) {
               /* Handle regular file */
     //      }
    
    
    
    char *name = "./processFile";
    char *arguments[2];
    arguments[0] = name;
    arguments[1] = argv[1];
    arguments[2] = NULL;
   execvp(name, arguments);

    exit(0);
}