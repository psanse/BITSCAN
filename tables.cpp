#include "tables.h"

//common masks and lookup tables always available
int Tables::pc[65536];								//1_bit population in 16 bit blocks
int Tables::lsb[65536];								//LSB lookup table 16 bits
int Tables::msb[65536];								//MSB in 16 bit blocks	
int Tables::pc8[256];								//population of 1-bits en U8
int Tables::pc_sa[65536];							//population of 1-bits en BITBOARD16 (Shift + Add implementations)
int Tables::msba[4][65536];							//MSB lookup table 16 bits con pos index
int Tables::lsba[4][65536];							//LSB lookup table 16 bits con pos index

BITBOARD	Tables::mask[64];						//masks for 64 bit block of a single bit
BITBOARD	Tables::mask_right[65];					//1_bit to the right of index (less significant bits, excluding index)
BITBOARD	Tables::mask_left[66];			

BITBOARD	Tables::mask0_1W;						
BITBOARD	Tables::mask0_2W;
BITBOARD	Tables::mask0_3W;
BITBOARD	Tables::mask0_4W;
U8	Tables::mask8[8];						//masks for 8 bit block of a single bit
BITBOARD	Tables::mask_entre[64/*a*/][64/*b*/];	//1-bits between intervals

#ifdef CACHED_INDEX_OPERATIONS 
int Tables::t_wdindex[MAX_CACHED_INDEX];
int Tables::t_wxindex[MAX_CACHED_INDEX];
int Tables::t_wmodindex[MAX_CACHED_INDEX];
#endif

//extended lookups
#ifdef EXTENDED_LOOKUPS				
int Tables::lsb_l[65536][16];				//LSB position list of 1-bits in BITBOARD16
#endif

//global initialization of tables
struct Init{
	Init(){Tables::InitAllTables();}
} initTables;

////////////////////
// magic number tables of 64 bits (always available since space requierement is trivial)

//8BYTES
const int Tables::T_64[67]={	
	-1,0,1,39,2,15,40,23,			
	3,12,16,59,41,19,24,54,
	4,-1,13,10,17,62,60,28,
	42,30,20,51,25,44,55,47,
	5,32,-1,38,14,22,11,58,
	18,53,63,9,61,27,29,50,
	43,46,31,37,21,57,52,8,
	26,49,45,36,56,7,48,35,
	6,34,33						};

//4BYTES (for 32 bits: present not used)
const int Tables::T_32[37]={	
	-1,0,1,26,2,23,27,-1,			
	3,16,24,30,28,11,-1,13,
	4,7,17,-1,25,22,31,15,
	29,10,12,6,-1,21,14,9,	
	5,20,8,19,18				};	

//De Bruijn Magic number 
const int Tables::indexDeBruijn64_ISOL[64] = {								
	63,  0, 58,  1, 59, 47, 53,  2,
	60, 39, 48, 27, 54, 33, 42,  3,
	61, 51, 37, 40, 49, 18, 28, 20,
	55, 30, 34, 11, 43, 14, 22,  4,
	62, 57, 46, 52, 38, 26, 32, 41,
	50, 36, 17, 19, 29, 10, 13, 21,
	56, 45, 25, 31, 35, 16,  9, 12,
	44, 24, 15,  8, 23,  7,  6,  5	};


 const int Tables::indexDeBruijn64_SEP[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63		};


/////////////////////////////////////

void Tables::init_masks(){

	BITBOARD uno = 1;

	for (int c=0;c<64;c++) 
				mask[c]= uno<<c;


	////////////////////////////
	//mask8[8]
	U8 uno8 = 1;

	for (int c=0;c<8;c++) 
				mask8[c]= uno8<<c;

	////////////////////////////
	//mask_right[65]
	BITBOARD aux=0;
	for (int c=0;c<64;c++)
	{
		for ( int j=0; j<c /* not included the element itself*/; j++)
		{
			mask_right[c] |= mask[j];

		}
	}
	//mask_right[0]=ZERO;
	mask_right[64]=ONE;

	////////////////////////////
	//mask_left[64] (from mask_right)
	for (int c=0;c<64;c++)
	{
		mask_left[c]= ~mask_right[c] ^ mask[c]/*erase the element itself*/;
	}
	
	mask_left[64]=ZERO;
	mask_left[MASK_LIM]=ONE;

	
	////////////////////////////
	//mask_entre[64][64] 
	
	for (int c=0;c<64;c++)	{
		for ( int j=0; j<64 ; j++ )		{
			if(j<c) continue; 

			if(j==c) mask_entre[c][j]=mask[c];
			else mask_entre[c][j]= mask_right[j] & mask_left[c] | mask[c] | mask[j];
		}
	}

	/////////////////////////////
	//máscara de 0s
	
	mask0_1W= ONE <<16;
	mask0_2W= (mask0_1W<<16) | (~mask0_1W);
	mask0_3W= (mask0_2W<<16) | (~mask0_1W);
	mask0_4W= (mask0_3W<<16) | (~mask0_1W);
}

