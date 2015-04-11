/*  
 * bbintrinsic_sparse.h file from the BITSCAN library, a C++ library
 * for bit set optimization. BITSCAN has been used to implement BBMC,
 * a very succesful bit-parallel algorithm for exact maximum clique. 
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

#include "bitboards.h"
using namespace std;

/////////////////////////////////
//
// Class BBIntrinS
// (Uses a number of optimizations for bit scanning)
//
///////////////////////////////////

class BBIntrinS: public BitBoardS{
public:	
				
	 BBIntrinS						(){};										
explicit BBIntrinS					(int popsize /*1 based*/, bool reset=true):BitBoardS(popsize,reset){}	
	 BBIntrinS						(const BBIntrinS& bbN):BitBoardS(bbN){}

	 void set_bbindex				(int bbindex){m_scan.bbi=bbindex;}			//refers to the position in the collection (not in the bitstring)
	 void set_posbit				(int posbit){m_scan.pos=posbit;}	
	
//////////////////////////////
// bitscanning
inline int lsbn64					() const;
inline int msbn64					() const; 
inline	int msbn64					(int& nElem)			const;				
inline	int lsbn64					(int& nElem)			const; 	

	//scan setup
	int init_scan					(scan_types);
	int init_scan_from				(int from, scan_types sct);				//currently only for the NON-DESTRUCTIVE case
	
	//bit scan forward (destructive)				
inline int next_bit_del				(); 												
inline int next_bit_del				(int& nBB);								//nBB: index of bitblock in the bitstring	(not in the collection)	
inline int next_bit_del_pos			(int& posBB);							//posBB: position of bitblock in the collection (not the index of the element)		

	//bit scan forward (non destructive)
virtual inline int next_bit			();
inline int next_bit					(int & nBB);							//nBB: index of bitblock in the bitstring	(not in the collection)				

	//bit scan backwards (non destructive)
virtual inline int previous_bit		(); 

	//bit scan backwards (destructive)
inline int previous_bit_del			(); 
inline int previous_bit_del			(int& nBB);
				
/////////////////
// Popcount
#ifdef POPCOUNT_64
virtual	 inline int popcn64				()	const;
virtual	 inline int popcn64				(int nBit)	const;							//population size from (and including) nBit
#endif

//////////////////
/// data members
public:
	struct scan_t{																			//Cache memory for bitscanning optimization
		scan_t():bbi(EMPTY_ELEM), pos(MASK_LIM){}
		int bbi;	//bitboard index in the collection (not in the bitstring) 	
		int pos;	//bit position for bitscan		
	};

	 scan_t m_scan;
};

///////////////////////
//
// INLINE FUNCTIONS
// 
////////////////////////

#ifdef POPCOUNT_64
inline 
int BBIntrinS::popcn64() const{
	BITBOARD pc=0;
	for(int i=0; i<m_aBB.size(); i++){
		pc+=__popcnt64(m_aBB[i].bb);
	}
return pc;
}


inline
int BBIntrinS::popcn64(int nBit) const{
//////////////////////////////
// population  size from (and including) nBit onwards
	BITBOARD pc=0;
	int nBB=WDIV(nBit);

	//find the biblock if it exists
	velem_cit it=lower_bound(m_aBB.begin(), m_aBB.end(), elem(nBB), elem_less());
	if(it!=m_aBB.end()){
		if(it->index==nBB){
			BITBOARD bb= it->bb&~Tables::mask_right[WMOD(nBit)];
			pc+= __popcnt64(bb);
			it++;
		}

		//searches in the rest of elements with greater index than nBB
		for(; it!=m_aBB.end(); ++it){
			pc+=  __popcnt64(it->bb);
		}
	}
	
return pc;
}

#endif

inline int BBIntrinS::lsbn64() const{
	unsigned long posbb;
	for(int i=0; i<m_aBB.size(); i++){
		if(_BitScanForward64(&posbb, m_aBB[i].bb))
			return(posbb+ WMUL(m_aBB[i].index));	
	}
return EMPTY_ELEM;
}

inline int BBIntrinS::lsbn64(int& nElem) const{
////////////////////
// lsbn + returns in nElem the index in the collection if the bit string is not EMPTY

	unsigned long posbb;
	for(int i=0; i<m_aBB.size(); i++){
		if(_BitScanForward64(&posbb, m_aBB[i].bb)){
			nElem=i;
			return(posbb+ WMUL(m_aBB[i].index));
		}
	}
return EMPTY_ELEM;
}


