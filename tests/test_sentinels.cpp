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
	//bbs.update_sentinels();
	//EXPECT_EQ(0,bbs.get_sentinel_L());
	//EXPECT_EQ(0,bbs.get_sentinel_H());

	////equality
	//BBSentinel bbs1(200);
	//bbs1=bbs;
	//EXPECT_EQ(bbs.get_sentinel_L(),bbs1.get_sentinel_L());
	//EXPECT_EQ(bbs.get_sentinel_H(),bbs1.get_sentinel_H());
	
	cout<<"--------------------------------------------------"<<endl;
}