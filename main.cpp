
#include <iostream>
#include "spine_bv.hpp"

int main(){
	//set value 300 in range 20-0
	spine_bv<101> spine1;
	spine1.set(20, 0, 300);
	u64 value1 = spine1.get(20, 0);
	u32 value11 = (u32)(spine1.get(20, 0));
	assert(300==value1);
	assert(300==value11);

	//set value 0x12 in range 80-60
	spine_bv<101> spine2;
	spine2.set(80, 60, 0x12);
	u64 value2 = spine2.get(80, 60);
	assert(0x12==value2);
	
	//set value 0x12345678 in range 100-50
	spine_bv<101> spine3;
	spine3.set(100, 50, 0x12345678);
	u64 value3 = spine3.get(100, 50);
	assert(0x12345678==value3);
	
	//set value 0x12345678 in range 100-68
	spine_bv<101> spine31;
	spine31.set(100, 68, 0x12345678);
	u64 value31 = spine31.get(100, 68);
	assert(0x12345678==value31);
	
	//set value 0x123456789abcdef in range 128-65
	spine_bv<301> spine4;
	spine4.set(128, 65, 0x123456789abcdef);
	u64 value4 = spine4.get(128, 65);
	assert(0x123456789abcdef==value4);
	u32 value41 = (u32)(spine4.get(128, 65));
	assert(0x89abcdef==value41);
	spine4.set(33,0,0x12345678);
	u32 value42 = spine4.to_uint();
	assert(0x12345678==value42);
	
	//set value 0xfedcba987654321 in range 128-65
	spine_bv<301> spine6;
	spine6.set(128, 65, 0xfedcba987654321);
	u64 value6 = spine6.get(128, 65);
	assert(0xfedcba987654321==value6);

	
	//set value 0x123456789abcdef in range 128-65, and get it out in range 96-65
	spine_bv<301> spine5;
	spine5.set(128, 65, 0x123456789abcdef);
	u64 value5 = spine5.get(96, 65);
	assert(0x89abcdef==value5);
	
	//set value by byte array, and get it out in range 15-8, which is the second item in the byte array
	char byte_array[8] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7};
	spine_bv<64> spine7;
	spine7.set_value_byte_arr(byte_array, 7);
	assert(spine7.get(15, 8) == 0x1);

	//set value by u32 array, and get it out in range 95-32, which is the second+third item in the u32 array
	u32 u32_array[8] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7};
	spine_bv<320> spine8;
	spine8.set_value_u32_arr(u32_array, 8);
	assert(spine8.get(95, 32) == 0x200000001);

	//set value by u64 array, and get it out fifth item in the u64 array
	u64 u64_array[8] = {0xab,0xcb,0x21,0x33,0x48,0xe5,0x6ff,0x7ee};
	spine_bv<640> spine9;
	spine9.set_value_u64_arr(u64_array, 8);
	assert(spine9.get(64*5-1, 64*4) == 0x48);

	//set value by u64 array, by using default size, which equals to SZ/64
	spine_bv<512> spine10;
	spine10.set_value_u64_arr(u64_array /*, 8*/);
	assert(spine10.get(64*5-1, 64*4) == 0x48);

	//set spine_bv using a direct number
	spine_bv<216> spine11 = (u64)(0x12345678);
	assert(spine11.get(31, 0) == (u64)0x12345678);

	//set spine_bv using another spine_bv, and test if they are equal using operator==
	spine_bv<216> spine12 = spine11;
	assert(spine12.get(31,0) == (u64)0x12345678);
	spine_bv<216> spine121;
	spine_bv<216> spine122;
	spine122 = spine121 = spine12;
	assert(spine122.to_u64() == (u64)0x12345678);
	assert(spine121.to_u64() == (u64)0x12345678);
	spine121 = spine121;
	assert(spine121.to_u64() == (u64)0x12345678);
	assert(spine121 == (u64)0x12345678);
	assert(spine121 == (u32)0x12345678);
	spine122.set_bit(3, 0);
	spine12 = spine122;
	assert(spine12.to_uint() == 0x12345670);

	//access particular bit in spine_bv 
	spine_bv<65> spine13;
	spine13.set(64, 60, 0x1F);
	assert(spine13.get_bit(64) == 1);
	assert(spine13.get_bit(63) == 1);
	assert(spine13.get_bit(60) == 1);
	assert(spine13.get_bit(59) == 0);
	spine13.set_bit(59, 1);
	assert(spine13.get_bit(59) == 1);

	spine_bv<135> spine14;
	spine14.set(130, 126, 0x1F);
	spine14.set(66, 62, 0x1F);
	spine14.set(5, 0, 0x3F);
	spine14 <<= 1;
	assert(spine14.get_bit(132) == 0);
	assert(spine14.get_bit(130) == 1);
	assert(spine14.get_bit(131) == 1);
	assert(spine14.get_bit(128) == 1);
	assert(spine14.get_bit(127) == 1);
	assert(spine14.get_bit(68) == 0);
	assert(spine14.get_bit(67) == 1);
	assert(spine14.get_bit(64) == 1);
	assert(spine14.get_bit(6) == 1);
	assert(spine14.get_bit(0) == 0);

	//operator <<=
	spine14 <<= 4;
	assert(spine14.get_bit(134) == 1);
	assert(spine14.get_bit(130) == 0);
	assert(spine14.get_bit(131) == 1);
	assert(spine14.get_bit(128) == 0);
	assert(spine14.get_bit(68) == 1);
	assert(spine14.get_bit(66) == 0);
	assert(spine14.get_bit(64) == 0);
	assert(spine14.get_bit(6) == 1);
	assert(spine14.get_bit(10) == 1);
	assert(spine14.get_bit(0) == 0);
	spine14.set_bit(130, 1);
	assert(spine14.get_bit(130) == 1);
	const spine_bv<20> spine141 = 0x13;
	assert((const_cast<spine_bv<20>& >(spine141)).get_bit(0) == 1);
	const spine_bv<20> spine142 = 0x12;
	assert(spine142.get_bit(0) == 0);

	//test to_string() function
	std::cout<<"spine14 to string: 0x"<<(spine14.to_string())<<std::endl;

	//test get_range()
	spine_bv<135> spine15;
	spine15.set(130, 120, 0x19a);
	spine_bv<5> spine16;
	spine16.set(4, 0, 0x0c);
	spine_bv<235> spine17;
	spine17.set(230, 220, 0x19a);
	spine_bv_base bv_base1 = spine15.get_range(129, 125);
	spine_bv_base bv_base2 = spine16.get_range(4, 0);
	assert(bv_base1 == bv_base2);
	spine_bv_base bv_base3 = spine17.get_range(230, 100);
	spine_bv_base bv_base4 = spine15.get_range(130, 0);
	assert(bv_base3 == bv_base4);
	assert(spine15.get_range(128, 126) == spine16.get_range(3, 1));
	assert(spine15.get_range(128, 120).get_range(128, 126) == spine16.get_range(4, 0).get_range(3, 1));
	assert(spine15.get_range(128, 125) != spine16.get_range(3, 1));
	spine_bv_base bv_base5 = bv_base1;
	assert(bv_base5 == bv_base1);
	spine_bv_base bv_base6;
	bv_base6 = bv_base5;
	assert(bv_base6 == bv_base1);

	//test set_range()
	spine_bv<200> spine18;
	spine18.set(128, 125, 0xF);
	spine_bv<100> spine19;
	spine19.set(83, 79, 0x1F);
	spine18.set_range(123, 53, spine19.get_range(83, 13)); //1111 0111 1100 00...
	assert(spine18.get(128, 65) == 0xF7C0000000000000);
	std::cout<<"spine18 range(128, 65) to_string: "<<spine18.get_range(128, 65).to_string()<<std::endl;
	std::cout<<"spine18 range(198, 100) to_string: "<<spine18.get_range(198, 100).to_string()<<std::endl;

	//test operator= and copy constructor between spine_bv and spine_bv_base
	spine_bv<200> spine20 = spine18.get_range(128, 65);
	assert(spine20.get(63, 0) == 0xF7C0000000000000);
	spine_bv<20> spine21; 
	spine21 = spine20.get_range(63, 60);
	assert(spine21.get(3, 0) == 0xF);
	spine_bv<2>spine22 = spine20.get_range(63, 63); //boundary testing
	assert(spine22.get_bit(0));
	assert(!spine22.get_bit(1));

	spine_bv<994> f4_pl_l3_hbus;
	f4_pl_l3_hbus.set(321,274,0xffffffffffff); //setting [321:274] bit field
	//std::cout<<std::hex<<f4_pl_l3_hbus.get(321,274)<<std::endl;
	assert(f4_pl_l3_hbus.get(321,274) == 0xffffffffffff);

	//test self_xor
	spine_bv<130> spine23;
	spine23.set(129, 128, 0x2); //1*1
	spine23.set(127, 64, 0xfffe); //15*1
	spine23.set(63, 0, 0xfeff); //15*1
	assert(spine23.self_xor());
	spine23.set(3, 0, 0xe);
	assert(!spine23.self_xor());

	//test bitwise operators &,|,^
	spine_bv<300> spine24;
	spine24.set(130, 115, 0xFFFF);
	spine_bv<300> spine241 = spine24;
	spine_bv<300> spine25;
	spine25.set(130, 115, 0x0F0F);
	spine_bv<300> spine26;
	spine26.set(130, 115, 0xF0F0);
	spine_bv<300> spine27 = spine24&spine25;
	spine_bv<300> spine271 = spine24.get_range(130, 115)&spine25.get_range(130, 115);
	assert(spine271.get(15, 0) == 0x0F0F);
	
	assert(spine24 == spine241);
	assert(spine27 == spine25);
	spine_bv<300> spine28 = spine25|spine26;
	assert(spine28 == spine24);
	spine_bv<300> spine29 = spine24^spine25;
	assert(spine29 == spine26);
	spine_bv<300> spine30 = (((spine24&spine25)|spine26)^spine25); // this works with using braces
	//spine_bv<300> spine30 = spine24&spine25|spine26^spine25; //this does not work 
	assert(spine30 == spine26);
	assert((spine25.get_range(130, 115) | spine26.get_range(130, 115)) == spine24.get_range(130, 115));
	assert((spine25.get_range(130, 115) ^ spine24.get_range(130, 115)) == spine26.get_range(130, 115));

	//test concatenation
	spine_bv<8> spine32 = 0xFF;
	spine_bv<8> spine33 = 0xEE;
	spine_bv<80> spine34 = 0xAA;
	spine_bv<280> spine35 = 0xBB;
	spine_bv<16> spine36 = spine32.concat(spine33.to_base());
	assert(spine36.to_uint() == 0xFFEE);
	spine_bv<383> spine37 = spine35.concat(spine34.to_base()).concat(spine33.to_base()).concat(spine32.to_base());
	assert(spine37.get(23, 16) == 0xAA);
	spine_bv<383> spine38 = spine35 + spine34.to_base() + spine33.to_base() + spine32.to_base();
	assert(spine37 == spine38);
	std::cout<<spine38.to_string()<<std::endl;	
	spine_bv<20> spine39 = spine32.get_range(3,0) + spine34.get_range(15, 0);
	assert(spine39.to_uint() == 0xF00AA);

	return 0;
}
