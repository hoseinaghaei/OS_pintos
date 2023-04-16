# سیستم‌های عامل - تمرین گروهی دوم

## مشخصات گروه

> > نام، نام خانوادگی و ایمیل خود را در ادامه وارد کنید.


> Hossein Aghaei h.aghaei.araei@gmail.com

> Armin DelgosarMaher arminmaher@gmail.com

> Seyyed Alireza Ghazanfari alireza79.ghazanfari@gmail.com

> AmirMahdi Kousheshi amk_amir82@yahoo.com

## مقدمه

> > اگر نکته‌ای درباره فایل‌های سابمیت شده یا برای TAها دارید، لطفا اینجا بیان کنید.

> > اگر از هر منبع برخط یا غیر برخطی به غیر از مستندات Pintos، متن درس، اسلایدهای درس یا نکات گفته شده در کلاس در تمرین
> > گروهی استفاده کرده‌اید، لطفا اینجا آن(ها) را ذکر کنید.

## ساعت زنگ‌دار

### داده ساختارها

> > پرسش اول: تعریف `struct`های جدید، `struct`های تغییر داده شده، متغیرهای گلوبال یا استاتیک، `typedef`ها
> > یا `enumeration`ها را در اینجا آورده و برای هریک در 25 کلمه یا کمتر توضیح بنویسید.

```c
// threads/thread.c

/* List of threads which has been put to sleep for some ticks in ascending order 
 * based on tick that must be woken up.
*/
struct list slept_threads;

// threads/thread.h

struct thread
{
    //on what tick the thread must be woken up.
    int64_t waking_tick;
};
```

### الگوریتم

> > پرسش دوم: به اختصار آن‌چه هنگام صدا زدن تابع `timer_sleep()` رخ می‌دهد و همچنین اثر `timer interrupt handler` را
> > توضیح دهید.

### * timer_sleep:
* the interrupt will be disabled first because when the time interrupt comes
we can not acquire lock or put the interrupt handler to sleep.
* we have the kernel tick since last booting in 'ticks' global variable, so we just add the user tick argument by this 
  and save the result in 'waking_tick' variable of the current thread.
* add the current thread to 'slept_threads' list using 'list_insert_ordered' function.
* call 'thread_block()' to block the current thread.
* enable the interrupt.

### * timer_interrupt_handler
* disable the interrupt
* check the 'slept_threads' list and find threads that reached to their tick and unblock them using 'thread_unblock()'
* enable the interrupt

> > پرسش سوم: مراحلی که برای کوتاه کردن زمان صرف‌شده در `timer interrupt handler` صرف می‌شود را نام ببرید.

* we sort the 'slept_threads' by their tick, so we can remove threads in O(1)

### همگام‌سازی

> > پرسش چهارم: هنگامی که چند ریسه به طور همزمان `timer_sleep()` را صدا می‌زنند، چگونه از `race condition` جلوگیری
> > می‌شود؟

* we disable the interrupt in the beginning so there is no concurrency at all.

> > پرسش پنجم: هنگام صدا زدن `timer_sleep()` اگر یک وقفه ایجاد شود چگونه از `race condition` جلوگیری می‌شود؟

* there would be no interrupt if we disable that.

### منطق

> > پرسش ششم: چرا این طراحی را استفاده کردید؟ برتری طراحی فعلی خود را بر طراحی‌های دیگری که مدنظر داشته‌اید بیان کنید.

* we could use lock for the 'slept_threads' modification, but it may fail when the timer interrupt comes,
 because the interrupt handler can not go to sleep.
* we could use a red-black tree for saving the slept threads but the removing time will increase to O(logN).

## زمان‌بند اولویت‌دار

### داده ساختارها

> > پرسش اول: تعریف `struct`های جدید، `struct`های تغییر داده شده، متغیرهای گلوبال یا استاتیک، `typedef`ها
> > یا `enumeration`ها را در اینجا آورده و برای هریک در ۲۵ کلمه یا کمتر توضیح بنویسید.

`priority` field is obvious. It is a number that defines priority of a thread in this system.
`dynamic_priority` is an int field. This is the priority of thread that is initialized by `priority` and at last when we are closing the thread should be equal to `priority`. But the system can change it for scheduling and handling donations. `acquired_locks_list` is a list of locks which are in a thread's hands. We will use this list for checking that now this thread should set its `dynamic_priority` equals to `priority` or not. We will discuss about it later. `waited_locks_for` is a list of locks that are not in thread's hands. We'll use this list for nested donations.

