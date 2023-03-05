#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler(struct intr_frame *);

void
syscall_init(void) {
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void
syscall_write(struct intr_frame *);

void
syscall_create(struct intr_frame *, uint32_t *);


bool
is_args_null(uint32_t *args, int args_size) {
    int i = 0;
    while (i < args_size) {
        if (args[i] == NULL) {
            return true;
        }
        i++;
    }
    return false;
}

bool
is_user_access_memory(uint32_t *args, int args_size) {
    int i = 0;
    while (i < args_size) {
        if (!(is_user_vaddr(args))) {
            return false;
        }
        i++;
        args++;
    }
    return true;
}


static bool
does_user_access_to_memory(uint32_t *args, int args_size) {
    struct thread *t = thread_current();

    // check if user access to the memory (user space)

    int i = 0;
    while (i < args_size) {
        if (args == NULL || !(is_user_vaddr(args)) || pagedir_get_page(t->pagedir, args) == NULL) {
            return false;
        }
        i += 1;
        args++;
    }
    return true;
}


static void
syscall_handler(struct intr_frame *f) {
    uint32_t *args = ((uint32_t *) f->esp);

    if (!does_user_access_to_memory(args, sizeof args)) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        thread_exit();
    }
    /*
     * The following print statement, if uncommented, will print out the syscall
     * number whenever a process enters a system call. You might find it useful
     * when debugging. It will cause tests to fail, however, so you should not
     * include it in your final submission.
     */


    /* printf("System call number: %d\n", args[0]); */
    switch (args[0]) {
        case SYS_WRITE:
            syscall_write(f);
            break;
        case SYS_HALT:
            shutdown_power_off();
            break;
        case SYS_CREATE:
            syscall_create(f, args);
            break;
        case SYS_OPEN:
            syscall_open(f, args);
            break;
        case SYS_PRACTICE:
            f->eax = args[1] + 1;
            break;
        default:
            break;
    }
    if (args[0] == SYS_EXIT) {
        f->eax = args[1];
        printf("%s: exit(%d)\n", &thread_current()->name, args[1]);
        thread_exit();
    }
}

void
syscall_write(struct intr_frame *f) {
    int fd = *(int *) (f->esp + 4);
    const void *buffer = *(const void **) (f->esp + 8);
    unsigned size = *(unsigned *) (f->esp + 12);

    if (fd == STDOUT_FILENO) {
        /* Writing to console. */
        putbuf(buffer, size);
        return;
    }
}

void
syscall_create(struct intr_frame *f, uint32_t *args) {

    if (!does_user_access_to_memory((void *) args[1], 1)) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        thread_exit();
    }
    f->eax = filesys_create((const char *) args[1], args[2]);
}

void
syscall_open(struct intr_frame *f, uint32_t *args) {
    if (!does_user_access_to_memory(args[1], 1)) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        thread_exit();
    }

    struct thread *t = thread_current();
}