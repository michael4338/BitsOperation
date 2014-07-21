
#ifndef __SPINE_BV_HPP__
#define __SPINE_BV_HPP__

#include <cstddef>
#include <assert.h>
#include <string>
#include <sstream>
#include <iostream>
#include <string.h>

#include "spine_bv_util.hpp" 

#define u64 unsigned long long int
#define u32 unsigned int

#define SZ64 64

class spine_bv_base
{
public:
	spine_bv_base():m_size_base(0), m_arr_base_pointer(NULL)
	{
		init_base();
	}

	spine_bv_base(int size):m_size_base(size), m_arr_base_pointer(NULL)
	{
		init_base();
	}

	spine_bv_base(int size, u64* buf, int msb, int lsb):m_size_base(size), m_arr_base_pointer(NULL)
	{
		init_base();
		memcpy(this->m_arr_base_pointer, buf, this->m_size_base*SZ64/8);
		m_msb = msb;
		m_lsb = lsb;
	}

	spine_bv_base(const spine_bv_base& bv_base)
	{
		this->m_size_base = bv_base.m_size_base;
		init_base();
		memcpy(this->m_arr_base_pointer, bv_base.m_arr_base_pointer, this->m_size_base*SZ64/8);
		this->m_msb = bv_base.m_msb;
		this->m_lsb = bv_base.m_lsb;
	}

	spine_bv_base& operator=(const spine_bv_base& bv_base)
	{
		if(m_size_base == 0) 
		{
			this->m_size_base = bv_base.m_size_base;
			init_base();
			memcpy(this->m_arr_base_pointer, bv_base.m_arr_base_pointer, this->m_size_base*SZ64/8);
			this->m_msb = bv_base.m_msb;
			this->m_lsb = bv_base.m_lsb;
			return *this;
		}
		
/*
		int num = ((m_msb-m_lsb)>(bv_base.m_msb-bv_base.m_lsb)?(bv_base.m_msb-bv_base.m_lsb+1):(m_msb-m_lsb+1));
		for(int i=0; i<num; i++)
		{
			int index_get = (m_lsb+i)/SZ64;
			int index_set = (bv_base.m_lsb+i)/SZ64;
			int pos_get = (m_lsb+i)%SZ64;
			int pos_set = (bv_base.m_lsb+i)%SZ64;

			//u64 mask_get = ((u64) (1)<<pos_get);
			u64 mask_set = ((u64) (1)<<pos_set);
				
			u64 bit = (bv_base.m_arr_base_pointer[index_set]&mask_set);
			m_arr_base_pointer[index_get] = ((m_arr_base_pointer[index_get] & ~((u64) (1)<<pos_get)) | (bit << pos_set));
		}	
*/
		spine_bv_util_struct left(this->m_arr_base_pointer, m_msb, m_lsb), right(const_cast<u64*>(bv_base.m_arr_base_pointer), bv_base.m_msb, bv_base.m_lsb);
		spine_bv_util bv_util;
		u64* buf;
		bv_util.util_do_operation(left, right, '=', buf);

		return *this;	
	}

	bool operator==(const spine_bv_base& bv_base)
	{
		if((bv_base.m_msb-bv_base.m_lsb) != (m_msb-m_lsb)) return false;

		spine_bv_util_struct left(this->m_arr_base_pointer, m_msb, m_lsb), right(const_cast<u64*>(bv_base.m_arr_base_pointer), bv_base.m_msb, bv_base.m_lsb);
		spine_bv_util bv_util;
		u64 buf[1] = {0};
		u64* out_buf = buf;
		bv_util.util_do_operation(left, right, 'c', out_buf);

		return (out_buf[0] == 0);

/*
		int num = m_msb-m_lsb+1;
		for(int i=0; i<num; i++)
		{
			int index_this = (m_lsb+i)/SZ64;
			int index_other = (bv_base.m_lsb+i)/SZ64;
			int pos_this = (m_lsb+i)%SZ64;
			int pos_other = (bv_base.m_lsb+i)%SZ64;
			u64 mask_this = ((u64) (1)<<pos_this);
			u64 mask_other = ((u64) (1)<<pos_other);
			u64 bit_this = ((m_arr_base_pointer[index_this]&mask_this) != 0);
			u64 bit_other = ((bv_base.m_arr_base_pointer[index_other]&mask_other) != 0);
			if(bit_this != bit_other) return false;
		}	
		return true;
*/
	}

