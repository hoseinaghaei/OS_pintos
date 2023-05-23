#include "tests/main.h"
#include "tests/lib.h"
#include "user/syscall.h"

void
test_main (void)
{
    int file_size = 64000;
    char *fname = "test.txt";
    int fd;
    CHECK (create (fname, file_size), "file created");
    CHECK (cache_reset (), "reset is ok");
    CHECK (fd = open (fname), "opened file");
    char buffer = 100;
    for (size_t i = 0; i < file_size; i++)
    {
        write (fd, &buffer, 1);
    }
    for (size_t i = 0; i < file_size; i++)
    {
        read (fd, &buffer, 1);
    }
    int number;
    CHECK (number = cache_write_number (), "get write number");
    CHECK (number >= 128 - 5 && number <= 128 + 5, "merge changes is correct");
}