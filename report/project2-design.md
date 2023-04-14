# سیستم‌های عامل - تمرین گروهی دوم

## مشخصات گروه

> > نام، نام خانوادگی و ایمیل خود را در ادامه وارد کنید.


> Hossein Aghaei h.aghaei.araei@gmail.com

> Armin DelgosarMaher arminmaher@gmail.com

نام نام خانوادگی <email@domain.example>

نام نام خانوادگی <email@domain.example>

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

> > پرسش دوم: داده‌ساختارهایی که برای اجرای `priority donation` استفاده شده‌است را توضیح دهید. (می‌توانید تصویر نیز قرار
> > دهید)

### الگوریتم

> > پرسش سوم: چگونه مطمئن می‌شوید که ریسه با بیشترین اولویت که منتظر یک قفل، سمافور یا `condition variable` است زودتر از
> > همه بیدار می‌شود؟

> > پرسش چهارم: مراحلی که هنگام صدازدن `lock_acquire()` منجر به `priority donation` می‌شوند را نام ببرید. دونیشن‌های تو
> > در تو چگونه مدیریت می‌شوند؟

> > پرسش پنجم: مراحلی که هنگام صدا زدن `lock_release()` روی یک قفل که یک ریسه با اولویت بالا منتظر آن است، رخ می‌دهد را
> > نام ببرید.

### همگام‌سازی

> > پرسش ششم: یک شرایط احتمالی برای رخداد `race condition` در `thread_set_priority` را بیان کنید و توضیح دهید که چگونه
> > پیاده‌سازی شما از رخداد آن جلوگیری می‌کند. آیا می‌توانید با استفاده از یک قفل از رخداد آن جلوگیری کنید؟

### منطق

> > پرسش هفتم: چرا این طراحی را استفاده کردید؟ برتری طراحی فعلی خود را بر طراحی‌های دیگری که مدنظر داشته‌اید بیان کنید.

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

**The reason we check this condition is maybe we only have one thread in our thread management queue then we don't need to context switch between threads because nex thread is sam as current thread**
 

> > پرسش نهم: وقتی یک ریسه‌ی هسته در ‍`Pintos` تابع `thread_exit` را صدا می‌زند، کجا و به چه ترتیبی صفحه شامل پشته
> > و `TCB` یا `struct thread` آزاد می‌شود؟ چرا این حافظه را نمی‌توانیم به کمک صدازدن تابع ‍`palloc_free_page` داخل تابع
> > ‍`thread_exit` آزاد کنیم؟

> > پرسش دهم: زمانی که تابع ‍`thread_tick` توسط `timer interrupt handler` صدا زده می‌شود، در کدام پشته اجرا می‌شود؟

> > پرسش یازدهم: یک پیاده‌سازی کاملا کاربردی و درست این پروژه را در نظر بگیرید که فقط یک مشکل درون تابع ‍`sema_up()`
> > دارد. با توجه به نیازمندی‌های پروژه سمافورها(و سایر متغیرهای به‌هنگام‌سازی) باید ریسه‌های با اولویت بالاتر را بر
> > ریسه‌های با اولویت پایین‌تر ترجیح دهند. با این حال پیاده‌سازی ریسه‌های با اولویت بالاتر را براساس اولویت
> > مبنا `Base Priority` به جای اولویت موثر ‍`Effective Priority` انتخاب می‌کند. اساسا اهدای اولویت زمانی که سمافور تصمیم
> > می‌گیرد که کدام ریسه رفع مسدودیت شود، تاثیر داده نمی‌شود. تستی طراحی کنید که وجود این باگ را اثبات کند. تست‌های `Pintos`
> > شامل کد معمولی در سطح هسته (مانند متغیرها، فراخوانی توابع، جملات شرطی و ...) هستند و می‌توانند متن چاپ کنند و می‌توانیم
> > متن چاپ شده را با خروجی مورد انتظار مقایسه کنیم و اگر متفاوت بودند، وجود مشکل در پیاده‌سازی اثبات می‌شود. شما باید
> > توضیحی درباره این که تست چگونه کار می‌کند، خروجی مورد انتظار و خروجی واقعی آن فراهم کنید.

## سوالات نظرسنجی

پاسخ به این سوالات دلخواه است، اما به ما برای بهبود این درس در ادامه کمک خواهد کرد. نظرات خود را آزادانه به ما
بگوئید—این سوالات فقط برای سنجش افکار شماست. ممکن است شما بخواهید ارزیابی خود از درس را به صورت ناشناس و در انتهای ترم
بیان کنید.

> > به نظر شما، این تمرین گروهی، یا هر کدام از سه وظیفه آن، از نظر دشواری در چه سطحی بود؟ خیلی سخت یا خیلی آسان؟

> > چه مدت زمانی را صرف انجام این تمرین کردید؟ نسبتا زیاد یا خیلی کم؟

> > آیا بعد از کار بر روی یک بخش خاص از این تمرین (هر بخشی)، این احساس در شما به وجود آمد که اکنون یک دید بهتر نسبت به
> > برخی جنبه‌های سیستم عامل دارید؟

> > آیا نکته یا راهنمایی خاصی وجود دارد که بهتر است ما آنها را به توضیحات این تمرین اضافه کنیم تا به دانشجویان ترم های
> > آتی در حل مسائل کمک کند؟

> > متقابلا، آیا راهنمایی نادرستی که منجر به گمراهی شما شود وجود داشته است؟

> > آیا پیشنهادی در مورد دستیاران آموزشی درس، برای همکاری موثرتر با دانشجویان دارید؟

این پیشنهادات میتوانند هم برای تمرین‌های گروهی بعدی همین ترم و هم برای ترم‌های آینده باشد.

> > آیا حرف دیگری دارید؟
