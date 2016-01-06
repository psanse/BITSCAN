/*  
 * bitboards.h file from the BITSCAN library, a C++ library for bit set
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
 */

#ifndef __BITBOARDS_H__
#define __BITBOARDS_H__

#include "bbobject.h"
#include "bitboard.h"	
#include <vector>	
#include <algorithm>
#include <functional>
#include <iterator>

using namespace std;

#define DEFAULT_CAPACITY	 2		//initial reserve of bit blocks for any new sparse bitstring (possibly remove)

/*template<class T>
class Graph;*/
 
/////////////////////////////////
//
// class BitBoardS
// (Manages sparse bit strings)
//
///////////////////////////////////

class BitBoardS: public BBObject{
	//template <class T> friend  class Graph;
public:
	struct elem_t{
		int index;
		BITBOARD bb;
		elem_t(int index=EMPTY_ELEM, BITBOARD bb=0):index(index), bb(bb){}
		bool operator ==(const elem_t& e) const { return (index==e.index && bb==e.bb);}
		bool operator !=(const elem_t& e) const { return (index!=e.index || bb!=e.bb);}
		bool test_not_0()const {return bb!=0;}
		void clear_bb() {bb=0;}
	};

	typedef struct elem_t elem;
	typedef vector<elem> velem;
	typedef vector<elem>::iterator velem_it;
	typedef vector<elem>::const_iterator velem_cit;

	struct elem_less:
		public binary_function <elem, elem, bool>{
			bool operator()(const elem& lhs, const elem& rhs){
				return lhs.index<rhs.index;
			}
	};

private:
	static int nElem;																						//to cache current bitblock position in the collection (not its index ni the bitstring) 
public:	
	friend bool	operator ==			(const BitBoardS&, const BitBoardS&);
	friend bool operator!=			(const BitBoardS& lhs, const BitBoardS& rhs);

    friend inline BitBoardS&  AND	(const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res);
	friend inline BitBoardS&  AND	(int first_block, const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res);
	friend inline BitBoardS&  AND	(int first_block, int last_block, const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res);
	friend BitBoardS&  OR			(const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res);
	friend BitBoardS&  ERASE		(const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res);			//removes rhs from lhs


	BitBoardS						():m_MAXBB(EMPTY_ELEM){}												//is this necessary?											
explicit BitBoardS					(int size, bool is_popsize=true );										//popsize is 1-based
	BitBoardS						(const BitBoardS& );	
virtual ~BitBoardS					(){clear();}	

	int init						(int size, bool is_popsize=true);					
	void clear						();
	void sort						();
	BitBoardS& operator =			(const BitBoardS& );	
/////////////////////
//setters and getters (will not allocate memory)
											
	int number_of_bitblocks			()						const {return m_aBB.size();}
	BITBOARD get_bitboard			(int index)				const {return m_aBB[index].bb;}			//index in the collection			
	BITBOARD find_bitboard			(int block_index)		const;									//O(log) operation
	pair<bool, int>	 find_pos		(int block_index)		const;									//O(log) operation
	pair<bool, velem_it> find_block	(int block_index, bool is_lb=true);	
	pair<bool, velem_cit>find_block (int block_index, bool is_lb=true)	const;	
	velem_it  begin					(){return m_aBB.begin();}
	velem_it  end					(){return m_aBB.end();}
	velem_cit  begin				()	const {return m_aBB.cbegin();}
	velem_cit  end					()  const {return m_aBB.cend();}
//////////////////////////////
// Bitscanning

	//find least/most signinficant bit
virtual inline	int msbn64				(int& nElem)			const;				
virtual inline	int lsbn64				(int& nElem)			const; 	
virtual	int msbn64						()						const;		//lookup
virtual	int lsbn64						()						const; 		//de Bruijn	/ lookup

