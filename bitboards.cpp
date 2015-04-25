// BitBoardS.cpp: implementation of the BitBoardS classd, wrapper for sparse bitstrings
//
//////////////////////////////////////////////////////////////////////

#include "bitboards.h"
#include "bbalg.h"
#include <iostream>
#include <sstream>

#include <cstdio>
#include <iterator>
 
using namespace std;

int BitBoardS::nElem=EMPTY_ELEM;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BitBoardS::BitBoardS(int size /*1 based*/, bool is_popsize ){
	(is_popsize)? m_MAXBB=INDEX_1TO1(size) : m_MAXBB=size;
	m_aBB.reserve(DEFAULT_CAPACITY);							//*** check efficiency
}

BitBoardS::BitBoardS (const BitBoardS& bbS){
//////////////
// copies state
	m_aBB=bbS.m_aBB;
	m_MAXBB=bbS.m_MAXBB;
}

int BitBoardS::init (int size, bool is_popsize){
///////////////
// allocates memory

	clear();
	(is_popsize)? m_MAXBB=INDEX_1TO1(size) : m_MAXBB=size;
	m_aBB.reserve(DEFAULT_CAPACITY);

return 0;	//for API compatibility
}

void  BitBoardS::clear(){
	m_aBB.clear();
	m_MAXBB=EMPTY_ELEM;
}

void BitBoardS::sort (){
	std::sort(m_aBB.begin(), m_aBB.end(), elem_less());
}	

int	 BitBoardS::set_bit	(int low, int high){
///////////////////
// sets bits to one in the corresponding CLOSED range
//
// REMARKS: 
// 1.only one binary search is performed for the lower block
// 2.does not check maximum possible size
//
//*** OPTIMIZATION: restrict sorting when required to [low,high] interval
			
	int	bbh=WDIV(high); 
	int bbl=WDIV(low); 
	bool req_sorting=false;
	//m_aBB.reserve(m_aBB.size()+high-low+1);					//maximum possible size, to push_back in place without allocation
	velem vapp;

	//checks consistency (ASSERT)
	if(bbh<bbl || bbl<0 || low>high || low<0){
		cerr<<"Error in set bit in range"<<endl;
		return -1;
	}

	//A) bbh==bbl
	pair<bool,velem_it> p=find_block(bbl);
	if(bbh==bbl){
		if(p.first){
			BITBOARD bb_high=p.second->bb| ~Tables::mask_left[high-WMUL(bbh)];
			BITBOARD bb_low=p.second->bb| ~Tables::mask_right[low-WMUL(bbl)];
			p.second->bb=bb_low & bb_high;
			return 0;		//no need for sorting
		}else{
			BITBOARD bb_high= ~Tables::mask_left[high-WMUL(bbh)];
			m_aBB.push_back(elem(bbl,bb_high&~ Tables::mask_right[low-WMUL(bbl)]));
			return 0;
		}
	}

	//B) bbl outside range 
	if(p.second==m_aBB.end()){
		//append blocks at the end
		m_aBB.push_back(elem(bbl,~Tables::mask_right[low-WMUL(bbl)]));
		for(int i=low+1; i<bbh; ++i){
				m_aBB.push_back(elem(i,ONE));
		}
		m_aBB.push_back(elem(bbh,~Tables::mask_left[high-WMUL(bbh)]));
		return 0;
	}
	

	//C) bbl  
	int block=bbl;
	if(p.first){	//block exists	
		p.second->bb|=~Tables::mask_right[low-WMUL(bbl)];
		++p.second;
	}else{ //block does not exist:marked for append
		vapp.push_back(elem(bbl,~Tables::mask_right[low-WMUL(bbl)]));
	}
	++block;
	
	//D) Remaining blocks
	while(true){
		//exit condition I
		if(p.second==m_aBB.end()){
			//append blocks at the end
			for(int i=block; i<bbh; ++i){
				m_aBB.push_back(elem(i,ONE));
			}
			m_aBB.push_back(elem(bbh,~Tables::mask_left[high-WMUL(bbh)]));
			req_sorting=true;
			break;
		}

		//exit condition II
		if(block==bbh){
			if(p.second->index==block){ //block exists: trim and overwrite
				p.second->bb|=~Tables::mask_left[high-WMUL(bbh)];
				break;
			}else{ //block doesn't exist, trim and append
				m_aBB.push_back(elem(bbh,~Tables::mask_left[high-WMUL(bbh)]));
				req_sorting=true;
				break;
			}
		}
						

		//update before either of the bitstrings has reached its end
		if(p.second->index==block){
			p.second->bb=ONE;
			++p.second; ++block; 
		}else if(block<p.second->index){				//m_aBB[pos].index<block cannot occur
			vapp.push_back(elem(block,ONE));			//not added in place to avoid loosing indexes
		//	m_aBB.push_back(elem(block,ONE));
			req_sorting=true;
			++block;
		}
	}

	//sorts if necessary
	m_aBB.insert(m_aBB.end(), vapp.begin(), vapp.end());
	if(req_sorting) 
				sort();				

return 0;
}

