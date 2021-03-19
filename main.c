#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include "globals.c"
#include "audiocd.c"
#include "gamepad.c"
#include "memcard.c"
#include "strplay.c"
#include "utils.c"
#include "game.c"

unsigned long __ramsize =   0x002000000; // force 2 megabytes of RAM
unsigned long __stacksize = 0x00004000; // force 16 kilobytes of stack

void printStartMsg(char* msg) {
	ClearImage(&cdb->draw.clip,0,64,96);	//non-blocking (requires DrawSync)
	FntPrint(msg);
	FntFlush(-1);
	DrawSync(0);
	nextFrameFlipBuff();
}

void printTexStartMsg(char* msg) {
	static RECT backgroundRect={x:704,y:272,w:320,h:240};
	MoveImage(&backgroundRect,0,cdb->draw.clip.y);
	FntPrint(msg);
	FntFlush(-1);
	DrawSync(0);
	nextFrameFlipBuff();
}

int main() {
	int i,j;
	
	ResetGraph(0);
	SetVideoMode(1);	//PAL
	SetDispMask(1);
	
	FntLoad(320, 0);	
	FntOpen(72, 96, 248, 64, 0, 512);
	
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0,   240, 320, 240);
	db[0].draw.dtd=0; db[1].draw.dtd=0;
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0, 0, 320, 240);
	db[0].disp.screen.y=28; db[1].disp.screen.y=28;
	nextFrameFlipBuff();					 
	for (i=0;i<8;i++) {
		printStartMsg("Juanmv94 presents:\n\nFlappy Adventure X");
	}
	PadInitDirect((u_char *)&gamePad[0], (u_char *)&gamePad[1]);
	//printStartMsg("Init audio");
	initFACDAudio();
	loadTimCD("\\SPLASH.TIM;1");
	printTexStartMsg("Loading Textures");
	loadTimCD("\\T1.TIM;1");
	printTexStartMsg("Setting polys");
	tpages[0]=GetTPage(2,2,768,0); tpages[1]=GetTPage(2,0,768,0); tpages[2]=GetTPage(2,1,768,0); tpages[3]=GetTPage(2,0,512,0);
	for (i=0;i<4;i++) SetDrawTPage(&tp[i],0,0,tpages[i]);							//Sprites TPAGE
	for (i=0;i<3;i++) {p[i].tpage=tpages[0]; setPolyFT4(&p[i]); setSprt(&s[i]);}	//World polygons
	for (i=0;i<8;i++) {setSprt16(&scoin[i]); setShadeTex(&scoin[i],1);}				//Coin polygons
	setSprt(&tik); setShadeTex(&tik,1);												//tik
	for (i=0;i<4;i++) {fp[i].tpage=tpages[0]; setPolyFT4(&fp[i]);}					//Flappy polygons
	for (i=0;i<4;i++) {cdp[i].tpage=tpages[3]; setPolyFT4(&cdp[i]); setShadeTex(&cdp[i],1); SetSemiTrans(&cdp[i],1);}	//CD polygons
	SetTile(&fullScreenBlack); SetSemiTrans(&fullScreenBlack,1);					//FullScreenBlack
	setSprt(&alertSprt); setShadeTex(&alertSprt,1);									//AlertSprt
	setSprt8(&charSprt); setSprt(&numSprt);	setShadeTex(&numSprt,1);				//AlertSprt, text, nums
	SetPolyF4(&f4); SetPolyF3(&f3);													//FlappyEngine
	printTexStartMsg("Init Geometry");
	InitGeom();										//initialise geometry subsystem
	SetGeomOffset(160, 120);						//geom screen center
	SetGeomScreen(persp*2);							//distance from viewpoint to screen
	printTexStartMsg("Init game world vertex");
	for (i=-32;i<32;i++) for (j=-32;j<32;j++)  {	//Init game world vertex
		SVECTOR *c=x[i+32][j+32];
		c[0].vx=c[1].vx=j<<4; c[0].vy=c[1].vy=i<<4;
		c[0].vz=0; c[1].vz=persp+16;
	}
	printTexStartMsg("Reading Memory Card 0");
	MemCardInit(0);
	loadMemCard();
	VSync(0);
	printTexStartMsg("Starting GamePad communication");
	PadStartCom();
	printTexStartMsg("Let's go!");
	introScreen(1); introScreen(2); introScreen(3);
	PlayStr(320,240,"\\INTRO.STR;1",2425);
	ClearImage(&cdb->draw.clip,0,0,0); DrawSync(0); nextFrameFlipBuff();
	printTexStartMsg("");	//Debug charset not existing anymore
	while (1) {
		u_char lvl=startLevel(0);
		u_char exitc=startLevel(lvl);
		u_char collected=ncoins-remCoins;
		if (exitc==2 && mCardData.saveData[lvl]<collected) mCardData.saveData[lvl]=collected;
	}
	return 0;
}