inline int BBIntrinS::msbn64() const{
////////////////////
// Returns the index in the collection if bitstring is not EMPTY
	unsigned long posBB;
	for(int i=m_aBB.size()-1; i>=0; i--){
		//Siempre me queda la duda mas de si es mas eficiente comparar con 0
		if(_BitScanReverse64(&posBB,m_aBB[i].bb))
				return (posBB+WMUL(m_aBB[i].index));
	}
	
return EMPTY_ELEM;  
}

inline int BBIntrinS::msbn64(int& nElem) const{
////////////////////
// Returns the index in the collection if bitstring is not EMPTY
	unsigned long posBB;
	for(int i=m_aBB.size()-1; i>=0; i--){
		//Siempre me queda la duda mas de si es mas eficiente comparar con 0
		if(_BitScanReverse64(&posBB,m_aBB[i].bb)){
			nElem=i;
			return (posBB+WMUL(m_aBB[i].index));
		}
	}
	
return EMPTY_ELEM;  
}


inline int BBIntrinS::next_bit() {
////////////////////////////
// date:5/9/2014
// non destructive bitscan for sparse bitstrings using intrinsics
// caches index in the collection and pos inside the bitblock
//
// comments
// 1-require previous assignment m_scan_bbi=0 and m_scan.pos=mask_lim

	unsigned long posbb;
			
	//search for next bit in the last block
	if(_BitScanForward64(&posbb, m_aBB[m_scan.bbi].bb & Tables::mask_left[m_scan.pos])){
		m_scan.pos =posbb;
		return (posbb + WMUL(m_aBB[m_scan.bbi].index));
	}else{											//search in the remaining blocks
		for(int i=m_scan.bbi+1; i<m_aBB.size(); i++){
			if(_BitScanForward64(&posbb,m_aBB[i].bb)){
				m_scan.bbi=i;
				m_scan.pos=posbb;
				return (posbb+ WMUL(m_aBB[i].index));
			}
		}
	}
	
return EMPTY_ELEM;
}


inline int BBIntrinS::next_bit(int& block_index) {
////////////////////////////
// date:5/9/2014
// non destructive bitscan for sparse bitstrings using intrinsics
// caches index in the collection and pos inside the bitblock
//
// comments
// 1-require previous assignment m_scan_bbi=0 and m_scan.pos=mask_lim

	unsigned long posbb;
			
	//search for next bit in the last block
	if(_BitScanForward64(&posbb, m_aBB[m_scan.bbi].bb & Tables::mask_left[m_scan.pos])){
		m_scan.pos =posbb;
		block_index= m_aBB[m_scan.bbi].index;
		return (posbb + WMUL(m_aBB[m_scan.bbi].index));
	}else{											//search in the remaining blocks
		for(int i=m_scan.bbi+1; i<m_aBB.size(); i++){
			if(_BitScanForward64(&posbb,m_aBB[i].bb)){
				m_scan.bbi=i;
				m_scan.pos=posbb;
				block_index=m_aBB[i].index;
				return (posbb+ WMUL(m_aBB[i].index));
			}
		}
	}
	
return EMPTY_ELEM;
}


inline int BBIntrinS::previous_bit	() {
////////////////////////////
// date:5/9/2014
// Non destructive bitscan for sparse bitstrings using intrinsics
// caches index in the collection and pos inside the bitblock
//
// comments
// 1-require previous assignment m_scan_bbi=number of bitblocks-1 and m_scan.pos=WORD_SIZE

	unsigned long posbb;
				
	//search int the last table
	if(_BitScanReverse64(&posbb, m_aBB[m_scan.bbi].bb & Tables::mask_right[m_scan.pos])){
		m_scan.pos =posbb;
		return (posbb + WMUL(m_aBB[m_scan.bbi].index));
	}else{											//not found in the last table. search in the rest
		for(int i=m_scan.bbi-1; i>=0; i--){
			if(_BitScanReverse64(&posbb,m_aBB[i].bb)){
				m_scan.bbi=i;
				m_scan.pos=posbb;
				return (posbb+ WMUL(m_aBB[i].index));
			}
		}
	}
	
return EMPTY_ELEM;
}


inline int BBIntrinS::next_bit_del() {
////////////////////////////
//
// date: 23/3/12
// Destructive bitscan (scans and deletes each 1-bit) for sparse bitstrings using intrinsics
//
// COMMENTS
// 1-Requires previous assignment m_scan_bbi=0 

	unsigned long posbb;

	for(int i=m_scan.bbi; i<m_aBB.size(); i++)	{
		if(_BitScanForward64(&posbb,m_aBB[i].bb)){
			m_scan.bbi=i;
			m_aBB[i].bb&=~Tables::mask[posbb];			//deleting before the return
			return (posbb + WMUL(m_aBB[i].index));
		}
	}
	
return EMPTY_ELEM;  
}

