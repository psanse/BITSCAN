
#include "bbsentinel.h"
#include <iostream>

using namespace std;

BBSentinel&  AND (const BitBoardN& lhs, const BBSentinel& rhs,  BBSentinel& res){
	res.m_BBL=rhs.m_BBL;
	res.m_BBH=rhs.m_BBH;
	for(int i=rhs.m_BBL; i<=rhs.m_BBH; i++){
		res.m_aBB[i]=lhs.get_bitboard(i)&rhs.m_aBB[i];
	}
return res;
}

void BBSentinel::init_sentinels(bool update){
////////////////
//sets sentinels to maximum scope of current bit string
	m_BBL=0; 
	m_BBH=m_nBB-1;
	if(update) update_sentinels();
}

void BBSentinel::set_sentinels(int low, int high){
	m_BBL=low;
	m_BBH=high;
}

void BBSentinel::clear_sentinels(){
	m_BBL=EMPTY_ELEM;
	m_BBH=EMPTY_ELEM;
}

int BBSentinel::update_sentinels(){
///////////////
// Updates sentinels
//
// RETURN EMPTY_ELEM or 0 (ok)
	
	//empty check 
	if(m_BBL==EMPTY_ELEM || m_BBH==EMPTY_ELEM) return EMPTY_ELEM;

	//Low sentinel
	if(!m_aBB[m_BBL]){
		for(m_BBL=m_BBL+1; m_BBL<=m_BBH; m_BBL++){
			if(m_aBB[m_BBL])
					goto high;
		}
		m_BBL=EMPTY_ELEM;
		m_BBH=EMPTY_ELEM;
		return EMPTY_ELEM;
	}

	//High sentinel
high:	;
	if(!m_aBB[m_BBH]){
		for(m_BBH=m_BBH-1; m_BBH>=m_BBL; m_BBH--){
			if(m_aBB[m_BBH])
					return 0;
			}
		m_BBL=EMPTY_ELEM;
		m_BBH=EMPTY_ELEM;
		return EMPTY_ELEM;
	}
return 0;
}

int BBSentinel::update_sentinels(int bbl, int bbh){
//////////////////////
//  last upgrade:  26/3/12 
//  COMMENTS: Bounding condition can be improved by assuming that the bitstring have a null table at the end
//  C-Char Style!

	//empty check 
	if(bbl==EMPTY_ELEM || bbh==EMPTY_ELEM) return EMPTY_ELEM;

	m_BBL=bbl;
	m_BBH=bbh;

	//Low
	if(!m_aBB[m_BBL]){
		for(m_BBL=m_BBL+1; m_BBL<=m_BBH; m_BBL++){
			if(m_aBB[m_BBL])
					goto high;
		}
		m_BBL=EMPTY_ELEM;
		m_BBH=EMPTY_ELEM;
		return EMPTY_ELEM;
	}

	//High
high:	;
		if(!m_aBB[m_BBH]){
		for(m_BBH=m_BBH-1; m_BBH>=m_BBL; m_BBH--){
			if(m_aBB[m_BBH])
					return 0;
		}
		m_BBL=EMPTY_ELEM;
		m_BBH=EMPTY_ELEM;
		return EMPTY_ELEM;
	}
return 0;	
}

int BBSentinel::update_sentinels_high(){

	//empty check 
	if(m_BBH==EMPTY_ELEM) return EMPTY_ELEM;

	//Update High
	if(!m_aBB[m_BBH]){
		for(m_BBH=m_BBH-1; m_BBH>=m_BBL; m_BBH--){
			if(m_aBB[m_BBH])
					return 0;
		}

		m_BBL=EMPTY_ELEM;
		m_BBH=EMPTY_ELEM;
		return EMPTY_ELEM;
	}
return 0;	
}

int BBSentinel::update_sentinels_low(){

	//empty check
	if(m_BBL==EMPTY_ELEM) return EMPTY_ELEM ;

	//Update High
	if(!m_aBB[m_BBL]){
		for(m_BBL=m_BBL+1; m_BBL<=m_BBH; m_BBL++){
			if(m_aBB[m_BBL])
					return 0;
		}
		m_BBL=EMPTY_ELEM;
		m_BBH=EMPTY_ELEM;
		return EMPTY_ELEM;
	}
return 0;	
}

