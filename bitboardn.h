/*  
 * bitboardn.h file from the BITSCAN library, a C++ library for bit set
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

#ifndef __BITBOARDN__H_
#define __BITBOARDN__H_

#include "bbobject.h"
#include "bitboard.h"	
#include <vector>	

using namespace std;

/////////////////////////////////
//
// class BitBoardN 
//
// Manages bit strings greater than WORD_SIZE 
// Does not use intrinsics nor does it cache information for very fast bitscanning
//
///////////////////////////////////
class BitBoardN:public BBObject{
public:	

//non standard independent operators (no allocation or copies)
	friend bool operator==			(const BitBoardN& lhs, const BitBoardN& rhs);
	friend bool operator!=			(const BitBoardN& lhs, const BitBoardN& rhs);

	friend BitBoardN&  AND			(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);							
	friend BitBoardN&  AND			(int first_block, const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);
	friend BitBoardN&  AND			(int first_block, int last_block, const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);

	friend BitBoardN&  OR			(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);
	friend BitBoardN&  ERASE		(const BitBoardN& lhs, const BitBoardN& rhs,  BitBoardN& res);				//removes rhs from lhs


//constructors, initialization, assignment
	 BitBoardN						(): m_nBB(EMPTY_ELEM),m_aBB(NULL){};										
explicit  BitBoardN					(int popsize /*1 based*/, bool reset=true);	
	 BitBoardN						(const BitBoardN& bbN);
	 BitBoardN						(const std::vector<int>& v);
virtual	~BitBoardN					();
		 
	void init						(int popsize, bool reset=true);										
	void init						(int popsize, const vector<int> & );								
virtual	BitBoardN& operator =		(const BitBoardN& );	

/////////////////////
//setters and getters (will not allocate memory)
	
	BITBOARD* get_bitstring			();
	const BITBOARD* get_bitstring	()			const;
	int number_of_bitblocks			()			const {return m_nBB;}
const BITBOARD get_bitboard			(int block) const {return m_aBB[block];}
	BITBOARD& get_bitboard			(int block)		  {return m_aBB[block];}

//////////////////////////////
// Bitscanning

	//find least/most signinficant bit
inline virtual int msbn64		()	const;		//lookup
inline virtual int lsbn64		()	const; 		//de Bruijn	/ lookup								

	//for looping (does not use state info)	
inline int next_bit			(int nBit)	const;					//de Bruijn
inline int next_bit_if_del	(int nBit)	const;					//de Bruijn
inline int previous_bit		(int nbit)	const;					//lookup 
	
/////////////////
// Popcount
virtual	inline int popcn64	()						const;		//lookup 
virtual	inline int popcn64	(int nBit/* 0 based*/)	const;
/////////////////////
//Set/Delete Bits 
inline	void  init_bit				(int bit);	
inline	int   init_bit				(int lbit, int rbit);
		int   init_bit				(int high, const BitBoardN& bb_add);						    //copies bb_add in range [0, high] *** rename probably
inline	void  copy_from_block		(int first_block, const BitBoardN& bb_add);						//copies from first_block (included) onwards
inline	void  copy_up_to_block		(int last_block, const BitBoardN& bb_add);						//copies up to last_block (included)
		 void  set_bit				(int bit);
inline  int	 set_bit				(int low, int high);											//closed range
inline  void  set_bit				();
	   void  set_bit				(const BitBoardN& bb_add);										//similar to OR, experimental

		void set_block				(int first_block, const BitBoardN& bb_add);						//OR:closed range
		void set_block				(int first_block, int last_block,  const BitBoardN& bb_add);	//OR:closed range

inline void  erase_bit				(int nbit);
inline int   erase_bit				(int low, int high);
inline void  erase_bit				();
inline	BitBoardN& erase_bit			(const BitBoardN& bb_del);										

inline	BitBoardN& erase_block			(int first_block, const BitBoardN& bb_del);						//range versions
inline	BitBoardN& erase_block			(int first_block, int last_block, const BitBoardN& bb_del);		//range versions
	
////////////////////////
//member operators (must have same block size)
				
	BitBoardN& operator &=	(const BitBoardN& );													//bitset_intersection
	BitBoardN& operator |=	(const BitBoardN& );													//bitset_union
	BitBoardN& operator ^=	(const BitBoardN& );													//bitet symmetric_difference

	BitBoardN&  AND_EQ		(int first_block, const BitBoardN& rhs );								//AND: range
	BitBoardN&  OR_EQ		(int first_block, const BitBoardN& rhs );								//OR:  range
		
	BitBoardN& flip			();