	bool operator!=(const spine_bv_base& bv_base)
	{
		return !(*this == bv_base);
	}

	spine_bv_base operator+ (const spine_bv_base& bv_base)
	{
		return concat(bv_base);
	}

	spine_bv_base operator& (const spine_bv_base& bv_base)
	{
		return do_bitwise_operation(bv_base, '&');
	}
	spine_bv_base operator| (const spine_bv_base& bv_base)
	{
		return do_bitwise_operation(bv_base, '|');
	}
	spine_bv_base operator^ (const spine_bv_base& bv_base)
	{
		return do_bitwise_operation(bv_base, '^');
	}

public:
	virtual ~spine_bv_base()
	{
		if(NULL != m_arr_base_pointer)
		{
			delete []m_arr_base_pointer;
			m_arr_base_pointer = NULL;
		}
	}

	spine_bv_base& get_range(int msb, int lsb)
	{
		assert(msb < m_size_base*SZ64);
		assert(lsb >= 0);
		m_msb = msb;
		m_lsb = lsb;
		return *this;
	}

	spine_bv_base concat(const spine_bv_base& bv_right)
	{
/*
		spine_bv_base bv_left = *this;
		u32 realloc_len = bv_right.get_range_size();
		u32 self_len = bv_left.get_range_size();
		u32 new_size = (realloc_len + self_len)/SZ64+1;
		bv_left.m_arr_base_pointer = (u64*)realloc(bv_left.m_arr_base_pointer, new_size*SZ64/8);		
		memset(bv_left.m_arr_base_pointer+bv_left.m_size_base, 0, new_size-bv_left.m_size_base);
		bv_left.m_size_base = new_size;
		
		for(int i=self_len-1; i>=0; i--) bv_left.set_range_bit(realloc_len+i, bv_left.get_range_bit(i));
		for(int i=0; i<(int)realloc_len; i++) bv_left.set_range_bit(i, bv_right.get_range_bit(i));
		bv_left.m_msb += realloc_len;
		return bv_left;
*/

		u32 self_len = get_range_size();
		u32 right_len = bv_right.get_range_size();
		u32 new_size = (right_len + self_len)/SZ64+1;
		u64* buf = new u64[new_size];
		memset(buf, 0, new_size*SZ64/8);

		spine_bv_util bv_util;
		u64* out_buf = NULL;
		spine_bv_util_struct left1(buf, right_len-1, 0), right1(const_cast<u64*>(bv_right.m_arr_base_pointer), bv_right.m_msb, bv_right.m_lsb);
		bv_util.util_do_operation(left1, right1, '=', out_buf);
		spine_bv_util_struct left2(buf, right_len+m_msb, right_len+m_lsb), right2(m_arr_base_pointer, m_msb, m_lsb);
		bv_util.util_do_operation(left2, right2, '=', out_buf);

		spine_bv_base res(new_size, buf, right_len+self_len-1, 0);
		delete []buf;
		return res;
	}

	void set_range_value_u64_arr(u64* arr, int size)
	{
		int len = (size>m_size_base?m_size_base:size);
		for(int i=0; i<len; i++) m_arr_base_pointer[i] = arr[i];
	}
	
	int get_range_size() const
	{
		return m_msb-m_lsb+1;
	}

