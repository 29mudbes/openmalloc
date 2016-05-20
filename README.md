# openmalloc

A good memory allocator needs to balance a number of goals:
Maximizing Compatibility
An allocator should be plug-compatible with others; in particular it should obey ANSI/POSIX conventions.
Maximizing Portability
Reliance on as few system-dependent features (such as system calls) as possible, while still providing optional support for other useful features found only on some systems; conformance to all known system constraints on alignment and addressing rules.
Minimizing Space
The allocator should not waste space: It should obtain as little memory from the system as possible, and should maintain memory in ways that minimize fragmentation -- ``holes''in contiguous chunks of memory that are not used by the program.
Minimizing Time
The malloc(), free() and realloc routines should be as fast as possible in the average case.
Maximizing Tunability
Optional features and behavior should be controllable by users either statically (via #define and the like) or dynamically (via control commands such as mallopt).
Maximizing Locality
Allocating chunks of memory that are typically used together near each other. This helps minimize page and cache misses during program execution.
Maximizing Error Detection
It does not seem possible for a general-purpose allocator to also serve as general-purpose memory error testing tool such as Purify. However, allocators should provide some means for detecting corruption due to overwriting memory, multiple frees, and so on.
Minimizing Anomalies
An allocator configured using default settings should perform well across a wide range of real loads that depend heavily on dynamic allocation -- windowing toolkits, GUI applications, compilers, interpretors, development tools, network (packet)-intensive programs, graphics-intensive packages, web browsers, string-processing applications, and so on.

De olika minnesallokeringsmetoderna som ska implementeras är:

[First-fit] Första bästa tillräckligt stora blocket används.
[Best-fit] Det minsta lediga blocket med tillräckligt många bytes allokeras.
[Worst-fit] Det största lediga blocket används och fragmenteras.
[Quick-fit] Som Best-fit, men istället för att alla block kontrolleras så har man ett antal listor med minne av olika storlek.

Dessa ska implementeras i enlighet med hjälp av ett antal givna acceptanstest.

Vidare ska även samtliga algoritmers prestanda utvärderas utförligt i några väl valda aspekter.

Hur mycket minne slösas bort i medel och i värsta fallet i de block som allokeras via malloc() ur ``Quick fit'' listorna?}

Quick fit listorna tar emot block av jämna tvåpotenser av ett
förutbestämt värde. Det betyder att den tillåtna blockstorleken i
varje lista dubblas för varje lista ända tills den sista listan. I
det värsta fallet får ett block precis inte plats i ett en lista utan är en
enhet för stor, vilket betyder att den måste placeras i listan för dubbelt så
stora block.

\begin{equation*}
\begin{array}{c}
n+1 > n\\
2n - (n+1)\\
n-1\\
\frac{n-1}{2n} \approx \frac{1}{2}\\
\end{array}
\end{equation*}

Där $n$ är storleken på blocket där minnet inte fick plats.
Eftersom minnet vi ville allokera bara är något större än det block den inte
fick plats i är den mycket nära hälften så stor nästa lista med dubbelt så många
block. Alltså spiller vi i värsta fall nästan $50\%$ av allt vi allokerar.

Medelvärdet av minnes spillet när man använder Quick Fit ökar när block
storleken ökar (man har fler listor som tillåter större block). Den största
andelen spill kommer att komma ifrån de större block listorna, eftersom dem har
mer minne att spilla. Medelvärdet kommer alltså att domineras av de större
blockstorlekarna. De större blockstorlekarna kommer dessutom att ha ett mycket
snarlikt medelvärde, alltså kan vi approximera hela medelvärdet till medelvärdet
hos listan med störst block. Medelvärdet av minnesspillet i den sista listan är:

\begin{equation*}
\sum^n_{i=1}\frac{i}{n} = \frac{n + 1}{2}
\end{equation*}
\begin{equation*}
\frac{\frac{n + 1}{2}}{n} = \frac{n+1}{2n} = \frac{1}{2} + \frac{1}{2n}
\end{equation*}

Där $n$ är storleken på det blocken i den sista listan. Det slutgiltiga uttrycket
för andelen minnesspill $\frac{1}{2} + \frac{1}{2n}$ består av två
termer: $\frac{1}{2}$ som är värsta fallet ($50\%$) och $\frac{1}{2n}$, när $n$
ökar kommer den andra termen minska i värde och uttrycket kommer närma sig värstafallet. 
Medelfallet kommer i praktiken tyvärr att ligga ganska nära värstafallet $50\%$ spill.


Programstrukturen var given i uppgiftsbeskrivningen, vi kompletterade den en del med hjälp av man-sidorna för malloc/free/realloc.
De ändringar vi gjorde var att i \texttt{malloc.c} ha en preprocessorvillkorssats som inkluderade de olika algoritmerna beroende på vad \texttt{STRATEGY} var satt till. Baskoden togs direkt från boken ``The C Programming Language'' precis som det tipsades om i boken, den koden ligger till grund för den större delen av våra implementationer.

