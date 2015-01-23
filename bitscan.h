
/*  
 * This is the one and only header file required to use the BITSCAN library,
 * a C++ library for bit set optimization. It has been used to implement BBMC, 
 * a very succesful bit-parallel algorithm for exact maximum clique. 
 * (see license file for references)
 *
 * Copyright (C) 2014
 * Author: Pablo San Segundo
 * Intelligent Control Research Group (CSIC-UPM) 
 *
 * Permission to use, modify and distribute this software is granted
 * provided that this copyright notice appears in all copies, in source code or
*  in binaries. For precise terms see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any kind,
 * express or implied, and with no claim as to its suitability for any
 * purpose.
 *
 */

#include "bbalg.h"
#include "bbsentinel.h"			
#include "bbintrinsic_sparse.h"	

//client data types
typedef BitBoard bitblock;
typedef BBIntrin bitarray;
typedef BBSentinel watched_bitarray;
typedef BBIntrinS sparse_bitarray;
typedef BitBoardN simple_bitarray;
typedef BitBoardS simple_sparse_bitarray;
typedef BBObject  bbo;

