تمرین گروهی ۳ - مستند طراحی
======================

گروه
-----

>>‫نام و آدرس پست الکترونیکی اعضای گروه را در این قسمت بنویسید.

* Seyyed Alireza Ghazanfari alireza79.ghazanfari@gmail.com
* Armin DelgosarMaher arminmaher@gmail.com
* AmirMahdi Kousheshi amk_amir82@yahoo.com
* Hossein Aghaei h.aghaei.araei@gmail.com

مقدمات
----------

>>‫ ‫‫اگر نکات اضافه‌ای در مورد تمرین یا برای دستیاران آموزشی دارید در این قسمت  ‫بنویسید.

>>‫ لطفا در این قسمت تمامی منابعی (غیر از مستندات Pintos، اسلاید‌ها و دیگر منابع‫ ‫درس) را که برای تمرین از آن‌ها استفاده کرده‌اید در این قسمت بنویسید.

بافر کش
============

داده‌ساختار‌ها و توابع
---------------------

>>‫ در این قسمت تعریف هر یک از `struct` ها، اعضای `struct` ها، متغیرهای سراسری یا ایستا، `typedef` ها یا `enum` هایی که ایجاد کرده‌اید یا تغییر داده‌اید را‫ بنویسید و دلیل هر کدام را در حداکثر ۲۵ کلمه توضیح دهید.

```c
typedef struct cache_block_item
{
    bool dirty;
    bool valid;
    char data[BLOCK_ITEM_SIZE];
    struct list_elem cache_elem;
    struct lock item_lock;
} cache_block_item;
```

In this part we define a struct for saving the data of each item in cache buffer. It has a data field that saves the data. a dirty and a valid field for saying that this part of disk is cache is valid or should be updated or should we dirty its data on disk. It has a list_elem to be able to be in lis of cache.
it has a lock which we will explain it completly in future.

```c
struct list cache;
struct lock cache_lock;
```

these two are for list of items. one of them is the list itself and keeping current cache data. the second one is a lock which we will completly explain it in future.


الگوریتم‌ها
------------

>>‫ توضیح دهید که الگوریتم مورد استفاده‌ی شما به چه صورت یک بلاک را برای جایگزین ‫ شدن انتخاب می‌کند؟

First of all we should say that we are planning to implement LRU (least recently used) policy for replacements. So we need a 64 items list for showing the time of using this cache block (that origin disk sector). For replacement as same as the origin of policy we will remove the last sector from the begining of the list `cache block` item and put new sector on a `cache block item` and add it to the end of the list. 

>>‫ روش پیاده‌سازی `read-ahead` را توضیح دهید.

As I understood the `read-ahead` concept means how to select an algorithm to predict and preload some data from disk which you expect to users need them at the near future.
So lets look at `inode_read_at()` and the algorithm of reading from disk. First of all we will identify that in which sector should we start reading and on what offset of that.

```c
      /* Disk sector to read, starting byte offset within sector. */
      block_sector_t sector_idx = byte_to_sector (inode, offset);
```
After that we will read that sector chunk by chunk. The point is that we decide to select data which are near to this specific part of disk to the cache too. One or two things we can do for example the simplest solution is that when this part of data is not on cache you should set as offset the end of sector to transfer all of sector to the cache. 

همگام سازی
-------------

>>‫ هنگامی که یک پردازه به طور مستمر در حال خواندن یا نوشتن داده در یک بلاک بافرکش‫ می‌باشد به چه صورت از دخالت سایر پردازه‌ها جلوگیری میشود؟

This problem is like all other synchronization problems which we had in the past. So first of all we should see each `cache block` as a share memory object between many threads. So we need to put a `lock` field in each `cache block item` to manage them. When a thread wants to use one of these items should `acquire` its lock and in that time itself and other threads can not use this item again. After finishing the job with this `cache block item` the thread must `release` the `lock`.

>>‫ در حین خارج شدن یک بلوک از حافظه‌ی نهان، چگونه از پروسه‌های دیگر جلوگیری می‌شود تا‫ به این بلاک دسترسی پیدا نکنند؟

