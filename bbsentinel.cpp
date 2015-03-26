
#include "bbsentinel.h"
#include <iostream>

using namespace std;

void BBSentinel::init_sentinels(){
////////////////
//sets sentinels to maximum scope of current bit string
	m_BBL=0;
	m_BBH=m_nBB-1;
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

int BBSentinel::update_sentinels(int low, int high){
//////////////////////
//  last upgrade:  26/3/12 
//  COMMENTS: Bounding condition can be improved by assuming that the bitstring have a null table at the end
//  C-Char Style!

	//empty check 
	if(low==EMPTY_ELEM || high==EMPTY_ELEM) return EMPTY_ELEM;

	m_BBL=low;
	m_BBH=high;

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
	o<<"Low: "<<m_BBL<<" High: "<<m_BBH<<" ";
}

//specializes the only bitscan function used
int BBSentinel::previous_bit_del(){
//////////////
// BitScan reverse order and distructive
//
// COMMENTS
// 1- update sentinels at the start of loop

	unsigned long posInBB;

	for(int i=m_BBH; i>=m_BBL; i--){
		if(_BitScanReverse64(&posInBB,m_aBB[i])){
			m_BBH=i;
			m_aBB[i]&=~Tables::mask[posInBB];			//erase before the return
			return (posInBB+WMUL(i));
		}
	}
return EMPTY_ELEM;  
}

int BBSentinel::next_bit_del (){
//////////////
// Bitscan distructive between sentinels
//
// COMMENTS
// 1- update sentinels at the start of loop

	unsigned long posInBB;

	for(int i=m_scan.bbi; i<=m_BBH; i++){
		if(_BitScanForward64(&posInBB,m_aBB[i]) ){
			m_BBL=i;
			m_aBB[i]&=~Tables::mask[posInBB];					//erase before the return
			return (posInBB+ WMUL(i));
		}
	}
return EMPTY_ELEM;  
}

int BBSentinel::next_bit_del (BBSentinel& bbN_del){
//////////////
// Bitscan distructive between sentinels
//
// COMMENTS
// 1- update sentinels at the start of loop

	unsigned long posInBB;

	for(int i=m_scan.bbi; i<=m_BBH; i++){
		if(_BitScanForward64(&posInBB, m_aBB[i]) ){
			m_scan.bbi;
			m_aBB[i]&=~Tables::mask[posInBB];					//erase before the return
			bbN_del.m_aBB[i]&=~Tables::mask[posInBB];
			return (posInBB+ WMUL(i));
		}
	}
	
return EMPTY_ELEM;  
}

int BBSentinel::next_bit_del (int& nBB, BBSentinel& bbN_del){
//////////////
// Bitscan distructive between sentinels
//
// COMMENTS
// 1- update sentinels at the start of loop

	unsigned long posInBB;

	for(int i=m_scan.bbi; i<=m_BBH; i++){
		if(_BitScanForward64(&posInBB, m_aBB[i]) ){
			m_scan.bbi=i;
			nBB=i;
			m_aBB[i]&=~Tables::mask[posInBB];					//erase before the return
			bbN_del.m_aBB[i]&=~Tables::mask[posInBB];
			return (posInBB+ WMUL(i));
		}
	}
	
return EMPTY_ELEM; 

}

int BBSentinel::next_bit(){
////////////////////////////
// last update:31/12/2013
// BitScan non destructive
//
// COMMENTS
// 1- update sentinels, set m_scan.bbi to m_BBL and set m_scan.pos to MASK_LIM at the start of loop

	unsigned long posInBB;
				
	if(_BitScanForward64(&posInBB, m_aBB[m_scan.bbi] & Tables::mask_left[m_scan.pos])){
		m_scan.pos =posInBB;
		return (posInBB + WMUL(m_scan.bbi));
	}else{													
		for(int i=m_scan.bbi+1; i<=m_BBH; i++){
			if(_BitScanForward64(&posInBB,m_aBB[i])){
				m_scan.bbi=i;
				m_scan.pos=posInBB;
				return (posInBB+ WMUL(i));
			}
		}
	}
return EMPTY_ELEM;
}

int BBSentinel::next_bit(int& nBB){
////////////////////////////
// last update:31/12/2013
// BitScan non destructive
//
// COMMENTS
// 1- update sentinels, set m_scan.bbi to m_BBL and set m_scan.pos to MASK_LIM at the start of loop

	unsigned long posInBB;
			
	//look uo in the last table
	if(_BitScanForward64(&posInBB, m_aBB[m_scan.bbi] & Tables::mask_left[m_scan.pos])){
		m_scan.pos =posInBB;
		nBB=m_scan.bbi;
		return (posInBB + WMUL(m_scan.bbi));
	}else{											//not found in the last table. look up in the rest
		for(int i=(m_scan.bbi+1); i<=m_BBH; i++){
			if(_BitScanForward64(&posInBB,m_aBB[i])){
				m_scan.bbi=i;
				m_scan.pos=posInBB;
				nBB=i;
				return (posInBB+ WMUL(i));
			}
		}
	}
return EMPTY_ELEM;
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
	case DESTRUCTIVE:
		update_sentinels();
		m_scan.bbi=m_BBL;
		break;
	case DESTRUCTIVE_REVERSE:
		update_sentinels();
		m_scan.bbi=m_BBH;
		break;
	default:
		cerr<<"bad scan type"<<endl;
		return -1;
	}
return 0;
}
