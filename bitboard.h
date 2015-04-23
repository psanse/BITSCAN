/*  
 * bitboard.h file from the BITSCAN library, a C++ library for bit set
 * optimization. BITSCAN has been used to implement BBMC, a very
 * succesful bit-parallel algorithm for exact maximum clique. 
 * (see license file for references)
 *
 * Copyright (C)
 * Author: Pablo San Segundo
 * Intelligent Control Research Group (CSIC-UPM) 
 *
 * Permission to use, modify and distribute this software is
 * granted provided that this copyright notice appears in all 
 * copies, in source code or in binaries. For precise terms 
 * see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any 
 * kind, express or implied, and with no claim as to its
 * suitability for any purpose.
 *
 */

#ifndef __BITBOARD_H__
#define __BITBOARD_H__

#include "tables.h"
#ifdef __GNUC__

#ifdef __MINGW64__
	#define __popcnt64 __builtin_popcountll
	#include <intrin.h>										//windows specific
#else
	#define __popcnt64 __builtin_popcountll
	#include <x86intrin.h>										//linux specific
	 static inline unsigned char _BitScanForward64(unsigned long* Index,  unsigned long long  Mask)
		{
			unsigned long long  Ret;
			__asm__
			(
				"bsfq %[Mask], %[Ret]"
				:[Ret] "=r" (Ret)
				:[Mask] "mr" (Mask)
			);
			*Index = (unsigned long)Ret;
			return Mask?1:0;
		}
		static inline unsigned char _BitScanReverse64(unsigned long* Index,  unsigned long long  Mask)
		{
			 unsigned long long  Ret;
			__asm__
			(
				"bsrq %[Mask], %[Ret]"
				:[Ret] "=r" (Ret)
				:[Mask] "mr" (Mask)
			);
			*Index = (unsigned long)Ret;
			return Mask?1:0;
		}
#endif
#else
#include <intrin.h>										//windows specific
#endif

#include <iostream>



#define DEBRUIJN_MN_64_ISOL		0x07EDD5E59A4E28C2
#define DEBRUIJN_MN_64_SEP		0x03f79d71b4cb0a89
#define DEBRUIJN_MN_64_SHIFT	58

/////////////////////////////////
//
// BITBOARD CLASS
// (contains different implementations of msb and lsb and popcount functions to test HW)
///////////////////////////////////

class BitBoard {
	
private:							//hidden constructor 
	BitBoard(){};

public:
/////////////////////
// BitScanning
		
	static int  lsb64_mod				(const BITBOARD);			//modulus of magic number perfect hashing	
    static int  lsb64_lup				(const BITBOARD);			//lookup implementations
	static int  lsb64_lup_1				(const BITBOARD);
	static int	msb64_lup				(const BITBOARD);
	static int  lsb64_pc				(const BITBOARD);			//popcount based implementation (2008)
	
	//ref
inline	static int  lsb64_de_Bruijn	(const BITBOARD bb_dato);		//De Bruijn magic word (2008)
inline	static int  msb64_de_Bruijn	(const BITBOARD bb_dato);		//De Bruijn magic word 
inline static  int lsb64_intrinsic	(const BITBOARD bb_dato);		//intrinsics	
inline static  int msb64_intrinsic	(const BITBOARD bb_dato);		//intrinsics	
		
/////////////////////
// Bit population
		
		static int popc64_lup_1		(const BITBOARD);	
inline	static int popc64_lup		(const BITBOARD);				//Lookup
inline  static int popc64			(const BITBOARD);				//Recommended default popcount which uses intrinsics if POPCOUNT_64 switch is ON (config.h)

//////////////////////
//  Masks
inline static BITBOARD MASK_1		(int low, int high);		//1-bit mask in the CLOSED range
inline static BITBOARD MASK_0		(int low, int high);		//0-bit mask in the CLOSED range

/////////////////////
// I/O
	static void print(const BITBOARD, std::ostream&  = std::cout);
};


