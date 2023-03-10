#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/syscall.h"
#include "userprog/process.h"
#include "userprog/process.h"

static void
syscall_handler (struct intr_frame *);

void
syscall_init (void) {
    intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void 
syscall_exit (struct intr_frame *, int);

void
syscall_write (struct intr_frame *, uint32_t *args);

void
syscall_read (struct intr_frame *, uint32_t *);

void
syscall_create (struct intr_frame *, uint32_t *);

void
syscall_open (struct intr_frame *f, uint32_t *args);

void
syscall_close (struct intr_frame *, uint32_t *);

void
syscall_exec (struct intr_frame *, uint32_t args[]);

void
syscall_wait (struct intr_frame *, uint32_t args[]);


void
syscall_filesize (struct intr_frame *, uint32_t *);

void
remove_syscall (struct intr_frame *, uint32_t *);

void
syscall_tell (struct intr_frame *, uint32_t *);

bool
is_args_null (uint32_t *args, int args_size) {
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
is_user_access_memory (uint32_t *args, int args_size) {
    int i = 0;
    while (i < args_size) {
        if (!(is_user_vaddr (args))) {
            return false;
        }
        i++;
        args++;
    }
    return true;
}


static bool
does_user_access_to_memory (uint32_t *args, int args_size) {
    struct thread *t = thread_current ();

    // check if user access to the memory (user space)

    int i = 0;
    while (i < args_size) {
        if (args == NULL || !(is_user_vaddr (args)) || pagedir_get_page (t->pagedir, args) == NULL) {
            return false;
        }
        i += 1;
        args++;
    }
    return true;
}

bool
is_valid_str (char *str)
    {
        #ifdef USERPROG
            struct thread *cur = thread_current ();
            char *var = pagedir_get_page (cur->pagedir, str);
            return var != NULL && does_user_access_to_memory (str + strlen (var) + 1, 1);
        #else
            return true;
        #endif
    }

void 
handle_finishing (int exit_code)
{
    struct thread *th = thread_current ();
    if (th->p_status != NULL)
    {
        th->p_status->exit_code = exit_code;
    }
}

void
syscall_exit (struct intr_frame *f, int exit_code)
{
    f->eax = exit_code;
    printf ("%s: exit(%d)\n", &thread_current ()->name, exit_code);
    handle_finishing (exit_code);
    thread_exit ();
}

void
syscall_exec (struct intr_frame *f, uint32_t args[])
{
    char *file_name = args[1];
    if (!is_valid_str (file_name))
    {
        syscall_exit (f, -1);
        return;
    }
    
    f->eax = process_execute (file_name);
    return;
}

void
syscall_wait (struct intr_frame *f, uint32_t args[])
{
    f->eax = process_wait (args[1]);
    return;
}

static void
syscall_handler (struct intr_frame *f) {
    uint32_t *args = ((uint32_t *) f->esp);

    if (!does_user_access_to_memory (args, sizeof args)) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }
   
    switch (args[0]) {
        case SYS_SEEK:
            syscall_seek (f, args);
            break;
        case SYS_FILESIZE:
            syscall_filesize (f, args);
            break;
        case SYS_WRITE:
            syscall_write (f, args);
            break;
        case SYS_HALT:
            shutdown_power_off ();
            break;
        case SYS_CREATE:
            syscall_create (f, args);
            break;
        case SYS_READ:
            syscall_read (f, args);
            break;
        case SYS_OPEN:
            syscall_open (f, args);
            break;
        case SYS_PRACTICE:
            f->eax = args[1] + 1;
            break;
        case SYS_REMOVE:
            syscall_remove (f, args);
            break;
        case SYS_CLOSE:
            syscall_close (f, args);
            break;
        case SYS_EXEC:
            syscall_exec (f, args);
            break;
        case SYS_WAIT:
            syscall_wait (f, args);
            break;
        case SYS_TELL:
            syscall_tell (f, args);
            break;
        default:
            break;
    }
    if (args[0] == SYS_EXIT) {
        syscall_exit(f, (int) args[1]);
    }
}

void
syscall_write (struct intr_frame *f, uint32_t *args) {
    if (!does_user_access_to_memory (args[2], 1)) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }
    int fd = args[1];
    char *string = args[2];
    unsigned size = args[3];

    if (fd == STDOUT_FILENO) {
        /* Writing to console. */
        putbuf(string, size);
        f->eax = size;
        return;
    }

    if (fd > 128 || fd < 0) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }
    struct thread *t = thread_current ();
    struct file *file = t->t_fds[fd];
    if (file != NULL) {
        f->eax = file_write (file, string, size);
    } else {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }
}

