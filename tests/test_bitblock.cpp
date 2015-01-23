//tests with masks for sparse and normal graphs

#include <algorithm>
#include <iterator>
#include <iostream>
#include <set>

#include "../bitscan.h"					//bit string library
#include "google/gtest/gtest.h"

using namespace std;

TEST(Masks, single_block) {

	BITBOARD bb=bitblock::MASK_0(10, 15);
	EXPECT_FALSE(bb & Tables::mask[10]);
	EXPECT_FALSE(bb & Tables::mask[15]);


	BITBOARD bb1=bitblock::MASK_1(10, 15);
	EXPECT_TRUE(bb1 & Tables::mask[10]);
	EXPECT_TRUE(bb1 & Tables::mask[15]);
}

TEST(Scan, single_block) {

	BITBOARD bb=0xf0f0f0f0;
	EXPECT_EQ(16,bitblock::popc64(bb));
	EXPECT_EQ(4,bitblock::lsb64_intrinsic(bb));
	EXPECT_EQ(31,bitblock::msb64_intrinsic(bb));
}
