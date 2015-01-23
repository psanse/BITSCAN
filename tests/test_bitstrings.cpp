#include <algorithm>
#include <iterator>
#include <iostream>
#include <set>

#include "../bitscan.h"				//bit string library
#include "google/gtest/gtest.h"

using namespace std;

class BitScanningTest: public ::testing::Test{
protected:
	BitScanningTest():bbn(301), bbi(301), bbs(301){}
	virtual void SetUp(){
	  for(int i=0; i<=300; i+=50){
		  bbn.set_bit(i);
		  bbi.set_bit(i);
		  bbs.set_bit(i);
		  sol.insert(i); 
	  }
	}
//////////////////////
// Objects to be acessed by all tests
	BitBoardN bbn;
	BBIntrin bbi;
	BBSentinel bbs;
	set<int> sol;
};


TEST(BitBoard, de_Bruijn) {
	BITBOARD bb=0x100F0010;
	EXPECT_EQ(BitBoard::msb64_de_Bruijn(bb),BitBoard::msb64_lup(bb));
}

TEST(Bitstrings, setters_and_getters) {
	BitBoardN bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	EXPECT_TRUE(bb.is_bit(10));
	EXPECT_TRUE(bb.is_bit(20));
	EXPECT_TRUE(bb.is_bit(64));
	EXPECT_FALSE(bb.is_bit(63));

	//assignment
	BitBoardN bb1(34);
	bb1.set_bit(22);
	bb1.set_bit(23);
	bb=bb1;

	EXPECT_TRUE(bb.is_bit(22));
	EXPECT_TRUE(bb.is_bit(23));
	EXPECT_EQ(1,bb.number_of_bitblocks());

	//copy constructor
	BitBoardN bb2(bb);
	EXPECT_TRUE(bb2.is_bit(22));
	EXPECT_TRUE(bb2.is_bit(23));
	EXPECT_EQ(1,bb2.number_of_bitblocks());

}

TEST(Bitstrings, boolean_properties){
	BitBoardN bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	BitBoardN bb1(130);
	bb1.set_bit(11);
	bb1.set_bit(21);
	bb1.set_bit(65);

	//is_dijoint
	EXPECT_TRUE(bb.is_disjoint(bb1));
	
	bb1.set_bit(64);
	EXPECT_FALSE(bb.is_disjoint(bb1));
}

TEST(Bitstrings, set_bit_range){
	BitBoardN bb(130);
	bb.set_bit(0, 64);
	EXPECT_TRUE(bb.is_bit(0));
	EXPECT_TRUE(bb.is_bit(64));
	
	BitBoardN bb1(130);
	bb1.set_bit(0, 0);
	EXPECT_TRUE(bb1.is_bit(0));
	
	bb1.set_bit(64, 64);
	EXPECT_TRUE(bb1.is_bit(64));
	EXPECT_TRUE(bb1.is_bit(0));
	
	bb1.set_bit(55, 56);
	EXPECT_TRUE(bb1.popcn64(4));
}

TEST(Bitstrings, erase_bit_range){
	BitBoardN bb(130);
	bb.set_bit(0, 129);

	bb.erase_bit(0, 64);
	EXPECT_TRUE(bb.is_bit(65));
	EXPECT_FALSE(bb.is_bit(64));
	

	bb.erase_bit(115, 116);
	EXPECT_TRUE(bb.is_bit(114));
	EXPECT_FALSE(bb.is_bit(115));

}

TEST(Bitstrings, algorithms) {
//simple test for algorithms in bbalg.h

	BitBoardN bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	////get_first_k_bits
	//BITBOARD b8=bb.get_bitstring()[0];
	//EXPECT_EQ(BitBoard::popc64_lup(get_first_k_bits(b8, 1)), 1);
	//EXPECT_EQ(BitBoard::popc64_lup(get_first_k_bits(b8, 2)), 2);
	//EXPECT_EQ(BitBoard::popc64_lup(get_first_k_bits(b8, 3)), 0);
}

