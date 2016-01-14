/*  
 * bbintrinsic.h file from the BITSCAN library, a C++ library for bit set
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
#include <vector>			//I/O

using namespace std;


/////////////////////////////////
//
// Class BBIntrin
// (Uses a number of optimizations for bit scanning)
//
///////////////////////////////////

class BBIntrin: public BitBoardN{
public:	

	struct scan_t{																			//For bitscanning optimization
		scan_t():bbi(EMPTY_ELEM), pos(MASK_LIM){}
		int bbi;	//bitboard index 	
		int pos;	//bit position for bitscan		
	};
		
	 BBIntrin						(){};										
explicit  BBIntrin				(int popsize /*1 based*/, bool reset=true):BitBoardN(popsize,reset){}	
	 BBIntrin						(const BBIntrin& bbN):BitBoardN(bbN){}
	 BBIntrin						(const std::vector<int>& v): BitBoardN(v){}
virtual ~BBIntrin					(){}

	 void set_bbindex				(int bbindex){m_scan.bbi=bbindex;}	
	 void set_posbit				(int posbit){m_scan.pos=posbit;}	
 
	
//////////////////////////////
// bitscanning


inline virtual int lsbn64			() const;
inline virtual int msbn64			() const; 


	//bit scan forward (destructive)
virtual	int init_scan					(scan_types);	
virtual	int init_scan_from			(int from, scan_types);

virtual inline int next_bit_del		(); 												
virtual inline int next_bit_del		(int& nBB /* table index*/); 
virtual inline int next_bit_del		(int& nBB,  BBIntrin& bbN_del); 	 


	//bit scan forward (non destructive)
virtual inline int next_bit			();

	//bit scan backwards (non destructive)
virtual inline int previous_bit		(); 

	//bit scan backwards (destructive)
 virtual inline int previous_bit_del		(); 


virtual inline int next_bit			(int &); 
virtual	inline int next_bit			(int &,  BBIntrin& ); 


inline int previous_bit_del				(int& nBB);
inline int previous_bit_del				(int& nBB,  BBIntrin& del ); 

/////////////////
// Popcount
#ifdef POPCOUNT_64
virtual	 inline int popcn64				()						const;
virtual	 inline int popcn64				(int nBit/*0 based*/)	const;
#endif

//////////////////
/// data members
	 scan_t m_scan;
};

///////////////////////
//
// INLINE FUNCTIONS
// 
////////////////////////
#ifdef POPCOUNT_64
inline int BBIntrin::popcn64() const{
	BITBOARD pc=0;
	for(int i=0; i<m_nBB; i++){
		pc+=__popcnt64(m_aBB[i]);
	}
return pc;
}


inline int BBIntrin::popcn64(int nBit) const{
/////////////////////////
// Population size from nBit(included) onwards
	
	BITBOARD pc=0;
	
	int nBB=WDIV(nBit);
	for(int i=nBB+1; i<m_nBB; i++){
		pc+=__popcnt64(m_aBB[i]);
	}

	//special case of nBit bit block
	BITBOARD bb=m_aBB[nBB]&~Tables::mask_right[WMOD(nBit)];
	pc+=__popcnt64(bb);

return pc;
}

#endif

inline int BBIntrin::next_bit(int &nBB_new)  {
////////////////////////////
// Date:23/3/2012
// BitScan not destructive
// Version that use static data of the last bit position and the last table
//
// COMMENTS
// 1-Require previous assignment BitBoardN::scan=0 and BBIntrin::scanv=MASK_LIM

	unsigned long posInBB;
			
	//Search int the last table
	if(_BitScanForward64(&posInBB, m_aBB[m_scan.bbi] & Tables::mask_left[m_scan.pos])){
		m_scan.pos =posInBB;
		nBB_new=m_scan.bbi;
		return (posInBB + WMUL(m_scan.bbi));
	}else{											//Not found in the last table. Search in the rest
		for(int i=m_scan.bbi+1; i<m_nBB; i++){
			if(_BitScanForward64(&posInBB,m_aBB[i])){
				m_scan.bbi=i;
				m_scan.pos=posInBB;
				nBB_new=i;
				return (posInBB+ WMUL(i));
			}
		}
	}
return EMPTY_ELEM;
}

