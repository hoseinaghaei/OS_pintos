#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/syscall.h"
#include "userprog/process.h"
#include "userprog/process.h"
#include "filesys/cache.h"

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

void
syscall_mkdir(struct intr_frame *, uint32_t *);

void
syscall_chdir(struct intr_frame *, uint32_t *);

void
syscall_readdir(struct intr_frame *, uint32_t *);

void
syscall_readdir(struct intr_frame *, uint32_t *);

struct dir *
get_file_dir (struct file *file)
{
    if (file == NULL) {
        return NULL;
    }

    struct inode *inode = file_get_inode(file);
    if (inode == NULL) {
        return NULL;
    }

    if (!is_inode_dir(inode)) {
        return NULL;
    }

    return (struct dir *) file;
}

void
file_descriptor_inode(struct inode **inode, int fd, struct thread *trd)
{
    *inode = file_get_inode (trd->t_fds[fd]);
}

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
        case SYS_SEEK: // checks if a file exists
            syscall_seek (f, args);
            break;
        case SYS_FILESIZE: // checkout a file's size
            syscall_filesize (f, args);
            break;
        case SYS_WRITE: // write in a file
            syscall_write (f, args);
            break;
        case SYS_HALT: // halt the OS
            shutdown_power_off ();
            break;
        case SYS_CREATE: // create a new file
            syscall_create (f, args);
            break;
        case SYS_READ: // read from a file
            syscall_read (f, args);
            break;
        case SYS_OPEN: // open a file and add it to FDs
            syscall_open (f, args);
            break;
        case SYS_PRACTICE: // get arg + 1
            f->eax = args[1] + 1;
            break;
        case SYS_REMOVE: // remove a file
            syscall_remove (f, args);
            break;
        case SYS_CLOSE: // close a file
            syscall_close (f, args);
            break;
        case SYS_EXEC: // execute a file in kernel
            syscall_exec (f, args);
            break;
        case SYS_WAIT: // wait for a child process
            syscall_wait (f, args);
            break;
        case SYS_TELL: // checkout current position in a file
            syscall_tell (f, args);
            break;
        case SYS_MKDIR: //create directory
            syscall_mkdir(f,args);
            break;
        case SYS_CHDIR: //change to a directory
            syscall_chdir(f,args);
            break;
        case SYS_READDIR: //read dirs in a directory
            syscall_readdir(f,args);
            break;
        case SYS_INUMBER: //get inumber
            syscall_inumber(f,args);
            break;
        case SYS_ISDIR: //check is a inode is dir
            syscall_isdir(f,args);
            break;
        case SYS_CACHE_HIT:
            syscall_cache_hit (f, args);
            break;
        case SYS_CACHE_RESET:
            syscall_cache_reset (f, args);
            break;
        case SYS_CACHE_WRITE_NUM:
            syscall_cache_write_num (f, args);
            break;
        default:
            break;
    }
    if (args[0] == SYS_EXIT) { // terminate a process
        syscall_exit (f, (int) args[1]);
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

    if (fd > 128 || fd < 0 || fd == STDIN_FILENO) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }
    struct thread *t = thread_current ();
    struct file *file = t->t_fds[fd];
    if (get_file_dir (file) == NULL) {
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
        handle_finishing (-1);
        thread_exit ();
    }
    const char *name = (const char *) args[1];
    f->eax = filesys_create (name, args[2], false);
}

void
syscall_read (struct intr_frame *f, uint32_t *args) {
    if (!does_user_access_to_memory(args[2], 1)) {
        printf("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing(-1);
        thread_exit ();
    }

    int fd = args[1];
    char *string = args[2];
    unsigned size = args[3];

    if (fd > 128 || fd < 0) {
        printf("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing(-1);
        thread_exit ();
    }

    struct thread *t = thread_current ();
    struct file *file = t->t_fds[fd];
    if (file == NULL) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }
    f->eax = file_read (file, string, size);
}

int
get_thread_available_fd (struct thread *t) {
    for (int i = 3; i < MAX_FILE_DESCRIPTOR_COUNT; i++) {
        if (t->t_fds[i] == NULL) {
            return i;
        }
    }
    return -1;
}

void 
syscall_cache_write_num (struct intr_frame *f, uint32_t *args)
{
    f->eax = get_num_writes ();
}

void 
syscall_cache_hit (struct intr_frame *f, uint32_t *args)
{
    f->eax = get_hit();
}

void 
syscall_cache_reset (struct intr_frame *f, uint32_t *args)
{
    reset_cache ();
}

void
syscall_open (struct intr_frame *f, uint32_t *args) {
    if (!does_user_access_to_memory (args[1], 1)) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing(-1);
        thread_exit ();
    }
    struct thread *t = thread_current ();
    int t_fd_id = get_thread_available_fd (t);
    f->eax = t_fd_id;
    if (t_fd_id > 0) {
        t->t_fds[t_fd_id] = filesys_open ((const char *) args[1]);
        if (t->t_fds[t_fd_id] == NULL)
            f->eax = -1;
    }
}

