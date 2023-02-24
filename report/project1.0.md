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
    move 0x24(%exp),%eax 
	address : 0xbfffffe4 + 0x24 = 0xc0000008 

۲.
    0x8048757
    

۳.
    a) _start
    b) move 0x24(%exp),%eax
  

۴.
   `grep -rnw 'lib/' -e '_start'`
    ```
    lib/user/user.lds:3:ENTRY(_start) 
    lib/user/entry.c:4:void _start (int argc, char *argv[]); 
    lib/user/entry.c:7:_start (int argc, char *argv[])
    ```
   
    ```
    #include <syscall.h>
    int main (int, char *[]);
    void _start (int argc, char *argv[]);
    void start (int argc, char *argv[]) 
    {
        exit (main (argc, argv));
    }
    ```
   
    ```
    08048754 <_start>:                                                                                      
    8048754:       83 ec 1c                sub    $0x1c,%esp                                                
    8048757:       8b 44 24 24             mov    0x24(%esp),%eax                                           
    804875b:       89 44 24 04             mov    %eax,0x4(%esp)                                            
    804875f:       8b 44 24 20             mov    0x20(%esp),%eax                                         
    8048763:       89 04 24                mov    %eax,(%esp)                                              
    8048766:       e8 35 f9 ff ff          call   80480a0 <main>                                           
    804876b:       89 04 24                mov    %eax,(%esp)   
    804876e:       e8 49 1b 00 00          call   804a2bc <exit>
    ```
   
   در کالینگ کانونشن x86 ابتدا ارگومان‌ها را از راست به چپ در داخل استک پوش میکنیم (استک به سمت پایین رشد می‌کند) و در نهایت ادرس دستور بعدی (ادرس بازگشت) را در استک پوینتر قرار میدهیم. یعنی زمانی که تابع مقصد شروع به کار می‌کند استک پوینتر به ادرس ریترن اشاره دارد.
   اینجا ابتدا استک پوینتر را ۲۸ بایت پایین می‌اوریم(۲۸ بایت حافظه میگیریم).سپس ادرس argv را در eax یا accumulator register قرار می‌دهیم و در خط بعد محتویات آن را در یک ادرس بالاتر از استک پوینتر می‌گذاریم.حالا نوبت به arc می‌رسد و در دو خط بعدی در حال کپی کردن آن هستیم. در نهایت call ادرس فعلی را در ادرس استک پوینتر سیو کرده و تابع main را کال می‌کند. در نهایت ریترن ولیو تابع main را در استک پوش کرده و این‌بار تابع exit را کال می‌کنیم.
   
   

۵. در دستور مربوطه، تابع _start سعی داشت تا  ارگومان دوم خود را از استک بردارد که به آن دسترسی نداشت. شاید تابع کالر، به درستی مقادیر را در استک قرار نداده بود.

## به سوی crash

۶.
0xc000e000, main
idle

pintos-debug: dumplist #0: 0xc000e000 {tid = 1, status = THREAD_RUNNING, name = "main", '\000' <repeats 11 times>, stack = 0xc000edec <incomplete sequence \357>, priority = 31, allelem = {
prev = 0xc0035910 <all_list>, next = 0xc0104020}, elem = {prev = 0xc0035920 <ready_list>, next = 0xc0035928 <ready_list+8>}, pagedir = 0x0, magic = 3446325067}
pintos-debug: dumplist #1: 0xc0104000 {tid = 2, status = THREAD_BLOCKED, name = "idle", '\000' <repeats 11 times>, stack = 0xc0104f34 "", priority = 0, allelem = {prev = 0xc000e020, next = 0xc0035918 <all_list+8>}, elem = {prev = 0xc0035920 <ready_list>, next = 0xc0035928 <ready_list+8>}, pagedir = 0x0, magic = 3446325067}

۷.
#0  process_execute (file_name=file_name@entry=0xc0007d50 "do-nothing") at ../../userprog/process.c:36
#1  0xc0020268 in run_task (argv=0xc00357cc <argv+12>) at ../../threads/init.c:288
#2  0xc0020921 in run_actions (argv=0xc00357cc <argv+12>) at ../../threads/init.c:340
#3  main () at ../../threads/init.c:133
------
sema_init (&temporary, 0);
process_wait (process_execute (task));
a->function (argv);
run_actions (argv);


۸.
main, idle, do-nothing\000\000\000\000\000

pintos-debug: dumplist #0: 0xc000e000 {tid = 1, status = THREAD_BLOCKED, name = "main", '\000' <repeats 11 times>, stack = 0xc000eeac "\001", priority = 31, allelem = {prev = 0xc0035910 <a
ll_list>, next = 0xc0104020}, elem = {prev = 0xc0037314 <temporary+4>, next = 0xc003731c <temporary+12>}, pagedir = 0x0, magic = 3446325067}
pintos-debug: dumplist #1: 0xc0104000 {tid = 2, status = THREAD_BLOCKED, name = "idle", '\000' <repeats 11 times>, stack = 0xc0104f34 "", priority = 0, allelem = {prev = 0xc000e020, next =
 0xc010a020}, elem = {prev = 0xc0035920 <ready_list>, next = 0xc0035928 <ready_list+8>}, pagedir = 0x0, magic = 3446325067}
pintos-debug: dumplist #2: 0xc010a000 {tid = 3, status = THREAD_RUNNING, name = "do-nothing\000\000\000\000\000", stack = 0xc010afd4 "", priority = 31, allelem = {prev = 0xc0104020, next =
 0xc0035918 <all_list+8>}, elem = {prev = 0xc0035920 <ready_list>, next = 0xc0035928 <ready_list+8>}, pagedir = 0x0, magic = 3446325067}

۹.

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
------
exactly in the line:
tid = thread_create (file_name, PRI_DEFAULT, start_process, fn_copy);
start_process becomes created.

۱۰.
 {edi = 0x0, esi = 0x0, ebp = 0x0, esp_dummy = 0x0, ebx = 0x0, edx = 0x0, ecx = 0x0, eax = 0x0, gs = 
0x23, fs = 0x23, es = 0x23, ds = 0x23, vec_no = 0x0, error_code = 0x0, frame_pointer = 0x0, eip = 0x8048754, cs = 0x1b, eflags = 0x202, esp = 0xc0000000, ss = 0x23}


۱۱. در تابع start_process، کرنل می‌خواهد برنامه‌ای را که در آدرس آن در ورودی داده شده‌است را آماده اجرا کند. کرنل ابتدا رجیستر‌ها را به یک مقدار اولیه مقداردهی می‌کند و سپس برنامه یوزر را به مموری لود کرده و در نهایت با دستورات اسمبلی موجود در asm volatile یک اینتراپت را شبیه‌سازی می‌کند و در نهایت اجرا به فضای کاربر داده می‌شود. در نتیجه از فضای کرنل به فضای کاربر سوییج شد که این کار باعث تغییر حالت یا مود پراسس می‌شود.

۱۲. نباید تغییری در مقادیر ریجستر‌ها ایجاد شده باشد. موردی که باید به آن دقت کرد این است که در دستور intr_exit رجیستر‌های فضای کرنل تغییر می‌کنند و نه فضای کاربر.
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

۱۳. 
	#0  _start (argc=<unavailable>, argv=<unavailable>) at ../../lib/user/entry.c:9


## دیباگ

۱۴.

۱۵.

۱۶.

۱۷.

۱۸.

۱۹.