	//for scanning all bits
	
virtual inline	int next_bit			(int nBit)				;				//uses cached elem position for fast bitscanning
virtual inline	int previous_bit		(int nBit)				;				//uses cached elem position for fast bitscanning

private:
	int next_bit					(int nBit)				const;			//de Bruijn 
	int previous_bit				(int nbit)				const;			//lookup

public:	
/////////////////
// Popcount
virtual inline	 int popcn64		()						const;			//lookup 
virtual inline	 int popcn64		(int nBit)				const;			

/////////////////////
//Set/Delete Bits (nbit is always 0 based)
		int   init_bit				(int nbit);	
		int   init_bit				(int lbit, int rbit);	
		int   init_bit				(int last_bit, const BitBoardS& bb_add);							//copies up to last_bit included
		int   init_bit				(int lbit, int rbit, const BitBoardS& bb_add);						//copies up to last_bit included
inline	int   set_bit				(int nbit);															//ordered insertion by bit block index
		int	  set_bit				(int lbit, int rbit);												//CLOSED range
BitBoardS&    set_bit				(const BitBoardS& bb_add);											//OR

BitBoardS&  set_block				(int first_block, const BitBoardS& bb_add);							//OR:closed range
BitBoardS&  set_block				(int first_block, int last_block, const BitBoardS& rhs);			//OR:closed range
		
inline	void  erase_bit				(int nbit);	
inline	velem_it  erase_bit			(int nbit, velem_it from_it);
		int	  erase_bit				(int lbit, int rbit);
		int	  clear_bit				(int lbit, int rbit);					//deallocates blocks
		void  shrink_to_fit			(){m_aBB.shrink_to_fit();}
		void  erase_bit				() {m_aBB.clear();}						//clears all bit blocks
BitBoardS&    erase_bit				(const BitBoardS&);				

BitBoardS&    erase_block			(int first_block, const BitBoardS& rhs );
BitBoardS&    erase_block			(int first_block, int last_block, const BitBoardS& rhs );
BitBoardS&    erase_block_pos		(int first_pos_of_block, const BitBoardS& rhs );

////////////////////////
//Operators 
 BitBoardS& operator &=				(const BitBoardS& );					
 BitBoardS& operator |=				(const BitBoardS& );
 BitBoardS& AND_EQ					(int first_block, const BitBoardS& rhs );	//in range
 BitBoardS& OR_EQ					(int first_block, const BitBoardS& rhs );	//in range
  		
/////////////////////////////
//Boolean functions
inline	bool is_bit					(int nbit)				const;				//nbit is 0 based
inline	bool is_empty				()						const;				//lax: considers empty blocks for emptyness
		bool is_disjoint			(const BitBoardS& bb)   const;
		bool is_disjoint			(int first_block, int last_block, const BitBoardS& bb)   const;
/////////////////////
// I/O 
virtual	void print					(ostream& = cout, bool show_pc = true) const;
	string to_string				();
	
	void to_vector					(std::vector<int>& )	const;
////////////////////////
//Member data
protected:
	velem m_aBB;					//a vector of sorted non-empty bit blocks
	int m_MAXBB;					//maximum possible number of elements
};


//////////////////////////
//
// BOOLEAN FUNCTIONS
//
//////////////////////////

bool BitBoardS::is_bit(int nbit)	const{
//////////////////////////////
// RETURNS: TRUE if the bit is 1 in the position nbit, FALSE if opposite case or ERROR
//
// REMARKS: could be implemented in terms of find_bitboard

	//lower_bound implementation
	int index=WDIV(nbit);
	velem_cit it=lower_bound(m_aBB.begin(), m_aBB.end(), elem(index), elem_less());
	if(it!=m_aBB.end()){
		if((*it).index==index)
			return ((*it).bb & Tables::mask[WMOD(nbit)]);
	}
	return false;
}


bool BitBoardS::is_empty ()	const{
///////////////////////
// 
// REMARKS:	The bit string may be empty either because it is known that there are no blocks (size=0)
//			or because the blocks contain no 1-bit (we admit this option explicitly for efficiency)

	if(m_aBB.empty()) return true;

	for(int i=0; i<m_aBB.size(); i++){
		if(m_aBB[i].bb) return false;
	}
	
	
	//find if all elements are 0 (check efficiency)
	/*velem_cit it=find_if (m_aBB.begin(), m_aBB.end(), mem_fun_ref(&elem::test_not_0)); 
	return (it==m_aBB.end());*/

return true;
}

inline
bool BitBoardS::is_disjoint	(const BitBoardS& rhs) const{
///////////////////
// true if there are no bits in common 
	int i1=0, i2=0;
	while(true){
		//exit condition I
		if(i1==m_aBB.size() || i2==rhs.m_aBB.size() ){		//size should be the same
					return true;
		}

		//update before either of the bitstrings has reached its end
		if(m_aBB[i1].index==rhs.m_aBB[i2].index){
			if(m_aBB[i1].bb & rhs.m_aBB[i2].bb)
							return false;	//bit in common	
			++i1; ++i2;
		}else if(m_aBB[i1].index<rhs.m_aBB[i2].index){
			i1++;
		}else if(rhs.m_aBB[i2].index<m_aBB[i1].index){
			i2++;
		}
	}

return true;
}

