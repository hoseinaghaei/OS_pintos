
تمرین گروهی ۱/۰ - آشنایی با pintos
======================

شماره گروه:
-----
> نام و آدرس پست الکترونیکی اعضای گروه را در این قسمت بنویسید.

حسین آقائی h.aghaei.araei@gmail.com

سید علیرضا غضنفری alireza79.ghazanfari@gmail.com 

امیرمهدی کوششی amk_amir82@yahoo.com

آرمین دلگسار ماهر arminmaher@gmail.com 




مقدمات
----------
> اگر نکات اضافه‌ای در مورد تمرین یا برای دستیاران آموزشی دارید در این قسمت بنویسید.


> لطفا در این قسمت تمامی منابعی (غیر از مستندات Pintos، اسلاید‌ها و دیگر منابع  درس) را که برای تمرین از آن‌ها استفاده کرده‌اید در این قسمت بنویسید.

آشنایی با pintos
============
>  در مستند تمرین گروهی ۱۹ سوال مطرح شده است. پاسخ آن ها را در زیر بنویسید.


## یافتن دستور معیوب

۱.
    mov    0x24(%esp),%eax
    address : 0xbfffffe4 + 0x24 = 0xc0000008 

۲.
    0x8048757
    
۳.
    at function `_start`
    mov    0x24(%esp),%eax
  
۴.

    grep -rnw 'lib/' -e '_start'
   
```
lib/user/user.lds:3:ENTRY(_start) 
lib/user/entry.c:4:void _start (int argc, char *argv[]);  
lib/user/entry.c:7:_start (int argc, char *argv[])
```

```c
#include <syscall.h>

int main (int, char *[]);
void _start (int argc, char *argv[]);

void
_start (int argc, char *argv[])
{
  exit (main (argc, argv));
}
```
<pre> <code>
    08048754 <_start>:                                                                                      
    8048754:       83 ec 1c                sub    $0x1c,%esp                                                
    8048757:       8b 44 24 24             mov    0x24(%esp),%eax                                           
    804875b:       89 44 24 04             mov    %eax,0x4(%esp)                                            
    804875f:       8b 44 24 20             mov    0x20(%esp),%eax                                         
    8048763:       89 04 24                mov    %eax,(%esp)                                              
    8048766:       e8 35 f9 ff ff          call   80480a0 <main>                                           
    804876b:       89 04 24                mov    %eax,(%esp)   
    804876e:       e8 49 1b 00 00          call   804a2bc <exit>
</code> </pre>
   
   در کالینگ کانونشن x86 ابتدا ارگومان‌ها را از راست به چپ در داخل استک پوش میکنیم (استک به سمت پایین رشد می‌کند) و در نهایت ادرس دستور بعدی (ادرس بازگشت) را در استک پوینتر قرار میدهیم. یعنی زمانی که تابع مقصد شروع به کار می‌کند استک پوینتر به ادرس ریترن اشاره دارد.
   اینجا ابتدا استک پوینتر را ۲۸ بایت پایین می‌اوریم(۲۸ بایت حافظه میگیریم).سپس ادرس argv را در eax یا accumulator register قرار می‌دهیم و در خط بعد محتویات آن را در یک ادرس بالاتر از استک پوینتر می‌گذاریم.حالا نوبت به arc می‌رسد و در دو خط بعدی در حال کپی کردن آن هستیم. در نهایت call ادرس فعلی را در ادرس استک پوینتر سیو کرده و تابع main را کال می‌کند. در نهایت ریترن ولیو تابع main را در استک پوش کرده و این‌بار تابع exit را کال می‌کنیم.
   
   

۵. در دستور مربوطه، تابع _start سعی داشت تا  ارگومان دوم خود را از استک بردارد که به آن دسترسی نداشت. شاید تابع کالر، به درستی مقادیر را در استک قرار نداده بود.

## به سوی crash

۶.

    0xc000e000, main
    idle

<pre> <code>
pintos-debug: dumplist #0: 0xc000e000 {tid = 1, status = THREAD_RUNNING, name = "main", '\000' <repeats 11 times>, stack = 0xc000edec <incomplete sequence \357>, priority = 31, allelem = {
prev = 0xc0035910 <all_list>, next = 0xc0104020}, elem = {prev = 0xc0035920 <ready_list>, next = 0xc0035928 <ready_list+8>}, pagedir = 0x0, magic = 3446325067}
pintos-debug: dumplist #1: 0xc0104000 {tid = 2, status = THREAD_BLOCKED, name = "idle", '\000' <repeats 11 times>, stack = 0xc0104f34 "", priority = 0, allelem = {prev = 0xc000e020, next = 0xc0035918 <all_list+8>}, elem = {prev = 0xc0035920 <ready_list>, next = 0xc0035928 <ready_list+8>}, pagedir = 0x0, magic = 3446325067}
</code> </pre>