/////////////////////////////////
// Inicio Tabla PopCount (poblacion 8 bits)

void Tables::init_popc8(){
	int n;
	int c;
	int k;
		
	pc8[0]=0;							//null bit population

	for (c=1;c<256;c++)	{
		n=0;
		for (k=0;k<=7;k++)		{
			 if (c & (1 << k))
							n++;		//increase the population count
		}
		pc8[c]=n;
	}
}

/////////////////////////////////
// Table begin PopCount (16 bits population)

void Tables::init_popc(){
////////////////////
//
	int n,c,k;
	pc[0]=0;				 //null bits population

	for (c=1;c<65536;c++)	{
		n=0;
		for (k=0;k<=15;k++)		{
			 if (c & (1 << k))
				n++;			//increase the population count
		}
		pc[c]=n;
	}


	//Implementacion Shift+Add: 
	pc_sa[0]=0;					//null bits population

	for (c=1;c<65536;c++)	{
		n=0;
		for(k=0;k<13;k+=4)
				n+=0xF & (c>>k);  //Sum of the number of bits every 4
		
		pc_sa[c]=n;
	}
}

////////////////////////////////////////
//  Tables for MSB / LSB

void Tables::init_mlsb(){
////////////////////////
// builds tables for most significant bit and least signifincat bit fast bitscan
	int c,k;

	msb[0] = EMPTY_ELEM;				//no 1-bit
	for ( c=1;c<65536;c++){
		for (k=15;k>=0;k--){
			 if (c & (1/*int*/ << k)) {
				 msb[c] = k;
				break;
			}
		}
	}

	lsb[0]= EMPTY_ELEM;					//no 1-bit
	for ( c=1;c<65536;c++){
		for (k=0;k<16;k++)	{
			 if (c & (1/*int*/ << k)) {
				 lsb[c] = k;	//0-15
				 break;
			}
		}
	}

	//lsb with position index
	for(k=0; k<4; k++)
		for ( c=1;c<65536;c++)
			lsba[k][c]=lsb[c]+k*16;

	for(k=0; k<4; k++)
		lsba[k][0]=EMPTY_ELEM;			

	//msb with position index
	for(k=0; k<4; k++)
		for (c=1;c<65536;c++)
			msba[k][c]=msb[c]+k*16;

	for(k=0; k<4; k++)
			msba[k][0]=EMPTY_ELEM;		
}

void Tables::init_lsb_l(){
////////////////////
// 16 bits conversion to a list of numbers
//
#ifdef EXTENDED_LOOKUPS

	int k;
	BITBOARD c;
	
	for ( c=0;c<65536;c++)	{
		for (k=0;k<16;k++){
			if(c & mask[k])
				lsb_l[c][k]=k;
			else lsb_l[c][k]=100 /*upper bound*/;
			
		}
	}

	//Order lsb_l[c] table from lowest to highest
	int tmp;
	for ( c=0;c<65536;c++){
		//a simple ordering strategy
		k=0;
		while (k < 16){
			if ( (k == 0) || (lsb_l[c][k-1] <= lsb_l[c][k]) ) k++;
			else 
			{ 
				tmp= lsb_l[c][k]; 
				lsb_l[c][k] = lsb_l[c][k-1]; 
				lsb_l[c][--k] = tmp;
			}
		}
	}

	//replace upper bound by -1
	for ( c=0;c<65536;c++){
		for (k=0;k<16;k++)	{
			if(lsb_l[c][k] ==100) 
					lsb_l[c][k]=EMPTY_ELEM;
		}
	}
#endif 	
}



void Tables::init_cached_index()
{
//index tables

#ifdef  CACHED_INDEX_OPERATIONS 
	for(int i=0; i<MAX_CACHED_INDEX; i++) 
						Tables::t_wdindex[i]=i/WORD_SIZE;


	for(int i=0; i<MAX_CACHED_INDEX; i++) 
						Tables::t_wxindex[i]=i*WORD_SIZE;


	for(int i=0; i<MAX_CACHED_INDEX; i++) 
						Tables::t_wmodindex[i]=i%WORD_SIZE;
#endif
}


//boot tables in RAM

int Tables::InitAllTables(){
	init_mlsb();
    init_popc();
    init_popc8();
    init_masks(); 
	init_lsb_l();
	init_cached_index();

return 1;
}