inline 
bool BitBoardS::is_disjoint	(int first_block, int last_block, const BitBoardS& rhs)   const{
///////////////////
// true if there are no bits in common in the closed range [first_block, last_block]
//
// REMARKS: 
// 1) no assertions on valid ranges

	int i1=0, i2=0;

	//updates initial element indexes it first_block is defined
	if(first_block>0){

		pair<bool, int> p1=this->find_pos(first_block);
		pair<bool, int> p2=rhs.find_pos(first_block);

		//checks whether both sparse bitstrings have at least one block greater or equal to first_block
		if(p1.second==EMPTY_ELEM || p2.second==EMPTY_ELEM) return true;
		i1=p1.second; i2=p2.second;
	}

	//main loop
	int nElem=this->m_aBB.size(); int nElem_rhs=rhs.m_aBB.size();
	while(! ((i1>=nElem || this->m_aBB[i1].index>last_block ) || (i2>=nElem_rhs || rhs.m_aBB[i2].index>last_block )) ){

		//update before either of the bitstrings has reached its end
		if(this->m_aBB[i1].index<rhs.m_aBB[i2].index){
			i1++;
		}else if(rhs.m_aBB[i2].index<this->m_aBB[i1].index){
			i2++;
		}else{
			if(this->m_aBB[i1].bb & rhs.m_aBB[i2].bb)
				return false;				
			i1++, i2++; 
		}

		/*if(lhs.m_aBB[i1].index==rhs.m_aBB[i2].index){
		BitBoardS::elem e(lhs.m_aBB[i1].index, lhs.m_aBB[i1].bb & rhs.m_aBB[i2].bb);
		res.m_aBB.push_back(e);
		i1++, i2++; 
		}else if(lhs.m_aBB[i1].index<rhs.m_aBB[i2].index){
		i1++;
		}else if(rhs.m_aBB[i2].index<lhs.m_aBB[i1].index){
		i2++;
		}*/
	}
	

return true;		//disjoint
}

////////////////
//
// Bit updates
//
/////////////////

void BitBoardS::erase_bit(int nbit /*0 based*/){
//////////////
// clears bitblock information (does not remove bitblock if empty) 
// REMARKS: range must be sorted

	int index=WDIV(nbit);

	//equal_range implementation 
	/*pair<velem_it, velem_it> p=equal_range(m_aBB.begin(), m_aBB.end(), elem(WDIV(nbit)), elem_less());
	if(distance(p.first,p.second)!=0){
		(*p.first).bb&=~Tables::mask[WMOD(nbit)];
	}*/
	
	//lower_bound implementation
	velem_it it=lower_bound(m_aBB.begin(), m_aBB.end(), elem(index), elem_less());
	if(it!=m_aBB.end()){
		//check if the element exists already
		if(it->index==index)
			it->bb&=~Tables::mask[WMOD(nbit)];
	}
}

BitBoardS::velem_it  BitBoardS::erase_bit (int nbit, BitBoardS::velem_it from_it){
	int index=WDIV(nbit);
		
	//lower_bound implementation
	velem_it it=lower_bound(from_it, m_aBB.end(), elem(index), elem_less());
	if(it!=m_aBB.end()){
		//check if the element exists already
		if(it->index==index)
			it->bb&=~Tables::mask[WMOD(nbit)];
	}
return it;
}

int  BitBoardS::set_bit (int nbit ){
///////////////
//  General function for setting bits  
//
//  REMARKS:
//  1-Penalty cost for insertion
//  2-The other idea would be to allow unordered insertion and sort afterwards. This would be sensible for initial setting of bits operation (i.e. when reading from file)

	int index=WDIV(nbit);

	//ASSERT
	if (index>=m_MAXBB){
		cout<<"bit outside population limit"<<endl;
		cout<<"bit: "<<nbit<<" block: "<<index<<" max block: "<<m_MAXBB<<endl;
		return -1;
	}

	//lower_bound implementation
	velem_it it=lower_bound(m_aBB.begin(), m_aBB.end(), elem(index), elem_less());
	if(it!=m_aBB.end()){
		//check if the element exists already
		if(it->index==index){
			it->bb|=Tables::mask[WMOD(nbit)];
		}else 	//new inserted element
			m_aBB.insert(it, elem(index,Tables::mask[WMOD(nbit)]));
	
	}else{
		//insertion at the end
		m_aBB.push_back(elem(index,Tables::mask[WMOD(nbit)]));
	}
	
return 0;  //ok
}

