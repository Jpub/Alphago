#include "go.h"

int BIT2FIRST[256] = {
	0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};


int Get_FirstBit32(int bit)
{ 
	if (bit & 0xffff) {
		if (bit & 0xff) {
			return BIT2FIRST[bit & 0xff];
		}
		else {
			return BIT2FIRST[(bit >> 8) & 0xff] + 8;
		}
	} else if (bit & 0xffff0000) {
		if (bit & 0x00ff0000) {
			return BIT2FIRST[(bit >> 16) & 0xff] + 16;
		}
		else {
			return BIT2FIRST[(bit >> 24) & 0xff] + 24;
		}
	}
	return 0;
}

int Get_FirstBit64(long long bit)  
{
  int res, bit2 = ((int)bit);

  if (bit2){
    res = Get_FirstBit32(bit2);
    return res;
  }
  bit2 = (int)(bit >> 32);
  if (bit2){
    res = Get_FirstBit32(bit2);
    return  res + 32;
  }

  return 0;
}

int pop_cnt(unsigned long long bits)
/* 64 bit のうち1がたった数を返す */
{
	bits = (bits & 0x5555555555555555LL) + (bits >> 1 & 0x5555555555555555LL);
	bits = (bits & 0x3333333333333333LL) + (bits >> 2 & 0x3333333333333333LL);
	bits = (bits & 0x0f0f0f0f0f0f0f0fLL) + (bits >> 4 & 0x0f0f0f0f0f0f0f0fLL);
	bits = (bits & 0x00ff00ff00ff00ffLL) + (bits >> 8 & 0x00ff00ff00ff00ffLL);
	bits = (bits & 0x0000ffff0000ffffLL) + (bits >> 16 & 0x0000ffff0000ffffLL);
	bits = (bits & 0x00000000ffffffffLL) + (bits >> 32 & 0x00000000ffffffffLL);
	return (int)bits;
}
