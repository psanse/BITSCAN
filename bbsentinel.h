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

class BBSentinel : public BBIntrin{
public:
	BBSentinel():m_BBH(EMPTY_ELEM), m_BBL(EMPTY_ELEM){init_sentinels();}
	BBSentinel(int popsize, bool bits_to_0=true): BBIntrin(popsize, bits_to_0){ init_sentinels();}
	BBSentinel(const BBSentinel& bbN) : BBIntrin(bbN){ m_BBH=bbN.m_BBH; m_BBL=bbN.m_BBL;}
	~BBSentinel(){};

////////////
// setters / getters
	void set_sentinel_H(unsigned int i){ m_BBH=i;}
	void set_sentinel_L(unsigned int i){ m_BBL=i;}
	void set_sentinels(int low, int high);
	void init_sentinels();							//sets sentinels to maximum scope of current bit string
	void clear_sentinels();							//sentinels to EMPTY
	int get_sentinel_L(){ return m_BBL;}
	int get_sentinel_H(){ return m_BBH;}
/////////////
// basic operators

	int  update_sentinels			();
	int  update_sentinels			(int low, int high);
	int  update_sentinels_high		();
	int  update_sentinels_low		();

	void update_sentinels_to_v(int v);				
//////////////
// I/O
	void print(std::ostream& o);

/////////////////
// bit scanning operations 
	int init_scan(scan_types sct);
	int previous_bit_del();
	int next_bit_del ();
	int next_bit_del (BBSentinel& bbN_del);
	int next_bit_del (int& nBB, BBSentinel& bbN_del);

	int next_bit();
	int next_bit(int& nBB);

protected:	
	 int m_BBH;										//explicit storage for sentinel high index
	 int m_BBL;										//explicit storage for sentinel low index
};

#endif