inline	int	single_disjoint		(const BitBoardN& rhs, int& vertex)			const;						//non disjoint by single element
/////////////////////////////
//Boolean functions
	inline bool is_bit				(int nbit)							const;
inline virtual bool is_empty		()									const;	
inline virtual bool is_empty		(int nBBL, int nBBH)				const;	
	inline bool is_singleton		()									const;						//only one element
	inline bool is_disjoint			(const BitBoardN& rhs)				const;
	inline bool is_disjoint			(int first_block, int last_block,const BitBoardN& rhs)	const;
	inline bool is_disjoint			(const BitBoardN& a, const  BitBoardN& b)		const;		//no bit in common with both a and b (not available in sparse bitsets)
/////////////////////
// I/O 
	void print				(std::ostream& = std::cout, bool show_pc = true) const;
	string to_string		();
	
	void to_vector			(std::vector<int>& )				const;
	
////////////////////////
//Member data
protected:
	BITBOARD* m_aBB;
	int m_nBB;				//number of BITBOARDS (1 based)
};

inline int BitBoardN::msbn64() const{
///////////////////////
// Look up table implementation (best found so far)

	union u {
		U16 c[4];
		BITBOARD b;
	};

	u val;

	for(int i=m_nBB-1; i>=0; i--){
		val.b=m_aBB[i];
		if(val.b){
			if(val.c[3]) return (Tables::msba[3][val.c[3]]+WMUL(i));
			if(val.c[2]) return (Tables::msba[2][val.c[2]]+WMUL(i));
			if(val.c[1]) return (Tables::msba[1][val.c[1]]+WMUL(i));
			if(val.c[0]) return (Tables::msba[0][val.c[0]]+WMUL(i));
		}
	}

return EMPTY_ELEM;		//should not reach here
}


inline int BitBoardN::next_bit(int nBit/* 0 based*/) const{
////////////////////////////
//
// Returns next bit from nBit in the bitstring (to be used in a bitscan loop)
//
// NOTES: if nBit is FIRST_BITSCAN returns lsb

	int index, npos;

	if(nBit==EMPTY_ELEM)
				return lsbn64();
	else{
		index=WDIV(nBit);

		//looks in same BB as nBit
		npos=BitBoard::lsb64_de_Bruijn(Tables::mask_left[WMOD(nBit) /*-WORD_SIZE*index*/] & m_aBB[index] );
		if(npos>=0)
			return (WMUL(index)/*WORD_SIZE*index*/ + npos);

		//looks in remaining BBs
		for(int i=index+1; i<m_nBB; i++){
			if(m_aBB[i])
				return(BitBoard::lsb64_de_Bruijn(m_aBB[i])+WMUL(i)/*WORD_SIZE*i*/ );
		}
	}
	
return -1;	
}

inline int BitBoardN::next_bit_if_del(int nBit/* 0 based*/) const{
////////////////////////////
// Returns next bit assuming, when used in a loop, that the last bit
// scanned is deleted prior to the call
		
	if(nBit==EMPTY_ELEM)
		return lsbn64();
	else{
		for(int i=WDIV(nBit); i<m_nBB; i++){
			if(m_aBB[i]){
				return(BitBoard::lsb64_de_Bruijn(m_aBB[i])+WMUL(i) );
			}
		}
	}
	
return -1;	
}


inline int BitBoardN::previous_bit(int nBit/* 0 bsed*/) const{
////////////////////////////
// Gets the previous bit to nBit. If nBits is -10 is a MSB
//
// COMMENTS
// 1-There is no control of EMPTY_ELEM

	if(nBit==EMPTY_ELEM) 
			return msbn64();
	

	int index=WDIV(nBit);
	int npos;

	union u {
		U16 c[4];
		BITBOARD b;
	};
	u val;
		
	
	//BitBoard pos
	npos=BitBoard::msb64_lup( Tables::mask_right[WMOD(nBit) /*nBit-WMUL(index)*/] & m_aBB[index] );
	if(npos!=EMPTY_ELEM)
			return (WMUL(index) + npos);

	for(int i=index-1; i>=0; i--){
		val.b=m_aBB[i];
		if(val.b){
			if(val.c[3]) return (Tables::msba[3][val.c[3]]+ WMUL(i));
			if(val.c[2]) return (Tables::msba[2][val.c[2]]+ WMUL(i));
			if(val.c[1]) return (Tables::msba[1][val.c[1]]+ WMUL(i));
			if(val.c[0]) return (Tables::msba[0][val.c[0]]+ WMUL(i));
		}
	}

return -1;		//should not reach here
}