void BBSentinel::update_sentinels_to_v(int v){
///////////////////
// Adpats sentinels to vertex v
	int bb_index=WDIV(v);
	if(m_BBL==EMPTY_ELEM || m_BBH==EMPTY_ELEM){
		m_BBL=m_BBH=bb_index;
	}else if(m_BBL>bb_index){
		m_BBL=bb_index;
	}else if(m_BBH<bb_index){
		m_BBH=bb_index;
	}
}

void BBSentinel::print(std::ostream& o){
	BitBoardN::print(o);
	o<<"("<<m_BBL<<","<<m_BBH<<")";
}



int BBSentinel::init_scan(scan_types sct){
	switch(sct){
	case NON_DESTRUCTIVE:
		update_sentinels();
		m_scan.bbi=m_BBL;
		m_scan.pos=MASK_LIM; 
		break;
	case NON_DESTRUCTIVE_REVERSE:
		update_sentinels();
		m_scan.bbi=m_BBH;
		m_scan.pos=WORD_SIZE;		//mask_right[WORD_SIZE]=ONE
		break;
	case DESTRUCTIVE:				//uses sentinels to iterate and updates them on the fly
		update_sentinels();
		break;
	case DESTRUCTIVE_REVERSE:		//uses sentinels to iterate and updates them on the fly
		update_sentinels();
		break;
	default:
		cerr<<"bad scan type"<<endl;
		return -1;
	}
return 0;
}


void  BBSentinel::erase_bit	(){
///////////////
// Redefinition of emptyness: in the sentinel range
// 
// Remarks: To ensure empty bitblocks in all the range, call init_sentinels(false) first
	if(m_BBL==EMPTY_ELEM || m_BBH==EMPTY_ELEM) return; 

	for(int i=m_BBL; i<=m_BBH; i++)	
				m_aBB[i]=ZERO;
}


BBSentinel& BBSentinel::erase_bit (const BitBoardN& bbn){
//////////////////////////////
// deletes 1-bits in bbn in current sentinel range
// 
// REMARKS:
// 1.Has to be careful with BitBoardN cast to int in constructor
	
	for(int i=m_BBL; i<=m_BBH; i++)
		m_aBB[i] &= ~ bbn.get_bitboard(i);		//**access

return *this;
}

void  BBSentinel::erase_bit_and_update(int nBit) {
	if(m_BBL==EMPTY_ELEM || m_BBH==EMPTY_ELEM ) return;		//empty bitsring
	
	int bb=WDIV(nBit);
	m_aBB[bb] &= ~Tables::mask[WMOD(nBit)];

	//update watched literals if necessary
	if(!m_aBB[bb]){
		if(m_BBL==bb){
			for(m_BBL=m_BBL+1; m_BBL<=m_BBH; m_BBL++){
				if(m_aBB[m_BBL])
						return;
			}
			m_BBL=EMPTY_ELEM;
			m_BBH=EMPTY_ELEM;
			return;
		}
		
		if(m_BBH==bb){
			for(m_BBH=m_BBH-1; m_BBH>=m_BBL; m_BBH--){
				if(m_aBB[m_BBH])
						return;
			}
			m_BBL=EMPTY_ELEM;
			m_BBH=EMPTY_ELEM;
			return;

		}
	}
}	



bool BBSentinel::is_empty() const{
////////////////
// New definition of emptyness with sentinels

	return (m_BBL==EMPTY_ELEM || m_BBH==EMPTY_ELEM);
}

bool BBSentinel::is_empty (int nBBL, int nBBH) const{
	int bbl=max(nBBL, m_BBL);
	int bbh=min(nBBH, m_BBH);

	for(int i=bbl; i<=bbh; ++i)
			if(m_aBB[i]) return false;

return true;	
}

BBSentinel& BBSentinel::operator= (const  BBSentinel& bbs){
///////////////
// redefinition of equality: same sentinels of the copied bbs, same bitblocks in sentinel range

	m_BBL=bbs.m_BBL;
	m_BBH=bbs.m_BBH;

	for(int i=m_BBL; i<=m_BBH; i++){
		this->m_aBB[i]=bbs.m_aBB[i];
	}

	return *this;

}

BBSentinel& BBSentinel::operator&=	(const  BitBoardN& bbn){
//////////////////
// AND operation in the range of the sentinels

	for(int i=m_BBL; i<=m_BBH; i++){
		this->m_aBB[i] &= bbn.get_bitboard(i);
	}

	return  *this;
}

