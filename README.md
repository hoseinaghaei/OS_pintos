# PintOS Phase 2 - Thread Scheduling and Priority

This repository contains the code and documentation for Phase 2 of the PintOS project. In this phase, we focus on thread scheduling and priority management within the Pint-OS operating system.

## Overview

Phase 2 of the PintOS project is distinct from Phase 1, as it revolves around the use of kernel threads and thread scheduling. Here are the three main components of this phase:

### 1. Timer Sleep Function Enhancement

In Phase 2, the first task is to enhance the `timer_sleep` function. It should no longer employ busy-waiting. Instead, it should add the sleeping threads to a waiting list and wake them up when the timer ticks. This enhancement helps in improving the efficiency of thread management.

- Check the implementation of the enhanced `timer_sleep` function in the `pintos/src/devices/timer.c` [file](https://github.com/hoseinaghaei/OS_pintos/blob/phase02/pintos/src/devices/timer.c).

### 2. Priority Scheduler Implementation

A key element of Phase 2 is the implementation of a Priority Scheduler. This scheduler selects the thread with the highest priority from the waiting list for execution. When implementing this scheduler, it's essential to consider the priority-donation problem and handle it appropriately.

- Most of the implementation for the Priority Scheduler can be found in the `pintos/src/threads/thread.c` [file](https://github.com/hoseinaghaei/OS_pintos/blob/phase02/pintos/src/threads/thread.c).

### 3. Scheduling Lab and Testing

To ensure the correctness and effectiveness of your scheduling implementation, this phase includes a lab for testing various types of scheduling scenarios. You can find the Jupyter Notebook file with test cases and analysis in the provided link.

- [Scheduling Lab (Jupyter Notebook)](https://github.com/hoseinaghaei/OS_pintos/blob/phase02/schedLab/Scheduling.ipynb)

## Getting Started

To start working on Phase 2 of the PintOS project, follow these steps:

1. Clone this repository to your local machine:

   ```bash
   git clone https://github.com/hoseinaghaei/OS_pintos.git