Vi hade problem med hur sorteringen skulle gå till i best/worst fit. Frågan var om man skulle försöka hålla listan sorterad in-place eller sortera om den varje gång. Men efter mycket om och men kom vi fram till att det var ingen stor förlust att sortera om listan efter varje operation från början.

Implementeringen var sedan ganska rakt på. Gick något fel fick man använda sig av `gdb' för att stega sig igenom programmet och inspektera variabler. I slutet kontrollerades beteendet med hjälp av testfallen.

Vi har valt att utföra två huvudtester; ett som mäter tidsprestanda och ett som mäter andelen overhead.

Vi mäter tiden det tar att köra ett antal varierande \texttt{malloc} resp. \texttt{free} med hjälp av unix-kommandot \texttt{`time'}.


Koden som kördes vid tillfällena visas här:


Nedan finnes resultat av tidskörningarna.

		\hline
		 & Körn. 1	& Körn. 2	& Körn. 3	& Körn. 4	& Körn. 5	& Körn. 6	& Std. avvikelse. \\
		\hline
		System		& 1,8 s			& 1,97 s	& 1,8 s		& 1,84 s	& 1,8 s		& 1,79 s	& 0,07 s \\
		First Fit	& 1,17 s		& 1,17 s	& 1,17 s	& 1,16 s	& 1,17 s	& 1,17 s	& 0,00 s \\
		Best Fit	& 1,43 s		& 1,42 s	& 1,43 s	& 1,43 s	& 1,43 s	& 1,42 s	& 0,01 s \\
		Worst Fit	& 1,39 s		& 1,41 s	& 1,39 s	& 1,39 s	& 1,39 s	& 1,39 s	& 0,01 s \\
		Quick Fit	& 1,58 s		& 1,52 s	& 1,52 s	& 1,52 s	& 1,55 s	& 1,53 s	& 0,02 s \\
		\hline
\end{tabular}

Vi kan utläsa att den enklaste algoritmen, `First Fit` är den snabbaste. Best och Worst fit har mer logik och kräver därför mer tid på sig. Långsammast, vilket är lite motsägelsefullt var `Quick Fit`. Detta beror på att våran Quick fit implementation aldrig förallokerar några block, utan bara allokerar så mycket som behövs. Just denna testkod är lite orättvis eftersom bara tre block allokeras åt gången, quick-fit listornas fördelar utnyttjas aldrig med så få block.


Sammanfattningsvis kan man se att vi ligger under systemets egna tidsåtgång, vilket betyder att vi egentligen inte har några prestandaproblem i just det här fallet.



Vi använder tre olika storlekar för mätdata en liten (64 bytes), en stor (10 sidor) och en slumpmässig (0 till 2 sidor). Dessa har allokerats 1000 gånger. Den faktiska åtgången har mäts m.h.a \texttt{sbrk(0)} och den optimala åtgången har även summerats. Dessa har dividerats för att få en faktor på hur stor del av datan som är overhead.





	\begin{tabular}{ | >{\bfseries}l | c | c | c | r |}
		\hline
					& Small blocks	& Big blocks	& Mixed blocks	& Avg \\
		\hline
		Best Fit	& 1,28	& 1,0004	& 1,051		& 1,110 \\
		First Fit	& 1,28	& 1,0004	& 1,0631	& 1,114 \\
		Worst Fit	& 1,28	& 1,0004	& 1,1854	& 1,155 \\
		Quick Fit	& 2		& 1,0004	& 1,0631	& 1,355 \\
		System		& 2,112	& 1,0006	& 1,0169	& 1,377 \\
		\hline
	\end{tabular}


Som väntat kan vi se att Quick Fit är slösaktig med minnet. Systemets egna malloc hamnar i dåligt läge på grund av den dåliga presteringen för små block. Vidare kan vi konstantera att för stora block är overhead-datan obetydlig, som sig bör.

Vi kan även se att för stora block så är quick fit ekvivalent med first fit, precis som den ska eftersom alla block hamnar i den sista listan.


Labben hade en klart annorlunda struktur är de tidigare labbarna. Nedan följer lite reflektioner kring laborationen:


Att leverera färdiga tester istället för en kravspec är en klar förbättring och gör utvecklingen mycket enklare.
Labben tvingade oss att använda och förstå debuggaren `gdb' för att hitta fel. En övning om enbart gdb hade nog varit grymt, ty den är inte särskilt intuitiv att använda.
Att ha 4 skilda algoritmer gjorde det lätt att arbeta parallellt.


Work: 4 weeks
Difficulty: 2/5