int BitBoardS::init_bit (int low, int high){
/////////////////////////
// sets bits in the closed range and clears the rest
	
	//*** ASSERT (MAX SIZE)  

	int	bbh=WDIV(high); 
	int bbl=WDIV(low);
	m_aBB.clear();
	
	//same bitblock
	if(bbh==bbl){
		BITBOARD bb= ~Tables::mask_right[low-WMUL(bbl)];
		m_aBB.push_back(elem(bbl, bb & ~Tables::mask_left[high-WMUL(bbh)]));
		return 0;
	}

	//first
	m_aBB.push_back(elem(bbl, ~Tables::mask_right[low-WMUL(bbl)]));

	//middle
	for(int block=bbl+1; block<bbh; ++block){
		m_aBB.push_back(elem(block,ONE));
	}

	//last
	m_aBB.push_back(elem(bbh, ~Tables::mask_left[high-WMUL(bbh)]));
	
return 0;
}



BitBoardS& BitBoardS::set_bit (const BitBoardS& rhs){
/////////////////////////////////
// sets 1-bits in rhs (equivalent to an |=)
//
// REMARKS: experimental, currently only defined for bit strings of same maximum size

	if(rhs.is_empty()) 	return *this;
	//m_aBB.reserve(rhs.m_aBB.size()+m_aBB.size());		//maximum possible size, to push_back in place without allocation
	velem vapp;
	velem_cit i2=rhs.m_aBB.cbegin();
	velem_it i1=m_aBB.begin();
	
	
	bool req_sorting;
	while(true){
		//exit condition I
		if( i2==rhs.m_aBB.end() ){		
					break;		
		}

		//exit condition II
		if(i1==m_aBB.end()){
			//append blocks at the end
			m_aBB.insert(m_aBB.end(),i2, rhs.m_aBB.end());
			req_sorting=true;
			break;
		}
		
		//update before either of the bitstrings has reached its end
		if(i1->index==i2->index){
			i1->bb|=i2->bb;
			++i1, ++i2; 
		}else if(i1->index<i2->index){
			++i1;
		}else if(i2->index<i1->index){
			vapp.push_back(*i2);
			//m_aBB.push_back(*i2);
			req_sorting=true;
			++i2;
		}
	}

	//always keep array sorted
	m_aBB.insert(m_aBB.end(), vapp.begin(), vapp.end());
	if(req_sorting)
				sort();

return *this;		
}

