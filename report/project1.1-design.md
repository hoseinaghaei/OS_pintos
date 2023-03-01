# تمرین گروهی ۱.۱ - مستند طراحی

گروه
-----
 > نام و آدرس پست الکترونیکی اعضای گروه را در این قسمت بنویسید.

seyyed alireza ghazanfari alireza79.ghazanfari@gmail.com

Armin Delgosar Maher arminmaher@gmail.com

AmirMahdi kuusheshi amk_amir82@yahoo.com 

Hossein Aghaei h.aghaei.araei@gmail.com

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

this jobs will fill the stack, so we need to check if stack has empty space. we chacked thread struct and found a variable called `magic`. this variable checked if stack overflowed or not. also we set a maximum size of arguments, so if the filename be grather than our maximum size of arguments, then we can raise stack overflow and also we can ignore the command and do not execute it. 

منطق طراحی
-----------------
> چرا Pintos به‌جای تابع‌ `strtok()` تابع‌ `strtok_r()` را پیاده‌سازی کرده‌است؟

threads will work with this function. `strtok_r()` is thread safe and `strtok()` is not. in `strtok` the last token staticaly implemented by function, so when thread wants to call this function, this would occur race condition between threads. so it works with `strtok_r()` so all threads can work with it safely.

> در Pintos عمل جدا کردن نام فایل از آرگومان‌ها، در داخل کرنل انجام می‌شود. در سیستم عامل‌های برپایه‌ی Unix، این عمل توسط shell انجام می‌شود. حداقل دو مورد از برتری‌های رویکرد Unix را توضیح دهید.

this will reduce the kernel complexity and increase its performence and efficienty. kernel would not handson parsing and checking arguments, it will only work with arguments, just this. kernel would be somehow generic.

also if we do this in shell, it can interpret commands and we can have more complex and efficient command that help us better, and this complex commands would not any effect on kernel.

shell would help kernel alot, because it can check if file exist and then execue it, it can handle the complexity and not press on kernel.

فراخوانی‌های سیستمی
================
داده‌ساختار‌ها
----------------
> در این قسمت تعریف هر یک از `struct` ها، اعضای `struct` ها، متغیرهای سراسری یا ایستا، `typedef` ها یا `enum` هایی که ای.جاد کرده‌اید یا تغییر داده‌اید را بنویسید و دلیل هر کدام را در حداکثر ۲۵ کلمه توضیح دهید.

This is a lock which we will use to handle read and write synch problem.
```c
static struct loc rw_lock;
```

We have to keep data of process and reponse data of threads too. We have a one to one mapping between pid and tid so this struct should relates to thread. Each process has a parent and we should keep and we should save all processes in a list to catch them by pid and this is for keeping exit code and thread's inside data for access of process to them after finishing thread. For synch problem we added exit_code to have it for process's usages. 