We had `lock` in last problems that we introduced. In this problem we'll use it to doesn't allow threads to use them when a thread is using it. Next to it we should define another `lock` for the list that we have in `LRU`. When a thread wants to change the list because of the list does not include the item we should acquire this list's `lock` and after finishing the change the thread must release it. 

منطق طراحی
-----------------

>>‫ یک سناریو را توضیح دهید که از بافر کش، `read-ahead` و یا از `write-behind` استفاده کند.

Assume that we have a huge file on disk. We have a thread that wants to read this file completly. The thread starts reading this file. First chunk of this file's data is not in the buffer cache So here when we are reading this chunk of data from disk we should pay attention, our policy for `read_ahead` was that we will read the remained part of sector or next sector's data to the cache for maybe future uses. And in this scenario this policy will done well. Because it will cache most of the file's data before its requests and it improve the performance.

فایل‌های قابل گسترش
=====================

داده‌ساختار‌ها و توابع
---------------------

>>‫ در این قسمت تعریف هر یک از `struct` ها، اعضای `struct` ها، متغیرهای سراسری‫ یا ایستا، `typedef` ها یا `enum` هایی که ایجاد کرده‌اید یا تغییر داده‌اید را بنویسید و‫ دلیل هر کدام را در حداکثر ۲۵ کلمه توضیح دهید.

```c
struct inode_disk
  {
    //block_sector_t start;               REMOVED
    off_t length;                       /* File size in bytes. */
    unsigned magic;                     /* Magic number. */
    //uint32_t unused[125];               REMOVED
    block_sector_t direct[123];
    block_sector_t indirect;
    block_sector_t in2direct;
    bool is_dir;
  };



struct inode
  {
    ...
    struct lock ilock;
    ...
  };

static int inode_alloc (struct inode_disk *inode_disk, off_t length);
static void inode_free (struct inode *inode);
```
In the altered inode_disk struct, first we removed `unused`, `start` fields due to gain some space(Because the total size of this block should not be more than 512 bytes due not to reach the sector size limitation) then we have used multi-level sectors. we have `direct`, `indirect`, `in2direct` sectors which shows the sector number in which our data is stored. We have 123 direct sectors and one number for each sector which shows our 1/2 level sector mappings.
We also have `inode_alloc` and `inode_free` functions to allocate and free inodes.

>>‫ بیشترین سایز فایل پشتیبانی شده توسط ساختار inode شما چقدر است؟

512 * (123 + 128 + 128 * 128) = 512 * 16636 = 8517632 Bytes

همگام سازی
----------

>>‫ توضیح دهید که اگر دو پردازه بخواهند یک فایل را به طور همزمان گسترش دهند، کد شما چگونه از‫ حالت مسابقه جلوگیری می‌کند.

We have added `ilock` for this reason, whenever two processes want to extend or change a same file, they should first `acquire` the `ilock` first. This approach prevents from changing a single file in one time by two processes.  

>>‫ فرض کنید دو پردازه‌ی A و B فایل F را باز کرده‌اند و هر دو به end-of-file اشاره کرده‌اند.‫ اگر  همزمان A از F بخواند و B روی آن بنویسد، ممکن است که A تمام، بخشی یا هیچ چیز از‫ اطلاعات نوشته شده توسط B را بخواند. همچنین A نمی‌تواند چیزی جز اطلاعات نوشته شده توسط B را‫ بخواند. مثلا اگر B تماما ۱ بنویسد، A نیز باید تماما ۱ بخواند. توضیح دهید کد شما چگونه از‫ این حالت مسابقه جلوگیری می‌کند.

The answer is as same as previous question. Two processes can not access a single file simultaneously. First one of them writes(or reads) and raises `EOF` then the other one starts its process.

>>‫ توضیح دهید همگام سازی شما چگونه "عدالت" را برقرار می‌کند. فایل سیستمی "عادل" است که‫ خواننده‌های اطلاعات به صورت ناسازگار نویسنده‌های اطلاعات را مسدود نکنند و برعکس. بدین ترتیب‫ اگر تعدادی بسیار زیاد پردازه‌هایی که از یک فایل می‌خوانند نمی‌توانند تا ابد مانع نوشده شدن‫ اطلاعات توسط یک پردازه‌ی دیگر شوند و برعکس.

