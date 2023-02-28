# تمرین گروهی ۱.۱ - مستند طراحی

گروه
-----
 > نام و آدرس پست الکترونیکی اعضای گروه را در این قسمت بنویسید.

seyyed alireza ghazanfari alireza79.ghazanfari@gmail.com

آرمین دلگسارماهر arminmaher@gmail.com

AmirMahdi kuusheshi amk_amir82@yahoo.com 

نام و نام خانوادگی <example@example.com> 

مقدمات
----------
> اگر نکات اضافه‌ای در مورد تمرین یا برای دستیاران آموزشی دارید در این قسمت  بنویسید.

> لطفا در این قسمت تمامی منابعی (غیر از مستندات Pintos، اسلاید‌ها و دیگر منابع درس) را که برای تمرین از آن‌ها استفاده کرده‌اید در این قسمت بنویسید.

```
googela googela, github, cs162 berkley, stanford documentationo for pintos, chatgpt.
```

پاس‌دادن آرگومان
============
داده‌ساختار‌ها
----------------
> در این قسمت تعریف هر یک از `struct` ها، اعضای `struct` ها، متغیرهای سراسری یا ایستا، `typedef` ها یا `enum` هایی که ایجاد کرده‌اید یا تغییر داده‌اید را بنویسید و دلیل هر کدام را در حداکثر ۲۵ کلمه توضیح دهید.

refer to the doc, we need to use `argc` and `argv` for our filename and syscalls. So, at first we define and init them.
```c
int argc;
char **argv;
```
We can also define `argv` like code below:

```c
#define MAX_ARG_SIZE 64
char *argv[MAX_ARG_SIZE];
```

when a filename comes, we need to parse and tokenize it, so we need a function that tokenize that for us by space. at first we need to check is the filename tokenable, then if it was, we token it and set it in `argv`,
```c
int check_if_fn_tokenable(char *filename);
```

```c
typedef char *token;
```

الگوریتم‌ها
------------
> به‌طور خلاصه توضیح دهید چگونه آرگومان‌ها را پردازش کرده‌اید؟ چگونه اعضای `argv[]` را به ترتیب درست در پشته قرار داده‌اید؟ و چگونه از سرریز پشته جلوگیری کرده‌اید؟

we use our tokenize function before calling `filesys_open()`, so we have to call it in load in proccess.c. we also set `argv` and `argc`.

we need to change `setup_stack()` too.  this function just has esp in its params, we also add `argc` and `argv` too.

```c
static bool setup_stack(void **esp, int argc, char **argv);
```

we set `argv[0]` before calling setup_stack, but it is also question how we process the arguments. we get a filename and after tokenize it, we set arguments reverse to the argv. in fact we init the argv[argc-1], then we init argv[argc-2] until we reach argv[1]. we set the argv[argc] NULL (somehow like personal hw1)
we set the return address too, because when the stack pointer check all arguments and reached return address.

this jobs will fill the stack, so we need to check if stack has empty space. we chacked thread struct and found a variable called `magic`. this variable checked if stack overflowed or not. we check magic ad stack pointer to find out if overflow occurred or not.

منطق طراحی
-----------------
> چرا Pintos به‌جای تابع‌ `strtok()` تابع‌ `strtok_r()` را پیاده‌سازی کرده‌است؟

> در Pintos عمل جدا کردن نام فایل از آرگومان‌ها، در داخل کرنل انجام می‌شود. در سیستم عامل‌های برپایه‌ی Unix، این عمل توسط shell انجام می‌شود. حداقل دو مورد از برتری‌های رویکرد Unix را توضیح دهید.

فراخوانی‌های سیستمی
================
داده‌ساختار‌ها
----------------
> در این قسمت تعریف هر یک از `struct` ها، اعضای `struct` ها، متغیرهای سراسری یا ایستا، `typedef` ها یا `enum` هایی که ای.جاد کرده‌اید یا تغییر داده‌اید را بنویسید و دلیل هر کدام را در حداکثر ۲۵ کلمه توضیح دهید.