inline
int BitBoardS::init_bit (int nBit){
///////////////////
// sets bit and clears the rest
	
	//**assert MAXSIZE: return -1

	m_aBB.clear();
	m_aBB.push_back(elem(WDIV(nBit), Tables::mask[WMOD(nBit)]));

return 0;
}	

////////////////
//
// Bit scanning
//
/////////////////

int BitBoardS::previous_bit	(int nBit){
/////////////////
// Uses cache of last index position for fast bit scanning
//

	if(nBit==EMPTY_ELEM)
			return msbn64(nElem);		//updates nElem with the corresponding bitblock
	
	int index=WDIV(nBit);
	int npos=BitBoard::msb64_lup(Tables::mask_right[WMOD(nBit) /*nBit-WMUL(index)*/] & m_aBB[nElem].bb);
	if(npos!=EMPTY_ELEM)
		return (WMUL(index) + npos);
	
	for(int i=nElem-1; i>=0; i--){  //new bitblock
		if( m_aBB[i].bb){
			nElem=i;
			return BitBoard::msb64_de_Bruijn(m_aBB[i].bb) + WMUL(m_aBB[i].index);
		}
	}
return EMPTY_ELEM;
}


int BitBoardS::next_bit(int nBit){
/////////////////
// Uses cache of last index position for fast bit scanning
//	
	if(nBit==EMPTY_ELEM)
		return lsbn64(nElem);		//updates nElem with the corresponding bitblock
	
	int index=WDIV(nBit);
	int npos=BitBoard::lsb64_de_Bruijn(Tables::mask_left[WMOD(nBit) /*-WORD_SIZE*index*/] & m_aBB[nElem].bb);
	if(npos!=EMPTY_ELEM)
		return (WMUL(index) + npos);
	
	for(int i=nElem+1; i<m_aBB.size(); i++){
		//new bitblock
		if(m_aBB[i].bb){
			nElem=i;
			return BitBoard::lsb64_de_Bruijn(m_aBB[i].bb) + WMUL(m_aBB[i].index);
		}
	}
return EMPTY_ELEM;
}

int BitBoardS::msbn64	(int& nElem)	const{
///////////////////////
// Look up table implementation (best found so far)
//
// RETURNS element index of the bitblock

	union u {
		U16 c[4];
		BITBOARD b;
	};

	u val;

	for(int i=m_aBB.size()-1; i>=0; i--){
		val.b=m_aBB[i].bb;
		if(val.b){
			nElem=i;
			if(val.c[3]) return (Tables::msba[3][val.c[3]]+WMUL(m_aBB[i].index));
			if(val.c[2]) return (Tables::msba[2][val.c[2]]+WMUL(m_aBB[i].index));
			if(val.c[1]) return (Tables::msba[1][val.c[1]]+WMUL(m_aBB[i].index));
			if(val.c[0]) return (Tables::msba[0][val.c[0]]+WMUL(m_aBB[i].index));
		}
	}

return EMPTY_ELEM;		//should not reach here
}

int BitBoardS::lsbn64 (int& nElem)		const	{
/////////////////
// different implementations of lsbn depending on configuration
//
// RETURNS element index of the bitblock
	
#ifdef DE_BRUIJN
	for(int i=0; i<m_aBB.size(); i++){
		if(m_aBB[i].bb){
			nElem=i;
#ifdef ISOLANI_LSB
			return(Tables::indexDeBruijn64_ISOL[((m_aBB[i].bb & -m_aBB[i].bb) * DEBRUIJN_MN_64_ISOL/*magic num*/) >> DEBRUIJN_MN_64_SHIFT]+ WMUL(m_aBB[i].index));	
#else
			return(Tables::indexDeBruijn64_SEP[((m_aBB[i].bb^ (m_aBB[i].bb-1)) * DEBRUIJN_MN_64_SEP/*magic num*/) >> DEBRUIJN_MN_64_SHIFT]+ WMUL(m_aBB[i].index));	
#endif
		}
	}
#elif LOOKUP
	union u {
		U16 c[4];
		BITBOARD b;
	};

	u val;

	for(int i=0; i<m_nBB; i++){
		val.b=m_aBB[i].bb;
		nElem=i;
		if(val.b){
			if(val.c[0]) return (Tables::lsba[0][val.c[0]]+WMUL(m_aBB[i].index));
			if(val.c[1]) return (Tables::lsba[1][val.c[1]]+WMUL(m_aBB[i].index));
			if(val.c[2]) return (Tables::lsba[2][val.c[2]]+WMUL(m_aBB[i].index));
			if(val.c[3]) return (Tables::lsba[3][val.c[3]]+WMUL(m_aBB[i].index));
		}
	}

#endif
return EMPTY_ELEM;	
}

