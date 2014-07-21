#ifndef __SPINE_BV_UTIL_HPP__
#define __SPINE_BV_UTIL_HPP__

#include <cstddef>
#include <assert.h>
#include <string>
#include <sstream>
#include <iostream>
#include <string.h>

#define u64 unsigned long long int
#define u32 unsigned int

#define SZ64 64

class spine_bv_util_struct{
public:
	spine_bv_util_struct(){}
	spine_bv_util_struct(u64* a_buf, int a_msb, int a_lsb):buf(a_buf), msb(a_msb), lsb(a_lsb){}
	~spine_bv_util_struct(){}
	u64* buf;
	int msb;
	int lsb;
	
};

class spine_bv_util
{
public:
	spine_bv_util(){}
	~spine_bv_util(){}

public:
	u64 util_get(const u64* buf, int msb, int lsb)
	{	
		u64 res = 0;

		int index_lsb = lsb/SZ64;
		int index_msb = msb/SZ64;

		if(index_lsb == index_msb)
		{
			int mask_len = msb-lsb+1;
			u64 mask = (mask_len == SZ64)?((u64)0-1):(((u64)1<<(mask_len))-1);
			res = (buf[index_lsb]>>(lsb%SZ64))&mask;
		}
		else
		{
			int leftshift_msb = SZ64-(lsb%SZ64);
			int rightshift_lsb = lsb%SZ64;
			int pos_msb = (msb)%SZ64+1;

			int mask_len_msb = pos_msb+leftshift_msb;
			u64 mask_msb = (mask_len_msb == SZ64)?((u64)0-1):(((u64)1<<(mask_len_msb))-1);

			u64 val_lsb = buf[index_lsb];
			u64 val_msb = buf[index_msb];

			u64 lsb_res = val_lsb>>rightshift_lsb;
			u64 msb_res = (val_msb<<leftshift_msb)&mask_msb;
			res = lsb_res+msb_res;
		} 

		return res;
	}

	void util_set(u64*& buf, int msb, int lsb, u64 val)
	{
		int index_lsb = lsb/SZ64;
		int index_msb = msb/SZ64;

		if(index_lsb == index_msb)
		{
			int mask_len = msb-lsb+1;
			u64 mask = (mask_len == SZ64)?((u64)0-1):(((u64)1<<(mask_len))-1);
			val &= mask;
			buf[index_lsb] = (buf[index_lsb]&(~(mask<<(lsb%SZ64)))) | (val<<(lsb%SZ64));
		}
		else
		{
			int mask_len_lsb = SZ64-lsb%SZ64;
			int mask_len_msb = msb%SZ64 + 1;
			u64 mask_lsb = ((u64)1<<(mask_len_lsb))-1;
			u64 mask_msb = ((u64)1<<(mask_len_msb))-1;
			
			u64 val_lsb = val & mask_lsb;
			u64 val_msb = (val>>mask_len_lsb) & mask_msb;

			buf[index_lsb] = ((buf[index_lsb]&(~(mask_lsb<<(lsb%SZ64)))) | (val_lsb<<(lsb%SZ64)));
			buf[index_msb] = ((buf[index_msb]&(~(mask_msb))) | (val_msb));
		}
	}

	void util_get_rounds(int& rounds_msb, int& rounds_lsb, int msb, int lsb, int rounds, int index)
	{
		if(index == rounds)
		{
			rounds_lsb = index*SZ64 + lsb;
			rounds_msb = msb;
		}
		else
		{
			rounds_lsb = index*SZ64 + lsb;
			rounds_msb = SZ64 + rounds_lsb - 1;
		}
		
		return;
	}

        void util_do_operation(spine_bv_util_struct& left_op, spine_bv_util_struct& right_op, char op, u64*& out_buf)
	{	
		int left_msb=0, left_lsb=0, right_msb=0, right_lsb=0;
		u64 left_value=0, right_value=0;

		int len_left = left_op.msb - left_op.lsb + 1;
		int len_right = right_op.msb - right_op.lsb + 1;
		if(len_left > len_right)
		{
			left_op.msb -= (len_left-len_right);
		}
		else
		{
			right_op.msb -= (len_right-len_left);
		}
		int rounds = (len_left>len_right?len_right:len_left)/SZ64;

		for(int i=0; i<=rounds; i++)
		{
			util_get_rounds(left_msb, left_lsb, left_op.msb, left_op.lsb, rounds, i);
			util_get_rounds(right_msb, right_lsb, right_op.msb, right_op.lsb, rounds, i);

			left_value = util_get(left_op.buf, left_msb, left_lsb);
			right_value = util_get(right_op.buf, right_msb, right_lsb);

			switch(op)
			{
				case '&':
					util_set(out_buf, (i+1)*SZ64-1, i*SZ64, left_value & right_value);
					break;
				case '|':
					util_set(out_buf, (i+1)*SZ64-1, i*SZ64, left_value | right_value);
					break;
				case '^':
					util_set(out_buf, (i+1)*SZ64-1, i*SZ64, left_value ^ right_value);
					break;
				case '=':
					util_set(left_op.buf, left_msb, left_lsb, right_value);
					break;
				case 'c':
					out_buf[0] = (left_value!=right_value);
					break;
				default:
					break;
			}
		}
		
		return;
	}

};


#endif





