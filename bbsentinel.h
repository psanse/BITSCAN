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
public:
	BBSentinel():m_BBH(EMPTY_ELEM), m_BBL(EMPTY_ELEM){init_sentinels(false);}
	BBSentinel(int popsize, bool bits_to_0=true): BBIntrin(popsize, bits_to_0){ init_sentinels(false);}
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
	void  erase_bit					();											//in sentinel range
	void  erase_bit					(int nBit) {BitBoardN::erase_bit(nBit);}	//required because of the cast-to-int construction of sentinels (1)
	BBSentinel& erase_bit			(const BitBoardN&);							//(1): required for SEQ coloring
	
	bool is_empty					()const;
	bool is_empty					(int nBBL, int nBBH) const;					//is empty in range

////////////////
// operators
	BBSentinel& operator=					(const BBSentinel&);
	BBSentinel& operator&=					(const BitBoardN&);

//////////////
// I/O
	void print(ostream& o=cout);

/////////////////
// bit scanning operations 

	int init_scan(scan_types sct);
	int previous_bit_del();
	int next_bit_del ();
	int next_bit_del (BBSentinel& bbN_del);
	
	int next_bit();
	int next_bit(int& nBB);

protected:	
	 int m_BBH;										//explicit storage for sentinel high index
	 int m_BBL;										//explicit storage for sentinel low index
};

#endif




