//tests with masks for sparse and normal (non-sparse) graphs

#include <algorithm>
#include <iterator>
#include <iostream>
#include <set>

#include "../bitscan.h"					//bit string library
#include "google/gtest/gtest.h"

using namespace std;

TEST(Masks, AND_OR) {

//non sparse
	bitarray bb(130);
	bitarray bb1(130);
	bitarray bbresAND(130);
	bitarray bbresOR(130);
	
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	bb1.set_bit(10);
	bb1.set_bit(64);
	bb1.set_bit(100);

	//AND
	cout<<AND(bb, bb1, bbresAND)<<endl;
	EXPECT_TRUE(bbresAND.is_bit(10));
	EXPECT_TRUE(bbresAND.is_bit(64));
	EXPECT_EQ(2, bbresAND.popcn64());

	//OR
	cout<<OR(bb, bb1, bbresOR)<<endl;
	EXPECT_EQ(4, bbresOR.popcn64());

	//&=
	bitarray bb2(130);
	bitarray bb3(130);

	bb2.set_bit(10);
	bb2.set_bit(20);
	bb2.set_bit(64);

	bb3.set_bit(10);
	bb3.set_bit(64);
	bb3.set_bit(100);

	bb2&=bb3;
	EXPECT_TRUE(bbresAND.is_bit(10));
	EXPECT_TRUE(bbresAND.is_bit(64));
	EXPECT_EQ(2,bb2.popcn64());
	
}

TEST(Masks, AND_OR_sparse) {

//non sparse
	sparse_bitarray bb(130);
	sparse_bitarray bb1(130);
	sparse_bitarray bbresAND(130);
	sparse_bitarray bbresOR(130);
	
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	bb1.set_bit(10);
	bb1.set_bit(64);
	bb1.set_bit(100);

	//AND
	cout<<AND(bb, bb1, bbresAND)<<endl;
	EXPECT_TRUE(bbresAND.is_bit(10));
	EXPECT_TRUE(bbresAND.is_bit(64));
	EXPECT_EQ(2, bbresAND.popcn64());

	//OR
	cout<<OR(bb, bb1, bbresOR)<<endl;
	EXPECT_EQ(4, bbresOR.popcn64());

	//&=
	sparse_bitarray bb2(130);
	sparse_bitarray bb3(130);

	bb2.set_bit(10);
	bb2.set_bit(20);
	bb2.set_bit(64);

	bb3.set_bit(10);
	bb3.set_bit(64);
	bb3.set_bit(100);

	bb2&=bb3;
	EXPECT_TRUE(bb2.is_bit(10));
	EXPECT_TRUE(bb2.is_bit(64));
	EXPECT_EQ(2,bb2.popcn64());

}

TEST(Masks, set_bits){

	bitarray bb(130);
	bitarray bb1(130);
	bitarray bbres(130);
	
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	bb1.set_bit(10);
	bb1.set_bit(64);
	bb1.set_bit(100);
	
	//set range
	bb.set_block(0,bb1);
	EXPECT_TRUE(bb.is_bit(100));
	EXPECT_TRUE(bb.is_bit(20));
		
	bb1.set_bit(129);
	bb.set_block(0,1,bb1);
	EXPECT_FALSE(bb.is_bit(129));
		
	bb.set_block(0,2,bb1);
	EXPECT_TRUE(bb.is_bit(129));

	//erase range
	bb.erase_block(2, bb1);
	EXPECT_FALSE(bb.is_bit(129));

	bb.erase_block(1, bb1);
	EXPECT_FALSE(bb.is_bit(100));
	EXPECT_FALSE(bb.is_bit(64));

	cout<<bb.erase_block(0, bb1)<<endl;
	EXPECT_TRUE(bb.is_bit(20));
}

TEST(Masks, set_bits_sparse){	
	sparse_bitarray bb(130);
	sparse_bitarray bb1(130);
	sparse_bitarray bbres(130);
	
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	bb1.set_bit(10);
	bb1.set_bit(64);
	bb1.set_bit(100);
	
	//set range
	bb.set_block(0,bb1);
	EXPECT_TRUE(bb.is_bit(100));
	
	bb1.set_bit(129);
	bb.set_block(0,1,bb1);
	EXPECT_FALSE(bb.is_bit(129));

	bb.set_block(0,2,bb1);
	EXPECT_TRUE(bb.is_bit(129));

	//erase range
	cout<<bb<<endl;
	bb.erase_block(2, bb1);
	EXPECT_FALSE(bb.is_bit(129));

	bb.erase_block(1, bb1);
	EXPECT_FALSE(bb.is_bit(100));
	EXPECT_FALSE(bb.is_bit(64));

	cout<<bb.erase_block(0, bb1)<<endl;
	EXPECT_TRUE(bb.is_bit(20));
}

TEST(Masks, erase_bits){
	bitarray bb(130);
	bitarray bb1(130);
	bitarray bbres(130);
	
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	bb1.set_bit(10);
	bb1.set_bit(64);
	bb1.set_bit(100);

	bb.erase_block(2,2,bb1);		//nothing deleted
	EXPECT_EQ(3, bb.popcn64());

	bb.erase_block(1,1,bb1);		//nothing deleted
	EXPECT_TRUE(bb.is_bit(10));
	EXPECT_FALSE(bb.is_bit(64));
	EXPECT_TRUE(bb.is_bit(20));
	EXPECT_FALSE(bb.is_bit(100));

	bb.erase_block(0,2,bb1);		//nothing deleted
	EXPECT_EQ(1, bb.popcn64());
}

TEST(Masks, erase_bits_sparse){
	sparse_bitarray bb(130);
	sparse_bitarray bb1(130);
	sparse_bitarray bbres(130);
	
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	bb1.set_bit(10);
	bb1.set_bit(64);
	bb1.set_bit(100);

	bb.erase_block(2,2,bb1);		//nothing deleted
	EXPECT_EQ(3, bb.popcn64());

	bb.erase_block(1,1,bb1);		//nothing deleted
	EXPECT_TRUE(bb.is_bit(10));
	EXPECT_FALSE(bb.is_bit(64));
	EXPECT_TRUE(bb.is_bit(20));
	EXPECT_FALSE(bb.is_bit(100));

	bb.erase_block(0,2,bb1);		//nothing deleted
	EXPECT_EQ(1, bb.popcn64());
}

