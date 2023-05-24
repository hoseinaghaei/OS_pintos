<div dir="rtl">

## گزارش تمرین گروهی ۳


گروه 

-----

> نام و آدرس پست الکترونیکی اعضای گروه را در این قسمت بنویسید.

* Seyyed Alireza Ghazanfari alireza79.ghazanfari@gmail.com
* Armin DelgosarMaher arminmaher@gmail.com
* AmirMahdi Kousheshi amk_amir82@yahoo.com
* Hossein Aghaei h.aghaei.araei@gmail.com

## bugs
First we had some problems with runing make check but and so it was because of our cache problems that we fixed.

next problem was for persistence tests that we had problem in opening fs.tar after that we fix our file system problem and solved this error.

# Algorithms

we had no more changes from this phase design of course for handling thread safety we add a new lock to each inode to handle synch on reading and writing operations and we will acquire it first of reading or writing and release after finishing the operation. So in result we remove the general lock we had. 

For dir section we add some syscalls as we were said in the doc and we had to change most of syscalls about filesys and rewrite it again. 

# Other points

we did not add any binary file to git. 

our code is so simple and we tried to use clean code to reduce complexity. Our code style is GNU code style and it's so similar to pintos coding style.

Abour lists that we had in cache we implement all operations and algorithms that we need ourselves.

no copy code.

no comment code in the final version.

## تست‌های پیاده‌سازی شده

- First Test scenario that we implemented is checking `hit rate` of our cache. We implement this test for checking that is our cache improve our system or not. It will check hit rate before and after loading a file to cache to show that this loading will be used by the system for future readings. How we implement this test is as same as we had in doc. After that we'll create a sample file and after that will open it and write some thing on it. Then we will reset our statistics about the cache such as current hit rate. The first level of test is counting hit rate for reading this file for the first time and after that again reset the stats and read again and check the difference between these two proportion. If the second proportion is more than first one we can say our cache is doing very well.

## خروجی تست‌ها


- kernel output

```c
Copying tests/filesys/extended/hit-rate to scratch partition...
Copying tests/filesys/extended/tar to scratch partition...
qemu-system-i386 -device isa-debug-exit -hda /tmp/ROWBSc3itY.dsk -hdb tmp.dsk -m 4 -net none -nographic -monitor null
PiLo hda1
Loading............
Kernel command line: -q -f extract run hit-rate
Pintos booting with 3,968 kB RAM...
367 pages available in kernel pool.
367 pages available in user pool.
Calibrating timer...  314,163,200 loops/s.
hda: 1,008 sectors (504 kB), model "QM00001", serial "QEMU HARDDISK"
hda1: 193 sectors (96 kB), Pintos OS kernel (20)
hda2: 240 sectors (120 kB), Pintos scratch (22)
hdb: 5,040 sectors (2 MB), model "QM00002", serial "QEMU HARDDISK"
hdb1: 4,096 sectors (2 MB), Pintos file system (21)
filesys: using hdb1
scratch: using hda2
Formatting file system...done.
Boot complete.
Extracting ustar archive from scratch device into file system...
Putting 'hit-rate' into the file system...
Putting 'tar' into the file system...
Erasing ustar archive...
Executing 'hit-rate':
(hit-rate) begin
(hit-rate) create file
(hit-rate) open file
(hit-rate) reset cache
(hit-rate) reset cache
(hit-rate) your cache is the best cache
(hit-rate) end
hit-rate: exit(0)
Execution of 'hit-rate' complete.
Timer: 118 ticks
Thread: 57 idle ticks, 58 kernel ticks, 3 user ticks
hdb1 (filesys): 519 reads, 452 writes
hda2 (scratch): 239 reads, 2 writes
Console: 1119 characters output
Keyboard: 0 keys pressed
Exception: 0 page faults
Powering off...
```
- test result

```c
PASS
```

- ck file

