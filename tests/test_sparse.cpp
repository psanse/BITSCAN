#include <algorithm>
#include <iterator>
#include <iostream>
#include <set>

#include "../bitscan.h"				//bit string library
#include "google/gtest/gtest.h"

#include "../bbintrinsic_sparse.h"

using namespace std;

TEST(Sparse, construction){
	BitBoardS bbs;
	bbs.init(1000);
	bbs.set_bit(500);
	bbs.set_bit(700);
	bbs.set_bit(900);	
	bbs.set_bit(1100);					//outside popsize range, not added (note the range is the last bitblock)
	EXPECT_EQ(3, bbs.popcn64());

	bbs.init(100000);
	bbs.set_bit(10);
	EXPECT_TRUE(bbs.is_bit(10));

	BitBoardS bbs1(bbs);
	EXPECT_TRUE(bbs1.is_bit(10));
}

TEST(Sparse, basics) {
	//sparse 
	BitBoardS bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	
	EXPECT_TRUE(bbsp.is_bit(10));
	EXPECT_TRUE(bbsp.is_bit(20));
	EXPECT_TRUE(bbsp.is_bit(64));
	EXPECT_FALSE(bbsp.is_bit(63));
	EXPECT_EQ(3,bbsp.popcn64());

	bbsp.erase_bit(10);
	EXPECT_FALSE(bbsp.is_bit(10));
	EXPECT_EQ(2,bbsp.popcn64());

	//find functions for blocks (not that the actual bit in WDIV(nBit) is not important, it is the bitblock index that has to exist)
	EXPECT_EQ(0, bbsp.find_pos(WDIV(10)).second);
	EXPECT_TRUE(bbsp.find_pos(WDIV(10)).first);
	EXPECT_EQ(1, bbsp.find_pos(WDIV(64)).second);
	EXPECT_TRUE(bbsp.find_pos(WDIV(64)).second);
	
	//test for a block which does not exist
	EXPECT_FALSE(bbsp.find_pos(WDIV(129)).first);
	EXPECT_EQ(EMPTY_ELEM, bbsp.find_pos(WDIV(129)).second);

	EXPECT_EQ(0x01, bbsp.find_bitboard(WDIV(64)));
}

TEST(Sparse, member_masks) {
	BitBoardS lhs;
	lhs.init(200);
	lhs.set_bit(63);
	lhs.set_bit(126);
	lhs.set_bit(189);	
	
	BitBoardS rhs;
	rhs.init(200);
	rhs.set_bit(63);
	rhs.set_bit(127);
	
	lhs.AND_EQ(1,rhs);
	EXPECT_TRUE(lhs.popcn64()==1);
	EXPECT_TRUE(lhs.is_bit(63));

	lhs.OR_EQ(1,rhs);
	EXPECT_TRUE(lhs.is_bit(127));
	EXPECT_EQ(2, lhs.popcn64());
} 

TEST(Sparse_intrinsic, basics_2) {
	//sparse 
	BBIntrinS bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);

	
	(bbsp.is_bit(10));
	EXPECT_TRUE(bbsp.is_bit(20));
	EXPECT_TRUE(bbsp.is_bit(64));
	EXPECT_FALSE(bbsp.is_bit(63));
	EXPECT_EQ(3,bbsp.popcn64());

	bbsp.erase_bit(10);
	EXPECT_FALSE(bbsp.is_bit(10));
	EXPECT_EQ(2,bbsp.popcn64());


	//assignment
	BBIntrinS bbsp1(34);
	bbsp1.set_bit(22);
	bbsp1.set_bit(23);
	bbsp=bbsp1;

	EXPECT_TRUE(bbsp1.is_bit(22));
	EXPECT_TRUE(bbsp1.is_bit(23));
	EXPECT_EQ(1,bbsp1.number_of_bitblocks());

	//copy constructor
	BBIntrinS bbsp2(bbsp);
	EXPECT_TRUE(bbsp2.is_bit(22));
	EXPECT_TRUE(bbsp2.is_bit(23));
	EXPECT_EQ(1,bbsp2.number_of_bitblocks());

}