inline
int BitBoardS::popcn64() const{
	int npc=0;
	union u	{
		U16 c[4];
		BITBOARD b;
	}val;

	for(int i=0; i<m_aBB.size(); i++){
		val.b = m_aBB[i].bb; 
		npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
	}

return npc;
}

inline
int BitBoardS::popcn64(int nBit) const{
	int npc=0;
	union u	{
		U16 c[4];
		BITBOARD b;
	}val;

	int nBB=WDIV(nBit);

	//find the biblock if it exists
	velem_cit it=lower_bound(m_aBB.begin(), m_aBB.end(), elem(nBB), elem_less());
	if(it!=m_aBB.end()){
		if(it->index==nBB){
			val.b= it->bb&~Tables::mask_right[WMOD(nBit)];
			npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
			it++;
		}
		
		//searches in the rest of elements with greater index than nBB
		for(; it!=m_aBB.end(); ++it){
			val.b = it->bb; //Loads union
			npc+= Tables::pc[val.c[0]] + Tables::pc[val.c[1]] + Tables::pc[val.c[2]] + Tables::pc[val.c[3]];
		}
	}
	
return npc;
}

inline
BitBoardS& AND (const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res){
///////////////////////////
// AND between sparse sets
//		
	int i2=0;
	res.erase_bit();					//experimental (and simplest solution)
	const int MAX=rhs.m_aBB.size()-1;

	//empty check of rhs required, the way it is implemented
	if(MAX==EMPTY_ELEM) return res;
	
	//optimization which works if lhs has less 1-bits than rhs
	for (int i1 = 0; i1 < lhs.m_aBB.size();i1++){
		for(; i2<MAX && rhs.m_aBB[i2].index<lhs.m_aBB[i1].index; i2++){}
		
		//update before either of the bitstrings has reached its end
		if(lhs.m_aBB[i1].index==rhs.m_aBB[i2].index){
				res.m_aBB.push_back(BitBoardS::elem(lhs.m_aBB[i1].index, lhs.m_aBB[i1].bb & rhs.m_aBB[i2].bb));
		}
	}

	//general purpose code assuming no a priori knowledge about density in lhs and rhs
	//int i1=0, i2=0;
	//while(i1!=lhs.m_aBB.size() && i2!=rhs.m_aBB.size() ){
	//	//update before either of the bitstrings has reached its end
	//	if(lhs.m_aBB[i1].index<rhs.m_aBB[i2].index){
	//		i1++;
	//	}else if(rhs.m_aBB[i2].index<lhs.m_aBB[i1].index){
	//		i2++;
	//	}else{
	//		BitBoardS::elem e(lhs.m_aBB[i1].index, lhs.m_aBB[i1].bb & rhs.m_aBB[i2].bb);
	//		res.m_aBB.push_back(e);
	//		i1++, i2++; 
	//	}
	//	
	//}
return res;
}

inline
BitBoardS& AND (int first_block, const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res){
///////////////////////////
// AND between sparse sets from first_block (included) onwards
//
			
	res.erase_bit();
	pair<bool, int> p1=lhs.find_pos(first_block);
	pair<bool, int> p2=rhs.find_pos(first_block);
	if(p1.second!=EMPTY_ELEM && p2.second!=EMPTY_ELEM){
		int i1=p1.second, i2=p2.second;
		while( i1!=lhs.m_aBB.size() && i2!=rhs.m_aBB.size() ){
			
			//update before either of the bitstrings has reached its end
			if(lhs.m_aBB[i1].index<rhs.m_aBB[i2].index){
				i1++;
			}else if(rhs.m_aBB[i2].index<lhs.m_aBB[i1].index){
				i2++;
			}else{
				BitBoardS::elem e(lhs.m_aBB[i1].index, lhs.m_aBB[i1].bb & rhs.m_aBB[i2].bb);
				res.m_aBB.push_back(e);
				i1++, i2++; 
			}


		/*	if(lhs.m_aBB[i1].index==rhs.m_aBB[i2].index){
				BitBoardS::elem e(lhs.m_aBB[i1].index, lhs.m_aBB[i1].bb & rhs.m_aBB[i2].bb);
				res.m_aBB.push_back(e);
				i1++, i2++; 
			}else if(lhs.m_aBB[i1].index<rhs.m_aBB[i2].index){
				i1++;
			}else if(rhs.m_aBB[i2].index<lhs.m_aBB[i1].index){
				i2++;
			}*/
		}
	}
return res;
}


