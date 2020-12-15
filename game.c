#include "bkg.h"
#include "feobjs.c"
#include "customlv.c"

void updateGameWorldPos() {
	pos.vx=(flappyPos.vx>400)?-400:(flappyPos.vx<-416)?416:-flappyPos.vx;
	if (PadRead(1) & PADLdown) pos.vy--;
	if (PadRead(1) & PADLup) pos.vy++;
	if ((pos.vy+flappyPos.vy)>32) {
		pos.vy=-flappyPos.vy+32;
		if (pos.vy<-420) pos.vy=-420;
	}else if ((pos.vy+flappyPos.vy)<-32) {
		pos.vy=-flappyPos.vy-32;
		if (pos.vy>436) pos.vy=436;
	}
}

int b(int x,int y) {
	if (x<0 || x>=63 || y<0 || y>=63) return 1;
	return level[x][y];
}

void printGame() {
	static int i,j;
	static MATRIX m;
	static POLY_FT4 *fpp;	//Current Flappy poly
	static SPRT_16 *scoinp;	//Current Coin poly
	static SVECTOR coinVertex={0, 0, persp};
	static SVECTOR flappyVertex[2][4] = {{{-8, -8, 0},{-8,8,0},{8,-8,0},{8,8,0}}};
	RECT backgroundRect={x:320+((416-pos.vx)*384/816),y:272,w:320,h:240};
	int i1=27-(pos.vy>>4), i2=36-(pos.vy>>4), j1=26-(pos.vx>>4),j2=37-(pos.vx>>4);
	if (i1<1) i1=1; if (j1<1) j1=1; if (i2>62) i2=62; if (j2>62) j2=62;
	MoveImage(&backgroundRect,0,cdb->draw.clip.y);	//Print background
	
	//Flappy Geometry calculations
	RotMatrix(&flappyAng, &m); //Get a rotation matrix from the vector
	TransMatrix(&m, &flappyPos);	//Sets the translation
	SetRotMatrix(&m);
	SetTransMatrix(&m);
	for (i=0;i<4;i++) RotTransSV(&flappyVertex[0][i],&flappyVertex[1][i],&flg);
	
	//World Geometry calculations
	RotMatrix(&ang, &m); //Get a rotation matrix from the vector
	TransMatrix(&m, &pos);	//Sets the translation
	TransMatrix(&m, &pos);	//Sets the translation
	SetRotMatrix(&m);
	SetTransMatrix(&m);
	for (i=i1-1;i<=i2;i++) for (j=j1-1;j<=j2;j++) if (level[i][j]) {
		POLY_FT4 *pp=&p[level[i][j]-1];
		//up face
		if (!b(i-1,j)) {
			RotTransPers4(&x[i][j][0],&x[i][j+1][0],&x[i][j][1],&x[i][j+1][1],(long*)&pp->x0,(long*)&pp->x1,(long*)&pp->x2,(long*)&pp->x3,&dmy,&flg);
			if (pp->y0>120) {
				pp->r0=pp->g0=pp->b0=100;
				DrawPrim(pp);	//blocking
			}
		}
		//down face
		if (!b(i+1,j)) {
			RotTransPers4(&x[i+1][j][0],&x[i+1][j+1][0],&x[i+1][j][1],&x[i+1][j+1][1],(long*)&pp->x0,(long*)&pp->x1,(long*)&pp->x2,(long*)&pp->x3,&dmy,&flg);
			if (pp->y0<120) {
				pp->r0=pp->g0=pp->b0=85;
				DrawPrim(pp);	//blocking
			}
		}
		//left face
		if (!b(i,j-1)) {
			RotTransPers4(&x[i][j][0],&x[i+1][j][0],&x[i][j][1],&x[i+1][j][1],(long*)&pp->x0,(long*)&pp->x1,(long*)&pp->x2,(long*)&pp->x3,&dmy,&flg);
			if (pp->x0>160) {
				pp->r0=pp->g0=pp->b0=70;
				DrawPrim(pp);	//blocking
			}
		}
		//right face
		if (!b(i,j+1)) {
			RotTransPers4(&x[i][j+1][0],&x[i+1][j+1][0],&x[i][j+1][1],&x[i+1][j+1][1],(long*)&pp->x0,(long*)&pp->x1,(long*)&pp->x2,(long*)&pp->x3,&dmy,&flg);
			if (pp->x0<160) {
				pp->r0=pp->g0=pp->b0=55;
				DrawPrim(pp);	//blocking
			}
		}
	}
	//FlappyEngine objects
	for (i=0;i<nFEObjects;i++) if (abs(FEObjects[i].dx+pos.vx)<128 && abs(FEObjects[i].dy+pos.vy)<128) {
		FEFuncArray[FEObjects[i].id](&FEObjects[i]);
	}
	
	//Flappy
	fpp=&fp[(frame>>3)&3];
	RotTransPers4(&flappyVertex[1][0],&flappyVertex[1][1],&flappyVertex[1][2],&flappyVertex[1][3],(long*)&fpp->x0,(long*)&fpp->x1,(long*)&fpp->x2,(long*)&fpp->x3,&dmy,&flg);
	DrawPrim(fpp);	//blocking
	
	DrawPrim(&tp);	//blocking
	
	//Coins
	scoinp=&scoin[(frame>>2)&7];
	for (i=0;i<ncoins;i++) if (!coinCollected[i] && abs(dcoin[i][0]+pos.vx)<128 && abs(dcoin[i][1]+pos.vy)<128) {
		if (abs(dcoin[i][0]-flappyPos.vx+4)<10 && abs(dcoin[i][1]-flappyPos.vy+4)<12) {	//coin touched?
			PLAYSFX(SFX_COIN);
			coinCollected[i]=1; remCoins--;
		} else {	//if not, just paint it
			coinVertex.vx=dcoin[i][0]; coinVertex.vy=dcoin[i][1];
			RotTransPers(&coinVertex,(long*)&scoinp->x0,&dmy,&flg);
			DrawPrim(scoinp);
		}
	}
	for (i=i1;i<i2;i++) for (j=j1;j<j2;j++) if (level[i][j]) {	//front face
		SPRT *sp=&s[level[i][j]-1];
		//RotTransPers4(&x[i][j][0],&x[i+1][j][0],&x[i][j+1][0],&x[i+1][j+1][0],(long*)&p.x0,(long*)&p.x1,(long*)&p.x2,(long*)&p.x3,&dmy,&flg);
		RotTransPers(&x[i][j][0],(long*)&sp->x0,&dmy,&flg);
		DrawPrim(sp);	//blocking
	}
	
	//HUD
	scoin[1].x0=300; scoin[1].y0=9;
	DrawPrim(&scoin[1]);
	if (remCoins) printNum(287,8,remCoins);
	else DrawPrim(&tik);
}