۷.
<pre> <code>
#0  process_execute (file_name=file_name@entry=0xc0007d50 "do-nothing") at ../../userprog/process.c:36
#1  0xc0020268 in run_task (argv=0xc00357cc <argv+12>) at ../../threads/init.c:288
#2  0xc0020921 in run_actions (argv=0xc00357cc <argv+12>) at ../../threads/init.c:340
#3  main () at ../../threads/init.c:133
</code> </pre>


<pre> <code>
sema_init (&temporary, 0);
process_wait (process_execute (task));
a->function (argv);
run_actions (argv);
</code> </pre>

<pre> <code>


</code> </pre>


۸.

    main, idle, do-nothing\000\000\000\000\000


<pre> <code>
pintos-debug: dumplist #0: 0xc000e000 {tid = 1, status = THREAD_BLOCKED, name = "main", '\000' <repeats 11 times>, stack = 0xc000eeac "\001", priority = 31, allelem = {prev = 0xc0035910 <all_list>, next = 0xc0104020}, elem = {prev = 0xc0037314 <temporary+4>, next = 0xc003731c <temporary+12>}, pagedir = 0x0, magic = 3446325067}
pintos-debug: dumplist #1: 0xc0104000 {tid = 2, status = THREAD_BLOCKED, name = "idle", '\000' <repeats 11 times>, stack = 0xc0104f34 "", priority = 0, allelem = {prev = 0xc000e020, next =0xc010a020}, elem = {prev = 0xc0035920 <ready_list>, next = 0xc0035928 <ready_list+8>}, pagedir = 0x0, magic = 3446325067}
pintos-debug: dumplist #2: 0xc010a000 {tid = 3, status = THREAD_RUNNING, name = "do-nothing\000\000\000\000\000", stack = 0xc010afd4 "", priority = 31, allelem = {prev = 0xc0104020, next = 0xc0035918 <all_list+8>}, elem = {prev = 0xc0035920 <ready_list>, next = 0xc0035928 <ready_list+8>}, pagedir = 0x0, magic = 3446325067}
</code> </pre>
۹.
<pre> <code>
process_execute (const char *file_name)
{
char *fn_copy;
tid_t tid;

sema_init (&temporary, 0);
/* Make a copy of FILE_NAME.
Otherwise there's a race between the caller and load(). */
fn_copy = palloc_get_page (0);
if (fn_copy == NULL)
return TID_ERROR;
strlcpy (fn_copy, file_name, PGSIZE);

/* Create a new thread to execute FILE_NAME. */
tid = thread_create (file_name, PRI_DEFAULT, start_process, fn_copy);
if (tid == TID_ERROR)
palloc_free_page (fn_copy);
return tid;
}
</code> </pre>
------
exactly in the line:
<pre> <code>
tid = thread_create (file_name, PRI_DEFAULT, start_process, fn_copy);
</code> </pre>
start_process becomes created.

۱۰.
<pre> <code>
{edi = 0x0, esi = 0x0, ebp = 0x0, esp_dummy = 0x0, ebx = 0x0, edx = 0x0, ecx = 0x0, eax = 0x0, gs = 
0x23, fs = 0x23, es = 0x23, ds = 0x23, vec_no = 0x0, error_code = 0x0, frame_pointer = 0x0, eip = 0x8048754, cs = 0x1b, eflags = 0x202, esp = 0xc0000000, ss = 0x23}
</code> </pre>

۱۱. در تابع start_process، کرنل می‌خواهد برنامه‌ای را که در آدرس آن در ورودی داده شده‌است را آماده اجرا کند. کرنل ابتدا رجیستر‌ها را به یک مقدار اولیه مقداردهی می‌کند و سپس برنامه یوزر را به مموری لود کرده و در نهایت با دستورات اسمبلی موجود در asm volatile یک اینتراپت را شبیه‌سازی می‌کند و در نهایت اجرا به فضای کاربر داده می‌شود. در نتیجه از فضای کرنل به فضای کاربر سوییج شد که این کار باعث تغییر حالت یا مود پراسس می‌شود.