inline bool BitBoardN::is_bit (int nbit/*0 based*/) const{
//////////////////////////////
// RETURNS: TRUE if the bit is 1 in the position nbit, FALSE if opposite case or ERROR

	return (m_aBB[WDIV(nbit)] & Tables::mask[WMOD(nbit)]);

}
 
inline bool BitBoardN::is_empty() const
{
	for(int i=0; i<m_nBB; i++)
			if(m_aBB[i]) return false;

return true;	
}

inline bool BitBoardN::is_empty	(int nBBL, int nBBH) const{
	for(int i=nBBL; i<=nBBH; ++i)
			if(m_aBB[i]) return false;

return true;	
}

inline bool BitBoardN::is_disjoint	(const BitBoardN& rhs) const{
	for(int i=0; i<m_nBB; ++i)
		if(m_aBB[i]& rhs.m_aBB[i]) return false;
return true;
}

inline bool BitBoardN::is_disjoint	(const BitBoardN& lhs,  const BitBoardN& rhs)	const{
	for(int i=0; i<m_nBB; ++i)
		if(m_aBB[i] & lhs.m_aBB[i] & rhs.m_aBB[i]) return false;
return true;
}



inline bool BitBoardN::is_disjoint (int first_block, int last_block,const BitBoardN& rhs)	const{
///////////////////
// no checking of block indexes
	for(int i=first_block; i<=last_block; ++i)
			if(m_aBB[i]& rhs.m_aBB[i]) return false;
return true;
}

inline void BitBoardN::erase_bit (int nbit /*0 based*/){

	m_aBB[WDIV(nbit)] &= ~Tables::mask[WMOD(nbit)];
}

inline void  BitBoardN::init_bit(int nbit){
///////////////////
// sets nbit and clears the rest
	erase_bit();
	set_bit(nbit);
}

inline int BitBoardN::init_bit(int low, int high){
///////////////////
// sets bits to 1 from lbit to rbit included and clears the rest
	int bbl= WDIV(low);
	int bbh= WDIV(high);

	//checks consistency (***use ASSERT)
	if(bbh<bbl || bbl<0 || low>high || low<0){
		cerr<<"Error in set bit in range"<<endl;
		return -1;
	}

	if(bbl==bbh){
		m_aBB[bbh]=BitBoard::MASK_1(low-WMUL(bbl), high-WMUL(bbh));
	}else{
		for(int i=bbl+1; i<=bbh-1; i++)	
			m_aBB[i]=ONE;

		m_aBB[bbl]=~Tables::mask_right[low-WMUL(bbl)];
		m_aBB[bbh]=~Tables::mask_left[high-WMUL(bbh)];	
	}

	//clears the rest
	for(int i=0; i<bbl; i++)	
		m_aBB[i]=ZERO;

	for(int i=bbh+1; i<m_nBB; i++)	
		m_aBB[i]=ZERO;

	return 0;
}

inline
int BitBoardN::init_bit (int high, const BitBoardN& bb_add){
//////////////////////////////////
// fast copying of bb_add up to, and including, high bit

	int bbh= WDIV(high);

	//***checks consistency (***use ASSERT)

	for(int i=0; i<=bbh; i++){
		m_aBB[i]=bb_add.m_aBB[i];
	}

	//trim last bit block up to high
	m_aBB[bbh]&=BitBoard::MASK_1(0, high-WMUL(bbh)); 


return 0;
}

inline void BitBoardN::copy_from_block (int first_block, const BitBoardN& bb_add){
//////////////
// copies from first_bit included onwards 

	for(int i=first_block; i<m_nBB; i++){
			m_aBB[i]=bb_add.m_aBB[i];
	}
}

inline void  BitBoardN::copy_up_to_block (int last_block, const BitBoardN& bb_add){
//////////////
// copies up to last_bit included and clears t
// OBSERVATIONS: No out-of-bounds check for last_bit (will throw exception)

	for(int i=0; i<=last_block; i++){
		m_aBB[i]=bb_add.m_aBB[i];
	}
}