We have to keep data of process too. We have a one to one mapping between pid and tid so this struct should relates to thread. Each process has a parent and we should keep and we should save all processes in a list to catch them by pid. 

```c
// /src/threads/thread.c
struct process
{
    int pid;
    struct thread* main_thread;
    struct process* parent_process;
    struct list_elem processes;
};
```

We need to define a struct for handling file description like a mapping between integer values and file pointers. Then store list of that struct in running thread to look up for each file in the memory using this
list. And also for iteration in the list of whole files in memory, it is better to hook each file descriptor's next and previous file descriptors using list_elem struct. And obviously in this struct we need to store mapped file's pointer to file address to read the file.

```c
// path: /src/userprog/process.h
typedef fid_t int;
struct file_descriptor
{
struct file *file;
struct list_elem file_descriptor_element;
fid_t file_id;
};
```

> توضیح دهید که توصیف‌کننده‌های فایل چگونه به فایل‌های باز مربوط می‌شوند. آیا این توصیف‌کننده‌ها در کل سیستم‌عامل به‌طور یکتا مشخص می‌شوند یا فقط برای هر پردازه یکتا هستند؟

As we told above, we need to add list of these structs in our thread struct, We defined a global variable
MAX_FILE_DESCRIPTOR_COUNT(.e.g 1024 for each process) to bound count of our file descriptors to avoid filling our memory.
And also we preferred to add separate file pointer for current exec file in thread,
because it is pretty easier to check if process doesn't try to access current exec file.

```c
// in thread struct
int MAX_FILE_DESCRIPTOR_COUNT = 1024;

struct thread
  {
    ...
    struct list file_descriptor_list[MAX_FILE_DESCRIPTOR_COUNT];
    struct file *exec_file;
    ...
  };
```

الگوریتم‌ها
------------
> توضیح دهید خواندن و نوشتن داده‌های کاربر از داخل هسته، در کد شما چگونه انجام شده است.

for reading user's data and writing them too we should use system calls which we have in pintos such as read and write. Like each other operating systems which we worked with we should have a default set of file descriptors in file descriptor table and define some items like stdin, stdout and stderr by default for each process on its creation time. So for read and write syscalls we should just check that file's which we are given be STDIN and STDOUT and matchable with those file descriptors. With given address of file we will found file descriptor id from our saved hash map inside of each process and check that is the correct file or not.
Also we'll check NULL value of file pointer too to react suitable in this situation. One last point is that we should handle running just one of read and write on an specific moment. So the simplest way to handle it is using a global variable as a lock (rw_lock) and acquire and release it at both read and write system call.
------------
> فرض کنید یک فراخوانی سیستمی باعث شود یک صفحه‌ی کامل (۴۰۹۶ بایت) از فضای کاربر در فضای هسته کپی شود. بیشترین و کمترین تعداد بررسی‌‌های جدول صفحات (page table) چقدر است؟ (تعداد دفعاتی که `pagedir_get_page()` صدا زده می‌شود.) در‌ یک فراخوانی سیستمی که فقط ۲ بایت کپی می‌شود چطور؟ آیا این عددها می‌توانند بهبود یابند؟ چقدر؟

>پیاده‌سازی فراخوانی سیستمی `wait` را توضیح دهید و بگویید چگونه با پایان یافتن پردازه در ارتباط است.

ok wait system call's behaviour is as same as its behaviour in Unix family of OS. The process that calls it should stop in that line until its child(one specified child by pid) ended and after that continue the running of the parent process. As you see in data structure part of these questions we added a struct to keep the data of process and its similar thread such as thread's state. When a process calls `wait` we have these data so first of all we will check the state of its thread. If it's `THREAD_BLOCKED` then there is no need to do anything else as we had in doc wait system call is for waiting for an specified child process so we assume that we have pid of child process next to data of process struct of parent process so we will find data of child process instance by its id and check its thread state. if its state is `THREAD_DYING` so we do nothing about changing parent process's thread state but on the other situations we will set its state to `THREAD_BLOCKED`. (thread_block func in thread.c)
In the other hand when the specified thread gonna be changed at state we have a function called `schedule` in thread.c and we should use `thread_unblock` for cur_thread's parent in that function to awake its parent if it is on sleep.
how to find the parent thread?
as you can remember we have defined a new struct for processes and we will keep all processes so we can find the parent process by filtering that list by their thread id and after that give the thread of parent process to unblock function.

