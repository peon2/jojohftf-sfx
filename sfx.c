#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "cps3dec.h"
#include "jojosfx.h"


void usage();

int main(int argc, char **argv) {

	if (argc != 2) {
		usage();
		exit(1);
	}

	char **overflow = malloc(1);

	int soundno = strtol(argv[1], overflow, 16);

	if (argv[1][0] == 'q' || !strcmp(argv[1], "help") || strcmp(*overflow, "")) {
		usage();
		exit(0);
	}

	free(overflow);

	FILE *f10, *f11, *f12, *f13;	

	if (!(f10 = fopen("jojoba-simm1.0","rb"))) ERROROUT;
	if (!(f11 = fopen("jojoba-simm1.1","rb"))) ERROROUT;
	if (!(f12 = fopen("jojoba-simm1.2","rb"))) ERROROUT;
	if (!(f13 = fopen("jojoba-simm1.3","rb"))) ERROROUT;

        fseek(f10, 0, SEEK_END);
	int size10 = ftell(f10);
	rewind(f10);

	Unlongptr p10 = malloc(size10*4);

	U32 val;

	for (int i = 0; i < size10; i++) {
		p10[i].bytes[0] = getc(f13);
		p10[i].bytes[1] = getc(f12);
		p10[i].bytes[2] = getc(f11);
		p10[i].bytes[3] = getc(f10);	
		val = cps3mask(0x6000000+i*4, K1, K2);
		p10[i].ulong ^= val;
	}

	fclose(f10);
	fclose(f11);
	fclose(f12);
	fclose(f13);

	f10 = NULL;
	f11 = NULL;
	f12 = NULL;
	f13 = NULL;
	free(f10);
	free(f11);
	free(f12);
	free(f13);

	U8 *p10b = (U8 *)flipendian((void *)(p10), size10*4, 4);
	U16 *p10w = (U16 *)flipendian((void *)(p10b), size10*4, 2);

	int sfxdataheaderpos = SFXOFFSET + p10[(SFXOFFSET + 4 + soundno*4)/4].ulong;
	SfxDataptr sdata = (SfxDataptr)&p10b[sfxdataheaderpos];
	int sfxdatapos = sfxdataheaderpos + (*sdata).offset;

	int i = 0;

	printf("Sfx data starts at 6%x\n\n", sfxdatapos);

	for(; p10b[sfxdatapos+i]!=0xC1; i++); // Why would hftf make me do this

	int c1word = (p10b[sfxdatapos+i+1] << 8) | p10b[sfxdatapos+i+2];

	for(; p10b[sfxdatapos+i]!=0xC2; i++);

	int banknumber = p10b[sfxdatapos+i+1];

	for(; p10b[sfxdatapos+i]!=0xC4; i++); 

	int rawsamplepointeroffset = p10b[sfxdatapos+i+1];

	for(; p10b[sfxdatapos+i]!=0xBF; i++);

	int x1 = p10b[sfxdatapos+i+1];
	int x2 = p10b[sfxdatapos+i+2];

	int rawsamplepointer = p10[(RAWSAMPLEHEADEROFFSET + banknumber*4)/4].ulong - FILE10START;	

	U16 bankoffset = p10w[(rawsamplepointer+rawsamplepointeroffset*2)/2];	
	int rawsampleheaderdetails = rawsamplepointer+bankoffset;
	RawSampleHeaderptr rsh = (RawSampleHeaderptr)&p10w[rawsampleheaderdetails/2];
	for(; (*rsh).head!=x1; rsh++);
	int rawsampleindex = (*rsh).rawsampleindex;
	int rawsampleoffset = RAWSAMPLEINDEXOFFSET+rawsampleindex*0x10;

	RawSampleptr rawsample = (RawSampleptr) &p10[rawsampleoffset/4];	

	int samplerate = (x2<<8)/c1word;
	if ((x2<<8)%c1word!=0) samplerate++; 

	puts("Sfx info:");
	printf("Sample Rate: %x\n", samplerate);
	printf("Start: %x\n", (*rawsample).start.ulong);
	printf("Unknown: %x\n",(*rawsample).dword2.ulong);
	printf("End %x\n",(*rawsample).end.ulong);
	printf("Sample rate %x\n", (*rawsample).rate.ulong);

	free(p10);
	free(p10b);
	free(p10w);

	#ifdef DEBUG 
	puts(""); 
	printf("sfxdataheaderpos: %x\n", sfxdataheaderpos); 
	printf("sfxdatapos: %x\n", sfxdatapos); 
	printf("banknumber: %x\n", banknumber); 
	printf("rawsamplepointeroffset: %x\n", rawsamplepointeroffset); 
	printf("BF X1: %x\n", x1);
	printf("BF X2: %x\n", x2);
	printf("c1word: %x\n", c1word);
	printf("rawsamplepointer: %x\n", rawsamplepointer); 
	printf("bankoffset: %x\n", bankoffset); 
	printf("rawsampleheaderdetails: %x\n", rawsampleheaderdetails);
	printf("rawsampleindex: %x\n", rawsampleindex); 
	printf("rawsampleoffset: %x\n", rawsampleoffset);
	#endif

	return 0;
}

void
usage() {
	puts("Usage: sfx [Hexadecimal Sound Number]");
}
