[![Build Status](https://travis-ci.org/montao/openmalloc.svg?branch=master)](https://travis-ci.org/montao/openmalloc)

# openmalloc

A good memory allocator needs to balance a number of goals:
## Maximizing Compatibility
An allocator should be plug-compatible with others; in particular it should obey ANSI/POSIX conventions.
## Maximizing Portability
Reliance on as few system-dependent features (such as system calls) as possible, while still providing optional support for other useful features found only on some systems; conformance to all known system constraints on alignment and addressing rules.
## Minimizing Space
The allocator should not waste space: It should obtain as little memory from the system as possible, and should maintain memory in ways that minimize fragmentation -- ``holes''in contiguous chunks of memory that are not used by the program.
## Minimizing Time
The malloc(), free() and realloc routines should be as fast as possible in the average case.
## Maximizing Tunability
Optional features and behavior should be controllable by users either statically (via #define and the like) or dynamically (via control commands such as mallopt).
## Maximizing Locality
Allocating chunks of memory that are typically used together near each other. This helps minimize page and cache misses during program execution.
## Maximizing Error Detection
It does not seem possible for a general-purpose allocator to also serve as general-purpose memory error testing tool such as Purify. However, allocators should provide some means for detecting corruption due to overwriting memory, multiple frees, and so on.
## Minimizing Anomalies
An allocator configured using default settings should perform well across a wide range of real loads that depend heavily on dynamic allocation -- windowing toolkits, GUI applications, compilers, interpretors, development tools, network (packet)-intensive programs, graphics-intensive packages, web browsers, string-processing applications, and so on.

Some issues that an allocator needs to address are:

## alignment 
- some memory alignment are faster than others
## fragmentation 
- naive allocation and freeing can leave holes that cause bloat
## performance 
- going back to the OS for more memory can be expensive
## Common requests 
- in practice applications (esp C++) often do a lot of small allocations so making these efficient can help a lot
The different memory allocation algorithms are

[First-fit] The first large enough block is used  
[Best-fit] The smallest free block with enough bytes is allocated  
[Worst-fit] The largest free block is used and fragmented  
[Quick-fit] Same as bestfit, but using lists  

How much memory is wasted on average and in the worst case in the blcks that are allocated via malloc() from the quick-fit lists? The quick fit lists receive blocks of even powers of 2 from a pre-calculated value. This means that the allowed block size in each list is doubled for every list until the last list. In the worst case, a block just can't fit in one list and is one unit too large, which means it must be placed in the list for double large blocks. 