------------
> هر دستیابی هسته به حافظه‌ی برنامه‌ی کاربر، که آدرس آن را کاربر مشخص کرده است، ممکن است به دلیل مقدار نامعتبر اشاره‌گر منجر به شکست شود. در این صورت باید پردازه‌ی کاربر خاتمه داده شود. فراخوانی های سیستمی پر از چنین دستیابی‌هایی هستند. برای مثال فراخوانی سیستمی `write‍` نیاز دارد ابتدا شماره‌ی فراخوانی سیستمی را از پشته‌ی کاربر بخواند، سپس باید سه آرگومان ورودی و بعد از آن مقدار دلخواهی از حافظه کاربر را (که آرگومان ها به آن اشاره می کنند) بخواند. هر یک از این دسترسی ها به حافظه ممکن است با شکست مواجه شود. بدین ترتیب با یک مسئله‌ی طراحی و رسیدگی به خطا (error handling) مواجهیم. بهترین روشی که به ذهن شما می‌رسد تا از گم‌شدن مفهوم اصلی کد در بین شروط رسیدگی به خطا جلوگیری کند چیست؟ همچنین چگونه بعد از تشخیص خطا، از آزاد شدن تمامی منابع موقتی‌ای که تخصیص داده‌اید (قفل‌ها، بافر‌ها و...) مطمئن می‌شوید؟ در تعداد کمی پاراگراف، استراتژی خود را برای مدیریت این مسائل با ذکر مثال بیان کنید.

همگام‌سازی
---------------
> فراخوانی سیستمی `exec` نباید قبل از پایان بارگذاری فایل اجرایی برگردد، چون در صورتی که بارگذاری فایل اجرایی با خطا مواجه شود باید `-۱` برگرداند. کد شما چگونه از این موضوع اطمینان حاصل می‌کند؟ چگونه وضعیت موفقیت یا شکست در اجرا به ریسه‌ای که `exec` را فراخوانی کرده اطلاع داده می‌شود؟

> پردازه‌ی والد P و پردازه‌ی فرزند C را درنظر بگیرید. هنگامی که P فراخوانی `wait(C)` را اجرا می‌کند و C  هنوز خارج نشده است، توضیح دهید که چگونه همگام‌سازی مناسب را برای جلوگیری از ایجاد شرایط مسابقه (race condition) پیاده‌سازی کرده‌اید. وقتی که C از قبل خارج شده باشد چطور؟ در هر حالت چگونه از آزاد شدن تمامی منابع اطمینان حاصل می‌کنید؟ اگر P بدون منتظر ماندن، قبل از C خارج شود چطور؟ اگر بدون منتظر ماندن بعد از C خارج شود چطور؟ آیا حالت‌های خاصی وجود دارد؟

منطق طراحی
-----------------
> به چه دلیل روش دسترسی به حافظه سطح کاربر از داخل هسته را این‌گونه پیاده‌سازی کرده‌اید؟

> طراحی شما برای توصیف‌کننده‌های فایل چه نقاط قوت و ضعفی دارد؟

As mentioned above, for each thread we will store its own file descriptors as a list. here are this strategy's advantage and disadvantages:

Advantages:

```text
Thread-safety:By keeping a list of file descriptors for each thread inside its struct, it ensures that each thread has its own set of file descriptors that cannot be accessed by other threads. This ensures thread safety and prevents race conditions that can occur when multiple threads try to access the same set of file descriptors simultaneously.

Improved performance: When a thread wants to access a file descriptor, it can quickly find it in its own list without having to search through a global list of file descriptors. This can improve performance by reducing the time it takes to access file descriptors.

Flexibility: By having a separate list of file descriptors for each thread, it allows each thread to have its own set of open files, which can be useful in some scenarios.
```
Disadvantages:

