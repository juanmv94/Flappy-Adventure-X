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
	FntPrint(msg);
	FntFlush(-1);
	DrawSync(0);
	nextFrameFlipBuff();
}

int main() {
	int i,j;
	u_short tpages[1]={GetTPage(2,2,768,0)};
	
	ResetGraph(0);
	SetVideoMode(1);	//PAL
	SetDispMask(1);
	
	FntLoad(320, 0);	
	FntOpen(8, 8, 320, 64, 0, 512);
	
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0,   240, 320, 240);
	db[0].draw.isbg=1; db[1].draw.isbg=1; db[0].draw.r0=0; db[1].draw.r0=0; db[0].draw.g0=0; db[1].draw.g0=0; db[0].draw.b0=60; db[1].draw.b0=60;
	db[0].draw.dtd=0; db[1].draw.dtd=0;
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0, 0, 320, 240);
	db[0].disp.screen.y=32; db[1].disp.screen.y=32;
	nextFrameFlipBuff();
	for (i=0;i<64;i++) {
		printStartMsg("Flappy Adventure X");
	}
	PadInitDirect((u_char *)&gamePad[0], (u_char *)&gamePad[1]);
	printStartMsg("Init audio");
	initFACDAudio();
	printStartMsg("Load Textures");
	loadTimCD("\\T1.TIM;1");
	printStartMsg("Setting polys");
	for (i=0;i<3;i++) {p[i].tpage=tpages[0]; setPolyFT4(&p[i]); setSprt(&s[i]);}	//World polygons
	for (i=0;i<8;i++) {setSprt16(&scoin[i]); setShadeTex(&scoin[i],1);}				//Coin polygons
	setSprt(&tik); setShadeTex(&tik,1);												//tik
	for (i=0;i<4;i++) {fp[i].tpage=tpages[0]; setPolyFT4(&fp[i]);}					//Flappy polygons
	SetDrawTPage(&tp,0,0,tpages[0]);												//Sprites TPAGE
	SetTile(&fullScreenBlack); SetSemiTrans(&fullScreenBlack,1);					//FullScreenBlack
	setSprt(&alertSprt); setShadeTex(&alertSprt,1);									//AlertSprt
	setSprt8(&charSprt); setSprt(&numSprt);	setShadeTex(&numSprt,1);				//AlertSprt, text, nums
	SetPolyF4(&f4); SetPolyF3(&f3);													//FlappyEngine
	printStartMsg("Init Geom");
	InitGeom();										//initialise geometry subsystem
	SetGeomOffset(160, 120);						//geom screen center
	SetGeomScreen(persp*2);							//distance from viewpoint to screen
	printStartMsg("Init game world vertex");
	for (i=-32;i<32;i++) for (j=-32;j<32;j++)  {	//Init game world vertex
		SVECTOR *c=x[i+32][j+32];
		c[0].vx=c[1].vx=j<<4; c[0].vy=c[1].vy=i<<4;
		c[0].vz=0; c[1].vz=persp+16;
	}
	printStartMsg("Reading Memory Card 0");
	//PadStopCom();	//Was not started yet
	loadMemCard();
	printStartMsg("Starting GamePad communication");
	PadStartCom();
	printStartMsg("Let's go!");
	introScreen(1); introScreen(2); introScreen(3);
	PlayStr(320,240,"\\INTRO.STR;1",2425);
	while (1) {
		startLevel(0);
	}
	return 0;
}