BitBoardS&  BitBoardS::set_block (int first_block, const BitBoardS& rhs){
/////////////////////////////////
//
// REMARKS: experimental, currently only defined for bit strings of same size

	//m_aBB.reserve(rhs.m_aBB.size()+m_aBB.size());						//maximum possible size, to push_back in place without allocation
	velem vapp;
	pair<bool, BitBoardS::velem_it> p1=find_block(first_block);
	pair<bool, BitBoardS::velem_cit> p2=rhs.find_block(first_block);
	bool req_sorting=false;
		

	if(p2.second==rhs.m_aBB.end()){		//check in this order (captures rhs empty on init)
		return *this;
	}

	if(p1.second==m_aBB.end()){
		//append rhs at the end
		m_aBB.insert(m_aBB.end(),p2.second, rhs.m_aBB.end());	
		req_sorting=true;
		sort();
		return *this;
	}
			
	while(true){
		if(p2.second==rhs.m_aBB.end()){	//exit condition I
			break;
		}else if(p1.second==m_aBB.end()){	 //exit condition II  		
			m_aBB.insert(m_aBB.end(),p2.second, rhs.m_aBB.end());
			req_sorting=true;
			break;			
		}

		//update before either of the bitstrings has reached its end
		if(p1.second->index==p2.second->index){
			p1.second->bb|=p2.second->bb;
			++p1.second, ++p2.second; 
		}else if(p1.second->index<p2.second->index){
			++p1.second;
		}else if(p2.second->index<p1.second->index){
			vapp.push_back(*p2.second);	
			//m_aBB.push_back(*p2.second);		
			req_sorting=true;
			++p2.second;
		}
	}

	//always keep array sorted
	m_aBB.insert(m_aBB.end(), vapp.begin(), vapp.end());
	if(req_sorting)
				sort();
	
return *this;		
}

BitBoardS&  BitBoardS::set_block (int first_block, int last_block, const BitBoardS& rhs){
/////////////////////////////////
//
// REMARKS: experimental, currently only defined for bit strings of same size 
	
	//m_aBB.reserve(m_aBB.size()+ last_block-first_block+1);						//maximum possible size, to push_back in place without allocation
	velem vapp;
	pair<bool, BitBoardS::velem_it> p1i=find_block(first_block);
	pair<bool, BitBoardS::velem_cit> p2i=rhs.find_block(first_block);
	pair<bool, BitBoardS::velem_cit> p2f=rhs.find_block(last_block);
	bool req_sorting=false;
		
	if(p2i.second==rhs.m_aBB.end()){ //check in this order (captures rhs empty on init)
		return *this;
	}

	BitBoardS::velem_cit p2it_end=(p2f.first)? p2f.second+1 : p2f.second;			//iterator to the last block +1 in the rhs
	if(p1i.second==m_aBB.end()){
		//append rhs at the end
		m_aBB.insert(m_aBB.end(),p2i.second, p2it_end);	
		sort();
		return *this;
	}

	do{
		//update before either of the bitstrings has reached its end
		if(p1i.second->index==p2i.second->index){
			p1i.second->bb|=p2i.second->bb;
			++p1i.second, ++p2i.second; 
		}else if(p1i.second->index<p2i.second->index){
			++p1i.second;
		}else if(p2i.second->index<p1i.second->index){
			vapp.push_back(*p2i.second);
			//m_aBB.push_back(*p2i.second);		//*** iterators!!!
			req_sorting=true;
			++p2i.second;
		}
						
		//exit condition   
		if( p1i.second==m_aBB.end() || p1i.second->index>last_block ){			
			m_aBB.insert(m_aBB.end(),p2i.second, (p2f.first)? p2f.second+1 : p2f.second);
			req_sorting=true;
			break;			
		}else if(p2i.second == p2it_end ){			//exit condition II
			break;
		}

	}while(true);

	//always keep array sorted
	m_aBB.insert(m_aBB.end(), vapp.begin(), vapp.end());
	if(req_sorting)
				sort();
	
return *this;		
}



