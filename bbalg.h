/*  
 * bbalg.h file from the BITSCAN library, a C++ library for bit set
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

#pragma once
#include "bitboardn.h"

//#define MIN(a, b)	((a)<(b))? (a):(b)
//#define MAX(a, b)	((a)>(b))? (a):(b)

inline bool UniformBoolean(double p){
/////////////
//returns 1 with prob p, 0 with 1-p
	double n_01=rand()/(double)RAND_MAX;
	return (n_01<=p);
}

//inline bool similar	(const BitBoardN& bb1 , const BitBoardN& bb2 , int nBitDiff){
////////////////////
//// TRUE if bitset_symmetric_difference is less or equal than nBitDiff
//	int pc=0;
//	for(int i=0; i<bb1.m_nBB; i++){
//		pc+=BitBoard::popc64(bb1.m_aBB[i]^bb2.m_aBB[i]);
//		if(pc>nBitDiff) 
//			return false;
//	}
//return true;
//}
//
//inline bool disjoint (const BitBoardN& bb1, const BitBoardN& bb2){
//////////////////////////////////
//// TRUE if bitset_intersection is empty 
//	for(int i=0; i<bb1.m_nBB; i++)
//			if(bb1.m_aBB[i] & bb2.m_aBB[i]) return false;
//return true;		
//}
//
//inline bool subsumes (const BitBoardN& bb1, const BitBoardN& bb2){
////////////////////
//// TRUE if bitset_symmetric_difference is in bb1 
//// bb2 empty is always subsumed
//	
//	for(int i=0; i<bb2.m_nBB; i++)
//		if( bb2.m_aBB[i] & ~bb1.m_aBB[i]) return false;
//
//return true;
//}


inline std::vector<int> to_vector(const BitBoardN& bbn){
	vector<int> res;

	int v=EMPTY_ELEM;
	while(1){
		if((v=bbn.next_bit(v))==EMPTY_ELEM)
			break;
		res.push_back(v);
	}
return res;
}

inline BITBOARD gen_random_bitboard(double p){
//generates a random BITBOARD with density p of 1-bits
	BITBOARD bb=0;
	for(int i=0; i<WORD_SIZE; i++){
		if(UniformBoolean(p)){
			bb|=Tables::mask[i];
		}
	}
return bb;
}


//inline BITBOARD get_first_k_bits(BITBOARD bb,  BYTE k /*1-64*/){
////////////////////////////////////
//// Returns BITBOARD of first k bits to 1 or 0 if k-bits to 1 could not be found
//
//	BITBOARD bb_aux, res=0;
//	int cont=0;
//
//	//control
//	if(k<1 || k>64) return 0;
//
//	while(bb){
//		bb_aux= bb & (-bb);	/*00..010..0*/
//		res |= bb_aux;
//		if(++cont==k) break;
//	
//	bb ^= bb_aux;
//	}
//
//return (cont==k)? res: 0;
//}