TEST(Bitstrings, population_count){
	BitBoardN bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	EXPECT_EQ(3, bb.popcn64());
	EXPECT_EQ(2, bb.popcn64(11));
	EXPECT_EQ(1, bb.popcn64(21));
	EXPECT_EQ(0, bb.popcn64(65));
	EXPECT_EQ(1, bb.popcn64(64));


	BBIntrin bbi(130);
	bbi.set_bit(10);
	bbi.set_bit(20);
	bbi.set_bit(64);

	EXPECT_EQ(3, bbi.popcn64());
	EXPECT_EQ(2, bbi.popcn64(11));
	EXPECT_EQ(1, bbi.popcn64(21));
	EXPECT_EQ(0, bbi.popcn64(65));
	EXPECT_EQ(1, bbi.popcn64(64));
}

TEST(Bitstrings, to_vector){
	BitBoardN bb(130);
	bb.set_bit(10);
	bb.set_bit(20);
	bb.set_bit(64);

	vector<int> sol;
	sol.push_back(10);  sol.push_back(20); sol.push_back(64);

	vector<int> vint;
	bb.to_vector(vint);
	EXPECT_EQ(sol, vint);

	//simple sparse bitstring
	BitBoardS bbs(130);
	bbs.set_bit(10);
	bbs.set_bit(20);
	bbs.set_bit(64);

	vint.clear();
	bbs.to_vector(vint);
	EXPECT_EQ(sol, vint);
}