It depends on our queuing algorithm. But even if it is even random access the fairness will occur then. But definitely all these are being handled in the kernel side and it can prevent `starvation` on a single file. 

منطق طراحی
----------

>>‫ آیا ساختار `inode` شما از طبقه‌بندی چند سطحه پشتیبانی می‌کند؟ اگر بله، دلیل خود را برای‫ انتخاب این ترکیب خاص از بلوک‌های مستقیم، غیر مستقیم و غیر مستقیم دوطرفه توضیح دهید.‌‫ اگر خیر، دلیل خود برای انتخاب ساختاری غیر از طبقه‌بندی چند سطحه و مزایا و معایب ساختار‫ مورد استفاده خود نسبت به طبقه‌بندی چند سطحه را توضیح دهید.

Yes, we have used multi-level structure. Because it is flexible for small and big files and even if their size changes in the system it can be handled.
We have 123 `direct` blocks which can be accessed faster than indirect ones. So we can handle files in small size ranges in a pretty easy way. For larger files we go through in direct blocks from which we have 128 units. and for even larger file we can use 2/step redirecting which is more suitable for larger files. We have 128*128 units for them.

زیرمسیرها
============

داده‌ساختار‌ها و توابع
---------------------

>>‫ در این قسمت تعریف هر یک از `struct` ها، اعضای `struct` ها، متغیرهای سراسری‫ یا ایستا، `typedef` ها یا `enum` هایی که ایجاد کرده‌اید یا تغییر داده‌اید را بنویسید و‫ دلیل هر کدام را در حداکثر ۲۵ کلمه توضیح دهید.

```c
struct dir {
    struct lock dir_lock;
};

struct thread
{
    struct dir *curr_dir;
};

struct inode_disk
{
    bool is_dir;
}

struct inode
{
    bool is_deleted;
    bool is_open_by_thread;
}

```
In `struct dir` we add a lock to this struct so we can made it thread safe and do some operations like deleting or the other things. Also in `struct thread` we add a current directory that a thread is doing it jobs and working with file descriptors. In `struct inode_disk` we added a `is_dir` boolean to check if the sector of block is for a directory or not. In `struct inode` we also add `is_deleted` and `is_open_by_thread` to check the race condition that may be create during working with inodes.



الگوریتم‌ها
-----------

>>‫ کد خود را برای طی کردن یک مسیر گرفته‌شده از کاربر را توضیح دهید.‫ آیا عبور از مسیرهای absolute و relative تفاوتی دارد؟

If we have the absolute path, so it is easy. We can easily get the path and find the destination to file or directory with the methods that they are already in the code. If the path would be relative, so we added a `curr_dir` in thread that would help us to concat this working directory to our relative path and made an absolute path and do the same thing as we tell first.


همگام سازی
-------------

>>‫ چگونه از رخ دادن race-condition در مورد دایرکتوری ها پیشگیری می‌کنید؟‫ برای مثال اگر دو درخواست موازی برای حذف یک فایل وجود داشته باشد و ‫ تنها یکی از آنها باید موفق شود یا مثلاً دو ریسه موازی بخواهند فایلی‫ یک اسم در یک مسیر ایجاد کنند و مانند آن.‫ آیا پیاده سازی شما اجازه می‌دهد مسیری که CWD یک ریسه شده یا پردازه‌ای‫ از آن استفاده می‌کند حذف شود؟ اگر بله، عملیات فایل سیستم بعدی روی آن‫ دایرکتوری چه نتیجه‌ای می‌دهند؟ اگر نه، چطور جلوی آن را می‌گیرید؟

In `struct inode` thee is a lock that could help up to prevent the race condition.
We will deleted it. When a directory wants to be delete, so at first we check the the parameter `is_open_by_thread` and then we can deleted it. If it was opend by a thread, so we set the `is_deleted` true and when the `is_open_by_thread` will be false so we delete the directory.

منطق طراحی
-----------------

>>‫ توضیح دهید چرا تصمیم گرفتید CWD یک پردازه را به شکلی که طراحی کرده‌اید‫ پیاده‌سازی کنید؟