int BitBoardS::clear_bit (int low, int high){
///////////////////
// clears bits in the corresponding CLOSED range (effectively deletes bitblocks)
//
// PARTICULAR CASES: low = EMPTY_ELEM, high= EMPTY_ELEM mark bitstring bounds
//
// EXPERIMENTAL
	
	
	int bbl=EMPTY_ELEM, bbh=EMPTY_ELEM; 
	pair<bool, BitBoardS::velem_it> pl;
	pair<bool, BitBoardS::velem_it> ph;

////////////////////////
//special cases
	if(high==EMPTY_ELEM && low==EMPTY_ELEM){
		m_aBB.clear();
		return 0;
	}

	if(high==EMPTY_ELEM){
		bbl=WDIV(low);
		pl=find_block(bbl);
		if(pl.second==m_aBB.end()) return 0;

		if(pl.first){	//lower block exists
			pl.second->bb&=Tables::mask_right[low-WMUL(bbl)];
			++pl.second;
		}

		//remaining
		m_aBB.erase(pl.second, m_aBB.end());
		return 0;
	}else if(low==EMPTY_ELEM){
		bbh=WDIV(high); 
		ph=find_block(bbh);
		if(ph.first){	//upper block exists
			ph.second->bb&=Tables::mask_left[high-WMUL(bbh)];
		}

		//remaining
		m_aBB.erase(m_aBB.begin(), ph.second);
		return 0;
	}

////////////////
// general cases

	//check consistency
	if(low>high){
		cerr<<"Error in set bit in range"<<endl;
		return -1;
	}
		

	bbl=WDIV(low);
	bbh=WDIV(high); 
	pl=find_block(bbl);
	ph=find_block(bbh);	


	//tratamiento
	if(pl.second!=m_aBB.end()){
		//updates lower bitblock
		if(pl.first){	//lower block exists
			if(bbh==bbl){		//case update in the same bitblock
				BITBOARD bb_low=pl.second->bb & Tables::mask_left[high-WMUL(bbh)];
				BITBOARD bb_high=pl.second->bb &Tables::mask_right[low-WMUL(bbl)];
				pl.second->bb=bb_low | bb_high;
				return 0;
			}

			//update lower block
			pl.second->bb&=Tables::mask_right[low-WMUL(bbl)];
			++pl.second;
		}

		//updates upper bitblock
		if(ph.first){	//lower block exists
			if(bbh==bbl){		//case update in the same bitblock
				BITBOARD bb_low=pl.second->bb & Tables::mask_left[high-WMUL(bbh)];
				BITBOARD bb_high=pl.second->bb &Tables::mask_right[low-WMUL(bbl)];
				pl.second->bb=bb_low | bb_high;
				return 0;
			}

			//update lower block
			ph.second->bb&=Tables::mask_left[high-WMUL(bbh)];
		}

		//remaining
		m_aBB.erase(pl.second, ph.second);
	}

	
return 0;
}


BitBoardS&  BitBoardS::erase_bit (const BitBoardS& rhs ){
////////////////////
// removes 1-bits from current object (equialent to set_difference)
	int i1=0, i2=0;
	while(true){
		//exit condition I
		if(i1==m_aBB.size() || i2==rhs.m_aBB.size() ){		//size should be the same
					return *this;
		}

		//update before either of the bitstrings has reached its end
		if(m_aBB[i1].index==rhs.m_aBB[i2].index){
			m_aBB[i1].bb&=~rhs.m_aBB[i2].bb;
			i1++, i2++; 
		}else if(m_aBB[i1].index<rhs.m_aBB[i2].index){
			i1++;
		}else if(rhs.m_aBB[i2].index<m_aBB[i1].index){
			i2++;
		}
	}

return *this;
}