```c
struct thread
  {
    int priority;                       /* Priority. */
    int dynamic_priority;              /* changable priority */
    struct list_elem acquired_locks_list;    /* list of locks which this thread acquired */
    struct list_elem waited_locks_for;       /* list of locks which this thread is in their waiters list */
  };
```
`MAX_NESTED_DONATION_DEPTH` is a constant that we added for some design decisions which will be explained more in next parts. `INTIAL_LOCK_PRIORITY` is a constant too. It's for initializing priorities when no thread has not acquired them.

```c
#define MAX_NESTED_DONATION_DEPTH 20
#define INITIAL_LOCK_PRIORITY -1
```
In `lcok`, we added a `priority` which is the max of its waiters' and holder's priority.

```c
/* Lock. */
struct lock
  {
    struct thread *holder;      /* Thread holding lock (for debugging). */
    int priority;               /* max priority of waiters' priority and holder's priority */
    struct semaphore semaphore; /* Binary semaphore controlling access. */
  };
```

> > پرسش دوم: داده‌ساختارهایی که برای اجرای `priority donation` استفاده شده‌است را توضیح دهید. (می‌توانید تصویر نیز قرار
> > دهید)

All the structures and constants which are defined in the last question will be used in the structure of priority donation management system.
because of that we want to review these structures in priority donation progress it's better to have an example in this area.
imagine we have thread1 that holds lock1 and thread2 wants to acquire lock1. thread2's priority is more than thread1's so first of all thread1 and thread2 have their base priorities which are acquiring with them and these numbers are in `priority` field. On the other hand thread1 is the holder of lock1 so `holder` field of lock1 is pointing to thread1 and lock1 is inserted into thread1's `acquired_locks_list`. But thread2 is added to `waiters` list of lock1 and lock1 is added to thread2's `waited_locks_for`.
when lock1 was created, it has no holder and used `INITIAL_LOCK_PRIORITY` to initial its priority field. But then thread1 came and put its `dynamic_priority` into this field of course at first with conditions of this example thread1's `dynamic_priority` is equal to its `priority`.
then thread2 came. Now we have priority donation situation and we should put thread2's `dynamic_priority` into lock1's `priority` and update thread1's `dynamic_priority` to it too. So the reason of existing this field is to keep the changes of thread priority over donation wihtout loosing base priority. At result, you can see that for a simple example of priority donation we could show each of these added items and their existance's goals.

### الگوریتم

> > پرسش سوم: چگونه مطمئن می‌شوید که ریسه با بیشترین اولویت که منتظر یک قفل، سمافور یا `condition variable` است زودتر از
> > همه بیدار می‌شود؟

a part of `sema_up` function in `synch.c` is:
```c
thread_unblock (list_entry (list_pop_front (&sema->waiters),
                                struct thread, elem));
```
this is line 117 of this file and as a part of it you can see that we will pop a waiter from `sema waiters`. So here is the exact point which we should change it to be confirmed about highest priority thread taking the lock. We'll put our new policy instead of `list_pop_front` function and for acheiving this target we define a function `pop_max_priority_thread`. This function will pop a thread from waiters list with highest priority so we will replace it in the code.


> > پرسش چهارم: مراحلی که هنگام صدازدن `lock_acquire()` منجر به `priority donation` می‌شوند را نام ببرید. دونیشن‌های تو
> > در تو چگونه مدیریت می‌شوند؟

For answering to this question we have to divide it to two sub-questions. 
- First one is about `lock_acquire()` and its steps when having `priority donation`. As thread 1, I'll call this function for having the lock. Assume that this lock has a full list of waiters and some thread has it now too. Thread 1 has a priority and wants to get the lock with that. So for `priority donation` we should compare incoming thread's priority with the lock's priority. If the incoming thread's priority is higher than the othre one we should change the lock's priority and the lock holder's priority to thread 1's priority.

- The point is that for comparing and setting locks in threads we need to keep the origin thread's priority from its dynamic priority so comparing and setting on last item is for `dynamic priorities`.

- And second question about nested donations. First of all we will define a constant `NESTED_DONATION_MAX_DEPTH` to specify a limited area for nested donations to skape from infinite loops or same problems. 

- The management of this process is like managing simple donation when you are donating on a lock you will change the `dynamic_priority` of its holder so this holder as a thread is in othres' locks waiters or holders so we should run donation on those too to change the locks' priority and update related threads' `dynamic_priority`.

- we added a list to thread data structure called `waited_locks_for`. This is list of all locks that this thread is waiting for them. This list is added to help us to manage nested donation better. we should update this list in `sema_up` and `sema_down` but the most important point is that when you want to update `dynamic_priority` of a thread you should call `donation_checker` on each lock of this list for that thread to update holders' priorities of those locks if it is needed.