۱۲. نباید تغییری در مقادیر ریجستر‌ها ایجاد شده باشد. موردی که باید به آن دقت کرد این است که در دستور intr_exit رجیستر‌های فضای کرنل تغییر می‌کنند و نه فضای کاربر.
<pre> <code>
eax            0x0      0
ecx            0x0      0
edx            0x0      0
ebx            0x0      0
esp            0xc0000000       0xc0000000
ebp            0x0      0x0
esi            0x0      0
edi            0x0      0
eip            0x8048754        0x8048754
eflags         0x202    [ IF ]
cs             0x1b     27
ss             0x23     35
ds             0x23     35
es             0x23     35
fs             0x23     35
gs             0x23     35
</code> </pre>
۱۳. 

    #0  _start (argc=<unavailable>, argv=<unavailable>) at ../../lib/user/entry.c:9

## دیباگ

۱۴.
همانطور که در بخش ۴ مشاهده کردیم در تابع _start برای قرار دادن argv و argc مربوط به ورودی های main به مشکل میخوریم.
پس کافیست تا مقدار درون ثبات استک پوینتر را جابجا کنیم تا در محدوده حافظه در دسترس برنامه قرار گیرد محل قرار دادن argv و argc
پس بدین ترتیب میزانی که قرار است در دستوری که باعث ایجاد مشکل segmentation error میشود را در همین مرحله preload از ثبات استک کم مکنیم.

    if_.esp -= 0x24;

این کد را قبل از asm volatile قرار میدهیم تا پیش از اینکه دسترسی به userspace برود این اطمینان را داشته باشیم که در ان فضا به مشکل عدم دسترسی بر نخواهیم خورد.


۱۵.
<br/>
the content which i found in do-stack-align.ck

<pre> <code>
# -*- perl -*-
use strict;
use warnings;
use tests::tests;
check_expected ([<<'EOF']);
do-stack-align: exit(12)
EOF
pass;
</code> </pre>
--------------

so i think the return value sould be 12.

۱۶.
<pre> <code>
0xbfffff98:     0x00000001      0x000000a2
</code> </pre>
۱۷.
<pre> <code>
(gdb) print args[0]
$1 = 1  ---> it is syscall code number
(gdb) print args[1]
$2 = 162 ---> it is exit code number 
</code> </pre>
These two values are as same as the two values on the top of the stack in hex.

۱۸.
<br/>
Here is my understanding from this temporary semaphore. First of all we saw that in init.c at first steps of checking we faced with process_execute function which is running an user's program. This function is an input argument of process_wait. So the flow is creating new thread for running new program and initialize temporary semaphore with 0 value.
after that passing the created thread id to process_wait. Inside of that function we have semaphore_down that looks at temporary value and it is zero so it can not pass this line and the thread will wait. In result we can say that this program will not allow a thread to finish before running process_execute and executing semaphore_up inside of that. Of course if this OS could handle multi-threading this flow couldn't run like now because some threads exit and increament the semaphore and after that the new thread could prevent waiting because temporary is already a positive integer (multi-thread in pthread_create position of course inside of process_execute)

sema_down is inside of process_wait function in process.c
<pre> <code>
int
process_wait (tid_t child_tid UNUSED)
{
  sema_down (&temporary);
  return 0;
}
</code> </pre>
۱۹.

main thread -> 0xc000e000
<pre> <code>
pintos-debug: dumplist #0: 0xc000e000 {tid = 1, status = THREAD_RUNNING, name = "main", '\000' <repeats 11 times>, stack = 0xc000edec "\375\003", priority = 31, allelem = {prev = 0xc0035910 <all_list>, next = 0xc0104020}, elem = {prev = 0xc0035920 <ready_list>, next = 0xc0035928 <ready_list+8>}, pagedir = 0x0, magic = 3446325067}
pintos-debug: dumplist #1: 0xc0104000 {tid = 2, status = THREAD_BLOCKED, name = "idle", '\000' <repeats 11 times>, stack = 0xc0104f34 "", priority = 0, allelem = {prev = 0xc000e020, next = 0xc010a020}, elem = {prev = 0xc0035920 <ready_list>, next = 0xc0035928 <ready_list+8>}, pagedir = 0x0, magic = 3446325067}
pintos-debug: dumplist #2: 0xc010a000 {tid = 3, status = THREAD_READY, name = "do-nothing\000\000\000\000\000", stack = 0xc010afd4 "", priority = 31, allelem = {prev = 0xc0104020, next = 0xc0035918 <all_list+8>}, elem = {prev = 0xc0035920 <ready_list>, next = 0xc0035928 <ready_list+8>}, pagedir = 0x0, magic = 3446325067}
</code> </pre>

</div>