TEST_F(BitScanningTest, non_destructive){
	std::set<int> res;
			
	int nBit=EMPTY_ELEM;
	while(true){
		nBit=bbn.next_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_TRUE(res==sol);

	res.clear();
	bbi.init_scan(BBObject::NON_DESTRUCTIVE);
	while(true){
		nBit=bbi.next_bit();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_TRUE(res==sol);
/////////////////////
	res.clear();
	
	if(bbs.init_scan(BBObject::NON_DESTRUCTIVE)!=EMPTY_ELEM){	//it is necessary to check if the bitstring is empty
		while(true){
			nBit=bbs.next_bit();
			if(nBit==EMPTY_ELEM) break;
			res.insert(nBit);	
		}
		EXPECT_TRUE(res==sol);
	}
}

TEST_F(BitScanningTest, non_destructive_with_starting_point){
	std::set<int> res;
			
	int nBit=50;
	while(true){
		nBit=bbn.next_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_EQ(5, res.size() );
	EXPECT_EQ(1, res.count(100));
	EXPECT_EQ(0, res.count(50));

	res.clear();
	bbi.init_scan_from(50,BBObject::NON_DESTRUCTIVE);
	while(true){
		nBit=bbi.next_bit();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_EQ(5, res.size() );
	EXPECT_EQ(1, res.count(100));
	EXPECT_EQ(0, res.count(50));
/////////////////////
	res.clear();
	
	bbs.init_scan_from(50, BBObject::NON_DESTRUCTIVE);
		while(true){
			nBit=bbs.next_bit();
			if(nBit==EMPTY_ELEM) break;
			res.insert(nBit);	
		}

	EXPECT_EQ(5, res.size() );
	EXPECT_EQ(1, res.count(100));
	EXPECT_EQ(0, res.count(50));
	
}

TEST_F(BitScanningTest, reverse_non_destructive){
	std::set<int> res;
			
	int nBit=EMPTY_ELEM;
	while(true){
		nBit=bbn.previous_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_TRUE(res==sol);

	res.clear();
	bbi.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);
	while(true){
		nBit=bbi.previous_bit();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_TRUE(res==sol);
/////////////////////
	res.clear();
	bbs.init_scan(BBObject::NON_DESTRUCTIVE_REVERSE);
	while(true){
		nBit=bbs.previous_bit();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	for(set<int>::iterator it=res.begin(); it!=res.end(); ++it){
		cout<<*it<<" ";
	}
	cout<<"----------------------------------------------"<<endl;

	EXPECT_TRUE(res==sol);
}

TEST_F(BitScanningTest, reverse_non_destructive_with_starting_point){
	std::set<int> res;
			
	int nBit=50;
	while(true){
		nBit=bbn.previous_bit(nBit);
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_EQ(1, res.size() );
	EXPECT_EQ(1, res.count(0));
	EXPECT_EQ(0, res.count(50));

	res.clear();
	bbi.init_scan_from(50,BBObject::NON_DESTRUCTIVE_REVERSE);
	while(true){
		nBit=bbi.previous_bit();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_EQ(1, res.size() );
	EXPECT_EQ(1, res.count(0));
	EXPECT_EQ(0, res.count(50));
/////////////////////
	res.clear();
	
	bbs.init_scan_from(50, BBObject::NON_DESTRUCTIVE_REVERSE);
	while(true){
		nBit=bbs.previous_bit();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}

	EXPECT_EQ(1, res.size() );
	EXPECT_EQ(1, res.count(0));
	EXPECT_EQ(0, res.count(50));
	
}

TEST_F(BitScanningTest, destructive){
	std::set<int> res;
	
	BitBoardN bbn1(bbn);
	int nBit=EMPTY_ELEM;
	while(true){
		nBit=bbn1.next_bit_if_del(nBit);
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);
	bbn1.erase_bit(nBit);
	}
	
	EXPECT_TRUE(res==sol);
	EXPECT_EQ(0,bbn1.popcn64());

	//intrinsic
	res.clear();
	BBIntrin bbi1(bbi);
	bbi1.init_scan(BBObject::DESTRUCTIVE);
	while(true){
		nBit=bbi1.next_bit_del();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_TRUE(res==sol);
	EXPECT_EQ(0,bbi1.popcn64());

	//sentinels
	res.clear();
	BBSentinel bbs1(bbs);
	if(bbs1.init_scan(BBObject::DESTRUCTIVE)!=EMPTY_ELEM){
		while(true){
			nBit=bbs1.next_bit_del();
			if(nBit==EMPTY_ELEM) break;
			res.insert(nBit);	
		}
	}

	EXPECT_TRUE(res==sol);
	EXPECT_EQ(0,bbs1.popcn64());
}


TEST_F(BitScanningTest, reverse_destructive){
	std::set<int> res;
	
	BitBoardN bbn1(bbn);
	int nBit=EMPTY_ELEM;
	while(true){
		nBit=bbn1.previous_bit(nBit);		
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);
	bbn1.erase_bit(nBit);
	}
	
	EXPECT_TRUE(res==sol);
	EXPECT_EQ(0,bbn1.popcn64());

	//intrinsic
	res.clear();
	BBIntrin bbi1(bbi);
	bbi1.init_scan(BBObject::DESTRUCTIVE_REVERSE);
	while(true){
		nBit=bbi1.previous_bit_del();
		if(nBit==EMPTY_ELEM) break;
		res.insert(nBit);	
	}
	
	EXPECT_TRUE(res==sol);
	EXPECT_EQ(0,bbi1.popcn64());

	//sentinels
	res.clear();
	BBSentinel bbs1(bbs);
	if(bbs1.init_scan(BBObject::DESTRUCTIVE_REVERSE)!=EMPTY_ELEM){
		while(true){
			nBit=bbs1.previous_bit_del();
			if(nBit==EMPTY_ELEM) break;
			res.insert(nBit);	
		}
	}
	EXPECT_TRUE(res==sol);
	EXPECT_EQ(0,bbs1.popcn64());
}

//TEST_F (BitScanningTest, algorithms){
//	EXPECT_TRUE(similar(bbn, bbi, 0));
//	EXPECT_TRUE(subsumes(bbn, bbi));
//	bbn.erase_bit(bbn.lsbn64());
//	EXPECT_FALSE(subsumes(bbn, bbi));
//	EXPECT_TRUE(subsumes(bbi, bbn));
//}


TEST(BitBoardTests, vector) {
	BitBoardN bbn1(50);
	int aux[] = {10,20,45,62};
	vector<int> v1(aux, aux + sizeof(aux) / sizeof(int));

	bbn1.set_bit(v1[0]);
	bbn1.set_bit(v1[1]);
	bbn1.set_bit(v1[2]);
	bbn1.set_bit(v1[3]);

	BitBoardN bbn2(v1);

	vector<int> v2=to_vector(bbn1);
	vector<int> v3=to_vector(bbn2);

	EXPECT_EQ(to_vector(bbn2), v1);
	EXPECT_EQ(to_vector(bbn1), v1);
	EXPECT_EQ(to_vector(bbn1), to_vector(bbn2));
 
}

TEST(BitBoardTests, BitBoardNTo0) {
	BitBoardN bb1(25);
	BitBoardN bb2(bb1);

	bb1.set_bit();			//sets to ONE the whole bitblock
	bb2.set_bit(0,24);

	vector<int> v1(25,0);
	
	for(int i=0;i<25;i++){
		v1[i]=i;
	}

	EXPECT_LT(v1, to_vector(bb1));
	EXPECT_LT(25, bb1.popcn64());
	EXPECT_EQ(to_vector(bb2), v1);
	EXPECT_EQ(25, bb2.popcn64());

}

TEST(BITBOARDTest, GenRandom){
///////////
// deterministic test, unless the seed is changed 
// date: 2/6/14

	//srand(time(NULL));
	vector<double> vd;
	const int NUM_TRIES=100;
	const double DENSITY=.7;
	const double TOLERANCE=.05;

	for(int i=0; i<NUM_TRIES; i++){
		BITBOARD bb=gen_random_bitboard(DENSITY);  //random bits
		vd.push_back(BitBoard::popc64(bb)/(double)WORD_SIZE);
	}

	//average value
	double sum_of_elems=0;
	for(vector<double>::iterator j=vd.begin();j!=vd.end();j++){
		sum_of_elems += *j;
	}
	double av_of_elems=sum_of_elems/NUM_TRIES;
	EXPECT_TRUE(abs(av_of_elems-DENSITY)<TOLERANCE);
}


class BasicFunctionsTest : public ::testing::Test {
protected:
	virtual void SetUp() {
		int aux[] = {4,8,15,16,23,42};
		val.assign (aux,aux+6);
		bbn.init(45,val);
		bbi.init(45,val);
		bbs.init(45,val);
	}

	vector<int> val;
	BitBoardN bbn;
	BBIntrin bbi;
	BBSentinel bbs;
};

TEST_F(BasicFunctionsTest, miscellanous){
	EXPECT_EQ(bbn.popcn64(),bbi.popcn64());
	EXPECT_EQ(bbn.popcn64(),bbs.popcn64());
	EXPECT_EQ(to_vector(bbn), to_vector(bbi));
	EXPECT_EQ(to_vector(bbn), to_vector(bbs));
}


TEST(Bitstrings, init_bit_range){
//date: 21/12/14
	BitBoardN bb(130);
	bb.init_bit(0, 64);
	EXPECT_TRUE(bb.is_bit(0));
	EXPECT_TRUE(bb.is_bit(64));
	

	BitBoardN bb1(130);
	bb1.init_bit(0, 0);
	EXPECT_TRUE(bb1.is_bit(0));
	EXPECT_EQ(1,bb1.popcn64());
	

	bb1.init_bit(64, 64);
	EXPECT_TRUE(bb1.is_bit(64));
	EXPECT_EQ(1,bb1.popcn64());
	

	bb1.init_bit(55, 56);
	EXPECT_TRUE(bb1.is_bit(55));
	EXPECT_TRUE(bb1.is_bit(56));
	EXPECT_EQ(2, bb1.popcn64());


	BitBoardN bb2(130);
	bb2.init_bit(43);
	bb2.init_bit(44);
	bb2.init_bit(129);					//this is the only one that counts
	EXPECT_TRUE(bb2.is_bit(129));
	EXPECT_EQ(1, bb2.popcn64());
}

TEST(Bitstrings, block_copying){
//date: 21/12/14
	BitBoardN bb(130);
	bb.init_bit(0,54);
	BitBoardN bb1(130);
	bb1.init_bit(50,100);

	bb.erase_bit();
	bb.copy_from_block(1,bb1);			//second block
	EXPECT_FALSE(bb.is_bit(33));
	EXPECT_FALSE(bb.is_bit(50));
	EXPECT_TRUE(bb.is_bit(64));

	bb.erase_bit();
	bb.copy_up_to_block(0,bb1);	
	EXPECT_TRUE(bb.is_bit(50));
	EXPECT_TRUE(bb.is_bit(63));
	EXPECT_FALSE(bb.is_bit(64));
}