BitBoardS& BitBoardS::operator &= (const BitBoardS& rhs){
///////////////////
// AND mask in place

	int i1=0, i2=0;
	while(true){
		//exit conditions 
		if(i1==m_aBB.size() ){		//size should be the same
					return *this;
		}else if(i2==rhs.m_aBB.size()){  //fill with zeros from last block in rhs onwards
			for(; i1<m_aBB.size(); i1++)
				m_aBB[i1].bb=ZERO;
			return *this;
		}

		//update before either of the bitstrings has reached its end
		if(m_aBB[i1].index<rhs.m_aBB[i2].index){
			m_aBB[i1].bb=0;
			i1++;
		}else if (rhs.m_aBB[i2].index<m_aBB[i1].index){
			i2++;
		}else{
			m_aBB[i1].bb &= rhs.m_aBB[i2].bb;
			i1++, i2++; 
		}

		/*if(m_aBB[i1].index==rhs.m_aBB[i2].index){
			m_aBB[i1].bb &= rhs.m_aBB[i2].bb;
			i1++, i2++; 
		}else if(m_aBB[i1].index<rhs.m_aBB[i2].index){
			m_aBB[i1].bb=0;
			i1++;
		}else if(rhs.m_aBB[i2].index<m_aBB[i1].index){
			i2++;
		}*/
	}

return *this;
}

BitBoardS& BitBoardS::operator |= (const BitBoardS& rhs){
///////////////////
// OR mask in place
// date:10/02/2015
// last_update: 10/02/2015

	int i1=0, i2=0;
	while(true){
		//exit conditions 
		if(i1==m_aBB.size() || i2==rhs.m_aBB.size() ){				//size should be the same
					return *this;
		}

		//update before either of the bitstrings has reached its end
		if(m_aBB[i1].index<rhs.m_aBB[i2].index){
			i1++;
		}else if(rhs.m_aBB[i2].index<m_aBB[i1].index){
			i2++;
		}else{
			m_aBB[i1].bb |= rhs.m_aBB[i2].bb;
			i1++, i2++; 
		}
	}

return *this;
}



BITBOARD BitBoardS::find_bitboard (int block_index) const{
///////////////////
// returns the bitblock of the block index or EMPTY_ELEM if it does not exist
	velem_cit it=lower_bound(m_aBB.begin(), m_aBB.end(), elem(block_index), elem_less());
	if(it!=m_aBB.end()){
		if(it->index==block_index){
			return it->bb;
		}
	}
return EMPTY_ELEM;
}

pair<bool, int>	BitBoardS::find_pos (int block_index) const{
////////////////
// returns first:true if block exists second:lower bound index in the collection or EMPTY_ELEM if no block exists above the index
	pair<bool, int> res(false, EMPTY_ELEM);
	velem_cit it=lower_bound(m_aBB.begin(), m_aBB.end(), elem(block_index), elem_less());
	if(it!=m_aBB.end()){
		res.second=it-m_aBB.begin();
		if(it->index==block_index){
			res.first=true;
		}
	}
return res;
}

pair<bool, BitBoardS::velem_it> BitBoardS::find_block (int block_index, bool is_lower_bound) 	{
	pair<bool, BitBoardS::velem_it>res;
	if(is_lower_bound)
		res.second=lower_bound(m_aBB.begin(), m_aBB.end(), elem(block_index), elem_less());
	else
		res.second=upper_bound(m_aBB.begin(), m_aBB.end(), elem(block_index), elem_less());
	res.first= (res.second!=m_aBB.end()) && (res.second->index==block_index);
return res;
}

pair<bool, BitBoardS::velem_cit> BitBoardS::find_block (int block_index, bool is_lower_bound) const 	{
	pair<bool, BitBoardS::velem_cit>res;
	if(is_lower_bound)
		res.second=lower_bound(m_aBB.begin(), m_aBB.end(), elem(block_index), elem_less());
	else
		res.second=upper_bound(m_aBB.begin(), m_aBB.end(), elem(block_index), elem_less());
	res.first= res.second!=m_aBB.end() && res.second->index==block_index;
return res;
}

///////////////////////////////
//
// Bit updates
//
//
////////////////////////////////