```text
Increased memory usage: Keeping a separate list of file descriptors for each thread can increase the memory usage of the system. This can be a concern in systems with limited memory resources.

Increased complexity: Maintaining separate lists of file descriptors for each thread can add complexity to the operating system design. This can make it harder to debug and maintain the system.

Limited scalability: Keeping a separate list of file descriptors for each thread may not be scalable in systems with a large number of threads. This is because each thread will have its own set of file descriptors, which can quickly add up and lead to resource constraints.  
```
> در حالت پیش‌فرض نگاشت `tid` به `pid` یک نگاشت همانی است. اگر این را تغییر داده‌اید، روی‌کرد شما چه نقاط قوتی دارد؟

We will use one to one methodology. (no change)
-----------------

سوالات افزون بر طراحی
===========
> تستی را که هنگام اجرای فراخوانی سیستمی از یک اشاره‌گر پشته‌ی(esp) نامعتبر استفاده کرده است بیابید. پاسخ شما باید دقیق بوده و نام تست و چگونگی کارکرد آن را شامل شود.

`sc-bad-sp.c` is the test which invokes a system call with the stack pointer (%esp) set to a bad address.  The process must be terminated with -1 exit code.
explanation:
```c
  asm volatile ("movl $.-(64*1024*1024), %esp; int $0x30");
```
This is the only part of this test which we should talk about.
Obviously it contains inline assemble code.
first instruction of this assembly code is:
```c
movl $.-(64*1024*1024), %esp
```
This instruction moves the value `$.-(64*1024*1024)` into the `esp` register. `$` indicates immediate value and `.` refers to the current program counter (PC) location. So the behaviour of this instruction is using the current PC location minus 64 megabytes to set a new stack pointer value for its register.
The next instruction is:
```c
int $0x30
```
We saw this instruction all around the different syscalls. This is a software interrupt with the value `0x30` which is used to request a system call and change the kernel mode to user mode. So it will end the process because it has no specified system call so it just exit the current process and finish the job.

> تستی را که هنگام اجرای فراخوانی سیستمی از یک اشاره‌گر پشته‌ی معتبر استفاده کرده ولی اشاره‌گر پشته آنقدر به مرز صفحه نزدیک است که برخی از آرگومان‌های فراخوانی سیستمی در جای نامعتبر مموری قرار گرفته اند مشخص کنید. پاسخ شما باید دقیق بوده و نام تست و چگونگی کارکرد آن را شامل شود.یک قسمت از خواسته‌های تمرین را که توسط مجموعه تست موجود تست نشده‌است، نام ببرید. سپس مشخص کنید تستی که این خواسته را پوشش بدهد چگونه باید باشد.

سوالات نظرخواهی
==============
پاسخ به این سوالات اختیاری است، ولی پاسخ به آن‌ها می‌تواند به ما در بهبود درس در ترم‌های آینده کمک کند. هر چه در ذهن خود دارید بگویید. این سوالات برای دریافت افکار شما هستند. هم‌چنین می‌توانید پاسخ خود را به صورت ناشناس در انتهای ترم ارائه دهید.

> به نظر شما، این تمرین یا هر یک از سه بخش آن، آسان یا سخت بودند؟ آیا وقت خیلی کم یا وقت خیلی زیادی گرفتند؟

> آیا شما بخشی را در تمرین یافتید که دید عمیق‌تری نسبت به طراحی سیستم عامل به شما بدهد؟

> آیا مسئله یا راهنمایی خاصی وجود دارد که بخواهید برای حل مسائل تمرین به دانشجویان ترم‌های آینده بگویید؟

> آیا توصیه‌ای برای دستیاران آموزشی دارید که چگونه دانشجویان را در ترم‌های آینده یا در ادامه‌ی ترم بهتر یاری کنند؟

> اگر نظر یا بازخورد دیگری دارید در این قسمت بنویسید.
