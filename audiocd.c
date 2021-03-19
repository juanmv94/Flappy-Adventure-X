#include <libcd.h>
#include <libsnd.h>
#include <libspu.h>

#define MALLOC_MAX 9
#define PLAYSFX(X) SpuSetKey(SpuOn,X)
#define STOPSFX(X) SpuSetKey(SpuOff,X)

#define SFX_ABS1 SPU_0CH
#define SFX_DIE SPU_1CH
#define SFX_HIT SPU_2CH
#define SFX_COIN1 SPU_3CH
#define SFX_COIN2 SPU_4CH
#define SFX_COIN3 SPU_5CH
#define SFX_COIN4 SPU_6CH
#define SFX_WING SPU_7CH
#define SFX_CHCK SPU_8CH
#define SFX_SWSH SPU_9CH
#define SFX_TTAC SPU_10CH
#define SFX_LSER SPU_11CH

u_char currcoinsnd=0;
u_long coinssnd[4]={SFX_COIN1,SFX_COIN2,SFX_COIN3,SFX_COIN4};
short cdvol=0x3fff;

#define PLAYSFXCOIN() SpuSetKey(SpuOn,coinssnd[currcoinsnd++&3])

//Generic CD

u_char* readFromCD(char* file) {
	CdlFILE fileInfo;
	int sizeSectors;
	u_char* mem;
	if(CdSearchFile(&fileInfo, file) <= 0) {	//CdSearchFile
	  printf("FILE SEARCH FAILED: %s\n",file);
	  return 0;
	}

	sizeSectors = (fileInfo.size + 2047) / 2048;
	mem = (u_char*)malloc(sizeSectors * 2048);
	if(mem == 0) {
	  printf("ALLOCATION FAILED %s\n",file);
	  return 0;
	}
	CdControl(CdlSetloc, (u_char *)&fileInfo.pos, 0);
	CdRead(sizeSectors, (u_long*)mem, CdlModeSpeed);
	while (CdReadSync(0,0));
	return mem;
}

//Audio

long spuadd[MALLOC_MAX];
char spu_malloc_rec [SPU_MALLOC_RECSIZ * (MALLOC_MAX + 1)];

void InitMusicCDSystem() {
	SpuCommonAttr l_c_attr; // structure for changing common voice attributes
	CdInit();
	SpuInit();
	SpuInitMalloc (MALLOC_MAX, spu_malloc_rec);
	l_c_attr.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR);
	l_c_attr.mvol.left  = 0x3fff; // set master left volume
	l_c_attr.mvol.right = 0x3fff; // set master right volume
    SpuSetCommonAttr (&l_c_attr);
	SpuSetCommonCDMix(SPU_ON);
	SpuSetCommonCDVolume(cdvol, cdvol);
}


long SendVAGToRAM(u_char* vag, long dataSize) {
	long SPUAddr;
	SpuSetTransferMode (SpuTransByDMA); // set transfer mode to DMA
	SPUAddr = SpuMalloc(dataSize-48); // allocate SPU memory for sound 1
	SpuSetTransferStartAddr (SPUAddr); // set transfer starting address to malloced area
	SpuWrite (vag+48, dataSize-48); // perform actual transfer
	SpuIsTransferCompleted(SPU_TRANSFER_WAIT);
	return SPUAddr;
}

void SetVoiceAttr(long chan, long SPUadd) {
	SpuVoiceAttr g_s_attr;	// structure for changing individual voice attributes
	
	// mask which specific voice attributes are to be set
	g_s_attr.mask = ( SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_PITCH | SPU_VOICE_WDSA | SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_RMODE |
	  SPU_VOICE_ADSR_AR | SPU_VOICE_ADSR_DR | SPU_VOICE_ADSR_SR | SPU_VOICE_ADSR_RR | SPU_VOICE_ADSR_SL);

	g_s_attr.voice = chan;
	g_s_attr.volume.left  = 0x1fff;
	g_s_attr.volume.right = 0x1fff;
	g_s_attr.pitch        = 0x1000;
	g_s_attr.addr         = SPUadd;
	g_s_attr.a_mode       = SPU_VOICE_LINEARIncN;
	g_s_attr.s_mode       = SPU_VOICE_LINEARIncN;
	g_s_attr.r_mode       = SPU_VOICE_LINEARDecN;
	g_s_attr.ar           = 0x0;
	g_s_attr.dr           = 0x0;
	g_s_attr.sr           = 0x0;
	g_s_attr.rr           = 0x0;
	g_s_attr.sl           = 0xf;

	SpuSetVoiceAttr (&g_s_attr);
}

