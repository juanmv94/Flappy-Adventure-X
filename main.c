#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include "globals.c"
#include "audiocd.c"
#include "strplay.c"
#include "utils.c"
#include "game.c"

unsigned long __ramsize =   0x002000000; // force 2 megabytes of RAM
unsigned long __stacksize = 0x00004000; // force 16 kilobytes of stack

int main() {
	int i,j;
	u_short tpages[1]={GetTPage(2,2,768,0)};
	
	initFACDAudio();
	PadInit(0);	//Initialise the 1st controller
	ResetGraph(0);
	SetVideoMode(1);	//PAL
	SetDispMask(1);
	
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0,   240, 320, 240);
	//db[0].draw.isbg=1; db[1].draw.isbg=1; db[0].draw.r0=0; db[1].draw.r0=0; db[0].draw.g0=0; db[1].draw.g0=0; db[0].draw.b0=60; db[1].draw.b0=60;
	db[0].draw.dtd=0; db[1].draw.dtd=0;
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0, 0, 320, 240);
	
	loadTimCD("\\T1.TIM;1");
	for (i=0;i<3;i++) {p[i].tpage=tpages[0]; setPolyFT4(&p[i]); setSprt(&s[i]);}	//World polygons
	for (i=0;i<8;i++) setSprt16(&scoin[i]);											//Coin polygons
	setSprt(&tik);																	//tik
	for (i=0;i<4;i++) {fp[i].tpage=tpages[0]; setPolyFT4(&fp[i]);}					//Flappy polygons
	SetDrawTPage(&tp,0,0,tpages[0]);												//Sprites TPAGE
	SetTile(&fullScreenBlack); SetSemiTrans(&fullScreenBlack,1);					//FullScreenBlack
	setSprt(&alertSprt); setSprt8(&charSprt); setSprt(&numSprt);					//AlertSprt, text, nums
	SetPolyF4(&f4); SetPolyF3(&f3);													//FlappyEngine
	
	InitGeom();										//initialise geometry subsystem
	SetGeomOffset(160, 120);						//geom screen center
	SetGeomScreen(persp*2);							//distance from viewpoint to screen
	for (i=-32;i<32;i++) for (j=-32;j<32;j++)  {	//Init game world vertex
		SVECTOR *c=x[i+32][j+32];
		c[0].vx=c[1].vx=j<<4; c[0].vy=c[1].vy=i<<4;
		c[0].vz=0; c[1].vz=persp+16;
	}

	introScreen(1); introScreen(2); introScreen(3);
	PlayStr(320,240,"\\INTRO.STR;1",2425);
			
	FntLoad(320, 0);	
	FntOpen(8, 8, 320, 64, 0, 512);
	
	while (1) {
		startLevel(0);
	}
	return 0;
}
