# Scalable Thread Management Library

## Overview
This project implements a scalable thread management library in C, designed to handle dynamic thread creation, task execution, and resource optimization. It includes a thread pool, task queue, synchronization mechanisms, and performance evaluation tools. The project is developed collaboratively by a team using POSIX threads (pthreads) on a Linux environment (WSL Ubuntu).

## Files and Contributions

### Thread.c
- **Purpose**: Implements basic thread creation and execution.
- **Contributor**: Rakhi (@rakhi120805)
- **Details**: This file (originally `first.c`) demonstrates creating and joining a single thread using pthreads. It serves as the foundation for understanding thread lifecycle management.

### thread_manager.c
- **Purpose**: Implements a producer-consumer model for task execution.
- **Contributor**: Rakhi (@rakhi120805)
- **Details**: This file uses a thread-safe message queue with a linked list to manage tasks. A producer thread adds tasks, and a consumer thread executes them, demonstrating dynamic task scheduling.

### taskqueue.c
- **Purpose**: Manages a task queue for thread pool operations.
- **Contributor**: [Anushka] (22anushkaroy)
- **Details**: This file likely provides a task queue implementation, possibly used by `thread_manager.c` or other modules for scheduling tasks across threads.

### Synchronization.c
- **Purpose**: Implements synchronization mechanisms for thread safety.
- **Contributor**: [Sneha Tiwari] (Snehatiwarideepti)
- **Details**: This file likely includes mutexes, condition variables, or semaphores to ensure thread-safe operations across the library, such as in task queuing or shared resource access.

### performance.c
- **Purpose**: Evaluates the performance of the thread management system.
- **Contributor**: [Anushka] (22anushkaroy)
- **Details**: This file likely measures metrics like task execution time, thread utilization, or throughput, providing insights into the library’s efficiency.

## Team Members
- **Rakhi (@rakhi120805)**: Implemented `Thread.c` and `thread_manager.c`, focusing on thread creation and producer-consumer task execution.
- **[Anushka] (22anushkaroy)**: Developed `taskqueue.c` for task management.
- **[Sneha Tiwari] (Snehatiwarideepti)**: Created `Synchronization.c` for thread safety.
- **[Anushka] (22anushkaroy)**: Built `performance.c` for performance evaluation.

## How to Build and Run
1. **Clone the Repository**:
   ```bash
   git clone https://github.com/rakhi120805/thread-lib.git
   cd thread-lib
