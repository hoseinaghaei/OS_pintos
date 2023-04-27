<div dir="rtl">

# گزارش تمرین گروهی


شماره گروه: 6

-----

> نام و آدرس پست الکترونیکی اعضای گروه را در این قسمت بنویسید.

> Hossein Aghaei h.aghaei.araei@gmail.com

> Armin DelgosarMaher arminmaher@gmail.com

> Seyyed Alireza Ghazanfari alireza79.ghazanfari@gmail.com

> AmirMahdi Kousheshi amk_amir82@yahoo.com 



## بخش اول (ساعت زنگ دار بهینه)

برای بخش اول هیچ تغییری نسبت به دیزاین نداشتیم.

## زمان‌بند اولویت‌دار

### داده ساختار ها

```c
// threads/thread.h

...

struct thread
{
    ...

    //struct list_elem acquired_locks_list;  
    struct list acquired_locks_list;
    // struct list_elem waited_locks_for;      
    struct lock *waited_lock;

    ...
};

```
دو تغییری که در دیزاین در ساختار thread داشتیم به صورت بالا تغییر کردند. تغییر اول بخاطر نحوه پیاده سازی توابع Pintos بوده و تغییر دوم نیز
به این دلیل بود که یک thread در لحظه تنها می‌تواند برای یک قفل منتظر بماند.

```c
// devices/synch.h
struct lock
  {
    struct list_elem elem;      /* Elem for held locks list. */
};

```
این فیلد را برای اضافه کردن لاک به لیست قفل‌های یک ترد اضافه کردیم.

### منطق
منطق پیاده‌سازی تغییر خاصی نسبت به توضیحات داک طراحی نداشت.