inline int BBIntrin::next_bit(int &nBB_new,  BBIntrin& bbN_del ) {
////////////////////////////
// Date:30/3/2012
// BitScan not destructive
// Version that use static data of the last bit position and the last table
//
// COMMENTS
// 1-Require previous assignment BitBoardN::scan=0 and BitBoardN::scanv=MASK_LIM

	unsigned long posInBB;
			
	//Search int the last table
	if(_BitScanForward64(&posInBB, m_aBB[m_scan.bbi] & Tables::mask_left[m_scan.pos])){
		m_scan.pos =posInBB;
		nBB_new=m_scan.bbi;
		bbN_del.m_aBB[m_scan.bbi]&=~Tables::mask[posInBB];
		return (posInBB + WMUL(m_scan.bbi));
	}else{											//Not found in the last table. Search in the rest
		for(int i=m_scan.bbi+1; i<m_nBB; i++){
			if(_BitScanForward64(&posInBB,m_aBB[i])){
				m_scan.bbi=i;
				m_scan.pos=posInBB;
				nBB_new=i;
				bbN_del.m_aBB[i]&=~Tables::mask[posInBB];
				return (posInBB+ WMUL(i));
			}
		}
	}
return EMPTY_ELEM;
}


inline int BBIntrin::msbn64() const{
////////////////////////////
//
// Date: 30/3/12
// Return the last bit
// 
// COMMENTS: New variable static scan which stores index of every BB

	 unsigned long posInBB;

	for(int i=m_nBB-1; i>=0; i--){
		//Siempre me queda la duda mas de si es mas eficiente comparar con 0
		if(_BitScanReverse64(&posInBB,m_aBB[i]))
			return (posInBB+WMUL(i));
	}
	
return EMPTY_ELEM;  
}

	
inline int BBIntrin::lsbn64() const{
	unsigned long posBB;
	for(int i=0; i<m_nBB; i++){
		if(_BitScanForward64(&posBB, m_aBB[i]))
			return(posBB+ WMUL(i));	
	}
return EMPTY_ELEM;
}



inline int BBIntrin::next_bit_del() {
////////////////////////////
//
// Date: 23/3/12
// BitScan with Delete (D- erase the bit scanned) and Intrinsic
// 
// COMMENTS: New variable static scan which stores index of every BB

	 unsigned long posInBB;

	for(int i=m_scan.bbi; i<m_nBB; i++)	{
		if(_BitScanForward64(&posInBB,m_aBB[i])){
			m_scan.bbi=i;
			m_aBB[i]&=~Tables::mask[posInBB];			//Deletes the current bit before returning
			return (posInBB+WMUL(i));
		}
	}
	
return EMPTY_ELEM;  
}


inline int BBIntrin::next_bit_del(int& nBB) {
//////////////
// Also return the number of table
	unsigned long posInBB;

	for(int i=m_scan.bbi; i<m_nBB; i++)	{
		if(_BitScanForward64(&posInBB,m_aBB[i])){
			m_scan.bbi=i;
			m_aBB[i]&=~Tables::mask[posInBB];			//Deleting before the return
			nBB=i;
			return (posInBB+WMUL(i));
		}
	}
	
return EMPTY_ELEM;  
}

inline int BBIntrin::next_bit_del(int& nBB, BBIntrin& bbN_del) {
//////////////
// BitScan DI it also erase the returned bit of the table passed
	unsigned long posInBB;

	for(int i=m_scan.bbi; i<m_nBB; i++){
		if(_BitScanForward64(&posInBB,m_aBB[i])){
			m_scan.bbi=i;
			m_aBB[i]&=~Tables::mask[posInBB];					//Deleting before the return
			bbN_del.m_aBB[i]&=~Tables::mask[posInBB];
			nBB=i;
			return (posInBB+WMUL(i));
		}
	}
	
return EMPTY_ELEM;  
}