TEST(Sparse_intrinsic, set_clear_bit_in_intervals) {
	sparse_bitarray bba(1000000);
	bba.set_bit(1200,1230);

	EXPECT_EQ(31, bba.popcn64());		
	EXPECT_EQ(1200, bba.lsbn64());
	EXPECT_EQ(1230, bba.msbn64());

	bba.clear_bit(1201, 1230);
	bba.shrink_to_fit();				//effectively deallocates space
	EXPECT_EQ(1, bba.popcn64());
	EXPECT_TRUE(bba.is_bit(1200));
}

TEST(Sparse, population_count){
	BitBoardS bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	EXPECT_EQ(3, bb.popcn64());
	EXPECT_EQ(2, bb.popcn64(11));
	EXPECT_EQ(1, bb.popcn64(21));
	EXPECT_EQ(0, bb.popcn64(65));
	EXPECT_EQ(1, bb.popcn64(64));

	BBIntrinS bbs(130);
	bbs.set_bit(10);
	bbs.set_bit(20);
	bbs.set_bit(64);

	EXPECT_EQ(3, bbs.popcn64());
	EXPECT_EQ(2, bbs.popcn64(11));
	EXPECT_EQ(1, bbs.popcn64(21));
	EXPECT_EQ(0, bbs.popcn64(65));
	EXPECT_EQ(1, bbs.popcn64(64));

	bbs.set_bit(129);						//the last possible bit in the population
	EXPECT_EQ(1, bbs.popcn64(129));
	EXPECT_EQ(0, bbs.popcn64(130));			//at the moment there is no population check

	//empty graphs
	BitBoardS bbs_empty(130);
	EXPECT_EQ(0, bbs_empty.popcn64(5));	

	BBIntrinS bbs_empty1(130);
	EXPECT_EQ(0, bbs_empty1.popcn64(5));	
}


TEST(Sparse, set_bits) {
	
	BitBoardS bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	EXPECT_TRUE(bbsp.is_bit(126));

	//range
	bbsp.set_bit(3, 27);
	cout<<bbsp<<endl;
	EXPECT_TRUE(bbsp.is_bit(3));
	EXPECT_TRUE(bbsp.is_bit(27));
	EXPECT_EQ(32, bbsp.popcn64());
	

	bbsp.set_bit(1002,1002);
	EXPECT_TRUE(bbsp.is_bit(1002));
	EXPECT_EQ(33, bbsp.popcn64());


	//range
	bbsp.set_bit(29, 125);
	EXPECT_FALSE(bbsp.is_bit(28));
	EXPECT_TRUE(bbsp.is_bit(29));
	EXPECT_TRUE(bbsp.is_bit(125));

	bbsp.set_bit(1001,1100);
	EXPECT_FALSE(bbsp.is_bit(999));
	EXPECT_TRUE(bbsp.is_bit(1001));
	EXPECT_TRUE(bbsp.is_bit(1100));
	EXPECT_FALSE(bbsp.is_bit(1101));
	cout<<bbsp<<endl;


//init member functions
	sparse_bitarray sb(150);
	sb.init_bit(30,40);
	EXPECT_TRUE(sb.is_bit(30));
	EXPECT_TRUE(sb.is_bit(40));
	EXPECT_EQ(11, sb.popcn64());

	sb.init_bit(55);
	EXPECT_TRUE(sb.is_bit(55));
	EXPECT_EQ(1, sb.popcn64());

}

TEST(Sparse, boolean_properties){
	sparse_bitarray bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	sparse_bitarray bb1(130);
	bb1.set_bit(11);
	bb1.set_bit(21);
	bb1.set_bit(65);

	//is_disjoint
	EXPECT_TRUE(bb.is_disjoint(bb1));
	
	bb1.set_bit(64);
	EXPECT_FALSE(bb.is_disjoint(bb1));

	//is disjoint with ranges
	EXPECT_TRUE(bb.is_disjoint(0,0,bb1));
	EXPECT_FALSE(bb.is_disjoint(0,1,bb1));
	EXPECT_FALSE(bb.is_disjoint(1,2,bb1));
	EXPECT_FALSE(bb.is_disjoint(1,7,bb1));
	
	//does not produce an out of range error
	bb1.erase_bit(64);
	EXPECT_TRUE(bb.is_disjoint(1,7,bb1));			
	EXPECT_TRUE(bb.is_disjoint(3,7,bb1));


}

