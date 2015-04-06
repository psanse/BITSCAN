#include <algorithm>
#include <iterator>
#include <iostream>
#include <set>

#include "../bitscan.h"				//bit string library
#include "google/gtest/gtest.h"
#include "../bitboardn.h"

using namespace std;

TEST(Sentinel, basic){
	cout<<"Sentinel:basic-------------------------------------"<<endl;
	BBSentinel bbs(200);
	bbs.set_bit(10);
	bbs.set_bit(20);
	bbs.set_bit(150);
	
	bbs.update_sentinels();
	EXPECT_EQ(0,bbs.get_sentinel_L());
	EXPECT_EQ(2,bbs.get_sentinel_H());
	EXPECT_EQ(3,bbs.popcn64());


	bbs.erase_bit(20);
	bbs.erase_bit(150);
	bbs.print();
	
	bbs.update_sentinels();
	EXPECT_EQ(0,bbs.get_sentinel_L());
	EXPECT_EQ(0,bbs.get_sentinel_H());

	//redefiniton of equality (same sentinels and bitblocks in sentinel range)
	BBSentinel bbs1(200);
	bbs1=bbs;
	EXPECT_EQ(bbs.get_sentinel_L(),bbs1.get_sentinel_L());
	EXPECT_EQ(bbs.get_sentinel_H(),bbs1.get_sentinel_H());
	EXPECT_TRUE(bbs1.is_bit(10));

	//bit deletion
	bbs1.erase_bit();									//in sentinel range
	EXPECT_EQ(EMPTY_ELEM,bbs1.update_sentinels());
	bbs1.init_sentinels(true);
	EXPECT_EQ(EMPTY_ELEM,bbs1.get_sentinel_L());
	EXPECT_EQ(EMPTY_ELEM,bbs1.get_sentinel_H());

	//AND
	bbs1=bbs;
	bbs1.set_bit(27);
	BBSentinel bbs2(200);
	AND(bbs1, bbs, bbs2);
	EXPECT_TRUE(bbs2.is_bit(10));
	bbs2.update_sentinels();
	EXPECT_EQ(1, bbs2.popcn64());

	//erase_bit and update
	bbs.init_sentinels();
	bbs.erase_bit();
	bbs.set_bit(10);
	bbs.set_bit(20);
	bbs.set_bit(150);
	bbs.update_sentinels();
	bbs.erase_bit_and_update(150);						//erases and updates sentinels		
	EXPECT_EQ(0,bbs.get_sentinel_L());
	EXPECT_EQ(0,bbs.get_sentinel_H());

				
	cout<<"--------------------------------------------------"<<endl;
}