Because this is so simple and have a good performance. With `curr_dir` and the `dir_lock` we can easily find the paths and work with them and prevent race condition and the other things. Also with the `lookup_dir` and `dir_create` we can handle the directory tasks. 


افزون بر طراحی
=================

>>  برای این پروژه ۲ ویژگی اختیاری درباره حافظه نهان بافر    وجود دارد:
>>`write-behind`
>>یا
>>`read-ahead`
>> .یک حافظه نهان بافر با
>>`write-behind`
>> به صورت متناوب بلوک های تغییر داده شده را در بلوک های فایل سیستم دستگاه می نویسد تا در صورت قطعی
>> برق سیستم اطلاعات زیادی از دست ندهد. بدون این ویژگی حافظه نهان
>> `back-write`
>> فقط زمانی که یک داده کثیف شده و
>> در حال خارج شدن از حافظه نهان است یا سیستم در حال خاموش شدن است داده را در دیسک می نویسد. یک حافظه نهان با
>> `read-ahead`
>>پیش بینی می کند که چه داده ای را سیستم نیاز خواهد داشت و در پس زمینه داده را واکشی می کند و می تواند
>>به خوبی کارایی را در خواندن فایل های متوالی یا خواندن فایل ها با الگو هایی با قابلیت پیش بینی آسان افزایش دهد.درباره یک
>>استراتژی ممکن برای پیاده سازی این دو ویژگی بحث کنید. شما باید به این سوال جدا از این که قصد پیاده سازی این ویژگی ها را
>>دارید یا خیر پاسخ دهید.

For this part, first of all you should pay attention that we need to do these works on the background (as you said in the question) so for both of them we need a seperated thread that is running on the background. For `write-behind` we should create and run its thread from starting point of our OS and it should iterate over the cache buffer blocks forever until powered down the system. Inside of these iterations it should write changed cache content (override) on relative disk sectors. How we identify a cache block that is changed? By a field named `dirty`. For `read-ahead` we should create and run a seperated thread when we are reading data of a disk's sector because we need to do reading-ahead on background of our system. Inside of this thread we'll read next sector of read sector and put it on cache.


### سوالات نظرسنجی

پاسخ به این سوالات دلخواه است، اما به ما برای بهبود این درس در ادامه کمک خواهد کرد.

نظرات خود را آزادانه به ما بگوئید—این سوالات فقط برای سنجش افکار شماست.

ممکن است شما بخواهید ارزیابی خود از درس را به صورت ناشناس و در انتهای ترم بیان کنید.

>>‫ به نظر شما، این تمرین گروهی، یا هر کدام از سه وظیفه آن، از نظر دشواری در چه سطحی بود؟ خیلی سخت یا خیلی آسان؟

خیلی آسان

>> چه مدت زمانی را صرف انجام این تمرین کردید؟ نسبتا زیاد یا خیلی کم؟

خیلی کم

>>‫ آیا بعد از کار بر روی یک بخش خاص از این تمرین (هر بخشی)، این احساس در شما به وجود آمد که اکنون یک دید بهتر نسبت به برخی جنبه‌های سیستم عامل دارید؟

بله، الان می‌توانم به راحتی کامپوترم را خاموش و روشن کنم. بسیار کار با سیستم عاملم قوی شده. خودم به تنهایی بک گراندم را عوض میکنم.

>>‫ آیا نکته یا راهنمایی خاصی وجود دارد که بهتر است ما آنها را به توضیحات این تمرین اضافه کنیم تا به دانشجویان ترم های آتی در حل مسائل کمک کند؟

بله وجود دارد.

>> متقابلا، آیا راهنمایی نادرستی که منجر به گمراهی شما شود وجود داشته است؟

بله بسیار زیاد. (سال بالایی ها گفتند درس را حذف نکن.)

>>‫ آیا پیشنهادی در مورد دستیاران آموزشی درس، برای همکاری موثرتر با دانشجویان دارید؟
این پیشنهادات میتوانند هم برای تمرین‌های گروهی بعدی همین ترم و هم برای ترم‌های آینده باشد.

بله، دیگر تی ای نشوید.

>>‫ آیا حرف دیگری دارید؟

بله، ایران درست نمیشه.
