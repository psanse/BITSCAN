/*  
 * tables.h file from the BITSCAN library, a C++ library for bit 
 * sets optimization. It has been used to implement BBMC, a very
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
 */

#ifndef __TABLES_H__
#define __TABLES_H__

#include "bbtypes.h"
#include "config.h"

#define MASK_LIM	WORD_SIZE+1							    //mask limit for bitscan operations of a single BITBOARD

#ifdef  CACHED_INDEX_OPERATIONS 
	#define WDIV(i) (Tables::t_wdindex[(i)])
	#define WMOD(i) (Tables::t_wmodindex[(i)])
	#define WMUL(i) (Tables::t_wxindex[(i)])
#else 
	#define WDIV(i) ((i)/WORD_SIZE)
	#define WMOD(i) ((i)%WORD_SIZE)
	#define WMUL(i) ((i)*WORD_SIZE)
#endif 

////////////////////
//MACROS for mapping bit index to bitblock index (0 or 1 based)
#define INDEX_0TO0(p)			(WDIV(p))					//p>0
#define INDEX_0TO1(p)			(WDIV(p)+1)					//p>0
#define INDEX_1TO1(p)			((((p)-1)/WORD_SIZE)+1)		//p>0
#define INDEX_1TO0(p)			((((p)-1)/WORD_SIZE))		//p>0


class Tables{
	friend class BitBoard;
	friend class BitBoardN;
	
private:
	Tables(){};
	virtual ~Tables(){};

public:
	static int InitAllTables();							//Driver for all inits
private:
	static void init_masks();
	static void init_popc8();
	static void init_popc();
	static void init_mlsb();
	static void init_lsb_l();							//Conditioned to EXTENDED_LOOKUPS 
	
	//Table
	static void init_cached_index();					//Conditioned to CACHED_INDEX_OPERATIONS 

////////////////////////////////////////
//data members

public:
	//commonly used tables
	static BITBOARD	mask[64];						//masks for 64 bit block of a single bit
	static U8 mask8[8];								//masks for 8 bit block of a single bit
	static BITBOARD mask_right[65];					//1_bit to the right of index (less significant bits, excluding index)
	static BITBOARD mask_left[66];					//1_bit to the left of index (more significant bits, excluding index)

//private:
	static BITBOARD mask_entre[64/*a*/][64/*b*/];	//1-bits between intervals (a<=b)

	//0 but word masks
	static BITBOARD  mask0_1W;						
	static BITBOARD  mask0_2W;
	static BITBOARD  mask0_3W;
	static BITBOARD	 mask0_4W;

	static int pc[65536];							//16 bit population count
	static int lsb[65536];							//LSB lookup table 16 bits
	static int lsba[4][65536];						//LSB lookup table 16 bits con pos indes
	static int msba[4][65536];						//MSB lookup table 16 bits con pos indes
	
	
	static int pc8[256];					//population count for 8 bits
	static int pc_sa[65536];				//populaton count for 16 bits (Shift + Add)
	static int msb[65536];					//MSB for 16 bits

#ifdef EXTENDED_LOOKUPS	
	static int lsb_l[65536][16];			//LSB for 16 bits list of position of 1-bits)
#endif
	
////////////////////////
//magic number tables

	static const int T_32[37];									//32 bits	
	static const int T_64[67];									//64 bits
	static const int indexDeBruijn64_ISOL[64];					//bit scan with b&(-b)	
	static const int indexDeBruijn64_SEP[64];					//bit scan with b^(b-1)

	//64 bit block index cache
#ifdef  CACHED_INDEX_OPERATIONS 
	static int t_wdindex[MAX_CACHED_INDEX];			
	static int t_wxindex[MAX_CACHED_INDEX];			
	static int t_wmodindex[MAX_CACHED_INDEX];
#endif

};

#endif