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

How much memory is wasted on avaerga and in the worst case in the blcks that are allocated via malloc() from the quick-fit lists? The quick fit lists receive blocks of even powers of 2 from a pre-calculated value. This means that the allowed block size in each list is doubled for every list until the last list. In the worst case, a block just can't fit in one list and is one unit too large, which means it must be placed in the list for double large blocks. 

Där $n$ är storleken på blocket där minnet inte fick plats. Eftersom minnet vi ville allokera bara är något större än det block den inte fick plats i är den mycket nära hälften så stor nästa lista med dubbelt så många block. Alltså spiller vi i värsta fall nästan $50\%$ av allt vi allokerar.

Medelvärdet av minnes spillet när man använder Quick Fit ökar när block storleken ökar (man har fler listor som tillåter större block). Den största andelen spill kommer att komma ifrån de större block listorna, eftersom dem har mer minne att spilla. Medelvärdet kommer alltså att domineras av de större blockstorlekarna. De större blockstorlekarna kommer dessutom att ha ett mycket snarlikt medelvärde, alltså kan vi approximera hela medelvärdet till medelvärdet hos listan med störst block. Medelvärdet av minnesspillet i den sista listan är:

Där $n$ är storleken på det blocken i den sista listan. Det slutgiltiga uttrycket för andelen minnesspill $\frac{1}{2} + \frac{1}{2n}$ består av två termer: $\frac{1}{2}$ som är värsta fallet ($50\%$) och $\frac{1}{2n}$, när $n$ ökar kommer den andra termen minska i värde och uttrycket kommer närma sig värstafallet. Medelfallet kommer i praktiken tyvärr att ligga ganska nära värstafallet $50\%$ spill.

Programstrukturen var given i uppgiftsbeskrivningen, vi kompletterade den en del med hjälp av man-sidorna för malloc/free/realloc.
De ändringar vi gjorde var att i \texttt{malloc.c} ha en preprocessorvillkorssats som inkluderade de olika algoritmerna beroende på vad \texttt{STRATEGY} var satt till. Baskoden togs direkt från boken ``The C Programming Language'' precis som det tipsades om i boken, den koden ligger till grund för den större delen av våra implementationer.

Vi hade problem med hur sorteringen skulle gå till i best/worst fit. Frågan var om man skulle försöka hålla listan sorterad in-place eller sortera om den varje gång. Men efter mycket om och men kom vi fram till att det var ingen stor förlust att sortera om listan efter varje operation från början.

Implementeringen var sedan ganska rakt på. Gick något fel fick man använda sig av `gdb' för att stega sig igenom programmet och inspektera variabler. I slutet kontrollerades beteendet med hjälp av testfallen.

Vi har valt att utföra två huvudtester; ett som mäter tidsprestanda och ett som mäter andelen overhead.

Vi mäter tiden det tar att köra ett antal varierande \texttt{malloc} resp. \texttt{free} med hjälp av unix-kommandot \texttt{`time'}.

Koden som kördes vid tillfällena visas här:

Nedan finnes resultat av tidskörningarna.

		
		 & Körn. 1	& Körn. 2	& Körn. 3	& Körn. 4	& Körn. 5	& Körn. 6	& Std. avvikelse. \\
		
		System		& 1,8 s			& 1,97 s	& 1,8 s		& 1,84 s	& 1,8 s		& 1,79 s	& 0,07 s \\
		First Fit	& 1,17 s		& 1,17 s	& 1,17 s	& 1,16 s	& 1,17 s	& 1,17 s	& 0,00 s \\
		Best Fit	& 1,43 s		& 1,42 s	& 1,43 s	& 1,43 s	& 1,43 s	& 1,42 s	& 0,01 s \\
		Worst Fit	& 1,39 s		& 1,41 s	& 1,39 s	& 1,39 s	& 1,39 s	& 1,39 s	& 0,01 s \\
		Quick Fit	& 1,58 s		& 1,52 s	& 1,52 s	& 1,52 s	& 1,55 s	& 1,53 s	& 0,02 s \\
		
We see that the simplest algorithm: 'First fit' is the fastest. Best and worst fit has more logic and therefore takes more time. The slowest was 'Quick fit" because
our quick fit implementaion never pre-allocate blocks, it only allocate what is needed. This test code is a bit unfair bcause only three blocks at a time is allocatd.
That way the advantage of quick-fit lists are not used with so few blocks. 

In conslusion we can see that we are below the system's malloc time, which means that we don't have a performance issue. 

We use three different sizes for measure data, one small (64 bytes), one large (10 pages) and one random (0 to 2 pages). These are allocated 100 times. 
The actual usage is measured with sbrk(0) and the optimal consumtion is also summed up. These are divided to get a factor of how much of the data is overhead. 

					& Small blocks	& Big blocks	& Mixed blocks	& Avg \\
		
		Best Fit	& 1,28	& 1,0004	& 1,051		& 1,110 \\
		First Fit	& 1,28	& 1,0004	& 1,0631	& 1,114 \\
		Worst Fit	& 1,28	& 1,0004	& 1,1854	& 1,155 \\
		Quick Fit	& 2		& 1,0004	& 1,0631	& 1,355 \\
		System		& 2,112	& 1,0006	& 1,0169	& 1,377 \\
		
As expected we can see that Quick Fit is wasteful with the resources. The system's malloc performs badly because of the bad performance for small blocks. 

Furthermore we see that for large blocks the overhead data has little or no meaning, as it should be. 
 
We can also see that for large blocks, quick fit is equavalent with first fit, like it should be beacuse all blocks occur in the last list. 
  
Reflections
Test-driven development is an improvement mostly, but it can be expensive. 
Good to know gdb. 
Good to work on different algorthms - easy to separate the work. 

Work: 4 weeks
Difficulty: 3/5