	bool get_range_bit(u32 pos) const
	{
		pos += m_lsb;
		assert(pos < (u32)m_size_base*SZ64);
		return ((m_arr_base_pointer[pos/SZ64]>>(pos%SZ64))&((u64)1)) == 1;
	}

	void set_range_bit(u32 pos, bool val)
	{
		pos += m_lsb;
		assert(pos < (u32)m_size_base*SZ64);
		int index = pos/SZ64;
		pos %= SZ64;
		u64 val64 = (u64)val;	
		m_arr_base_pointer[index] = (m_arr_base_pointer[index]&(~((u64)1<<pos)))|(val64<<pos);
	}

	std::string to_string() const
	{
		std::string result;
		int len = m_msb-m_lsb+1;
		if(len <= 0) return result;

		std::stringstream stream;
		int prefix = len%4;
		u32 current_val = 0;
		if(prefix != 0)
		{
			for(int i=0; i<prefix; i++)
			{
				current_val <<= 1;
				current_val += get_range_bit(m_msb-m_lsb-i);
			}
			stream<<std::hex<<current_val;
		}

		current_val = 0;
		int counter = 0;
		for(int i=len-prefix-1; i>=0; i--)
		{
			current_val <<= 1;
			current_val += get_range_bit(i);
			counter ++;
			if(counter%4 == 0)
			{
				stream<<std::hex<<current_val;
				current_val = 0;
			}
		}

		result = stream.str();
		return result;
	}

private:
	void init_base()
	{
		if(m_size_base != 0) 
		{
			m_arr_base_pointer = new u64[m_size_base];
			memset(m_arr_base_pointer, 0, m_size_base*SZ64/8);
		}
		else
		{
			m_arr_base_pointer = NULL;
		}
		m_lsb = 0;
		m_msb = m_size_base*SZ64-1;
	}

	spine_bv_base do_bitwise_operation(const spine_bv_base& bv_base, char op)
	{
		assert(bv_base.get_range_size() == get_range_size());
		u64* out_buf = new u64[m_size_base];
		spine_bv_util_struct left(this->m_arr_base_pointer, m_msb, m_lsb), right(const_cast<u64*>(bv_base.m_arr_base_pointer), bv_base.m_msb, bv_base.m_lsb);

		spine_bv_util bv_util;
		bv_util.util_do_operation(left, right, op, out_buf);

		spine_bv_base res(m_size_base, out_buf, get_range_size()-1, 0);
		delete []out_buf;
		return res;
	}

//private:
public:
	int m_size_base;
	u64* m_arr_base_pointer;
	int m_msb;
	int m_lsb;
};







template <u32 SZ>
class spine_bv : public spine_bv_base
{
public:
	spine_bv() : spine_bv_base()
	{
		init();
	}

	spine_bv<SZ> (u64 val) : spine_bv_base()
	{
		init();
		this->set_value(val);
	}
	
	spine_bv<SZ> (u64* buf) : spine_bv_base()
	{
		init();
		memcpy(this->m_arr, buf, this->m_size*SZ64/8);
	}
	
	spine_bv<SZ> (const spine_bv<SZ>& bv) : spine_bv_base()
	{
		init();
		memcpy(this->m_arr, bv.m_arr, this->m_size*SZ64/8);
	}

	spine_bv<SZ> (const spine_bv_base& bv_base) : spine_bv_base()
	{
		init();
		*this = bv_base;
	}

	~spine_bv()
	{
		if(NULL != m_bv_base)
		{
			delete m_bv_base;
			m_bv_base = NULL;
		}
	}

public:
	spine_bv<SZ>& operator=(u64 val)
	{
		init();
		this->set_value(val);
		return *this;
	}

	spine_bv<SZ>& operator=(spine_bv<SZ>& bv)
	{
		//init();
		memcpy(this->m_arr, bv.m_arr, this->m_size*SZ64/8);
		//swap(*this, bv);
		return *this;
	}

