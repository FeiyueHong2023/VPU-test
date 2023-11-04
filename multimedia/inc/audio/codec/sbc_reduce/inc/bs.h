/*
 * bs.h
 *
 *  Created on: 2021��2��27��
 *      Author: DELL
 */
#pragma once
#ifndef __BS_H__
#define __BS_H__
#include <stdint.h>

#if 0
typedef struct{
	void* buff0;
	unsigned short* buf;
	void* buff_origin;
	int limit;
}bs_t;

static inline  unsigned bsusedbits(bs_t*bs){
	unsigned bits = RAE_BITPTR();
	unsigned v =  (char*)bs->buf - (char*)bs->buff0;
	return (v<<3)+bits;
}

//bs read
extern unsigned bsrinit(bs_t* bs,void*buff, int bytes);

static inline unsigned short bsread(bs_t*bs,int bits){
	unsigned short v=AE_LB(bits);
	AE_DB(bs->buf,bits);
	return v;
}

static inline unsigned short bspeek(bs_t*bs,int bits){
	return AE_LB(bits);
}
static inline void bspop(bs_t*bs,int bits){
	AE_DB(bs->buf,bits);
}

static inline void bspop1(bs_t*bs,int bits){
	while (bits > 16)
	{
		AE_DB(bs->buf,16);
		bits -= 16;
	}
	AE_DB(bs->buf,bits);
}

//bs write
extern unsigned bswinit(bs_t* bs,void*buff0);

static inline void bssetbits(bs_t*bs,uint8_t bits){
	WAE_BITSUSED(bits);
}
static inline void bswrite(bs_t*bs,unsigned val){
	AE_SB(bs->buf,val);
}
static inline void bsflash(bs_t*bs){
	AE_SBF(bs->buf);
}
#define bswritei(bs,val,i_bits) AE_SBI((bs)->buf,val,i_bits)
#endif

#if 1
static const uint32_t sbcBitMask[16 + 1] = {
    0x0,        0x1,        0x3,       0x7,       0xf,       0x1f,
    0x3f,       0x7f,       0xff,      0x1ff,     0x3ff,     0x7ff,
    0xfff,      0x1fff,     0x3fff,    0x7fff,    0xffff};

typedef struct {
	uint8_t *buffer;
	uint32_t index;
	uint32_t bitsInCache;
	uint32_t cacheWord;
	uint32_t bitsUsed;
} bs_t;

static inline  unsigned bsusedbits(bs_t*bs){
	uint32_t used = bs->index*8 + bs->bitsInCache;
	return used;
}

extern unsigned bsrinit(bs_t* bs,void*buff, int bytes);

static inline uint16_t bsread(bs_t*bs, int num){
	int tmpnum = num - bs->bitsInCache;
	uint32_t bits = 0;
	if(tmpnum > 0){
		bits = bs->cacheWord << tmpnum;
		bs->cacheWord = (bs->buffer[bs->index]<<24) |
		(bs->buffer[bs->index+1]<<16) |
		(bs->buffer[bs->index+2]<<8) |
		(bs->buffer[bs->index+3]);
		bs->index+=4;
		bs->bitsInCache += 32;
	}
	bs->bitsInCache-=num;
	return (bits | (bs->cacheWord >> bs->bitsInCache)) & sbcBitMask[num];
}

extern unsigned bswinit(bs_t* bs,void*buff0);

static inline void bssetbits(bs_t*bs,uint8_t bits){
	bs->bitsUsed = bits==0?16:bits;
}

static inline void bswrite(bs_t*bs,unsigned val){
	uint32_t validMask = sbcBitMask[bs->bitsUsed];
	if(bs->bitsInCache + bs->bitsUsed < 32){
		bs->cacheWord = (bs->cacheWord << bs->bitsUsed)
		| (val & validMask);
		bs->bitsInCache += bs->bitsUsed;
	} else {
		int tmp = 32 - bs->bitsInCache;
		int remaintmp = bs->bitsUsed - tmp;
		val = val & validMask;
		uint32_t cacheWord = tmp==32?0:(bs->cacheWord<<tmp);
		cacheWord |= (val>>remaintmp);
		bs->buffer[bs->index++] = (uint8_t)(cacheWord>>24);
		bs->buffer[bs->index++] = (uint8_t)(cacheWord>>16);
		bs->buffer[bs->index++] = (uint8_t)(cacheWord>>8);
		bs->buffer[bs->index++] = (uint8_t)(cacheWord>>0);
		bs->cacheWord = val;
		bs->bitsInCache = remaintmp;
	}
}

static inline void bsflash(bs_t*bs){
	int byteInCache = (bs->bitsInCache + 7)/8;
	int shf = (byteInCache - 1) * 8;
	for (int i = 0; i < byteInCache; i++) {
		bs->buffer[bs->index++] = (uint8_t)(bs->cacheWord >> shf);
		shf-=8;
	}
	bs->cacheWord = 0;
	bs->bitsInCache = 0;
}

static inline void bswritei(bs_t*bs,unsigned val, unsigned i_bits){
	uint32_t validMask = sbcBitMask[i_bits];
	if(bs->bitsInCache + i_bits < 32){
		bs->cacheWord = (bs->cacheWord << i_bits)
		| (val & validMask);
		bs->bitsInCache+=i_bits;
	} else {
		int tmp = 32 - bs->bitsInCache;
		int remaintmp = i_bits - tmp;
		val = val & validMask;
		uint32_t cacheWord = tmp==32?0:(bs->cacheWord<<tmp);
		cacheWord |= (val>>remaintmp);
		bs->buffer[bs->index++] = (uint8_t)(cacheWord>>24);
		bs->buffer[bs->index++] = (uint8_t)(cacheWord>>16);
		bs->buffer[bs->index++] = (uint8_t)(cacheWord>>8);
		bs->buffer[bs->index++] = (uint8_t)(cacheWord>>0);
		bs->cacheWord = val;
		bs->bitsInCache = remaintmp;
	}
}
#endif

#endif /*  */
