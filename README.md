# Free List Memory Allocator

A simple **free list-based memory allocator** written in C. This project demonstrates the fundamental concepts behind dynamic memory allocation by implementing a custom allocator on top of a fixed-size memory pool.

## Features

- Fixed-size memory pool (4096 bytes)
- First-fit allocation strategy
- Doubly linked free list
- Block splitting during allocation
- 8-byte memory alignment
- Memory layout visualization

> **Note:** This project currently supports allocation only (`allocate`). Memory deallocation (`free`) and block coalescing are not yet implemented.

---

# Memory Layout

The allocator manages a static memory buffer of **4096 bytes**. Every memory block begins with a header that stores metadata about the block.

```
+-----------------------+
| Block Header          |
|-----------------------|
| size                  |
| next                  |
| prev                  |
+-----------------------+
| Payload               |
|                       |
|                       |
+-----------------------+
```

The header structure is defined as:

```c
typedef struct Block {
    size_t size;
    struct Block *next;
    struct Block *prev;
} Block;
```

The `size` field stores the total size of the block, including both the header and the payload.

---

# Allocation Strategy

The allocator uses the **First-Fit** strategy.

When memory is requested:

1. Traverse the free list.
2. Find the first block large enough to satisfy the request.
3. If the block is larger than necessary, split it into:
   - an allocated block
   - a smaller free block
4. Otherwise, allocate the entire block and remove it from the free list.

Illustration:

```
Before allocation

+-------------------------------------------+
|                 Free Block                |
+-------------------------------------------+

Allocate 64 bytes

+-------------+-----------------------------+
| Allocated   |         Free                |
+-------------+-----------------------------+
```

---

# Memory Alignment

All allocations are aligned to **8-byte boundaries**.

The allocator calculates the required block size using:

```c
(bytes + HEADER_SIZE + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1)
```

This ensures that returned pointers satisfy standard alignment requirements.

---

# Initialization

Before using the allocator, initialize the memory pool:

```c
memory_init();
```

Initially, the entire memory pool consists of a single free block.

```
+--------------------------------------+
|          4096 Bytes (Free)           |
+--------------------------------------+
```

---

# Example Usage

```c
memory_init();

void *a = allocate(64);
void *b = allocate(128);
void *c = allocate(512);
```

Each allocation searches the free list and either splits or removes a free block depending on its size.

---

# Visualizing Memory

The allocator provides the function:

```c
draw_memory();
```

Example output:

```
[88 bytes (Allocated)]
[152 bytes (Allocated)]
[536 bytes (Allocated)]
[3320 bytes (Free)]
```

This output represents the current layout of the managed memory pool.

---

# Project Structure

```
.
├── include/
│   └── allocator.h
├── src/
│   └── allocator.c
└── README.md
```

---

# Current Limitations

This implementation is intentionally minimal and does **not** yet include:

- Memory deallocation (`free`)
- Block coalescing
- Heap expansion
- Fragmentation handling
- Thread safety
- Alternative allocation strategies (Best-Fit, Worst-Fit, Next-Fit)

---

# Possible Improvements

Potential extensions include:

- Implement `free()`
- Merge adjacent free blocks
- Add allocation statistics
- Support multiple allocation strategies
- Improve memory visualization
- Add unit tests and benchmarks

---

# Learning Objectives

This project is intended as an educational implementation for understanding:

- Dynamic memory allocation
- Heap management
- Free lists
- Block splitting
- Memory alignment
- Pointer arithmetic
- Low-level memory management in C

Although simple, it demonstrates many of the core techniques used by real-world memory allocators and serves as a solid foundation for implementing more advanced allocators.
