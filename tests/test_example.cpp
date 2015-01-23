
#include "../bitscan.h"
#include "google/gtest/gtest.h"
#include <iostream>

using namespace std;

TEST(Example, scanning){
	vector<int> res;
	int nBit=EMPTY_ELEM;
	BITBOARD bbn(0x100);
	
	sparse_bitarray mysb(100);
	mysb.set_bit(50);
	mysb.set_bit(95);
	
	bitarray myb(100);
	myb.set_bit(50);
	myb.set_bit(95);
	
	watched_bitarray mywb(100);
	mywb.set_bit(50);
	mywb.set_bit(95);
	
	simple_bitarray mysimple(100);
	mysimple.set_bit(50);
	mysimple.set_bit(95);
	
	//scanning in simple_array
	while(true){
		nBit=mysimple.next_bit(nBit);
		if(nBit==EMPTY_ELEM)
			break;
		res.push_back(nBit);
	}

	EXPECT_EQ(mysimple.popcn64(),res.size());

	//scanning from a specific position in simple_array
	res.clear();
	nBit=49;
	while(true){
		nBit=mysimple.next_bit(nBit);
		if(nBit==EMPTY_ELEM)
			break;
		res.push_back(nBit);
	}
	EXPECT_EQ(1, count(res.begin(), res.end(),50));

	
	//scanning watched_array
	res.clear();
	mywb.update_sentinels();				
	mywb.init_scan(bbo::NON_DESTRUCTIVE);
	while(true){
		nBit=mywb.next_bit();
		if(nBit==EMPTY_ELEM)
			break;
		res.push_back(nBit);
	}
	EXPECT_EQ(mywb.popcn64(),res.size());

	//scanning sparse array
	res.clear();
	if(mysb.init_scan(bbo::NON_DESTRUCTIVE)!=EMPTY_ELEM){
		while(true){
			nBit=mysb.next_bit();
			if(nBit==EMPTY_ELEM)
				break;
			res.push_back(nBit);
		}
	}
	EXPECT_EQ(mysb.popcn64(),res.size());

	
	//scanning from a specific position in sparse array
	res.clear();
	if(mysb.init_scan_from(51,bbo::NON_DESTRUCTIVE_REVERSE)!=EMPTY_ELEM){
		while(true){
			nBit=mysb.previous_bit();
			if(nBit==EMPTY_ELEM)
				break;
			res.push_back(nBit);
		}
	}

	EXPECT_EQ(1, count(res.begin(), res.end(),50));

	//scanning destructive sparse array
	res.clear();
	int pc=mysb.popcn64();
	cout<<mysb<<endl;
	if(mysb.init_scan(bbo::DESTRUCTIVE)!=EMPTY_ELEM){
		while(true){
			nBit=mysb.next_bit_del();
			if(nBit==EMPTY_ELEM)
				break;
			res.push_back(nBit);
		}
	}

	EXPECT_EQ(pc, res.size());
	EXPECT_EQ(0, mysb.popcn64());
}









