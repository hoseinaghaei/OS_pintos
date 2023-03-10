# تمرین گروهی ۱ - گزارش

گروه ۶
-----
 > نام و آدرس پست الکترونیکی اعضای گروه را در این قسمت بنویسید.

seyyed alireza ghazanfari alireza79.ghazanfari@gmail.com

Armin Delgosar Maher arminmaher@gmail.com

AmirMahdi kuusheshi amk_amir82@yahoo.com 

Hossein Aghaei h.aghaei.araei@gmail.com

داده ساختار ها
----------
```c
struct process_status
   {
      int pid;
      int exit_code;
      struct list_elem elem;
      struct semaphore wait_sem;
      struct semaphore exec_sem;
      struct lock rw_lock;
   };

struct thread_input
   {
      void *fname;
      struct process_status *status;
      bool success;
   };
```

برای سیسکال‌های `exec` و `wait` ما از داده ساختارهای بالا استفاده کردیم. همچنین داده ساختار `thread` نیز به صورت زیر تغییر کرد.

```c
struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */
    int priority;                       /* Priority. */
    struct list_elem allelem;           /* List element for all threads list. */
    struct file *t_fds[MAX_FILE_DESCRIPTOR_COUNT];
//    struct file_descriptor* file_descriptor_list[MAX_FILE_DESCRIPTOR_COUNT];
    struct file *exec_file;
    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */

    struct process_status *p_status;
    struct list children;
    struct file *executed_file;

#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                  /* Page directory. */
#endif

    /* Owned by thread.c. */
    unsigned magic;                     /* Detects stack overflow. */
  };
```

الگوریتم‌ها
------------
به طور کلی ابتدا رشته ورودی را توکن کرده و سپس بر اساس سایز استک‌مان را allign کردیم. بعد از اینکه دستورات را در  استک گذاشتیم سیسکال‌ها را پیاده سازی کردیم.

ابتدا طبق خود پیشنهاد داک با پیاده سازی printf جلو رفتیم و دستورات را و سیسکال‌ها را یک به یک پیاده سازی کرده و جلو رفتیم.

در این بین باگ‌های زیادی داشتیم که عموما علاوه بر دیباگ کردن هر تست، مجبور بودیم خود تست‌‌ها را ببینیم که متوجه شدیم یک سری از دستورات که سیسکال هایشان پیاده سازی نشده است در این تست‌ها صدا زده شده اند.

تمامی سیسکال‌ها همینجوری زده شد و جلو رفت به جز سیسکال‌های `exec` و `wait` که ابتدا داده ساختارهای بالا را اضافه کردیم و سپس سیسکال‌هایش را اضافه کردیم.

بعد از اینکه هر تردی را میساختیم، هنگام اجرا کردن آن در تابع process_start ابتدا sema_up کردیم که مشخص شود ترد در حال اجرا است و بعد از اینکه process_exit میشد تابع sema_up را صدا میزنیم.

یکی از تست‌هایی که خیلی مشکل ایجاد میکرد تست multi-oom بود بعد از ساعت‌ها دیباگ و خالی کردن ریسورس‌های گرفته شده پاس شد (خیلی سخت بود واقعا این تست، اندازه کل درس بود)

به طور خلاصه روند جلو رفتن پروژه به صورت تک تک پیاده سازی سیسکال‌ها و اجرای تست‌‌های آن سیسکال‌ها و دیباگ کردن تست‌هایش بود.

همچنین یک تابع کلی برای چک کردن پوینتر یوزر و دسترسی آن به مموری و همچنین چک کردن file descriptor ها نیز وجود داشت تا خطاهای احتمالی را رفع کند.

یکی از باگ‌هایی که میخوردیم سر تست‌های rox بود که با اضافه کردن `executed_file` در داده ساختار ترد و بستن و ست کردن آن فایل در تابع های  `filesys_open` و `filesys_close` و `filesys_read` و همچنین جلوگیری از نوشتن بر روی فایل توانستیم این تست‌ها را پاس کنیم.

همچنین برای هر ترد ارایه‌ای از file ها به نام `t_fds` تعریف کردیم که `file descriptor` های هر ترد هست و برای سیسکال open از این استفاده میکردیم و همچنین هنگام خواندن و نوشتن نیز از این ها استفاده میکردیم.