/////////////////////////
//
// INLINE FUNCTIONS
//
/////////////////////////

 inline int BitBoard::popc64_lup (	const BITBOARD bb_dato){
//////////////////////////////
// Lookup implementation 
//
// COMMENTS
// more compact than lookup without union but similar in time
	register union x {
		U16 c[4];
		BITBOARD b;
	} val;				

	val.b = bb_dato; //Carga unisn

return (Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]]); //Suma de poblaciones 
}


inline int BitBoard::popc64(const BITBOARD bb_dato){
///////////////////////////////
// Default implementation for population count. Uses intrinsics if  POPCOUNT_64 switch is ON (config.h)

#ifdef POPCOUNT_64
	return __popcnt64(bb_dato);
#else
	//lookup table popcount
	register union x {
		U16 c[4];
		BITBOARD b;
	} val;				

	val.b = bb_dato; //Carga unisn

	return (Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]]); //Suma de poblaciones  
#endif
}


inline int BitBoard:: lsb64_de_Bruijn(const BITBOARD bb_dato){
////////////////////////
// Uses de Bruijn perfect hashing in two versions:
// a) ISOLANI_LSB with hashing bb &(-bb)
// b) All 1 bits to LSB with hashing bb^(bb-1)
// 
//	Option b) would seem to exploit the CPU HW better on average and is defined as default
//  To change this option go to config.h file


#ifdef ISOLANI_LSB
	return (bb_dato==0)? EMPTY_ELEM : Tables::indexDeBruijn64_ISOL[((bb_dato & -bb_dato) * DEBRUIJN_MN_64_ISOL) >> DEBRUIJN_MN_64_SHIFT];
#else
	return (bb_dato==0)? EMPTY_ELEM : Tables::indexDeBruijn64_SEP[((bb_dato ^ (bb_dato-1)) * DEBRUIJN_MN_64_SEP) >> DEBRUIJN_MN_64_SHIFT];
#endif
}

inline	int BitBoard::msb64_de_Bruijn (const BITBOARD bb_dato){
////////////////////////
// date: 27/7/2014
// Uses de Bruijn perfect hashing but first has to create 1-bits from the least significant to MSB
//
// More practical than LUPs of 65535 entries, but requires efficiency tests for a precise comparison

	if(bb_dato==0) return EMPTY_ELEM;

   //creates all 1s up to MSB position
   BITBOARD bb=bb_dato;
   bb |= bb >> 1; 
   bb |= bb >> 2;
   bb |= bb >> 4;
   bb |= bb >> 8;
   bb |= bb >> 16;
   bb |= bb >> 32;

   //applys same computation as for LSB-de Bruijn
   return Tables::indexDeBruijn64_SEP[(bb*DEBRUIJN_MN_64_SEP)>>DEBRUIJN_MN_64_SHIFT];
}

inline int BitBoard::lsb64_intrinsic( const BITBOARD bb_dato){
	unsigned long index;
	if(_BitScanForward64(&index,bb_dato))
		return(index);
	
return EMPTY_ELEM;
}

inline  int BitBoard::msb64_intrinsic (const BITBOARD bb_dato){
	unsigned long index;
	if(_BitScanReverse64(&index,bb_dato))
		return(index);
	
return EMPTY_ELEM;
	
}


inline BITBOARD BitBoard::MASK_1(int low, int high){
//////////////
// returns 1-bit mask (remaining bits to 0) in the CLOSED range (high and low must be numbers between 0 and 63 and low<=high)

	return ~Tables::mask_right[low] & ~Tables::mask_left[high];
}

inline BITBOARD BitBoard::MASK_0(int low, int high){
//////////////
// returns 0-bit mask (remaining bits to 1) in the CLOSED range (high and low must be numbers between 0 and 63 and low<=high)

	return Tables::mask_right[low] | Tables::mask_left[high];
}

#endif