```c
# -*- perl -*-
use strict;
use warnings;
use tests::tests;
use tests::random;
check_expected (IGNORE_EXIT_CODES => 1, [<<'EOF']);
(hit-rate) begin
(hit-rate) create file
(hit-rate) open file
(hit-rate) reset cache
(hit-rate) reset cache
(hit-rate) your cache is the best cache
(hit-rate) end
EOF
pass;
```
in this test we use two new syscall which we define them. One of them is to set hit and miss number to 0 and one of them is for getting (# hit / (# hit + # miss)) and we will compare this propotion to decide. 

for this test we add a new data struct too for saving and counting number of hits and number of misses.


- Second test is for second scenario of doc. In this test we want to review this point that when we want to write a block of data completely, we won't read it from disk first and we will write it directly. First of all we add a variable to the last session new defined data structure to keep number of writes on disk too and we will increment it when we are flushing cache data to disk. So now we will add a new syscall to track this number in the test file and we will add setting this variable 0 to reset function that we define in last question. 

- And now about test and its result. first of all we will reset the number of writes to disk to 0 and then create a file with 64KB size and fill it by a random data. Of course we will write on that file 1B repeatedly and after that we will read all of its data in a same way. Then we will use our new syscall and get the number of writes on disk and we will compare with 128 - 5 and 128 + 5 (for tokerance) and if it is in this range we will pass the test.

## خروجی تست‌ها

- kernel output

```c
Copying tests/filesys/extended/merge_changes to scratch partition...
Copying tests/filesys/extended/tar to scratch partition...
qemu-system-i386 -device isa-debug-exit -hda /tmp/pgQrSwFtWK.dsk -hdb tmp.dsk -m 4 -net none -nographic -monitor null
PiLo hda1
Loading............
Kernel command line: -q -f extract run merge_changes
Pintos booting with 3,968 kB RAM...
367 pages available in kernel pool.
367 pages available in user pool.
Calibrating timer...  392,806,400 loops/s.
hda: 1,008 sectors (504 kB), model "QM00001", serial "QEMU HARDDISK"
hda1: 193 sectors (96 kB), Pintos OS kernel (20)
hda2: 239 sectors (119 kB), Pintos scratch (22)
hdb: 5,040 sectors (2 MB), model "QM00002", serial "QEMU HARDDISK"
hdb1: 4,096 sectors (2 MB), Pintos file system (21)
filesys: using hdb1
scratch: using hda2
Formatting file system...done.
Boot complete.
Extracting ustar archive from scratch device into file system...
Putting 'merge_changes' into the file system...
Putting 'tar' into the file system...
Erasing ustar archive...
Executing 'merge_changes':
(merge_changes) begin
(merge_changes) file created
(merge_changes) reset is ok
(merge_changes) opened file
(merge_changes) get write number
(merge_changes) merge changes is correct
(merge_changes) end
merge_changes: exit(0)
Execution of 'merge_changes' complete.
Timer: 209 ticks
Thread: 92 idle ticks, 55 kernel ticks, 63 user ticks
hdb1 (filesys): 770 reads, 671 writes
hda2 (scratch): 238 reads, 2 writes
Console: 1184 characters output
Keyboard: 0 keys pressed
Exception: 0 page faults
Powering off...
```
- test result 

```c
PASS
```

- ck file

```c
# -*- perl -*-
use strict;
use warnings;
use tests::tests;
use tests::random;
check_expected (IGNORE_EXIT_CODES => 1, [<<'EOF']);
(merge_changes) begin
(merge_changes) file created
(merge_changes) reset is ok
(merge_changes) opened file
(merge_changes) get write number
(merge_changes) merge changes is correct
(merge_changes) end
EOF
pass;
```



## ایرادهای هسته

- First of all if kernel return unreliable hit rate we can not use it to test and show that our cache is working correctly.

- Second one is if kernel update the list of cache between two operation of reading on our file in hit rate test it will make cache unstable for testing hit rate.

- For second test we can say that kernel still can write on disk directly on some special conditions and we are just calculating the number of writings from cache to disk so if this operation happend we will loose this test because we can not trust to the number of writes and reads that we had on cache on a file.

## تجربه تست نوشتن

easy


## مسئولیت هر فرد

refactoring and clean code -- armin
buffer cache -- hossein & alireza
o.w. amir mahdi


