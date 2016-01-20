/*  
 * config.h file from the BITSCAN library, a C++ library for bit set
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

#define WORD_SIZE			64								//DO NOT CHANGE

//popcount
#define POPCOUNT_64										//will use intrinsic _popcn64 function (DEFAULT)
//#undef  POPCOUNT_64											//will use lookup table popcount function

//bitscanning implementation choice
#define DE_BRUIJN
#ifndef DE_BRUIJN
	#define LOOKUP
#endif

#ifdef DE_BRUIJN
	#define ISOLANI_LSB										//b&(-b) implementation
    #undef  ISOLANI_LSB										//b^(b-1) implementation (DEFAULT)
#endif

////////////////////
//Use of C memory allocation alignment primitives instead of C++ new statement				
#define _MEM_ALIGNMENT 				32						//change this for different allignments (now deprecated in favor of 64 bits)
#undef  _MEM_ALIGNMENT										//DEFAULT
 
//////////////////////
// precomputed tables
//#define CACHED_INDEX_OPERATIONS								//uses extra storage space for fast bitscanning (DEFAULT)

#ifdef CACHED_INDEX_OPERATIONS
	#define MAX_CACHED_INDEX		15001					//size of cached WMOD, WDIV, WMUL indexes (number of bits)
#endif

#define EXTENDED_LOOKUPS									//uses extra storage space for fast lookup (bitscan operations which used extended lookups have been removed in this release)
#undef	EXTENDED_LOOKUPS									//DO NOT REMOVE
