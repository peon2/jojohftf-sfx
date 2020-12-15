#ifndef CPS3DEC_H
#define CPS3DEC_H

#define K1 0x23323EE3
#define K2 0x03021972

#define BANKSIZE 0x800000
#define FILE10START 0x6000000

typedef unsigned char U8;
typedef unsigned short U16;

#if UINT_MAX == 4294967295U
typedef unsigned int U32;
#else
typedef unsigned long U32;
#endif

union unlong {
	U8 bytes[4];
	U32 ulong;
};

U16 
lrot(U16 v, int n) 
{ 
	U32 a = v>>(16-n); 
	return ((v<<n)|a)%0x10000;
}

U16 
rotxor(U16 v, U16 x) 
{
	U16 r = v+lrot(v,2);
	return lrot(r, 4)^(r&(v^x));
}

U32 
cps3mask(U32 addr, U32 k1, U32 k2)
{ 
	addr ^= k1;
	U16 v = (addr&0xffff)^0xffff; 
	v = rotxor(v, k2&0xffff);  
	v ^= (addr>>16)^0xffff;
	v = rotxor(v, k2>>16);
	v ^= (addr&0xffff)^(k2&0xffff);
	return v | (v<<16);
}

void *
flipendian(void *raw, int length, int wl) // data as bytes, number of bytes to flip, word length
{
	U8 *data = (U8 *)raw;
	U8 *reversed = malloc(length);
	for (int i=0; i < length; i++) 
		reversed[i] = data[wl*(i/wl) + wl - (i%wl) - 1];

	return reversed;
}

#endif