void initFACDAudio() {
	u_char *p;
	InitMusicCDSystem();
	
	p=readFromCD("\\SFX_ABS1.VAG;1"); spuadd[0]=SendVAGToRAM(p,6800); free(p); SetVoiceAttr(SFX_ABS1,spuadd[0]);
	p=readFromCD("\\SFX_DIE.VAG;1"); spuadd[1]=SendVAGToRAM(p,18992); free(p); SetVoiceAttr(SFX_DIE,spuadd[1]);
	p=readFromCD("\\SFX_HIT.VAG;1"); spuadd[2]=SendVAGToRAM(p,13792); free(p); SetVoiceAttr(SFX_HIT,spuadd[2]);
	p=readFromCD("\\SFX_COIN.VAG;1"); spuadd[3]=SendVAGToRAM(p,25280); free(p); SetVoiceAttr(SFX_COIN1,spuadd[3]); SetVoiceAttr(SFX_COIN2,spuadd[3]); SetVoiceAttr(SFX_COIN3,spuadd[3]); SetVoiceAttr(SFX_COIN4,spuadd[3]);
	p=readFromCD("\\SFX_WING.VAG;1"); spuadd[4]=SendVAGToRAM(p,8144); free(p); SetVoiceAttr(SFX_WING,spuadd[4]);
	p=readFromCD("\\SFX_CHCK.VAG;1"); spuadd[5]=SendVAGToRAM(p,15200); free(p); SetVoiceAttr(SFX_CHCK,spuadd[5]);
	p=readFromCD("\\SFX_SWSH.VAG;1"); spuadd[6]=SendVAGToRAM(p,17728); free(p); SetVoiceAttr(SFX_SWSH,spuadd[6]);
	p=readFromCD("\\SFX_TTAC.VAG;1"); spuadd[7]=SendVAGToRAM(p,11072); free(p); SetVoiceAttr(SFX_TTAC,spuadd[7]);
	p=readFromCD("\\SFX_LSER.VAG;1"); spuadd[8]=SendVAGToRAM(p,8208); free(p); SetVoiceAttr(SFX_LSER,spuadd[8]);
}

//XA Play
u_char audioChannel=0;

u_char buffer[2340]; //CD buffer, not really needed for XA play, but we can read CdlLOC data from sectors
u_long startposXA=0,currentposXA=0,endposXA=0;

void Sound_CD_XAChangeChannel(u_char channel) {
	CdlFILTER	filter;

	filter.file = 1;
	filter.chan = channel; audioChannel=channel;
	CdControl(CdlSetfilter, (u_char *)&filter, 0);
}

void callBackXA(int intr, u_char *result) {	//Callback for each sector of XA read
	if (intr == CdlDataReady) {
		CdGetSector((u_long*)buffer,585);
		currentposXA = CdPosToInt((CdlLOC *)buffer);
        if(currentposXA >= endposXA) {
			CdlLOC sp;
			//CdControlF(CdlPause,0);
			CdIntToPos(startposXA,&sp);
			CdControl(CdlReadS, (u_char *)&sp, 0);
			Sound_CD_XAChangeChannel(audioChannel^8);
		}
	} else printf("UnHandled Callback Occured\n");	
}

void Sound_CD_XAPlay(char *filename, u_char channel) {
	CdlFILE		fp;
	CdlFILTER	filter;
	u_char		param[4];

	if (CdSearchFile(&fp, filename) == 0) return;
	startposXA = CdPosToInt(&fp.pos);
	endposXA = startposXA+fp.size/2048;
	/* set ADPCM filter */
	filter.file = 1;
	filter.chan = channel; audioChannel=channel;
	CdControl(CdlSetfilter, (u_char *)&filter, 0);

	param[0] = CdlModeRT | CdlModeSF | CdlModeSize1 | CdlModeSpeed;
	CdControlB(CdlSetmode, param, 0);
	//VSync(3);	//speed change

	CdControl(CdlReadS, (u_char *)&fp.pos, 0);
	CdReadyCallback((CdlCB)callBackXA);	//Callback for each sector of XA read
}

void Sound_CD_XAStop() {
	CdControl(CdlPause, 0, 0);
	CdReadyCallback(0);
}