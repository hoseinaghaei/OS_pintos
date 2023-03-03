#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler(struct intr_frame *);

void
syscall_init(void) {
    intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void
syscall_write(struct intr_frame *);

static void
syscall_handler(struct intr_frame *f) {
    uint32_t *args = ((uint32_t *) f->esp);

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
    }
    if (args[0] == SYS_EXIT) {
        f->eax = args[1];
        printf("%s: exit(%d)\n", &thread_current()->name, args[1]);
        thread_exit();
    }
}

void syscall_write(struct intr_frame *f) {
    int fd = *(int *) (f->esp + 4);
    const void *buffer = *(const void **) (f->esp + 8);
    unsigned size = *(unsigned *) (f->esp + 12);

    if (fd == STDOUT_FILENO) {
        /* Writing to console. */
        putbuf(buffer, size);
        return size;
    } else {
        /* Unsupported file descriptor. */
        return -1;
    }
}