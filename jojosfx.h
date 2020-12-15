#ifndef JOJOSFX_H
#define JOJOSFX_H

#define SFXOFFSET 0x760000
#define RAWSAMPLEINDEXOFFSET 0x75C000
#define RAWSAMPLEHEADEROFFSET 0x758000

#define ERROROUT errorout(__FILE__, __LINE__)

typedef union unlong Unlong;
typedef union unlong *Unlongptr;

struct sfxdata {
	U8 byte1; 
	U16 word1;
	U16 word2;
	U16 word3;
	U16 word4;
	U16 word5; 
	U16 word6; 
	U16 word7; 
	U16 word8; 
	U16 word9; 
	U16 word10; 
	U16 word11; 
	U16 word12; 
	U16 offset; 
	U16 word14; 
	U16 word15; 
	U16 word16; 
};

typedef struct sfxdata SfxData;
typedef struct sfxdata *SfxDataptr;

struct rawsampleheader {
	U8 head2;
	U8 head;
	U16 head3;
	U16 rawsampleindex;
	U16 word4;
	U16 word5;
	U16 word6;
};

typedef struct rawsampleheader RawSampleHeader;
typedef struct rawsampleheader *RawSampleHeaderptr;

struct rawsample {
	Unlong start;	
	Unlong dword2;	
	Unlong end;	
	Unlong rate;	
};

typedef struct rawsample RawSample;
typedef struct rawsample *RawSampleptr;

void
errorout(char *file, int line)
{
	printf("%s:%d\nMight be missing a simm file: %s\n", file, line, strerror(errno));
	exit(errno);

}

#endif