inline void BitBoardN::set_bit	(int nbit /*0 based*/){

	m_aBB[WDIV(nbit)] |= Tables::mask[WMOD(nbit)];
}

inline int  BitBoardN::set_bit (int low, int high){
/////////////////////
// Set all bits (0 based)  to 1 in the closed range (including both ends)
// date: 22/9/14

	int bbl= WDIV(low);
	int bbh= WDIV(high); 
	
	//checks consistency (ASSERT)
	if(bbh<bbl || bbl<0 || low>high || low<0){
		cerr<<"Error in set bit in range"<<endl;
		return -1;
	}

	if(bbl==bbh){
		BITBOARD bb1=m_aBB[bbh]| ~Tables::mask_left[high-WMUL(bbh)];
		BITBOARD bb2=m_aBB[bbl]| ~Tables::mask_right[low-WMUL(bbl)];
		m_aBB[bbh]=bb1 & bb2;
	}
	else{
		for(int i=bbl+1; i<=bbh-1; i++)	
			m_aBB[i]=ONE;

		//lower
		m_aBB[bbh]|=~Tables::mask_left[high-WMUL(bbh)];			
		m_aBB[bbl]|=~Tables::mask_right[low-WMUL(bbl)];
	}
return 0;
}

inline void	BitBoardN::set_bit (){
///////////////
// sets all bit blocks to ONE
	for(int i=0; i<m_nBB; i++)	
				m_aBB[i]=ONE;
}

inline
void  BitBoardN::set_bit (const BitBoardN& bb_add){
//////////////
// copies 1-bits (equivalent to OR, set_union etc)
	for(int i=0; i<m_nBB; i++)	
		m_aBB[i]|=bb_add.m_aBB[i];
}

inline
void  BitBoardN::set_block (int first_block, const BitBoardN& bb_add){
//////////////
// copies bb_add 1-bits (equibvalent to OR, set_union etc) from the first block onwards
	for(int i=first_block; i<m_nBB; i++)	
				m_aBB[i]|=bb_add.m_aBB[i];
}

inline
void  BitBoardN::set_block (int first_block, int last_block, const BitBoardN& bb_add){
//////////////
// copies bb_add 1-bits (equibvalent to OR, set_union etc) in CLOSED RANGE
	for(int i=first_block; i<=last_block; i++)	
				m_aBB[i]|=bb_add.m_aBB[i];
}

inline void	BitBoardN::erase_bit (){
///////////////
// sets all bit blocks to ZERO
	for(int i=0; i<m_nBB; i++)	
				m_aBB[i]=ZERO;
}

inline int  BitBoardN::erase_bit (int low, int high){
/////////////////////
// Set all bits (0 based)  to 0 in the closed range (including both ends)
// date: 22/9/14	

	int bbl= WDIV(low);
	int bbh= WDIV(high); 
	
	//checks consistency (ASSERT)
	if(bbh<bbl || bbl<0 || low>high || low<0){
		cerr<<"Error in set bit in range"<<endl;
		return -1;
	}

	if(bbl==bbh){
		BITBOARD bb1=m_aBB[bbh] & Tables::mask_left[high-WMUL(bbh)];
		BITBOARD bb2=m_aBB[bbl] & Tables::mask_right[low-WMUL(bbl)];
		m_aBB[bbh]=bb1 | bb2;
	}
	else{
		for(int i=bbl+1; i<=bbh-1; i++)	
			m_aBB[i]=ZERO;

		//lower
		m_aBB[bbh] &= Tables::mask_left[high-WMUL(bbh)];			
		m_aBB[bbl] &= Tables::mask_right[low-WMUL(bbl)];
	}
return 0;
}