>>> steps
- first of all we will disable interrupts
- next check these conditions and if does not pass wait
```c
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (!lock_held_by_current_thread (lock));
```
- after that we will call `sema_up`
- if no one has this lock the current thread will get it.
- if this lock has a holder in first condition our priority is less than the holder's so we just add current thread to the list of waiters
- if current thread's priority is higher than lock's priority then we will donate priorities
- we'll set lock holder to current thread too.
- we'll enable interrupts.

> > پرسش پنجم: مراحلی که هنگام صدا زدن `lock_release()` روی یک قفل که یک ریسه با اولویت بالا منتظر آن است، رخ می‌دهد را
> > نام ببرید.

this is the raw code of this function
```c
void
lock_release (struct lock *lock)
{
  ASSERT (lock != NULL);
  ASSERT (lock_held_by_current_thread (lock));

  lock->holder = NULL;
  sema_up (&lock->semaphore);
}
```
so as you can see we are doing two important work in this code. First of all we are putting `NULL` in holder and second point is that we are calling `sema_up()`.

So first we know that handling this item which the thread with highest priority who is waiting for this lock get it is handled by `sema_up`. when we want to give a lock to a thread we should update its `dynamic_priority`.
>>> steps
- here again like the last part for making this part of code blocking we will disable interrupts.
- first we check that lock exists or not
- check that lock's holder is current thread or not
- then set lock's holder `NULL`
- then call `sema_up`
- if the thread has just this lock in `acquired_locks_list` then we should pop it from that list and put `base_priority` into `dynamic_priority`.
- else we will put max of locks' priorities to `dynamic_priority`
- and `sema_up` will give the lock to highest priority. (as we had in last questions)
- at the end we'll enable interrupts.

### همگام‌سازی

> > پرسش ششم: یک شرایط احتمالی برای رخداد `race condition` در `thread_set_priority` را بیان کنید و توضیح دهید که چگونه
> > پیاده‌سازی شما از رخداد آن جلوگیری می‌کند. آیا می‌توانید با استفاده از یک قفل از رخداد آن جلوگیری کنید؟

in the last part when we want to update thread's `dynamic_priority`, maybe thread itself has some changes in `base_priority` so because of that it needs to update its `dynamic_priority` field too with different value of course.
so in this situatation because of disabling interrupts `race condition` wont happen.
yes i think we can put lock inside of `thread_set_priority` to handle this problem too (with out noticing to interrupt disabling).

### منطق

> > پرسش هفتم: چرا این طراحی را استفاده کردید؟ برتری طراحی فعلی خود را بر طراحی‌های دیگری که مدنظر داشته‌اید بیان کنید.

We had some another ideas that had some differences from what is now using on different parts of designing for example one idea was keep all priorities that are assigned in a stack or a list in thread structure to in release time can use this stack with pop the last priority and achieving to the older priority but our design is now better than this idea because it's so simpler and faster and does not need a stack for each thread. In each decision we tried to choose the optimal, fastest and simplest way for solving the problem.
And of course it needs less memory to another ideas which we had.

## سوالات افزون بر طراحی

> > پرسش هشتم: در کلاس سه صفت مهم ریسه‌ها که سیستم عامل هنگامی که ریسه درحال اجرا نیست را ذخیره می‌کند، بررسی کردیم:
> > ‍‍ `program counter` ، ‍‍‍`stack pointer` و `registers`. بررسی کنید که این سه کجا و چگونه در `Pintos` ذخیره می‌شوند؟
> > مطالعه ‍`switch.S` و تابع ‍`schedule` در فایل `thread.c` می‌تواند مفید باشد.

If we take a deeper look in `schedule` function in `threads.c` which is mentioned below:
```c
static void
schedule (void)
{
  struct thread *cur = running_thread ();
  struct thread *next = next_thread_to_run ();
  struct thread *prev = NULL;

  ASSERT (intr_get_level () == INTR_OFF);
  ASSERT (cur->status != THREAD_RUNNING);
  ASSERT (is_thread (next));

  if (cur != next)
    prev = switch_threads (cur, next);
  thread_schedule_tail (prev);
}
```
First it sets three pointers to:
- Current thread which is running
- Next thread which we want to switch to
- Another pointer with Null value to fill with current value's pointer after context switch which is returned by `switch_threads`function

Then after checking few conditions to make sure context switch can occur, if `cur != next`, then it will switch threads via `switch_threads` function.