int BitBoardS::lsbn64() const{
/////////////////
// different implementations of lsbn depending on configuration

#ifdef DE_BRUIJN
	for(int i=0; i<m_aBB.size(); i++){
		if(m_aBB[i].bb)
#ifdef ISOLANI_LSB
			return(Tables::indexDeBruijn64_ISOL[((m_aBB[i].bb & -m_aBB[i].bb) * DEBRUIJN_MN_64_ISOL/*magic num*/) >> DEBRUIJN_MN_64_SHIFT]+ WMUL(m_aBB[i].index));	
#else
			return(Tables::indexDeBruijn64_SEP[((m_aBB[i].bb^ (m_aBB[i].bb-1)) * DEBRUIJN_MN_64_SEP/*magic num*/) >> DEBRUIJN_MN_64_SHIFT]+ WMUL(m_aBB[i].index));	
#endif
	}
#elif LOOKUP
	union u {
		U16 c[4];
		BITBOARD b;
	};

	u val;

	for(int i=0; i<m_nBB; i++){
		val.b=m_aBB[i].bb;
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



 int BitBoardS::msbn64() const{
///////////////////////
// Look up table implementation (best found so far)

	union u {
		U16 c[4];
		BITBOARD b;
	};

	u val;

	for(int i=m_aBB.size()-1; i>=0; i--){
		val.b=m_aBB[i].bb;
		if(val.b){
			if(val.c[3]) return (Tables::msba[3][val.c[3]]+WMUL(m_aBB[i].index));
			if(val.c[2]) return (Tables::msba[2][val.c[2]]+WMUL(m_aBB[i].index));
			if(val.c[1]) return (Tables::msba[1][val.c[1]]+WMUL(m_aBB[i].index));
			if(val.c[0]) return (Tables::msba[0][val.c[0]]+WMUL(m_aBB[i].index));
		}
	}

return EMPTY_ELEM;		//should not reach here
}
 

int BitBoardS::next_bit(int nBit/* 0 based*/)  const {
////////////////////////////
// RETURNS next bit from nBit in the bitstring (to be used in a bitscan loop)
//			 if nBit is FIRST_BITSCAN returns lsb
//
// NOTES: 
// 1. A  preliminay implementation. It becomes clear the necessity of caching the index of the vector at each iteration (*)
	
	
	if(nBit==EMPTY_ELEM){
		return lsbn64();
	}
		
	//look in the remaining bitblocks (needs caching last i visited)
	int index=WDIV(nBit);
	for(int i=0; i<m_aBB.size(); i++){
		if(m_aBB[i].index<index) continue;					//(*)
		if(m_aBB[i].index==index){
			int npos=BitBoard::lsb64_de_Bruijn(Tables::mask_left[WMOD(nBit) /*-WORD_SIZE*index*/] & m_aBB[i].bb);
			if(npos!=EMPTY_ELEM) return (WMUL(index) + npos);
			continue;
		}
		//new bitblock
		if( m_aBB[i].bb){
			return BitBoard::lsb64_de_Bruijn(m_aBB[i].bb) + WMUL(m_aBB[i].index);
		}
	}
	
return -1;	
}




int BitBoardS::previous_bit(int nBit/* 0 bsed*/) const{
////////////////////////////
// Gets the previous bit to nBit. 
// If nBits is FIRST_BITSCAN is a MSB
//
// NOTES: 
// 1. A  preliminary implementation. It becomes clear the necessity of caching the index of the vector at each iteration (*)
		
	if(nBit==EMPTY_ELEM)
				return msbn64();
		
	//look in the remaining bitblocks
	int index=WDIV(nBit);
	for(int i=m_aBB.size()-1; i>=0; i--){
		if(m_aBB[i].index>index) continue;							//(*)
		if(m_aBB[i].index==index){
			int npos=BitBoard::msb64_lup(Tables::mask_right[WMOD(nBit) /*-WORD_SIZE*index*/] & m_aBB[i].bb);
			if(npos!=EMPTY_ELEM) return (WMUL(index) + npos);
			continue;
		}
		if(m_aBB[i].bb ){
			return BitBoard::msb64_lup(m_aBB[i].bb) + WMUL(m_aBB[i].index);
		}
	}
	
return -1;	
}


//////////////
//
// Operators
//
///////////////
BitBoardS& BitBoardS::operator = (const BitBoardS& bbs){
///////////////
// Deep copy of collection 

	if(this!=&bbs){
		this->m_aBB=bbs.m_aBB;
		this->m_MAXBB=bbs.m_MAXBB;
	}

return *this;
}









BitBoardS&  OR	(const BitBoardS& lhs, const BitBoardS& rhs,  BitBoardS& res){
///////////////////////////
// OR between sparse sets out of place (does not requires sorting)
// 	
// REMARKS: does not check for null information

	int i1=0, i2=0;
	res.erase_bit();		//experimental (and simplest solution)
	while(true){
		//exit condition I
		if(i1==lhs.m_aBB.size()){
			res.m_aBB.insert(res.m_aBB.end(), rhs.m_aBB.begin()+i2, rhs.m_aBB.end());
			return res;
		}else if(i2==rhs.m_aBB.size()){
			res.m_aBB.insert(res.m_aBB.end(), lhs.m_aBB.begin()+i1, lhs.m_aBB.end());
			return res;
		}
		
		//update before either of the bitstrings has reached its end
		if(lhs.m_aBB[i1].index<rhs.m_aBB[i2].index){
			BitBoardS::elem e(lhs.m_aBB[i1].index, lhs.m_aBB[i1].bb );
			res.m_aBB.push_back(e);
			++i1;
		}else if(rhs.m_aBB[i2].index<lhs.m_aBB[i1].index){
			BitBoardS::elem e(rhs.m_aBB[i2].index, rhs.m_aBB[i2].bb );
			res.m_aBB.push_back(e);
			++i2;
		}else{
			BitBoardS::elem e(lhs.m_aBB[i1].index, lhs.m_aBB[i1].bb | rhs.m_aBB[i2].bb);
			res.m_aBB.push_back(e);
			++i1, ++i2; 
		}

	/*	if(lhs.m_aBB[i1].index==rhs.m_aBB[i2].index){
			BitBoardS::elem e(lhs.m_aBB[i1].index, lhs.m_aBB[i1].bb | rhs.m_aBB[i2].bb);
			res.m_aBB.push_back(e);
			++i1, ++i2; 
		}else if(lhs.m_aBB[i1].index<rhs.m_aBB[i2].index){
			BitBoardS::elem e(lhs.m_aBB[i1].index, lhs.m_aBB[i1].bb );
			res.m_aBB.push_back(e);
			++i1;
		}else if(rhs.m_aBB[i2].index<lhs.m_aBB[i1].index){
			BitBoardS::elem e(rhs.m_aBB[i2].index, rhs.m_aBB[i2].bb );
			res.m_aBB.push_back(e);
			++i2;
		}*/
	}
return res;
}


////////////////////////////
////
//// I/O FILES
////
////////////////////////////

void BitBoardS::print (std::ostream& o, bool show_pc ) const  {
/////////////////////////
// shows bit string as [bit1 bit2 bit3 ... <(pc)>]  (if empty: [ ]) (<pc> optional)
	
	o<<"[";
		
	int nBit=EMPTY_ELEM;
	while(1){
		nBit=next_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		o<<nBit<<" ";
	}

	if(show_pc){
		int pc=popcn64();
		if(pc)	o<<"("<<popcn64()<<")";
	}
	
	o<<"]";
}

string BitBoardS::to_string (){
	ostringstream sstr;
	sstr<<"[";
	this->print();
	int nBit=EMPTY_ELEM;
	while(true){
		nBit=next_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		sstr<<nBit<<" ";
	}

	sstr<<"("<<popcn64()<<")";
	sstr<<"]";

return sstr.str();
}

void BitBoardS::to_vector (std::vector<int>& vl)const{
//////////////////////
// copies bit string to vector 
//
	vl.clear();

	int v=EMPTY_ELEM;
	while(true){
		v=next_bit(v);
		if(v==EMPTY_ELEM) break;

		vl.push_back(v);
	}
}