void checkPauseAndAlert() {
	if (alertmsg) {
		PLAYSFX(SFX_SWSH);
		fullScreenBlack.r0=64; fullScreenBlack.g0=64; fullScreenBlack.b0=64;
		DrawPrim(&fullScreenBlack);
		DrawPrim(&alertSprt);
		print(96,90,alertmsg);
		nextFrameFlipBuff();
		while (!(PadRead(1) & PADRdown)) VSync(0);
		while (PadRead(1) & PADRdown) VSync(0);
		alertmsg=0;
	} else if (PadRead(1) & PADstart) {
		PLAYSFX(SFX_SWSH);
		fullScreenBlack.r0=96; fullScreenBlack.g0=96; fullScreenBlack.b0=48;
		DrawPrim(&fullScreenBlack);
		nextFrameFlipBuff();
		while (PadRead(1) & PADstart) VSync(0);
		while (!(PadRead(1) & PADstart)) VSync(0);
		while (PadRead(1) & PADstart) VSync(0);
		PLAYSFX(SFX_SWSH);
	}
}

void fdie() {
	u_char i,j;
	flappyAng.vz=1024;
	PLAYSFX(SFX_HIT);
	PLAYSFX(SFX_DIE);
	for (i=0;i<8;i++) {
		nextFrameFlipBuff();
		printGame();
		if ((flappyPos.vy&15)<=8 || !b((flappyPos.vy>>4)+33,(flappyPos.vx>>4)+32)) flappyPos.vy++;	//Not ground
		else STOPSFX(SFX_DIE);
		updateGameWorldPos();
	}
	for (i=0;i<32;i++) {	//the same with fullScreenBlack
		nextFrameFlipBuff();
		printGame();
		fullScreenBlack.r0=i<<3; fullScreenBlack.g0=i<<3; fullScreenBlack.b0=i<<3;
		DrawPrim(&fullScreenBlack);
		if ((flappyPos.vy&15)<=8 || !b((flappyPos.vy>>4)+33,(flappyPos.vx>>4)+32)) flappyPos.vy++;	//Not ground
		else STOPSFX(SFX_DIE);
	}
	//Restart level
	flappyPos.vx=spawnpoint[0]<<4; flappyPos.vy=spawnpoint[1]<<4|9;
	vacc=0; fflags=0; flappyAng.vy=0; pos.vy=0;
	
	memcpy(coinCollected,lastCoinCollected,ncoins); remCoins=lastRemCoins;
}