	spine_bv<SZ>& operator=(const spine_bv<SZ>& bv)
	{
		//init();
		memcpy(this->m_arr, bv.m_arr, this->m_size*SZ64/8);
		//swap(*this, bv);
		return *this;
	}

	spine_bv<SZ>& operator=(const spine_bv_base& bv_base)
	{
		init();
		set_range(SZ-1, 0, bv_base);
		return *this;
	}

	void operator<<=(int times)
	{
		for(int i=0; i<times; i++)
		{
			int size = (SZ%SZ64==0?SZ/SZ64:SZ/SZ64+1);
			for(int j=size-1; j>=0; j--)
			{
				this->m_arr[j] <<= 1;
				if(j != 0)
				{
					bool last_head = get_bit(SZ64*j-1);
					set_bit(SZ64*j, last_head);
				}
			}
		}
	}

	bool operator==(const spine_bv<SZ>& bv)
	{
		if(this->m_size == bv.m_size)
			return memcmp(this->m_arr, bv.m_arr, this->m_size*SZ64/8) == 0;
		return false;
	}

	bool operator==(u64 val)
	{
		return val==this->get_value();
	}

	bool operator==(u32 val)
	{
		return val==this->to_uint();
	}

	spine_bv<SZ> operator& (const spine_bv<SZ>& bv)
	{
		return doOperation(bv, '&');
	}

	spine_bv<SZ> operator| (const spine_bv<SZ>& bv)
	{
		return doOperation(bv, '|');
	}

	spine_bv<SZ> operator^ (const spine_bv<SZ>& bv)
	{
		return doOperation(bv, '^');
	}

	spine_bv_base operator+ (const spine_bv_base& bv_base)
	{
		return concat(bv_base);
	}

public:
	u64 get(int msb, int lsb) const
	{
		assert(msb >= lsb);
		assert(lsb >= 0);
		assert((u32)msb < SZ);
		assert((u32)(msb-lsb) <= SZ64);

		//return this->get_by_bit(msb, lsb);
		return this->get_by_value(msb, lsb);
	}

	void set(int msb, int lsb, u64 val)
	{
		assert(msb >= lsb);
		assert(lsb >= 0);
		assert((u32)msb < SZ);
		assert((u32)(msb-lsb) < SZ);

		//this->set_by_bit(msb, lsb, val);
		this->set_by_value(msb, lsb, val);
	}

	bool get_bit(u32 pos) const
	{
		//return (bool)get(pos, pos);
		assert(pos < SZ);
		return ((m_arr[pos/SZ64]>>(pos%SZ64))&((u64)1)) == 1;
	}

	void set_bit(u32 pos, bool val)
	{
		//set(pos, pos, val);
		assert(pos < SZ);
		int index = pos/SZ64;
		pos %= SZ64;
		u64 val64 = (u64)val;
		m_arr[index] = (m_arr[index]&(~((u64)1<<pos))) | (val64<<pos);
	}

	u64 get_value() const
	{
		int msb = SZ>SZ64-1?SZ64-1:SZ-1;
		return get(msb, 0);
	}

	void set_value(u64 val)
	{
		int msb = SZ>SZ64-1?SZ64-1:SZ-1;
		set(msb, 0, val);
	}

	void set_value_byte_arr(char* arr, int size=SZ/8, int offset=0)
	{
		assert(NULL != arr);
		assert(offset >= 0);

		int bytesize = sizeof(unsigned char)*8;

		for(int i=0; i<size; i++)
		{
			unsigned char bytevalue = (unsigned char)(arr[i]);
			set(offset+bytesize-1, offset, bytevalue);		
			offset += bytesize;
		}
	}

	void set_value_u32_arr(u32* arr, int size=SZ/32, int offset=0)
	{
		assert(NULL != arr);
		assert(offset >= 0);

		int uintsize = sizeof(u32)*8;

		for(int i=0; i<size; i++)
		{
			u32 uintval = (u32)(arr[i]);
			set(offset+uintsize-1, offset, uintval);		
			offset += uintsize;
		}
	}

