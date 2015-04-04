[![Build Status](https://webapi.biicode.com/v1/badges/pablodev/pablodev/bitscan/master)](https://www.biicode.com/pablodev/bitscan) 


BITSCAN
===================
BITSCAN is a C++ 64-bit library which manipulates bit arrays (alias bit strings, bit sets etc.) and is specifically oriented for fast bitscanning operations. BITSCAN is the result of years of research in combinatorial optimization problems and has been shown very useful when modelling sets. In this type of models a bit string represents the population and members belonging to the set are marked as a 1-bit and referred by the position in the bitstrig.

The BITSCAN library is still in alpha but has been used succesfully to implement *[BBMC](https://www.researchgate.net/publication/220472408_An_exact_bit-parallel_algorithm_for_the_maximum_clique_problem?ev=prf_pub "Exact maximum clique algorithm")*, an efficient state of the art maximum clique algorithm. It has also been applied in [PASS](http://phdtree.org/pdf/34678243-a-new-dsatur-based-algorithm-for-exact-vertex-coloring/), an exact, and efficient, vertex coloring DSATUR-based algorithm.  For further references to *BBMC* and *PASS* as well as legal stuff please read the *legal.txt* file. 

BITSCAN has been tested in LINUX and Windows OS. Please note that 64-bit configurations are required. 

The library currently offers the following types for bitstrings

- `bitblock`: Wrapper for bit manipulation of 64-bit numbers (*lsb*, *msb*, *popcount* etc.).
- `simple_bitarray`: Extends bit manipulation to bit arrays
- `bitarray`: Main type for standard bit manipualtion. Uses compiler intrinsics (or assembler equivalents) enhancements.
- `watched_bitarray`: Extends the bitarray type for populations with low density but not really sparse.Empty bit blocks are still stored in full, but two pointers (aka sentinels) which point (alias *watch*) the highest and lowest empty blocks respectively, determine the range of useful bitmasks.
- `simple_sparse_bitarray`: General operations for sparse bit arrays.
- `sparse_bitarray`: Main type for efficiente sparse bit arrays.  Uses compiler intrinsics (or assembler equivalents) enhancements.

Normally clients should be using just the `bitarray` or `sparse_bitarray` types. Watched bit arrays have proven useful in some combinatorial problems. One such example may be found [here](http://download.springer.com/static/pdf/797/chp%253A10.1007%252F978-3-319-09584-4_12.pdf?auth66=1411550130_ba322f209d8b171722fa67741d3f77e9&ext=.pdf "watched bit arrays"). 

The library features a number of optimizations for typical enumeration of set members (in this context, *bitscanning*) which have been found useful for fast implementation of algorithms during research, such as  destructive vs non-destructive scans, forward and reverse_forward scans, random starting points, caching indexes etc. A comparison with other well known implementations (such as BOOST or STL bit strings) can be found [here](http://blog.biicode.com/bitscan-efficiency-at-glance/ "BITSCAN efficiency at a glance ").

GETTING STARTED
-------------------------------
To include the BITSCAN library in your project just add the dependency to the *bitscan.h file*. Here is a simple example which defines an efficient bitstring for bitscanning with a population size of 100 and sets the eleventh element to 1. 

    #include "pablodev/bitscan/bitscan.h"
	//...
    int main(){
    	bitarray bbi(100);
		bbi.set_bit(10);
		cout<<bbi;
    }
 You will find many more examples in the *tests* folder of the block.  

EXAMPLES OF BITSCANNING OPERATIONS
-------------------------------

The BITSCAN library features specific optmization techniques for *destructive* and *non-destructive* scans. In a destructive bit scan (DBS), whenever a 1-bit is read, *it is removed from the bit string* (at the end of the process the bit string is empty). In the (conventional) non-destructive case (NDBS), the bit string information remains unchanged. 
Efficient DBS loops are easier to control than NDBS ones because LSB / MSB operations always work. In contrast, efficient NDBS scanning loops should cache information of the last bit read to find the next 1-bit faster.


Here is an example of fast bit scan loop syntax in BITSCAN:

    bitarray bba(100);						//set with population size of 100 elements
	int nBit=EMPTY_ELEM;
	//...

    bba.init_scan(bbo::NON_DESTRUCTIVE)
	while(true){
		nBit=bba.next_bit();
		if(nBit==EMPTY_ELEM) break;

		//int foo=nBit...
	}

The *init_scan* member function configures cache information appropiately. Currently four types of scans are supported: DESTRUCTIVE, NON\_DESTRUCTIVE and the reverse counterparts (DESTRUCTIVE\_REVERSE and NON\_DESTRUCTIVE\_REVERSE). For each type there is an appropiate family of functions. Here is another example, this time for sparse bit arrays:


    sparse_bitarray bba(10000);
	int nBit=EMPTY_ELEM;
	//...

    if(bba.init_scan(bbo::DESTRUCTIVE)!=EMPTY_ELEM){
	  while(true){
		 nBit=bbi.next_bit_del();
		 if(nBit==EMPTY_ELEM) break;
		 
		//int foo=nBit ...
	  }
	}

Note that in this case the *if* clause is necessary because sparse bit strings have empty semantics. Morever the loop funcion now differs from the previous case because it deletes each population member from the set once it is found (DESTRUCTIVE type).

The interface for other bit string types is the same.


CONFIGURATION PARAMETERS
-------------------------

The file *config.h* contains customization parameters. Critical issues are:


1. POPCOUNT_64: Disable in processor architectures which do not support intrinsic popcount64 assembly instruction. 
2. DE\_BRUIJN: When enabled uses De Bruijn hashing for fast bit scanning.
3. CACHED\_INDEX\_OPERATIONS: When enabled, uses additional memory to cache bitboard indexes for fast bitscanning.  The default cache size is a population size of 15001 (i.e. MAX\_CACHED\_INDEX=15001). Disable for bitarrays with population greater than 15000.

Acknowledgements
-------------------------
This research has been partially funded by the Spanish Ministry of Economy and Competitiveness (MINECO), national grant DPI 2010-21247-C02-01.