inline
BitBoardS& AND (int first_block, int last_block, const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res){
///////////////////////////
// AND between sparse sets in closed range
// last update: 21/3/15 -BUG corrected concerning last_block and first_block value optimization  
//
// REMARKS: no assertions on valid ranges
		
	res.erase_bit();
	int i1=0, i2=0;

	//updates initial element indexes it first_block is defined
	if(first_block>0){

		pair<bool, int> p1=lhs.find_pos(first_block);
		pair<bool, int> p2=rhs.find_pos(first_block);

		//checks whether both sparse bitstrings have at least one block greater or equal to first_block
		if(p1.second==EMPTY_ELEM || p2.second==EMPTY_ELEM) return res;
		i1=p1.second; i2=p2.second;
	}


	//main loop	
	int nElem_lhs=lhs.m_aBB.size(); int nElem_rhs=rhs.m_aBB.size();
	while(! ((i1>=nElem_lhs|| lhs.m_aBB[i1].index>last_block) || (i2>=nElem_rhs || rhs.m_aBB[i2].index>last_block)) ){
			
		//update before either of the bitstrings has reached its end
		if(lhs.m_aBB[i1].index<rhs.m_aBB[i2].index){
			i1++;
		}else if(rhs.m_aBB[i2].index<lhs.m_aBB[i1].index){
			i2++;
		}else{
			BitBoardS::elem e(lhs.m_aBB[i1].index, lhs.m_aBB[i1].bb & rhs.m_aBB[i2].bb);
			res.m_aBB.push_back(e);
			i1++, i2++; 
		}

		/*if(lhs.m_aBB[i1].index==rhs.m_aBB[i2].index){
			BitBoardS::elem e(lhs.m_aBB[i1].index, lhs.m_aBB[i1].bb & rhs.m_aBB[i2].bb);
			res.m_aBB.push_back(e);
			i1++, i2++; 
		}else if(lhs.m_aBB[i1].index<rhs.m_aBB[i2].index){
			i1++;
		}else if(rhs.m_aBB[i2].index<lhs.m_aBB[i1].index){
			i2++;
		}*/
	}

return res;
}

inline
BitBoardS&  ERASE (const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res){
/////////////////
// removes rhs from lhs
// date of creation: 17/12/15

	
	const int MAX=rhs.m_aBB.size()-1;
	if(MAX==EMPTY_ELEM){ return (res=lhs);  }		//copy before returning
	res.erase_bit();

	//this works better if lhs is as sparse as possible (iterating first over rhs is illogical here becuase the operation is not symmetrical)
	int i2=0;
	for (int i1 = 0; i1 < lhs.m_aBB.size();i1++){
		for(; i2<MAX && rhs.m_aBB[i2].index<lhs.m_aBB[i1].index; i2++){}
		
		//update before either of the bitstrings has reached its end
		if(lhs.m_aBB[i1].index==rhs.m_aBB[i2].index){
				res.m_aBB.push_back(BitBoardS::elem(lhs.m_aBB[i1].index, lhs.m_aBB[i1].bb &~ rhs.m_aBB[i2].bb));
		}else{
			res.m_aBB.push_back(BitBoardS::elem(lhs.m_aBB[i1].index, lhs.m_aBB[i1].bb));
		}
	}
return res;
}

inline
BitBoardS&  BitBoardS::erase_block (int first_block, const BitBoardS& rhs ){
////////////////////
// removes 1-bits from current object (equialent to set_difference) from first_block (included) onwards			

	pair<bool, BitBoardS::velem_it> p1=find_block(first_block);
	pair<bool, BitBoardS::velem_cit> p2=rhs.find_block(first_block);
	
	//optimization based on the size of rhs being greater
	//for (int i1 = 0; i1 < lhs.m_aBB.size();i1++){

	//iteration
	while( ! ( p1.second==m_aBB.end() ||  p2.second==rhs.m_aBB.end() ) ){
		////exit condition I
		//if(p1.second==m_aBB.end() || p2.second==rhs.m_aBB.end() ){		//should be the same
		//	return *this;
		//}

		//update before either of the bitstrings has reached its end
		if(p1.second->index<p2.second->index){
			++p1.second;
		}else if(p2.second->index<p1.second->index){
			++p2.second;
		}else{
			p1.second->bb&=~p2.second->bb;
			++p1.second, ++p2.second; 
		}

	}
	
return *this;
}