```c
// /src/threads/thread.c
struct thread_status
{
   int pid;
   int ppid;
   int exit_code;
   struct thread* thread;
   struct list_elem processes;
   struct semaphore wait_sem;
   struct semaphore exec_sem;
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

Actually each process(and also thread because they have 1-to-1 relationship) has its own file descriptors, and it is not implemented as a global list in whole system. About the implementation, as we told above, we need to add list of these structs in our thread struct, We defined a global variable
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


> فرض کنید یک فراخوانی سیستمی باعث شود یک صفحه‌ی کامل (۴۰۹۶ بایت) از فضای کاربر در فضای هسته کپی شود. بیشترین و کمترین تعداد بررسی‌‌های جدول صفحات (page table) چقدر است؟ (تعداد دفعاتی که `pagedir_get_page()` صدا زده می‌شود.) در‌ یک فراخوانی سیستمی که فقط ۲ بایت کپی می‌شود چطور؟ آیا این عددها می‌توانند بهبود یابند؟ چقدر؟

We know each page in user side has a virtual address and if we want to be aware of which physical address they are mapped to we should call pagedir_get_page() function for each process. Here we have a full page with size of 4096 bytes. In the worst case every byte of this full page has different virtual address then we need to call pagedir_get_page() 4096 times to find out what are the whole pages addresses.(Actually here I just mentioned the times we need to call pagedir_get_page() to get each `PTE`(page table entry) but in fact we also need to call another function to fetch `page directory` then using that we can get `page entries`, then this will add another function calls). and in the best case our full page is located in single page then we only need to call pagedir_get_page() once.(We assumed that each page has size of 4kb in our system). We can improve this numbers using some tricks like: `Have grater page sizes in our memory` or `Use better memory management scheme(Here we thought by default it is straight forward)` or `We can optimize our code` or maybe `Use hardware suppport`.
and for amount of copied bytes in each syscall we can use `memcpy()` to copy larger scales from user side to kernel side, this works fine but has some side effects like latency and higher memory usage in each syscall.


>پیاده‌سازی فراخوانی سیستمی `wait` را توضیح دهید و بگویید چگونه با پایان یافتن پردازه در ارتباط است.

ok wait system call's behaviour is as same as its behaviour in Unix family of OS. The process that calls it should stop in that line until its child(one specified child by pid) ended and after that continue the running of the parent process. As you see in data structure part of these questions we added a struct to keep the data of process and its similar thread such as thread's state. When a process calls `wait` we have these data so first of all we will check the state of its thread. If it's `THREAD_BLOCKED` then there is no need to do anything else as we had in doc wait system call is for waiting for an specified child process so we assume that we have pid of child process next to data of process struct of parent process so we will find data of child process instance by its id and check its thread state. if its state is `THREAD_DYING` so we do nothing about changing parent process's thread state but on the other situations we will set its state to `THREAD_BLOCKED`. (thread_block func in thread.c)
In the other hand when the specified thread gonna be changed at state we have a function called `schedule` in thread.c and we should use `thread_unblock` for cur_thread's parent in that function to awake its parent if it is on sleep.
how to find the parent thread?
as you can remember we have defined a new struct for processes and we will keep all processes so we can find the parent process by filtering that list by their thread id and after that give the thread of parent process to unblock function.
note that we change the name of struct fromm proccess to thread_status (because of changing the entity of data).

> هر دستیابی هسته به حافظه‌ی برنامه‌ی کاربر، که آدرس آن را کاربر مشخص کرده است، ممکن است به دلیل مقدار نامعتبر اشاره‌گر منجر به شکست شود. در این صورت باید پردازه‌ی کاربر خاتمه داده شود. فراخوانی های سیستمی پر از چنین دستیابی‌هایی هستند. برای مثال فراخوانی سیستمی `write‍` نیاز دارد ابتدا شماره‌ی فراخوانی سیستمی را از پشته‌ی کاربر بخواند، سپس باید سه آرگومان ورودی و بعد از آن مقدار دلخواهی از حافظه کاربر را (که آرگومان ها به آن اشاره می کنند) بخواند. هر یک از این دسترسی ها به حافظه ممکن است با شکست مواجه شود. بدین ترتیب با یک مسئله‌ی طراحی و رسیدگی به خطا (error handling) مواجهیم. بهترین روشی که به ذهن شما می‌رسد تا از گم‌شدن مفهوم اصلی کد در بین شروط رسیدگی به خطا جلوگیری کند چیست؟ همچنین چگونه بعد از تشخیص خطا، از آزاد شدن تمامی منابع موقتی‌ای که تخصیص داده‌اید (قفل‌ها، بافر‌ها و...) مطمئن می‌شوید؟ در تعداد کمی پاراگراف، استراتژی خود را برای مدیریت این مسائل با ذکر مثال بیان کنید.

we can have a error handling function and befoe accessing the arguments in stack, check that if they are in the user space or not using these 2 functions : 
 	1) is_user_vaddr
 	2) pagedir_get_page(current_thread->pagedir, addr)
 	
if we got an error we return error status (like -1) to user and then release all resourses using thread_exit() function. if we wanna remove completely the pages that we got, we can use pagedir_destroy(int pd).
    

همگام‌سازی
---------------
> فراخوانی سیستمی `exec` نباید قبل از پایان بارگذاری فایل اجرایی برگردد، چون در صورتی که بارگذاری فایل اجرایی با خطا مواجه شود باید `-۱` برگرداند. کد شما چگونه از این موضوع اطمینان حاصل می‌کند؟ چگونه وضعیت موفقیت یا شکست در اجرا به ریسه‌ای که `exec` را فراخوانی کرده اطلاع داده می‌شود؟

So the first part of question is about this point that the process that is running exec shouldn't be terminated before loading the program completely. Obviously the description of question tells us that we should convert this part of a code to an atomic code (action). The only technique which we learned for doing this is semaphore (until this moment of course).
first of all i think we need to save thread's exit code in a struct which process has access to it such as `thread_status`.
For handling exec syscall in kernel we should use `process_execute` function in process.c. what we want in this step?

we wanna wait after thread creation in process until loading completion. In other side if we had a complete load flow we should not wait in that location. So these two sentences are like the definitions of semaphore. We define a semaphore for each `thread_status` call `exec_sem` and use sema_down exactly after thread creationg in `process_execute` and sema_up exactly after loading in `start_process` for sharing the result of thread's exit code with process we have to set thread's exit code in thread_status instance related to that thread and in `process_execute` should use this data for returning the suitable exit code. 

----------------

> پردازه‌ی والد P و پردازه‌ی فرزند C را درنظر بگیرید. هنگامی که P فراخوانی `wait(C)` را اجرا می‌کند و C  هنوز خارج نشده است، توضیح دهید که چگونه همگام‌سازی مناسب را برای جلوگیری از ایجاد شرایط مسابقه (race condition) پیاده‌سازی کرده‌اید. وقتی که C از قبل خارج شده باشد چطور؟ در هر حالت چگونه از آزاد شدن تمامی منابع اطمینان حاصل می‌کنید؟ اگر P بدون منتظر ماندن، قبل از C خارج شود چطور؟ اگر بدون منتظر ماندن بعد از C خارج شود چطور؟ آیا حالت‌های خاصی وجود دارد؟

First of all we should view the race condition. One scenario that we can think about is that the parent process calls wait and before its ending the child process finishes its job and it will be terminated. So this problem is a race condition. 
ok for solving this problem we need to use synchronization methods in wait and thread execution parts of code. We'll define a semaphore for each thread inside of `thread_status` struct. We will use semaphore of child thread in parent process wait syscall and sema_down it to wait in that code until the child terminate and finish its work and use sema_up on its semaphore to free the waited parent and all resources. Of course we will put sema_down on child thread's semaphore in the beginning of its execution. The important point is that we initialize the semaphore by one when we create thread (more exact when we create thread_status).  
so in first scenario C is finished before wait on P -> C will sema_down and semaphore is now 0 and then it finishes its job and frees all resources and incremeant semaphore so parent would not wait in semaphore.
scenario 2:
if P wants to finish before termination of C -> C decremeant semaphore and P should wait for it.
about special situations. yes of course we didn't handle multi threading so it's not good for that kind of problems but for single thread form this solution is good enough to handle pintos tasks phase 1 yet.

منطق طراحی
-----------------
> به چه دلیل روش دسترسی به حافظه سطح کاربر از داخل هسته را این‌گونه پیاده‌سازی کرده‌اید؟

according to warmuح document, there are two ways to check and handle user access memory. we choose the first one because it is easier than way two to implememting and we are really lazy. in this way we just need to check a pointer and then handle the user access memory. altough the second way is faster beside its complexity.

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

-------------
For the first part of this question:
`sc-bad-arg.c`
description:
  Sticks a system call number (SYS_EXIT) at the very top of the stack, then invokes a system call with the stack pointer (%esp) set to its address.  The process must be terminated with -1 exit code because the argument to the system call would be above the top of the user address space.
behaviour:
  Again we have a `asm volatile` so it's an inline assembly code. First instruction of this assembly code is:
  ```c
  movl $0xbffffffc, %%esp
  ```
  This instruction moves the value `0xbffffffc` into the stack pointer register `esp`. This sets the stack pointer to the top of the user-space stack, which is a high address in memory.
  ```c
  movl %0, (%%esp)
  ```
  This moves the value of the first input operand `%0` into the memory location pointed to by the stack pointer register `%%esp`.
  Next instruction again is:
  ```c
  int $0x30
  ```
  we reviewd this instruction in the last question.
  and the last part is:
  ```c
  : : "i" (SYS_EXIT)
  ```
  This is for identifying the inputs and outputs of assembly code. first `:` indicates outputs which is empty in this assembly code. Second `:` shows inputs that we have a constant value `"i" (SYS_EXIT)`. `SYS_EXIT` is a constant for calling the exit system call inside of kernel. The `"i"` constraint tells the compiler to generate code that loads the constant value into a register.
  but where is the problem?

  here when the assembly code want to push the input operand (SYS_EXIT constant) inside of stack your stack pointer is pointing to `0xc0000000` (PHYS_BASE which is defined in guide doc) so this data should be in this address and upper which is not accessable address space for user so we will face segmentation error and this program will fail with the message of its last code line.

-------------
سوالات نظرخواهی
==============
پاسخ به این سوالات اختیاری است، ولی پاسخ به آن‌ها می‌تواند به ما در بهبود درس در ترم‌های آینده کمک کند. هر چه در ذهن خود دارید بگویید. این سوالات برای دریافت افکار شما هستند. هم‌چنین می‌توانید پاسخ خود را به صورت ناشناس در انتهای ترم ارائه دهید.

> به نظر شما، این تمرین یا هر یک از سه بخش آن، آسان یا سخت بودند؟ آیا وقت خیلی کم یا وقت خیلی زیادی گرفتند؟

Very hard.
Very ziad

> آیا شما بخشی را در تمرین یافتید که دید عمیق‌تری نسبت به طراحی سیستم عامل به شما بدهد؟

No, not at all.

> آیا مسئله یا راهنمایی خاصی وجود دارد که بخواهید برای حل مسائل تمرین به دانشجویان ترم‌های آینده بگویید؟

Yes, withdraw the course.

> آیا توصیه‌ای برای دستیاران آموزشی دارید که چگونه دانشجویان را در ترم‌های آینده یا در ادامه‌ی ترم بهتر یاری کنند؟

Yes, told them withdraw the course.

> اگر نظر یا بازخورد دیگری دارید در این قسمت بنویسید.

اهای تویی که سوالای بالایی رو اینطوری جواب دادی اگه راضی نیست از شرایط درس رو حذف کن:)