TEST(Sparse, set_blocks) {
	
	BitBoardS bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	BitBoardS bbsp1(10000);

	bbsp1.set_bit(5);
	bbsp1.set_bit(8);
	bbsp1.set_bit(129);
	bbsp1.set_bit(5000);

	//range
	bbsp.set_block(2,2, bbsp1);
	EXPECT_TRUE(bbsp.is_bit(129));
	EXPECT_FALSE(bbsp.is_bit(5000));


	//range
	bbsp.set_block(2,INDEX_1TO0(5000), bbsp1);
	EXPECT_TRUE(bbsp.is_bit(5000));

	bbsp.erase_block(0, bbsp1);
	EXPECT_EQ(7, bbsp.popcn64());

	//range
	bbsp.set_block(2,bbsp1);
	EXPECT_TRUE(bbsp.is_bit(129));
	EXPECT_TRUE(bbsp.is_bit(5000));
}

TEST(Sparse, erase_bits) {
	
	BitBoardS bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);


	bbsp.erase_bit(126);
	EXPECT_FALSE(bbsp.is_bit(126));

	//range
	bbsp.erase_bit(2, 1000);
	EXPECT_FALSE(bbsp.is_bit(2));
	EXPECT_FALSE(bbsp.is_bit(1000));
	EXPECT_EQ(3, bbsp.popcn64());
	EXPECT_EQ(4, bbsp.number_of_bitblocks());		//all bitblocks are still there (with 0 value)
	

	//range
	bbsp.erase_bit(1,1);
	EXPECT_FALSE(bbsp.is_bit(1));
	EXPECT_EQ(2, bbsp.popcn64());

	//erase in a sequential loop
	int nBit=0;
	sparse_bitarray::velem_it it=bbsp.begin();
	while(true){
		it=bbsp.erase_bit(nBit++,it);
		if(nBit>1000) break;
	}
	EXPECT_TRUE(bbsp.is_bit(9000));
	EXPECT_EQ(1,bbsp.popcn64());
	
	//range trying to erase blocks in higher positions than any existing
	bbsp.clear();
	bbsp.set_bit(55, 58);		
	bbsp.erase_bit(77, 100);		//no bits in index>=2 so nothing is done
	EXPECT_FALSE(bbsp.is_empty());
	
	//last bit in range does not correspond to a block but there are blocks with higher index
	bbsp.clear();
	bbsp.set_bit(1, 5);	
	bbsp.set_bit(200,205);	
	bbsp.erase_bit(67, 69);	
	bbsp.print();
	EXPECT_EQ(11, bbsp.popcn64());
	EXPECT_TRUE(bbsp.is_bit(200));

	//first bit in range does not correspond to a block
	bbsp.clear();
	bbsp.set_bit(70, 75);	
	bbsp.erase_bit(63, 70);	
	EXPECT_EQ(5,bbsp.popcn64());
	EXPECT_FALSE(bbsp.is_bit(70));
	EXPECT_TRUE(bbsp.is_bit(71));
}

TEST(Sparse, clear_bits) {

	BitBoardS bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);
	
	//clear bits removing bitblocks in range
	bbsp.clear_bit(2, 1000);
	EXPECT_FALSE(bbsp.is_bit(2));
	EXPECT_FALSE(bbsp.is_bit(1000));
	EXPECT_EQ(3, bbsp.popcn64());
	EXPECT_EQ(3,bbsp.number_of_bitblocks());		//first and 9000 bitblocks are there + 1000 bitblock set to 0

	bbsp.clear_bit(2, 1064);
	EXPECT_EQ(2,bbsp.number_of_bitblocks());		//removes 1000 bitblock

	//range
	bbsp.clear_bit(1,1);
	EXPECT_FALSE(bbsp.is_bit(1));
	EXPECT_EQ(2, bbsp.popcn64());
	EXPECT_EQ(2,bbsp.number_of_bitblocks());


