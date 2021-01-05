#define persp 100

u_char lvlnum;

//Game cells
u_char level[63][63];

//World
SVECTOR ang={0,0,0};	//angle (512=45d)
VECTOR pos = {0, 0, 0};	//translation vector (position)
SVECTOR x[64][64][2];	//vertices

//Flappy attributes
VECTOR flappyPos = {0, 0, persp};	//translation vector (position)
SVECTOR flappyAng={0,0,0};	//angle (512=45d)
u_char fflags=0;	//1=direction, 2=floor, 4=force floor, 8=death, 16=override cam, 32=pause
short vacc;

u_char ncoins;
short *dcoin;
u_char remCoins,lastRemCoins;
u_char *lastCoinCollected, *coinCollected;

signed char spawnpoint[2]={3,8};
char *alertmsg=0;
u_char levelExitCode;		//0=level in progress, 1=failed (pause menu exit, for example), 2=end success
u_long frame;

//Graphic elements
u_short tpages[3];
DR_TPAGE tp[3];	//DR TPAGE for sprites

//Flappy
POLY_FT4 fp[4]={	//We still need to set tpage
	{r0:128, g0:128, b0:128, u0:0, v0:238, u1:0, v1:255, u2:17, v2:238, u3:17, v3:255},
	{r0:128, g0:128, b0:128, u0:18, v0:238, u1:18, v1:255, u2:35, v2:238, u3:35, v3:255},
	{r0:128, g0:128, b0:128, u0:36, v0:238, u1:36, v1:255, u2:53, v2:238, u3:53, v3:255},
	{r0:128, g0:128, b0:128, u0:18, v0:238, u1:18, v1:255, u2:35, v2:238, u3:35, v3:255}};
//World
POLY_FT4 p[3]={		//We still need to set tpage
	{u0:0, v0:0, u1:0, v1:31, u2:31, v2:0, u3:31, v3:31},
	{u0:32, v0:0, u1:32, v1:31, u2:63, v2:0, u3:63, v3:31},
	{u0:64, v0:0, u1:64, v1:31, u2:95, v2:0, u3:95, v3:31}};
SPRT s[3]={
	{r0:128, g0:128, b0:128, w:32, h:32, u0:0, v0:0},
	{r0:128, g0:128, b0:128, w:32, h:32, u0:32, v0:0},
	{r0:128, g0:128, b0:128, w:32, h:32, u0:64, v0:0}};
//Coins
SPRT_16 scoin[8]={
	{u0:0, v0:198}, {u0:16, v0:198}, {u0:32, v0:198}, {u0:48, v0:198},
	{u0:64, v0:198}, {u0:80, v0:198}, {u0:96, v0:198}, {u0:112, v0:198}};
SPRT tik={w:24, h:24, u0:0, v0:214, x0:280, y0:6};
//FullScreenBlack, alert, text
TILE fullScreenBlack={r0:0,g0:0,b0:0,x0:0,y0:0,w:320,h:240};
SPRT alertSprt={w:256, h:142, u0:0, v0:32, x0:32, y0:48};
SPRT_8 charSprt;
SPRT numSprt={w:12, h:18, v0:198};

long dmy,flg;	//Work for RotTransPers

struct DB {
	DRAWENV	draw;	//Drawing environment
	DISPENV	disp;	//Display environment
};

struct DB db[2];
struct DB* cdb;

char stringholder[128];

u_char audioChannel=0;
char* songnames[]={"FADVX level0 song","Dj Gordy & SK - Secrets","Orange feat Sanna - Eternity","ACE OF BEAT - Responsible","On and on","New System - This Is The Night","Atmospheric - Oxigene 1997","Sylver - Smile Has Left ur Eyes"};

/////////////////FlappyEngine//////////////
POLY_F4	f4; POLY_F3 f3;
struct FEObject {
	u_char id;
	u_char dataSize;
	short dx,dy;
	u_char *data;
};

u_char nFEObjects;
struct FEObject *FEObjects;