inline int BBIntrin::next_bit() {
////////////////////////////
// Date:23/3/2012
// BitScan not destructive
// Version that use static data of the last bit position and the last table
//
// COMMENTS
// 1-Require previous assignment m_scan_bbl=0 and m_scan.pos=MASK_LIM

	unsigned long posInBB;
			
	//Search for next bit in the last block
	if(_BitScanForward64(&posInBB, m_aBB[m_scan.bbi] & Tables::mask_left[m_scan.pos])){
		m_scan.pos =posInBB;
		return (posInBB + WMUL(m_scan.bbi));
	}else{											//Search in the remaining blocks
		for(int i=m_scan.bbi+1; i<m_nBB; i++){
			if(_BitScanForward64(&posInBB,m_aBB[i])){
				m_scan.bbi=i;
				m_scan.pos=posInBB;
				return (posInBB+ WMUL(i));
			}
		}
	}
	
return EMPTY_ELEM;
}


inline int BBIntrin::previous_bit		() {
////////////////////////////
// Date:13/4/2012
// BitScan not destructive in reverse order (end-->begin)
//
// COMMENTS
// 1-Require previous assignment BitBoardN::scan=0 and BitBoardN::scanv=MASK_LIM

	unsigned long posInBB;
				
	//Search int the last table
	if(_BitScanReverse64(&posInBB, m_aBB[m_scan.bbi] & Tables::mask_right[m_scan.pos])){
		m_scan.pos =posInBB;
		return (posInBB + WMUL(m_scan.bbi));
	}else{											//Not found in the last table. Search in the rest
		for(int i=m_scan.bbi-1; i>=0; i--){
			if(_BitScanReverse64(&posInBB,m_aBB[i])){
				m_scan.bbi=i;
				m_scan.pos=posInBB;
				return (posInBB+ WMUL(i));
			}
		}
	}
	
return EMPTY_ELEM;
}

inline int BBIntrin::previous_bit_del() {
//////////////
// BitScan DI 
	unsigned long posInBB;

	for(int i=m_scan.bbi; i>=0; i--){
		if(_BitScanReverse64(&posInBB,m_aBB[i])){
			m_scan.bbi=i;
			m_aBB[i]&=~Tables::mask[posInBB];			//Deleting before the return
			return (posInBB+WMUL(i));
		}
	}
return EMPTY_ELEM;  
}

inline int BBIntrin::previous_bit_del(int& nBB, BBIntrin& del) {
//////////////
// BitScan DI it also erase the returned bit of the table passed
	unsigned long posInBB;

	for(int i=m_scan.bbi; i>=0; i--){
		if(_BitScanReverse64(&posInBB,m_aBB[i])){
			m_scan.bbi=i;
			m_aBB[i]&=~Tables::mask[posInBB];			//Deleting before the return
			del.m_aBB[i]&=~Tables::mask[posInBB];
			nBB=i;
			return (posInBB+WMUL(i));
		}
	}
	
return EMPTY_ELEM;  
}

inline int BBIntrin::previous_bit_del(int& nBB) {
//////////////
// BitScan DI 
	unsigned long posInBB;

	for(int i=m_scan.bbi; i>=0; i--){
		if(_BitScanReverse64(&posInBB,m_aBB[i])){
			m_scan.bbi=i;
			m_aBB[i]&=~Tables::mask[posInBB];			//Deleting before the return
			nBB=i;
			return (posInBB+WMUL(i));
		}
	}
	
return EMPTY_ELEM;  
}

inline
int BBIntrin::init_scan(scan_types sct){
	switch(sct){
	case NON_DESTRUCTIVE:
		set_bbindex(0);
		set_posbit(MASK_LIM);
		break;
	case NON_DESTRUCTIVE_REVERSE:
		set_bbindex(m_nBB-1);
		set_posbit(WORD_SIZE);		//mask_right[WORD_SIZE]=ONE
		break;
	case DESTRUCTIVE:
		set_bbindex(0); 
		break;
	case DESTRUCTIVE_REVERSE:
		set_bbindex(m_nBB-1);
		break;
	default:
		cerr<<"bad scan type"<<endl;
		return -1;
	}
return 0;
}

inline
int BBIntrin::init_scan_from (int from, scan_types sct){
	switch(sct){
	case NON_DESTRUCTIVE:
	case NON_DESTRUCTIVE_REVERSE:
		set_bbindex(WDIV(from));
		set_posbit(WMOD(from));
		break;
	case DESTRUCTIVE:
	case DESTRUCTIVE_REVERSE:
		set_bbindex(WDIV(from)); 
		break;
	default:
		cerr<<"bad scan type"<<endl;
		return -1;
	}
return 0;
}












