/*  
 * A file from the BITSCAN library, a C++ library for bit set
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

#ifndef __BB_SENTINEL_H__
#define __BB_SENTINEL_H__

#include "bbintrinsic.h"
#include "bbalg.h"			//MIN, MAX

 using namespace std;

class BBSentinel : public BBIntrin{
	friend BBSentinel&  AND	(const BitBoardN& lhs, const BBSentinel& rhs,  BBSentinel& res);		//updates sentinels
public:
	BBSentinel():m_BBH(EMPTY_ELEM), m_BBL(EMPTY_ELEM){init_sentinels(false);}
explicit BBSentinel(int popsize, bool bits_to_0=true): BBIntrin(popsize, bits_to_0){ init_sentinels(false);}
	BBSentinel(const BBSentinel& bbN) : BBIntrin(bbN){ m_BBH=bbN.m_BBH; m_BBL=bbN.m_BBL;}
	~BBSentinel(){};

////////////
// setters / getters
	void set_sentinel_H(unsigned int i){ m_BBH=i;}
	void set_sentinel_L(unsigned int i){ m_BBL=i;}
	void set_sentinels(int low, int high);
	void init_sentinels(bool update=false);								//sets sentinels to maximum scope of current bit string
	void clear_sentinels();												//sentinels to EMPTY
	int get_sentinel_L(){ return m_BBL;}
	int get_sentinel_H(){ return m_BBH;}
/////////////
// basic sentinel

	int  update_sentinels			();
	int  update_sentinels			(int bbl, int bbh);
	int  update_sentinels_high		();
	int  update_sentinels_low		();

	void update_sentinels_to_v		(int v);	

//////////////
// basic overwritten operations (could be extended)
	
	//erase: will not update sentinels	
virtual	void  erase_bit				();											//in sentinel range
virtual	void  erase_bit				(int nBit) {BitBoardN::erase_bit(nBit);}	//required because of the cast-to-int construction of sentinels (1)
	void  erase_bit_and_update		(int nBit);									//erases and updates sentinels			
	BBSentinel& erase_bit			(const BitBoardN&);							//(1): required for SEQ coloring
	
virtual	bool is_empty				()const;
virtual	bool is_empty				(int nBBL, int nBBH) const;					//is empty in range

#ifdef POPCOUNT_64
	int popcn64					() const;
#endif

////////////////
// operators
	BBSentinel& operator=		(const BBSentinel&);
	BBSentinel& operator&=		(const BitBoardN&);

//////////////
// I/O
	void print(ostream& o=cout);

/////////////////
// bit scanning operations 

virtual	int init_scan(scan_types sct);

virtual inline int previous_bit_del();							//**TODO- empty bitstring
virtual inline	int next_bit_del ();
virtual inline	int next_bit_del (BBSentinel& bbN_del);
	
virtual inline	int next_bit();
virtual inline	int next_bit(int& nBB);

protected:	
	 int m_BBH;										//explicit storage for sentinel high index
	 int m_BBL;										//explicit storage for sentinel low index
};

#endif

#ifdef POPCOUNT_64
inline int BBSentinel::popcn64() const{
	BITBOARD pc=0;
	for(int i=m_BBL; i<=m_BBH; i++){
		pc+=__popcnt64(m_aBB[i]);
	}
return pc;
}

#endif

//specializes the only bitscan function used
inline
int BBSentinel::previous_bit_del(){
//////////////
// BitScan reverse order and distructive
//
// COMMENTS
// 1- update sentinels at the start of loop

	unsigned long posInBB;

	for(int i=m_BBH; i>=m_BBL; i--){
		if(_BitScanReverse64(&posInBB,m_aBB[i])){
			m_BBH=i;
			m_aBB[i]&=~Tables::mask[posInBB];			//erase before the return
			return (posInBB+WMUL(i));
		}
	}
return EMPTY_ELEM;  
}

inline
int BBSentinel::next_bit_del (){
//////////////
// Bitscan distructive between sentinels
//
// COMMENTS
// 1- update sentinels at the start of loop

	unsigned long posInBB;

	for(int i=m_BBL; i<=m_BBH; i++){
		if(_BitScanForward64(&posInBB,m_aBB[i]) ){
			m_BBL=i;
			m_aBB[i]&=~Tables::mask[posInBB];					//erase before the return
			return (posInBB+ WMUL(i));
		}
	}
return EMPTY_ELEM;  
}

inline
int BBSentinel::next_bit_del (BBSentinel& bbN_del){
//////////////
// Bitscan distructive between sentinels
//
// COMMENTS
// 1- update sentinels at the start of loop (experimental, does not use sentinesl of bbN_del)

	unsigned long posInBB;

	for(int i=m_BBL; i<=m_BBH; i++){
		if(_BitScanForward64(&posInBB, m_aBB[i]) ){
			m_BBL=i;
			m_aBB[i]&=~Tables::mask[posInBB];					//erase before the return
			bbN_del.m_aBB[i]&=~Tables::mask[posInBB];
			return (posInBB+ WMUL(i));
		}
	}
	
return EMPTY_ELEM;  
}


inline
int BBSentinel::next_bit(){
////////////////////////////
// last update:31/12/2013
// BitScan non destructive
//
// COMMENTS
// 1- update sentinels, set m_scan.bbi to m_BBL and set m_scan.pos to MASK_LIM at the start of loop

	unsigned long posInBB;
				
	if(_BitScanForward64(&posInBB, m_aBB[m_scan.bbi] & Tables::mask_left[m_scan.pos])){
		m_scan.pos =posInBB;
		return (posInBB + WMUL(m_scan.bbi));
	}else{													
		for(int i=m_scan.bbi+1; i<=m_BBH; i++){
			if(_BitScanForward64(&posInBB,m_aBB[i])){
				m_scan.bbi=i;
				m_scan.pos=posInBB;
				return (posInBB+ WMUL(i));
			}
		}
	}
return EMPTY_ELEM;
}

inline
int BBSentinel::next_bit(int& nBB){
////////////////////////////
// last update:31/12/2013
// BitScan non destructive
//
// COMMENTS
// 1- update sentinels, set m_scan.bbi to m_BBL and set m_scan.pos to MASK_LIM at the start of loop

	unsigned long posInBB;
			
	//look uo in the last table
	if(_BitScanForward64(&posInBB, m_aBB[m_scan.bbi] & Tables::mask_left[m_scan.pos])){
		m_scan.pos =posInBB;
		nBB=m_scan.bbi;
		return (posInBB + WMUL(m_scan.bbi));
	}else{											//not found in the last table. look up in the rest
		for(int i=(m_scan.bbi+1); i<=m_BBH; i++){
			if(_BitScanForward64(&posInBB,m_aBB[i])){
				m_scan.bbi=i;
				m_scan.pos=posInBB;
				nBB=i;
				return (posInBB+ WMUL(i));
			}
		}
	}
return EMPTY_ELEM;
}