void updatePlayer() {
	if (PadRead(1) & PADRdown && vacc>=0)  {	//jump button
		vacc=-19;
		PLAYSFX(SFX_WING);
	}
	flappyPos.vy+=vacc>>3;
	if ((flappyPos.vy&15)>8 && b((flappyPos.vy>>4)+33,(flappyPos.vx>>4)+32)) {	//ground
		switch (b((flappyPos.vy>>4)+33,(flappyPos.vx>>4)+32)) {
			case 1:	//normal ground
			if (PadRead(1) & PADLleft) {flappyPos.vx--; fflags|=1;}
			if (PadRead(1) & PADLright) {flappyPos.vx++; fflags&=0xFE;}
			flappyAng.vy=(fflags&1)?2048:0;
			break;
			case 2:	//ice ground
			if (fflags&1) flappyPos.vx--; else flappyPos.vx++;
			break;
			default: //obstacle
			fflags|=8;
			return;
		}
		if (!fflags|2) {	//hit floor first time, to stay
			fflags|=2;
			vacc=0;
			flappyAng.vz=0;
			flappyPos.vy=(flappyPos.vy&0xFFFFFFF0)|9;
		}
		if ((flappyPos.vx&15)>8 && b((flappyPos.vy>>4)+32,(flappyPos.vx>>4)+33) || (flappyPos.vx&15)<8 && b((flappyPos.vy>>4)+32,(flappyPos.vx>>4)+31)) {	//Wallhit
			flappyPos.vx=(flappyPos.vx&0xFFFFFFF0)|8;
		}
	} else if (fflags&4){	//ground (forced)
		fflags&=0xFB;
		vacc=0;
		flappyAng.vz=0;
		if (PadRead(1) & PADLleft) {flappyPos.vx--; fflags|=1;}
		if (PadRead(1) & PADLright) {flappyPos.vx++; fflags&=0xFE;}
		flappyAng.vy=(fflags&1)?2048:0;
	} else {	//fly
		if ((flappyPos.vy&15)<7 && b((flappyPos.vy>>4)+31,(flappyPos.vx>>4)+32)) {	//ceilhit
			fflags|=8;
			return;
		}
		if ((!fflags&1) && (flappyPos.vx&15)>8 && b((flappyPos.vy>>4)+32,(flappyPos.vx>>4)+33)	//Wallhit
		|| (fflags&1) && (flappyPos.vx&15)<8 && b((flappyPos.vy>>4)+32,(flappyPos.vx>>4)+31)
		|| (!fflags&1) && (flappyPos.vx&15)>8 && (flappyPos.vy&15)<8 && b((flappyPos.vy>>4)+31,(flappyPos.vx>>4)+33)
		|| (!fflags&1) && (flappyPos.vx&15)>8 && (flappyPos.vy&15)>9 && b((flappyPos.vy>>4)+33,(flappyPos.vx>>4)+33)
		|| (fflags&1) && (flappyPos.vx&15)<7 && (flappyPos.vy&15)<8 && b((flappyPos.vy>>4)+31,(flappyPos.vx>>4)+31)
		|| (fflags&1) && (flappyPos.vx&15)<7 && (flappyPos.vy&15)>9 && b((flappyPos.vy>>4)+33,(flappyPos.vx>>4)+31)) {
			fflags|=8;
			return;
		}
		fflags&=0xFD;
		if (vacc<23) vacc++;
		flappyAng.vz=vacc<<4;
		if (fflags&1) flappyPos.vx--; else flappyPos.vx++;
	}
}

u_char startLevel(u_char lvlnum) {
	loadTim((long)bkg);
	remCoins=lastRemCoins=ncoins;
	coinCollected=(u_char*)calloc(ncoins,1); lastCoinCollected=(u_char*)calloc(ncoins,1);
	flappyPos.vx=spawnpoint[0]<<4; flappyPos.vy=spawnpoint[1]<<4|9;
	vacc=0; fflags=0; flappyAng.vy=0; pos.vy=0;
	levelExitCode=0; frame=0;
	while (!levelExitCode) {
		nextFrameFlipBuff();
		updatePlayer();
		updateGameWorldPos();
		printGame();
		levelCustomCode[lvlnum].onFrame();
		if (fflags&8) {fdie(); continue;}
		checkPauseAndAlert();
		FntPrint("Fla pos: %d, %d [%d, %d] + %d, %d\nWld pos: %d, %d\nfflags: %02x, vline: %d",flappyPos.vx,flappyPos.vy,flappyPos.vx>>4,flappyPos.vy>>4,flappyPos.vx&15,flappyPos.vy&15,pos.vx,pos.vy,fflags,VSync(1));
		FntFlush(-1);
		DrawSync(0);
		frame++;
	}
	free(coinCollected); free(lastCoinCollected);
	return levelExitCode;
}