inline
BitBoardS&  BitBoardS::AND_EQ(int first_block, const BitBoardS& rhs ){
//////////////////////
// left intersection (AND). bits in rhs remain starting from closed range [first_block, END[

	pair<bool, BitBoardS::velem_it> p1=find_block(first_block);
	pair<bool, BitBoardS::velem_cit> p2=rhs.find_block(first_block);
	
	//optimization based on the size of rhs being greater
	//for (int i1 = 0; i1 < lhs.m_aBB.size();i1++){...}

	//iteration
	while( true ){
		//exit condition 
		if(p1.second==m_aBB.end() ){		//size should be the same
					return *this;
		}else if( p2.second==rhs.m_aBB.end()){  //fill with zeros from last block in rhs onwards
			for(; p1.second!=m_aBB.end(); ++p1.second)
				p1.second->bb=ZERO;
			return *this;
		}

		//update before either of the bitstrings has reached its end
		if(p1.second->index<p2.second->index){
			p1.second->bb=0;
			++p1.second;
		}else if(p2.second->index<p1.second->index){
			++p2.second;
		}else{
			p1.second->bb&=p2.second->bb;
			++p1.second, ++p2.second; 
		}

	}
	
return *this;
}

inline
BitBoardS&  BitBoardS::OR_EQ(int first_block, const BitBoardS& rhs ){
//////////////////////
// left union (OR). Bits in rhs are added starting from closed range [first_block, END[

	pair<bool, BitBoardS::velem_it> p1=find_block(first_block);
	pair<bool, BitBoardS::velem_cit> p2=rhs.find_block(first_block);
	
	//iteration
	while(true){
		//exit condition 
		if(p1.second==m_aBB.end() || p2.second==rhs.m_aBB.end() ){				//size should be the same
					return *this;
		}

		//update before either of the bitstrings has reached its end
		if(p1.second->index<p2.second->index){
			++p1.second;
		}else if(p2.second->index<p1.second->index){
			++p2.second;
		}else{
			p1.second->bb|=p2.second->bb;
			++p1.second, ++p2.second; 
		}
	}
	
return *this;
 }


inline
BitBoardS&  BitBoardS::erase_block_pos (int first_pos_of_block, const BitBoardS& rhs ){
/////////////////////
// erases bits from a starting block in *this (given as the position in the bitstring collection, not its index) till the end of the bitstring, 

	int i2=0;							//all blocks in rhs are considered
	const int MAX=rhs.m_aBB.size()-1;
	
	//optimization which works if rhs has less 1-bits than this
	for (int i1 = first_pos_of_block; i1 <m_aBB.size(); i1++){
		for(; i2<MAX && rhs.m_aBB[i2].index< m_aBB[i1].index; i2++){}
		
		//update before either of the bitstrings has reached its end
		if(m_aBB[i1].index==rhs.m_aBB[i2].index){
				m_aBB[i1].bb&=~rhs.m_aBB[i2].bb;
		}
	}

return *this;
}


inline
BitBoardS&  BitBoardS::erase_block (int first_block, int last_block, const BitBoardS& rhs ){
////////////////////
// removes 1-bits from current object (equialent to set_difference) from CLOSED RANGE of blocks	

	pair<bool, BitBoardS::velem_it> p1=find_block(first_block);
	pair<bool, BitBoardS::velem_cit> p2=rhs.find_block(first_block);
	if(p1.second==m_aBB.end() || p2.second==rhs.m_aBB.end() )
		 return *this;

	//iterates	
	do{
		//update before either of the bitstrings has reached its end
		if(p1.second->index==p2.second->index){
			p1.second->bb&=~p2.second->bb;
			++p1.second, ++p2.second; 
		}else if(p1.second->index<p2.second->index){
			++p1.second;
		}else if(p2.second->index<p1.second->index){
			++p2.second;
		}

		//exit condition I
		if(p1.second==m_aBB.end() || p1.second->index>last_block || p2.second==rhs.m_aBB.end() || p2.second->index>last_block ){
			break;
		}
	}while(true);
	
return *this;
}