inline int BitBoardN::lsbn64() const{
/////////////////
// different implementations of lsbn depending on configuration

#ifdef DE_BRUIJN
	for(int i=0; i<m_nBB; i++){
		if(m_aBB[i])
#ifdef ISOLANI_LSB
			return(Tables::indexDeBruijn64_ISOL[((m_aBB[i] & -m_aBB[i]) * DEBRUIJN_MN_64_ISOL/*magic num*/) >> DEBRUIJN_MN_64_SHIFT]+ WMUL(i));	
#else
			return(Tables::indexDeBruijn64_SEP[((m_aBB[i]^ (m_aBB[i]-1)) * DEBRUIJN_MN_64_SEP/*magic num*/) >> DEBRUIJN_MN_64_SHIFT]+ WMUL(i));	
#endif
	}
#elif LOOKUP
	union u {
		U16 c[4];
		BITBOARD b;
	};

	u val;

	for(int i=0; i<m_nBB; i++){
		val.b=m_aBB[i];
		if(val.b){
			if(val.c[0]) return (Tables::lsba[0][val.c[0]]+WMUL(i));
			if(val.c[1]) return (Tables::lsba[1][val.c[1]]+WMUL(i));
			if(val.c[2]) return (Tables::lsba[2][val.c[2]]+WMUL(i));
			if(val.c[3]) return (Tables::lsba[3][val.c[3]]+WMUL(i));
		}
	}

#endif
return EMPTY_ELEM;	
}


inline bool BitBoardN::is_singleton()const{
/////////////////////////////
// optimized for dense graphs
	int pc=0;
	for(int i=0; i<m_nBB; i++){
		if((pc+= BitBoard::popc64(m_aBB[i]))>1) return false; 
	}
	if(pc) return true;
return false;
}


int BitBoardN::popcn64() const{
	int npc=0;
	union u	{
		U16 c[4];
		BITBOARD b;
	}val;

	for(int i=0; i<m_nBB; i++){
		val.b = m_aBB[i]; //Loads union
		npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
	}

return npc;
}

int BitBoardN::popcn64(int nBit) const{
/////////////////////////
// Population size from nBit(included) onwards

	int npc=0;
	union u	{
		U16 c[4];
		BITBOARD b;
	}val;

	int nBB=WDIV(nBit);

	for(int i=nBB+1; i<m_nBB; i++){
		val.b = m_aBB[i]; //Loads union
		npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
	}

	//special case of nBit bit block
	val.b = m_aBB[nBB]&~Tables::mask_right[WMOD(nBit)];		//Loads union
	npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
	
return npc;
}


inline int BitBoardN::single_disjoint (const BitBoardN& rhs, int& vertex) const{
/////////////////////
// PARAMS 
// vertex: Single vertex which is common to lhs (this) and rhs or EMPTY_ELEM (return value MUST BE 1)
// 
// RETURN value: 0 if disjoint, 1 if single_disjoint, EMPTY_ELEM otherwise 


	int pc=0;
	vertex=EMPTY_ELEM;
	bool first_time=true;
	
	for(int i=0; i<m_nBB; i++){
		pc+=BitBoard::popc64(this->m_aBB[i] & rhs.m_aBB[i]);
		if(pc>1){
			vertex=EMPTY_ELEM;
			return EMPTY_ELEM;
		}else if(pc==1 && first_time ){  //store vertex position
			vertex=BitBoard::lsb64_intrinsic(this->m_aBB[i] & rhs.m_aBB[i] )+ WMUL(i);
			first_time=false;
		}
	}
	
	return pc;		//disjoint
}

inline
bool operator==	(const BitBoardN& lhs, const BitBoardN& rhs){
	
	for(int i=0; i<lhs.m_nBB; i++)
		if( lhs.m_aBB[i]!=rhs.m_aBB[i] ) return false;

return true;
}

inline
bool operator!=	(const BitBoardN& lhs, const BitBoardN& rhs){
	 return ! operator==(lhs, rhs);
}

inline
BitBoardN& BitBoardN::erase_bit (const BitBoardN& bbn){
//////////////////////////////
// deletes bbn from current bitstring

	for(int i=0; i<m_nBB; i++)
			m_aBB[i] &= ~ bbn.m_aBB[i];
return *this;
}

inline
BitBoardN& BitBoardN::erase_block	(int first_block, const BitBoardN& bb_del){
/////////////////////////////
// deletes from block_first (included) to the end of the bitsring
	for(int i=first_block; i<m_nBB; i++)
			m_aBB[i] &= ~ bb_del.m_aBB[i];
return *this;
}

inline
BitBoardN& BitBoardN::erase_block(int block_first, int block_last, const BitBoardN& bb_del){
/////////////////////////////
// deletes from block_first (CLOSED RANGE) the 1-bits in bb_del
// assert(block_first<=block_last) 
// 
// REMARKS: population size not checked

	for(int i=block_first; i<=block_last; i++)
			m_aBB[i] &= ~ bb_del.m_aBB[i];

return *this;
}

#endif