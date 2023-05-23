/* Child process for syn-read test.
   Reads the contents of a test file a byte at a time, in the
   hope that this will take long enough that we can get a
   significant amount of contention in the kernel file system
   code. */

#include "user/syscall.h"

#include "tests/lib.h"
#include "tests/main.h"

#define FILE_SIZE 64

void
test_main ()
{
   char *fname = "a";
   int fd;
   CHECK (create (fname, FILE_SIZE), "create file");
   CHECK (fd = open (fname), "open file");
   char buffer = 100;
   for (size_t i = 0; i < FILE_SIZE; i++)
   {
      write (fd, &buffer, 1);
   }
   
   CHECK(cache_reset (), "reset cache");
   for (size_t i = 0; i < FILE_SIZE; i++)
   {
      read (fd, &buffer, 1);
   }
   
   int fhr = hit_number ();        /* first hit rate */
   close (fd);

   int newfd = open (fname);
   CHECK(cache_reset (), "reset cache");
   for (size_t i = 0; i < FILE_SIZE; i++)
   {
      read (newfd, &buffer, 1);
   }
   int shr = hit_number ();        /* second hit rate */
   close (newfd);
   CHECK (shr > fhr, "your cache is the best cache");
}