//end slices
	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	bbsp.clear_bit(1001,EMPTY_ELEM);
	EXPECT_TRUE(bbsp.is_bit(1000));
	EXPECT_EQ(3,bbsp.number_of_bitblocks());

	bbsp.clear_bit(EMPTY_ELEM, 1);
	EXPECT_TRUE(bbsp.is_bit(2));
	EXPECT_EQ(3,bbsp.number_of_bitblocks());

	bbsp.clear_bit(EMPTY_ELEM, EMPTY_ELEM);
	EXPECT_TRUE(bbsp.is_empty());
	cout<<"----------------------------"<<endl;

}


TEST(Sparse_non_instrinsic, scanning){
	BitBoardS bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);	
	vector<int> v;

	//direct loop
	int nBit=EMPTY_ELEM;
	while(true){
		nBit=bbsp.next_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		v.push_back(nBit);
	}
		
	EXPECT_EQ(10, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(64, v[2]);
	EXPECT_EQ(bbsp.number_of_bitblocks(), 2);				//number of blocks 
	bbsp.erase_bit(64);
	EXPECT_EQ(bbsp.number_of_bitblocks(), 2);				//one of the blocks is empty, so still 2
	EXPECT_FALSE(bbsp.is_bit(64));

	//reverse loop
	bbsp.set_bit(64);
	v.clear();

	nBit=EMPTY_ELEM;
	while(true){
		nBit=bbsp.previous_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		v.push_back(nBit);
	}

	EXPECT_EQ(64, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(10, v[2]);
}


TEST(Sparse_intrinsic, non_destructive_scanning){
	BBIntrinS bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);	
	vector<int> v;

	//non destructive
	int nBit;
	if(bbsp.init_scan(BBObject::NON_DESTRUCTIVE)!=EMPTY_ELEM){
		while(true){
			nBit=bbsp.next_bit();
			if(nBit==EMPTY_ELEM) break;
			v.push_back(nBit);
		}
	}
		
	EXPECT_EQ(10, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(64, v[2]);
	EXPECT_EQ(bbsp.number_of_bitblocks(), 2);				//number of blocks 
	bbsp.erase_bit(64);
	EXPECT_EQ(bbsp.number_of_bitblocks(), 2);				//one of the blocks is empty, so still 2
	EXPECT_FALSE(bbsp.is_bit(64));
	EXPECT_EQ(2, bbsp.popcn64());

	//non destructive reverse
	bbsp.set_bit(64);
	v.clear();

	if(bbsp.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE)!=EMPTY_ELEM){
		while(true){
			nBit=bbsp.previous_bit();
			if(nBit==EMPTY_ELEM) break;
			v.push_back(nBit);
		}
	}

	EXPECT_EQ(64, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(10, v[2]);
}

TEST(Sparse_intrinsic, non_destructive_scanning_with_starting_point){
	BBIntrinS bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);	
	vector<int> v;

	int nBit;
	EXPECT_EQ(2,bbsp.number_of_bitblocks());				//number of blocks 

	//non destructive
	if(bbsp.init_scan_from(20,BBObject::NON_DESTRUCTIVE)!=EMPTY_ELEM){
		while(true){
			nBit=bbsp.next_bit();
			if(nBit==EMPTY_ELEM) break;
			v.push_back(nBit);
		}
	}
		

	EXPECT_EQ(64, v[0]);
	EXPECT_EQ(1, v.size());
	
	
	//non destructive reverse
	v.clear();
	if(bbsp.init_scan_from(20,BBObject::NON_DESTRUCTIVE_REVERSE)!=EMPTY_ELEM){
		while(true){
			nBit=bbsp.previous_bit();
			if(nBit==EMPTY_ELEM) break;
			v.push_back(nBit);
		}
	}
		
	
	EXPECT_EQ(10, v[0]);
	EXPECT_EQ(1, v.size());
}

TEST(Sparse_intrinsic, destructive_scanning){
	BBIntrinS bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);	
	vector<int> v;

	//non destructive
	int nBit;
	if(bbsp.init_scan(BBObject::NON_DESTRUCTIVE)!=EMPTY_ELEM){
		while(true){
			nBit=bbsp.next_bit_del();
			if(nBit==EMPTY_ELEM) break;
			v.push_back(nBit);
		}
	}
		
	EXPECT_TRUE(bbsp.is_empty());
	EXPECT_EQ(10, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(64, v[2]);

	//non destructive reverse
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);
	v.clear();
	if(bbsp.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE)!=EMPTY_ELEM){
		while(true){
			nBit=bbsp.previous_bit_del();
			if(nBit==EMPTY_ELEM) break;
			v.push_back(nBit);
		}
	}

	EXPECT_TRUE(bbsp.is_empty());

	EXPECT_EQ(64, v[0]);
	EXPECT_EQ(20, v[1]);
	EXPECT_EQ(10, v[2]);
}