	void set_value_u64_arr(u64* arr, int size=SZ/64, int offset=0)
	{
		assert(NULL != arr);
		assert(offset >= 0);

		int longsize = sizeof(u64)*8;

		for(int i=0; i<size; i++)
		{
			u64 longval = (u64)(arr[i]);
			set(offset+longsize-1, offset, longval);		
			offset += longsize;
		}
	}

	u32 to_uint() const
	{
		int msb = SZ>31?31:SZ-1;
		u32 res = (u32)(get(msb, 0));
		return res;
	}

	u64 to_u64() const
	{
		return get_value();
	}

	std::string to_string() const
	{
		std::string result;
		if(SZ == 0) return result;

		std::stringstream stream;
		int prefix = SZ%4;
		if(prefix != 0)
		{
			u64 prefix_val = get(SZ-1, SZ-prefix);
			stream<<std::hex<<prefix_val;
		}

		for(int i=SZ-prefix; i>0; i-=4)
		{
			u64 cur_val = get(i-1, i-4>=0?i-4:0);
			stream << std::hex << cur_val;
		}
			
		result = stream.str();
		return result;
	}

	spine_bv_base& get_range(int msb, int lsb) 
	{
		build_spine_bv_base();
		assert(NULL != m_bv_base);
		return m_bv_base->get_range(msb, lsb);
	}

	spine_bv_base& to_base()
	{
		return get_range(SZ-1, 0);
	}

	spine_bv_base concat(const spine_bv_base& bv_base)
	{
		return to_base().concat(bv_base);		
	}

	spine_bv_base& set_range(int msb, int lsb, const spine_bv_base& bv_base)
	{
		assert(msb >= lsb);
		assert(lsb >= 0);
		assert((u32)msb < SZ);

/*
		int range_len = ((msb-lsb+1)>(bv_base.get_range_size())?(bv_base.get_range_size()):(msb-lsb+1));
		for(int i=0; i<range_len; i++)
		{
			bool bit = bv_base.get_range_bit(i);
			set_bit_to_array(lsb+i, bit);
		}
*/
		
		spine_bv_util_struct left(this->m_arr, msb, lsb), right(const_cast<u64*>(bv_base.m_arr_base_pointer), bv_base.m_msb, bv_base.m_lsb);
		spine_bv_util bv_util;
		u64* buf = NULL;
		bv_util.util_do_operation(left, right, '=', buf);

		return get_range(msb, lsb);
	}

	bool self_xor()
	{
		u64 residue = 0;
		int one_counter = 0;

		for(int i=0; i<(int)m_size; i++)
		{
			residue ^= m_arr[i];
		}
		while(residue)
		{
			if(residue%2) one_counter++;
			residue >>= 1;
		}
		
		return one_counter%2;
	}

	

private:
	u32 m_size;
	u64 m_arr[SZ/SZ64+1];
	spine_bv_base* m_bv_base;

private:
	inline void init()
	{
		this->m_size = SZ/SZ64+1;
		memset(m_arr, 0, this->m_size*SZ64/8);
		m_bv_base = NULL;
	}

	inline u64 get_by_bit(int msb, int lsb) const
	{
		u64 res = 0;
		for(int i=msb; i>=lsb; i--)
		{
			res = (res<<1) + (int)(get_bit_from_array(i));
		}
		
		return res;
	}