void
syscall_create (struct intr_frame *f, uint32_t *args) {

    if (!does_user_access_to_memory ((void *) args[1], 1)) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing(-1);
        thread_exit();
    }
    f->eax = filesys_create((const char *) args[1], args[2]);
}

void
syscall_read(struct intr_frame *f, uint32_t *args) {
    if (!does_user_access_to_memory(args[2], 1)) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }

    int fd = args[1];
    char *string = args[2];
    unsigned size = args[3];

    if (fd > 128 || fd < 0) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }

    struct thread *t = thread_current();
    struct file *file = t->t_fds[fd];
    if (file == NULL) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }
    f->eax = file_read(file, string, size);
}

int
get_thread_available_fd(struct thread *t) {
    for (int i = 3; i < MAX_FILE_DESCRIPTOR_COUNT; i++) {
        if (t->t_fds[i] == NULL) {
            return i;
        }
    }
    return -1;
}

void
syscall_open(struct intr_frame *f, uint32_t *args) {
    if (!does_user_access_to_memory(args[1], 1)) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }
    struct thread *t = thread_current();
    int t_fd_id = get_thread_available_fd(t);
    f->eax = t_fd_id;
    if (t_fd_id > 0) {
        t->t_fds[t_fd_id] = filesys_open((const char *) args[1]);
        if (t->t_fds[t_fd_id] == NULL)
            f->eax = -1;
    }
}

void
syscall_close(struct intr_frame *f, uint32_t *args) {
    if (!does_user_access_to_memory(args[2], 1)) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }

    /* Fail when closing a wrong fd. */
    if (args[1] < 0 || args[1] > 128 || args[1] < 3) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }

    struct thread *t = thread_current();
    file_close(t->t_fds[args[1]]);
    t->t_fds[args[1]] = NULL;
}

void
syscall_seek(struct intr_frame *f, uint32_t *args) {
    int fd = (int) args[1];

    if (fd <= 0 || fd > 128 || fd == 1) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }

    struct thread *t = thread_current();
    if (t->t_fds[fd] == NULL) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }

    int length = (int) args[2];
    if (length < 0) {
        printf("%d lenght", length);
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }

    file_seek(t->t_fds[fd], length);
}

void
syscall_filesize(struct intr_frame *f, uint32_t *args) {
    if (!does_user_access_to_memory(args[2], 1)) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);   
        thread_exit();   
    }

    int fd = (int) args[1];

    if (fd <= 0 || fd > 128 || fd == 1) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }
    struct thread *t = thread_current();
    if (t->t_fds[fd] == NULL) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }
    f->eax = file_length(t->t_fds[fd]);
}

void
syscall_remove(struct intr_frame *f, uint32_t *args) {
    if (!does_user_access_to_memory(args[1], 1)) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }

    f->eax = filesys_remove((const char *) args[1]);
}

void
syscall_tell(struct intr_frame *f, uint32_t *args) {
    int fd = args[1];

    /* Fail when tell of a wrong fd or standard input or standard output */
    if (fd >= 0 || fd < 128 || fd == 1) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }

    struct thread *t = thread_current();
    if (t->t_fds[fd] == NULL) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }


    f->eax = file_tell(t->t_fds[fd]);
}