void
syscall_close (struct intr_frame *f, uint32_t *args) {
    if (!does_user_access_to_memory(args[2], 1)) {
        printf ("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing (-1);
        thread_exit ();
    }

    if (args[1] < 0 || args[1] > 128 || args[1] < 3) {
        printf ("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing (-1);
        thread_exit ();
    }

    struct thread *t = thread_current ();
    file_close (t->t_fds[args[1]]);
    t->t_fds[args[1]] = NULL;
}

void
syscall_seek (struct intr_frame *f, uint32_t *args) {
    int fd = (int) args[1];

    if (fd <= 0 || fd > 128 || fd == 1) {
        printf("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }

    struct thread *t = thread_current ();
    if (t->t_fds[fd] == NULL) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }

    int length = (int) args[2];
    if (length < 0) {
        printf ("%d lenght", length);
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }

    file_seek (t->t_fds[fd], length);
}

void
syscall_filesize (struct intr_frame *f, uint32_t *args) {
    if (!does_user_access_to_memory (args[2], 1)) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }

    int fd = (int) args[1];

    if (fd <= 0 || fd > 128 || fd == 1) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }
    struct thread *t = thread_current ();
    if (t->t_fds[fd] == NULL) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }
    f->eax = file_length (t->t_fds[fd]);
}

void
syscall_remove (struct intr_frame *f, uint32_t *args) {
    if (!does_user_access_to_memory(args[1], 1)) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }

    f->eax = filesys_remove((const char *) args[1]);
}

void
syscall_tell (struct intr_frame *f, uint32_t *args) {
    int fd = args[1];

    if (fd <= 2 || fd >= 128) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }

    struct thread *t = thread_current ();
    if (t->t_fds[fd] == NULL) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }

    f->eax = file_tell (t->t_fds[fd]);
}

void
syscall_mkdir (struct intr_frame *f, uint32_t* args)
{
    if (!does_user_access_to_memory(args[1], 1)) {
        printf("%s: exit(-1)\n", &thread_current()->name);
        handle_finishing(-1);
        thread_exit();
    }

    const char *path = (const char *) args[1];

    f->eax = filesys_create (path, 0, true);
}

void
syscall_chdir (struct intr_frame *f, uint32_t* args){
    char *name = (char *) args[1];
    struct dir *dir = dir_open_directory (name);
    if (dir != NULL)
    {
        dir_close (thread_current ()->curr_dir);
        thread_current ()->curr_dir = dir;
        f->eax = 1;
    }
    else {
        f->eax = 0;
    }
}

void
syscall_readdir (struct intr_frame *f, uint32_t* args){
    int fd = args[1];
    char *name = (char *) args[2];
    struct thread *curr_thread = thread_current();

    if (curr_thread->t_fds[fd] == NULL) {
        f->eax = 0;
        return;
    }

    if (fd > 128 || fd < 0 || fd == 1)
    {
        f->eax = 0;
        return;
    }

    struct dir *dir = get_file_dir (curr_thread->t_fds[fd]);
    if (dir == NULL)
    {
        f->eax = false;
        return;
    }

    f->eax = dir_readdir (dir, name);
}

void
syscall_inumber (struct intr_frame *f, uint32_t* args){
    struct inode *inode;
    struct thread * curr_thread = thread_current();
    int fd = args[1];

    if (fd > 128 || fd < 0 || fd == 0 || fd == 1){
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }

    if (curr_thread->t_fds[fd] == NULL) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }

    file_descriptor_inode (&inode, args[1], curr_thread);

    f->eax = inode_get_inumber (inode);
}

void
syscall_isdir(struct intr_frame *f, uint32_t* args){
    struct inode *inode;
    struct thread * curr_thread = thread_current();
    int fd = args[1];
    if (fd > 128 || fd < 0 || fd == 0 || fd == 1){
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }

    if (curr_thread->t_fds[fd] == NULL) {
        printf ("%s: exit(-1)\n", &thread_current ()->name);
        handle_finishing (-1);
        thread_exit ();
    }

    file_descriptor_inode (&inode, args[1], curr_thread);

    f->eax = is_inode_dir (inode);
}

