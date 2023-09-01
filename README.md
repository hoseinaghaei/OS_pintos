# PintOS Phase 1 - User Program Execution

This repository contains the code and documentation for Phase 1 of the PintOS project. PintOS is an educational operating system used in the CS162 Operating Systems course (or a similar course) to learn about operating system concepts and implementation.

## Phase 0 (Initial Setup)

Before diving into Phase 1, it's essential to set up and familiarize yourself with PintOS. Phase 0 primarily focuses on building and running Pint-OS and includes some introductory questions to help you understand the project better.

### Phase 0 Documentation
- [Phase 0 Documentation](https://github.com/hoseinaghaei/OS_pintos/blob/phase01/proj0-intro.pdf): Detailed instructions and questions for Phase 0.

## Phase 1 (User Program Execution)

In Phase 1, we aim to implement the functionality required to run [user programs](https://github.com/hoseinaghaei/OS_pintos/blob/phase01/proj1-userprog.pdf). This phase comprises several key components:

### 1. Argument Passing

You need to make adjustments to the `src/userprog/process.c` [file](https://github.com/hoseinaghaei/OS_pintos/blob/phase01/src/userprog/process.c) to implement argument passing. Specifically, you should focus on the following functions:
- `setup_stack`: This function sets up the initial user program stack.
- `push_args_to_stack`: This function is responsible for pushing arguments onto the stack.

### 2. Process Control Syscall

You'll be working with the `src/userprog/syscall.c` [file](https://github.com/hoseinaghaei/OS_pintos/blob/phase01/src/userprog/syscall.c) to implement process control syscalls. These syscalls are crucial for managing user processes.

### 3. File System Syscalls and File Descriptors

In this phase, you'll also handle file system syscalls and file descriptors. It's essential to ensure that your implementation is thread-safe. You can find details about these tasks in the above-mentioned files and their associated commits.

## Getting Started

To begin with, Phase 1 of the PintOS project, follow these steps:

1. Clone this repository to your local machine:

   ```bash
   git clone https://github.com/hoseinaghaei/OS_pintos.git