inline int BBIntrinS::next_bit_del(int& block_index) {
////////////////////////////
//
// date: 23/3/12
// Destructive bitscan for sparse bitstrings using intrinsics
//
// COMMENTS
// 1-Requires previous assignment m_scan_bbi=0 

	unsigned long posbb;

	for(int i=m_scan.bbi; i<m_aBB.size(); i++)	{
		if(_BitScanForward64(&posbb,m_aBB[i].bb)){
			m_scan.bbi=i;
			block_index=m_aBB[i].index;
			m_aBB[i].bb&=~Tables::mask[posbb];			//deleting before the return
			return (posbb + WMUL(m_aBB[i].index));
		}
	}
	
return EMPTY_ELEM;  
}

inline int BBIntrinS::next_bit_del_pos (int& posBB){
////////////////////////////
//
// date: 29/10/14
// Destructive bitscan which returns the position of the bitblock scanned in the collection
// (not the index attribute)
//
// COMMENTS
// 1-Requires previous assignment m_scan_bbi=0 

	unsigned long posbb;

	for(int i=m_scan.bbi; i<m_aBB.size(); i++)	{
		if(_BitScanForward64(&posbb,m_aBB[i].bb)){
			posBB=m_scan.bbi=i;
			m_aBB[i].bb&=~Tables::mask[posbb];			//deleting before the return
			return (posbb + WMUL(m_aBB[i].index));
		}
	}
	
return EMPTY_ELEM;  
}


inline int BBIntrinS::previous_bit_del() {
////////////////////////////
//
// date: 23/3/12
// Destructive bitscan for sparse bitstrings using intrinsics
//
// COMMENTS
// 1-Requires previous assignment m_scan_bbi=number of bitblocks-1

	unsigned long posbb;

	for(int i=m_scan.bbi; i>=0; i--){
		if(_BitScanReverse64(&posbb,m_aBB[i].bb)){
			m_scan.bbi=i;
			m_aBB[i].bb&=~Tables::mask[posbb];			//deleting before the return
			return (posbb+WMUL(m_aBB[i].index));
		}
	}
	
return EMPTY_ELEM;  
}

inline int BBIntrinS::previous_bit_del(int & bb_index) {
////////////////////////////
//
// date: 23/3/12
// Destructive bitscan for sparse bitstrings using intrinsics
//
// COMMENTS
// 1-Requires previous assignment m_scan_bbi=number of bitblocks-1

	unsigned long posbb;

	for(int i=m_scan.bbi; i>=0; i--){
		if(_BitScanReverse64(&posbb,m_aBB[i].bb)){
			m_scan.bbi=i;
			bb_index=m_aBB[i].index;
			m_aBB[i].bb&=~Tables::mask[posbb];			//deleting before the return
			return (posbb+WMUL(m_aBB[i].index));
		}
	}
return EMPTY_ELEM;  
}

inline
int BBIntrinS::init_scan (scan_types sct){
	if(m_aBB.empty()) return EMPTY_ELEM;				//necessary check since sparse bitstrings have empty semantics (i.e. sparse graphs)

	switch(sct){
	case NON_DESTRUCTIVE:
		set_bbindex(0);
		set_posbit(MASK_LIM);
		break;
	case NON_DESTRUCTIVE_REVERSE:
		set_bbindex(m_aBB.size()-1);
		set_posbit(WORD_SIZE);
		break;
	case DESTRUCTIVE:
		set_bbindex(0); 
		break;
	case DESTRUCTIVE_REVERSE:
		set_bbindex(m_aBB.size()-1);
		break;
	default:
		cerr<<"bad scan type"<<endl;
		return -1;
	}
return 0;
}

inline
int BBIntrinS::init_scan_from (int from, scan_types sct){
////////////////////////
// scans starting at from until the end of the bitarray
//
// REMARKS: at the moment, only working for the NON-DESTRUCTIVE case

	pair<bool, int> p= find_pos(WDIV(from));
	if(p.second==EMPTY_ELEM) return EMPTY_ELEM;
	switch(sct){
	case NON_DESTRUCTIVE:
	case NON_DESTRUCTIVE_REVERSE:
		set_bbindex(p.second); 
		(p.first)? set_posbit(WMOD(from)) : set_posbit(MASK_LIM);
		break;
	/*case DESTRUCTIVE:
	case DESTRUCTIVE_REVERSE:
		set_bbindex(p.second);
		break;*/
	default:
		cerr<<"bad scan type"<<endl;
		return -1;
	}
return 0;
}