	inline u64 get_by_value(int msb, int lsb) const
	{
		u64 res = 0;

		int index_lsb = lsb/SZ64;
		int index_msb = msb/SZ64;

		if(index_lsb == index_msb)
		{
			int mask_len = msb-lsb+1;
			u64 mask = (mask_len == SZ64)?((u64)0-1):(((u64)1<<(mask_len))-1);
			res = (m_arr[index_lsb]>>(lsb%SZ64))&mask;
		}
		else
		{
			int leftshift_msb = SZ64-(lsb%SZ64);
			int rightshift_lsb = lsb%SZ64;
			int pos_msb = (msb)%SZ64+1;

			int mask_len_msb = pos_msb+leftshift_msb;
			u64 mask_msb = (mask_len_msb == SZ64)?((u64)0-1):(((u64)1<<(mask_len_msb))-1);

			u64 val_lsb = this->m_arr[index_lsb];
			u64 val_msb = this->m_arr[index_msb];

			u64 lsb_res = val_lsb>>rightshift_lsb;
			u64 msb_res = (val_msb<<leftshift_msb)&mask_msb;
			res = lsb_res+msb_res;
		} 

		return res;
	}

	inline void set_by_bit(int msb, int lsb, u64 val)
	{
		for(int i=0; i<SZ64 && i<=(msb-lsb); i++)
		{
			bool bit = (((u64)1)<<i)&val;
			set_bit_to_array(lsb+i, bit);
		}
	}

	inline void set_by_value(int msb, int lsb, u64 val)
	{
		int index_lsb = lsb/SZ64;
		int index_msb = msb/SZ64;

		if(index_lsb == index_msb)
		{
			int mask_len = msb-lsb+1;
			u64 mask = (mask_len == SZ64)?((u64)0-1):(((u64)1<<(mask_len))-1);
			val &= mask;
			m_arr[index_lsb] = (m_arr[index_lsb]&(~(mask<<(lsb%SZ64)))) | (val<<(lsb%SZ64));
		}
		else
		{
			int mask_len_lsb = SZ64-lsb%SZ64;
			int mask_len_msb = msb%SZ64 + 1;
			u64 mask_lsb = ((u64)1<<(mask_len_lsb))-1;
			u64 mask_msb = ((u64)1<<(mask_len_msb))-1;
			
			u64 val_lsb = val & mask_lsb;
			u64 val_msb = (val>>mask_len_lsb) & mask_msb;

			this->m_arr[index_lsb] = ((this->m_arr[index_lsb]&(~(mask_lsb<<(lsb%SZ64)))) | (val_lsb<<(lsb%SZ64)));
			this->m_arr[index_msb] = ((this->m_arr[index_msb]&(~(mask_msb))) | (val_msb));

		}
	}

	inline bool get_bit_from_array(int pos) const
	{
		int index = pos/SZ64;
		assert((u32)index < m_size);
		pos %= SZ64;	
		u64 mask = ((u64) (1)<<pos);
		return (m_arr[index]&mask) != 0;
	}

	inline void set_bit_to_array(int pos, u64 bit)
	{
		int index = pos/SZ64;
		assert((u32)index < m_size);
		pos %= SZ64;	
        	m_arr[index] = ((m_arr[index] & ~((u64) (1)<<pos)) | (bit << pos));
	}

	inline void swap(spine_bv<SZ>& first, spine_bv<SZ>& second) 
	{
		//std::swap(first.m_size, second.m_size); 
		//std::swap(first.m_arr, second.m_arr);
	}

	void build_spine_bv_base()
	{
		if(NULL == m_bv_base)
		{
			m_bv_base = new spine_bv_base(this->m_size);
			m_bv_base->set_range_value_u64_arr(this->m_arr, this->m_size);		
		}
	}
	
	spine_bv<SZ> doOperation(const spine_bv<SZ>& bv, char op)
	{
		u64 buf[SZ/SZ64+1] = {0};
		u64* out_buf = buf;
		spine_bv_util_struct left(this->m_arr, (int)SZ-1, 0), right(const_cast<u64*>(bv.m_arr), (int)SZ-1, 0);

		spine_bv_util bv_util;
		bv_util.util_do_operation(left, right, op, out_buf);

		spine_bv<SZ> res(out_buf);
		return res;
	}

};


#endif
