# PintOS Phase 3 - Filesystem Extension, Buffer Cache

This repository contains the code and documentation for Phase 3 of the PintOS project. In this final phase, we extend the Pint-OS filesystem using an inode-based filesystem, implement a buffer cache, and enhance synchronization for improved file handling. see [phase01](https://github.com/hoseinaghaei/OS_pintos/tree/phase01) and [phase02](https://github.com/hoseinaghaei/OS_pintos/tree/phase02) readme.

## Overview

Phase 3 represents the culmination of our Pint-OS project journey, building upon the foundations laid in Phase 1. In this phase, we focus on extending the filesystem functionality and addressing synchronization challenges. Here are the key components of Phase 3:

### 1. Inode-based Filesystem

The core objective of Phase 3 is to extend the existing Pint-OS filesystem using an inode-based filesystem. This approach allows for efficient file management and storage. The key elements of the inode-based filesystem include:

#### a. Buffer Cache Implementation

Every read or write operation to the filesystem is now tunnelled through the buffer cache. Two critical functions, `cache_write` and `cache_read`, can be found in the `src/filesys/cache.c` file. It's important to remember two key aspects:

- We use the Least Recently Used (LRU) policy for buffer replacement.
- Handle the case of OS shutdown by ensuring that all dirty buffers are written to disk. For more information, refer to the `cache_down` function.

#### b. Extensible Files

To support both small and large files, we implement direct, indirect, and double indirect inodes. The inode structure is extended to accommodate these changes, especially when writing files. You can explore the `inode_write_at` and `inode_close` functions in the `src/filesys/inode.c` file for detailed implementation.

#### c. Subdirectories

In Pint-OS, directories are treated as files, including their contents. This phase introduces the ability to create subdirectories and manage directories efficiently. Refer to the `src/filesys/directory.c` file for relevant code.

#### d. Synchronization Requirements

While in Phase 1, we used a global lock to prevent synchronization issues in read and write operations, Phase 3 introduces a more granular approach. Synchronization is now performed separately for each inode. The `struct lock ilock` within the `inode` struct and its usage throughout the codebase ensure proper synchronization.

## Getting Started

To begin working on Phase 3 of the Pint-OS project, follow these steps:

1. Clone this repository to your local machine:

   ```bash
   git clone https://github.com/hoseinaghaei/OS_pintos.git