TEST(Sparse, operators){
	BitBoardS bbsp(130);
	bbsp.set_bit(10);
	bbsp.set_bit(20);
	bbsp.set_bit(64);

	BitBoardS bbsp1(130);
	bbsp1.set_bit(30);
	bbsp1.set_bit(54);
	bbsp1.set_bit(128);
	
	//OR
	BitBoardS resOR(130);
	OR(bbsp1, bbsp, resOR);
	EXPECT_EQ(6, resOR.popcn64());

	//=
	bbsp=resOR;
	EXPECT_EQ(6, bbsp.popcn64());
	EXPECT_TRUE(bbsp==resOR);

	//AND
	BitBoardS bbsp2(130,true);
	bbsp2.set_bit(54);
	BitBoardS resAND(130);
	AND(bbsp1,bbsp2,resAND);
	EXPECT_TRUE(resAND.is_bit(54));
	EXPECT_EQ(1, resAND.popcn64());
}

TEST(Sparse, insertion) {
	BitBoardS bbsp(130);					
	bbsp.set_bit(64);
	bbsp.set_bit(65);
	bbsp.set_bit(12);					//obligatory

	EXPECT_TRUE(bbsp.is_bit(12));
}

TEST(Sparse, integration) {
	BitBoardS bbs(10000);
	bbs.set_bit(10);
	bbs.set_bit(1000);
	bbs.set_bit(70);
	bbs.set_bit(150);
	
	BitBoardS bbs1(10000);
	bbs1.set_bit(1000);

	BitBoardS bbs2(10000);
	AND(bbs1, bbs, bbs2);
	EXPECT_EQ(1, bbs2.popcn64());
	EXPECT_TRUE(bbs2.is_bit(1000));
	EXPECT_TRUE(bbs2==bbs1);

	BitBoardS bbs3(10000);
	OR(bbs1,bbs, bbs3);
	EXPECT_EQ(4, bbs3.popcn64());
	EXPECT_TRUE(bbs3.is_bit(1000));
	EXPECT_TRUE(bbs3.is_bit(10));
	EXPECT_TRUE(bbs3.is_bit(70));
	EXPECT_TRUE(bbs3.is_bit(150));

	bbs3.erase_bit(1000);
	EXPECT_EQ(3, bbs3.popcn64());			//even though one bit block is empty
	EXPECT_EQ(4, bbs3.number_of_bitblocks());		
}

TEST(Sparse, next_bit_del_pos) {
	cout<<"----------------------------------"<<endl;
	BBIntrinS bbsp(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);

	//bitscannning loop
	bbsp.init_scan(bbo::DESTRUCTIVE);
	int posBB=EMPTY_ELEM;
	while(true){
		int	nBit=bbsp.next_bit_del_pos(posBB);
		if(nBit==EMPTY_ELEM) 
				break;
		cout<<"nBit: "<<nBit<<" pos: "<<posBB<<endl;
		
	}

	cout<<"----------------------------------"<<endl;
}

TEST(Sparse, erase_block_pos) {
	BBIntrinS bbsp(10000);
	BBIntrinS bberase(10000);
	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);
	
	bberase.set_bit(0);
	bberase.set_bit(1);
	bberase.set_bit(2);
	bberase.set_bit(126);
	bberase.set_bit(127);
	bberase.set_bit(1000);
	bberase.set_bit(9000);

	bbsp.erase_block_pos(1,bberase);
	EXPECT_FALSE(bbsp.is_bit(126));

	bbsp.erase_block_pos(2,bberase);
	EXPECT_FALSE(bbsp.is_bit(1000));


	bbsp.erase_block_pos(3,bberase);
	EXPECT_FALSE(bbsp.is_bit(9000));
}