inline
int	 BitBoardS::erase_bit (int low, int high){
///////////////////
// clears bits in the corresponding CLOSED range
		
	int	bbh=WDIV(high); 
	int bbl=WDIV(low); 

	//checks consistency (ASSERT)
	if(bbh<bbl || bbl<0 || low>high || low<0){
		cerr<<"Error in set bit in range"<<endl;
		return -1;
	}
		
	//finds low bitblock and updates forward
	velem_it itl=lower_bound(m_aBB.begin(), m_aBB.end(), elem(bbl), elem_less());
	if(itl!=m_aBB.end()){
		if(itl->index==bbl){	//lower block exists
			if(bbh==bbl){		//case update in the same bitblock
				BITBOARD bb_low=itl->bb & Tables::mask_left[high-WMUL(bbh)];
				BITBOARD bb_high=itl->bb &Tables::mask_right[low-WMUL(bbl)];
				itl->bb=bb_low | bb_high;
				return 0;
			}

			//update lower block
			itl->bb&=Tables::mask_right[low-WMUL(bbl)];
			++itl;
		}

		//iterate over the rest
		for(; itl!=m_aBB.end(); ++itl){
			if(itl->index>=bbh){		//exit condition
				if(itl->index==bbh){	//extra processing if the end block exists
					if(bbh==bbl){		
						BITBOARD bb_low=itl->bb & Tables::mask_left[high-WMUL(bbh)];
						BITBOARD bb_high=itl->bb &Tables::mask_right[low-WMUL(bbl)];
						itl->bb=bb_low | bb_high;
						return 0;
					}

					itl->bb &=Tables::mask_left[high-WMUL(bbh)];
				}
			return 0;
			}
			//Deletes block
			itl->bb=ZERO;
		}
	}
return 0;
}

inline
int BitBoardS::init_bit (int high, const BitBoardS& bb_add){
//////////////////////////////////
// fast copying of bb_add up to and including high bit
	
	m_aBB.clear();
	int	bbh=WDIV(high); 
	pair<bool, BitBoardS::velem_cit> p=bb_add.find_block(bbh);
	if(p.second==bb_add.end())
		copy(bb_add.begin(), bb_add.end(),insert_iterator<velem>(m_aBB,m_aBB.begin()));
	else{
		if(p.first){
			copy(bb_add.begin(), p.second, insert_iterator<velem>(m_aBB,m_aBB.begin()));
			m_aBB.push_back(elem(bbh, p.second->bb & ~Tables::mask_left[high-WMUL(bbh)]));
		}else{
			copy(bb_add.begin(), ++p.second, insert_iterator<velem>(m_aBB,m_aBB.begin()));
		}
	}
return 0;
}

inline
int  BitBoardS::init_bit (int low, int high,  const BitBoardS& bb_add){
/////////////
// fast copying of bb_add in the corresponding CLOSED range
// date of creation: 19/12/15 (bug corrected in 6/01/2016)

	//***ASSERT

	m_aBB.clear();
	int	bbl=WDIV(low);
	int	bbh=WDIV(high);


	//finds low bitblock and updates forward
	velem_cit itl=lower_bound(bb_add.begin(), bb_add.end(), elem(bbl), elem_less());
	if(itl!=bb_add.end()){
		if(itl->index==bbl){	//lower block exists
			if(bbh==bbl){		//case update in the same bitblock
				m_aBB.push_back(elem( bbh, itl->bb & BitBoard::MASK_1(low-WMUL(bbl), high-WMUL(bbh)) ));
				return 0;
			}else{
				//add lower block
				m_aBB.push_back(elem(bbl, itl->bb &~ Tables::mask_right[low-WMUL(bbl)] ));
				++itl;
			}
		}

		//copied the rest if elements
		for(; itl!=bb_add.end(); ++itl){
			if(itl->index>=bbh){		//exit condition
				if(itl->index==bbh){	
					m_aBB.push_back(elem(bbh, itl->bb&~Tables::mask_left[high-WMUL(bbh)]));
				}
			return 0;
			}
			//copies the element as is
			m_aBB.push_back(*itl);
		}
	}

return 0;		//should not reach here
}

inline
bool operator == (const BitBoardS& lhs, const BitBoardS& rhs){
/////////////////////
// Simple equality check which considers exact copy of bit strings 
// REMARKS: does not take into account information, i.e. bit blocks=0

	return(lhs.m_aBB==rhs.m_aBB);
}

inline
bool operator!=	(const BitBoardS& lhs, const BitBoardS& rhs){
	return ! operator==(lhs, rhs);
}

#endif