/*  
 * bbobject.h file from the BITSCAN library, a C++ library for bit set
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


#ifndef __BB_OBJECT_H__
#define  __BB_OBJECT_H__

#include <iostream>
using namespace std;


class BBObject{
public:
	enum scan_types	{NON_DESTRUCTIVE, NON_DESTRUCTIVE_REVERSE, DESTRUCTIVE, DESTRUCTIVE_REVERSE};				//types of bit scans
	friend ostream& operator<<(ostream& o , const BBObject& bb){bb.print(o); return o;}

protected:
	BBObject(){}	
	virtual void print(ostream &o, bool show_pc=true) const {this->print(o, show_pc);}
	
	////////
	// bit scanning
	
	virtual int init_scan(scan_types)										{return 0;}
};

#endif