TEST(Sparse, copy_up_to_some_bit) {
	BBIntrinS bbsp(10000);
	BBIntrinS bbcopy(10000);
	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);
	bbsp.set_bit(9999);
	
	bbcopy.init_bit(126, bbsp);
	EXPECT_EQ(4, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(126));

	bbcopy.init_bit(1000, bbsp);
	EXPECT_EQ(6, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(1000));

	bbcopy.init_bit(9000, bbsp);
	EXPECT_EQ(7, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(9000));

	bbcopy.init_bit(9999, bbsp);
	EXPECT_TRUE(bbcopy==bbsp);
}

TEST(Sparse, copy_in_closed_range){
	BBIntrinS bbsp(10000);
	BBIntrinS bbcopy(10000);
	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);
	bbsp.set_bit(9999);

	bbcopy.init_bit(126, 1000, bbsp);
	bbcopy.print();
	EXPECT_EQ(3, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(126));
	EXPECT_TRUE(bbcopy.is_bit(127));
	EXPECT_TRUE(bbcopy.is_bit(1000));

	bbcopy.clear();
	bbcopy.init_bit(127, 129, bbsp);
	EXPECT_EQ(1, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(127));

	bbcopy.clear();
	bbcopy.init_bit(0, 10000, bbsp);
	EXPECT_TRUE( bbcopy==bbsp);

	bbcopy.clear();
	bbcopy.init_bit(9000, 9999, bbsp);
	EXPECT_EQ(2, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(9000));
	EXPECT_TRUE(bbcopy.is_bit(9999));

	bbcopy.clear();
	bbcopy.init_bit(0, 0, bbsp);
	EXPECT_EQ(1, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(0));

	bbcopy.clear();
	bbcopy.init_bit(9999, 9999, bbsp);
	EXPECT_EQ(1, bbcopy.popcn64());
	EXPECT_TRUE(bbcopy.is_bit(9999));
}

TEST(Sparse, copy_in_closed_range_special_cases){
	BBIntrinS bbsp(120);
	BBIntrinS bbcopy(120);
	bbsp.set_bit(64);

	bbcopy.init_bit(0, 51, bbsp);
	EXPECT_EQ(0, bbcopy.popcn64());

	bbcopy.clear();
	bbcopy.init_bit(64, 65, bbsp);
	EXPECT_EQ(1, bbcopy.popcn64());
	EXPECT_TRUE(64, bbcopy.is_bit(64));

	bbcopy.clear();
	bbcopy.init_bit(63, 64, bbsp);
	EXPECT_EQ(1, bbcopy.popcn64());
	EXPECT_TRUE(64, bbcopy.is_bit(64));
}

TEST(Sparse, keep_operations) {
	BBIntrinS bbsp(10000);
	BBIntrinS bbkeep(10000);
	BBIntrinS bbkeep2(10000);
	BBIntrinS bbkeep3(10000);

	bbsp.set_bit(0);
	bbsp.set_bit(1);
	bbsp.set_bit(2);
	bbsp.set_bit(126);
	bbsp.set_bit(127);
	bbsp.set_bit(1000);
	bbsp.set_bit(9000);
	
	bbkeep.set_bit(0);
	bbkeep.set_bit(1);
	bbkeep.set_bit(2);
	bbkeep.set_bit(126);
	bbkeep.set_bit(127);
	bbkeep.set_bit(1000);
	bbkeep.set_bit(9000);

	bbkeep2.set_bit(0);
	bbkeep2.set_bit(1);
	bbkeep2.set_bit(126);
	bbkeep2.set_bit(1000);

	bbkeep3.set_bit(0);

	//tests
	bbsp&=bbkeep;
	EXPECT_TRUE(bbsp==bbkeep);

	bbsp.AND_EQ(1, bbkeep2);
	bbsp.print();
	EXPECT_TRUE(bbsp.is_bit(126));
	EXPECT_TRUE(bbsp.is_bit(0));
	EXPECT_FALSE(bbsp.is_bit(127));		//last bit of block 1 deleted
	EXPECT_EQ(5, bbsp.popcn64());

	
	bbsp&=bbkeep3;
	EXPECT_TRUE(bbsp.is_bit(0));
	EXPECT_EQ(1, bbsp.popcn64());

}


