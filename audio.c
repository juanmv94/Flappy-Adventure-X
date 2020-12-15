#include <libsnd.h>
#include <libspu.h>
#include "sounds.h"

#define MALLOC_MAX 7
#define PLAYSFX(X) SpuSetKey(SpuOn,X)
#define STOPSFX(X) SpuSetKey(SpuOff,X)

#define SFX_ABS1 SPU_0CH
#define SFX_DIE SPU_1CH
#define SFX_HIT SPU_2CH
#define SFX_COIN SPU_3CH
#define SFX_WING SPU_4CH
#define SFX_CHCK SPU_5CH
#define SFX_SWSH SPU_6CH

long spuadd[MALLOC_MAX];
char spu_malloc_rec [SPU_MALLOC_RECSIZ * (MALLOC_MAX + 1)];

void InitMusicSystem() {
	SpuCommonAttr l_c_attr; // structure for changing common voice attributes
	SpuInit();
	SpuInitMalloc (MALLOC_MAX, spu_malloc_rec);
	l_c_attr.mask = (SPU_COMMON_MVOLL | SPU_COMMON_MVOLR);
	l_c_attr.mvol.left  = 0x3fff; // set master left volume
	l_c_attr.mvol.right = 0x3fff; // set master right volume
    SpuSetCommonAttr (&l_c_attr);
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

void initFAAudio() {
	InitMusicSystem();
	
	spuadd[0]=SendVAGToRAM(sfx_abs1,6800);
	spuadd[1]=SendVAGToRAM(sfx_die,18992);
	spuadd[2]=SendVAGToRAM(sfx_hit,13792);
	spuadd[3]=SendVAGToRAM(sfx_coin,25280);
	spuadd[4]=SendVAGToRAM(sfx_wing,8144);
	spuadd[5]=SendVAGToRAM(sfx_chck,15200);
	spuadd[6]=SendVAGToRAM(sfx_swsh,17728);
	
	SetVoiceAttr(SFX_ABS1,spuadd[0]);
	SetVoiceAttr(SFX_DIE,spuadd[1]);
	SetVoiceAttr(SFX_HIT,spuadd[2]);
	SetVoiceAttr(SFX_COIN,spuadd[3]);
	SetVoiceAttr(SFX_WING,spuadd[4]);
	SetVoiceAttr(SFX_CHCK,spuadd[5]);
	SetVoiceAttr(SFX_SWSH,spuadd[6]);
}