**The reason we check this condition is maybe we only have one thread in our thread management queue then we don't need to context switch between threads because next thread is same as current thread**
 
On the other hand, we have `switch_threads` assembly function:

```asm
.globl switch_threads
.func switch_threads
switch_threads:
	pushl %ebx
	pushl %ebp
	pushl %esi
	pushl %edi

.globl thread_stack_ofs
	mov thread_stack_ofs, %edx

	movl SWITCH_CUR(%esp), %eax
	movl %esp, (%eax,%edx,1)

	movl SWITCH_NEXT(%esp), %ecx
	movl (%ecx,%edx,1), %esp

	popl %edi
	popl %esi
	popl %ebp
	popl %ebx
        ret
.endfunc
```

As we can see, when the function above is called, its input arguments and a space for its return value is being pushed to stack(
pintos stack grows downwards) then it pushes CPU's main registers to current thread's stack later it sets current thread's stack offset in `edx` register
and it sets SP itself including offset(which stack pointer is calculated using `SWITCH_CUR` then it should be set in the correct pinter to stack in thread data structure) inside `(%eax,%edx,1)` address.

Later, it is time to load next thread in registers.
We calculate the next thread's SP address using its offset(We know the new thread must have the similar schema with the previous thread which we have set its fields and saved in memory)
then we add the calculated thread head pointer to `ecx` register then calculate the SP via the value inside `edx`.(We don't need to set `edx` again because offset for stack of each thread is same for all the threads), then we pop
registers from SP to make it ready for run in CPU.
then we return the value of the pointer to previous thread.(We now return values for each function is always stored in `eax` register and here when we return the function pointer to head of the previous thread is in `eax`)

> > پرسش نهم: وقتی یک ریسه‌ی هسته در ‍`Pintos` تابع `thread_exit` را صدا می‌زند، کجا و به چه ترتیبی صفحه شامل پشته
> > و `TCB` یا `struct thread` آزاد می‌شود؟ چرا این حافظه را نمی‌توانیم به کمک صدازدن تابع ‍`palloc_free_page` داخل تابع
> > ‍`thread_exit` آزاد کنیم؟

When `thread_exit` is called the context switch is not still completed, and freeing its space can be harmful. Because of that it is better to free its allocated space when the next thread is running.
Here is the `thread_exit` function:
```c
void
thread_exit (void)
{
  ASSERT (!intr_context ());

#ifdef USERPROG
  process_exit ();
#endif

  /* Remove thread from all threads list, set our status to dying,
     and schedule another process.  That process will destroy us
     when it calls thread_schedule_tail(). */
  intr_disable ();
  list_remove (&thread_current()->allelem);
  thread_current ()->status = THREAD_DYING;
  schedule ();
  NOT_REACHED ();
}
```
As we can see, `schedule` is called here to context switch between two threads after setting current thread's status as `THREAD_DYING` and removing it from thread list.
then in schedule function in these lines:
```c
  if (cur != next)
    prev = switch_threads (cur, next);
  thread_schedule_tail (prev);
```
Context switch has been done and new thread is running these lines. So we can free previous thread's allocated space without any problems.
If we check `thread_schedule_tail` function at the last of the function it does this:
```c
  if (prev != NULL && prev->status == THREAD_DYING && prev != initial_thread)
    {
      ASSERT (prev != cur);
      palloc_free_page (prev);
    }
```
which first checks `prev` is the same as `current thread` then it frees its allocated pages.

> > پرسش دهم: زمانی که تابع ‍`thread_tick` توسط `timer interrupt handler` صدا زده می‌شود، در کدام پشته اجرا می‌شود؟

Here is the `thread_tick` function:
```c
/* Called by the timer interrupt handler at each timer tick.
   Thus, this function runs in an external interrupt context. */
void
thread_tick (void)
{
  struct thread *t = thread_current ();

  /* Update statistics. */
  if (t == idle_thread)
    idle_ticks++;
#ifdef USERPROG
  else if (t->pagedir != NULL)
    user_ticks++;
#endif
  else
    kernel_ticks++;

  /* Enforce preemption. */
  if (++thread_ticks >= TIME_SLICE)
    intr_yield_on_return ();
}
```

As we know this function is being calls per `tick` which is an external interrupt, and all interrupts sould be handled in kernel mode.
So, this interrupt will be handled by kernel thread as well.

> > پرسش یازدهم: یک پیاده‌سازی کاملا کاربردی و درست این پروژه را در نظر بگیرید که فقط یک مشکل درون تابع ‍`sema_up()`
> > دارد. با توجه به نیازمندی‌های پروژه سمافورها(و سایر متغیرهای به‌هنگام‌سازی) باید ریسه‌های با اولویت بالاتر را بر
> > ریسه‌های با اولویت پایین‌تر ترجیح دهند. با این حال پیاده‌سازی ریسه‌های با اولویت بالاتر را براساس اولویت
> > مبنا `Base Priority` به جای اولویت موثر ‍`Effective Priority` انتخاب می‌کند. اساسا اهدای اولویت زمانی که سمافور تصمیم
> > می‌گیرد که کدام ریسه رفع مسدودیت شود، تاثیر داده نمی‌شود. تستی طراحی کنید که وجود این باگ را اثبات کند. تست‌های `Pintos`
> > شامل کد معمولی در سطح هسته (مانند متغیرها، فراخوانی توابع، جملات شرطی و ...) هستند و می‌توانند متن چاپ کنند و می‌توانیم
> > متن چاپ شده را با خروجی مورد انتظار مقایسه کنیم و اگر متفاوت بودند، وجود مشکل در پیاده‌سازی اثبات می‌شود. شما باید
> > توضیحی درباره این که تست چگونه کار می‌کند، خروجی مورد انتظار و خروجی واقعی آن فراهم کنید.

when we use `base priority` there will be a mistake. The threads that run based on `base priority` be will different from `effective priority`. If there will be no `priority donation`, we will face something like below:

```c
void main(){
1 lock_init();
2 sema_down(lock->sema);
3 set_priority(5);
4 threade_create(A,10);
5 thread_yield();
6 threade_create(B,15);
7 thread_yield();
8 threade_create(C,20);
9 thread_yield();
10 sema_up(lock->sema);
}

void A(){
lock.acquire();
sema_down(lock->sema);
printf("A");
sema_up(lock->sema);
lock.release();
}

void B(){
sema_down(lock->sema);
printf("B");
sema_up(lock->sema);
}

void C(){
lock.acquire();
printf("C");
lock.release();
}
```

The expected result would be: `ABC`, why? Cause at first the thread `A` would be run an will acquire the lock and semaphore will be down. So line 5 would be run and then thread `B` will be create. In this thread at first the semaphore will be down. So there is a priority donation to thread `A` that it could be run and up the semaphore and release the lock. So it will be print A and then the sema_up called the thread B now is on and will print B and then line 8 will be run and thread `C` will be create and print C. But in base priority, the semaphote in thread B never up before, so it will block and line 8 will be run. So in this case the thread `C` Will be run but it needs the lock but the lock is in the hand of thread `A`. So all threads all now block and main thread will be run and line 10 will be run so the smaphore will be up. In this case thread B will be continue so it print B and then the sema also uped in thread B so thread A will be continue and print A and then the sema is up and lock will be released si the thread `C` acquire the lock and print C and then release the lock.

## سوالات نظرسنجی

پاسخ به این سوالات دلخواه است، اما به ما برای بهبود این درس در ادامه کمک خواهد کرد. نظرات خود را آزادانه به ما
بگوئید—این سوالات فقط برای سنجش افکار شماست. ممکن است شما بخواهید ارزیابی خود از درس را به صورت ناشناس و در انتهای ترم
بیان کنید.

> > به نظر شما، این تمرین گروهی، یا هر کدام از سه وظیفه آن، از نظر دشواری در چه سطحی بود؟ خیلی سخت یا خیلی آسان؟

خیلی آسان

> > چه مدت زمانی را صرف انجام این تمرین کردید؟ نسبتا زیاد یا خیلی کم؟

کمتر از ۱۰ دقیقه

> > آیا بعد از کار بر روی یک بخش خاص از این تمرین (هر بخشی)، این احساس در شما به وجود آمد که اکنون یک دید بهتر نسبت به
> > برخی جنبه‌های سیستم عامل دارید؟

نه اصلا

> > آیا نکته یا راهنمایی خاصی وجود دارد که بهتر است ما آنها را به توضیحات این تمرین اضافه کنیم تا به دانشجویان ترم های
> > آتی در حل مسائل کمک کند؟

یک دنیا ممنون از زحماتتون

> > متقابلا، آیا راهنمایی نادرستی که منجر به گمراهی شما شود وجود داشته است؟

خیر اصلا

> > آیا پیشنهادی در مورد دستیاران آموزشی درس، برای همکاری موثرتر با دانشجویان دارید؟

دارم بله

این پیشنهادات میتوانند هم برای تمرین‌های گروهی بعدی همین ترم و هم برای ترم‌های آینده باشد.

> > آیا حرف دیگری دارید؟

بله
