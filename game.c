#include "feobjs.c"
#include "customlv.c"

void loadLvlCD(char* filename) {
	u_char i;
	u_char* lvl=readFromCD(filename);
	u_char* lp=lvl;
	for (i=0;i<63;i++) {memcpy(level[i],lp,63);lp+=63;}
	spawnpoint[0]=*(lp++); spawnpoint[1]=*(lp++);
	ncoins=*(lp++);
	dcoin=(short*)malloc(ncoins<<2);
	memcpy(dcoin,lp,ncoins<<2); lp+=(ncoins<<2);
	nFEObjects=*(lp++);
	FEObjects=(struct FEObject*)malloc(nFEObjects*sizeof(struct FEObject));
	for (i=0;i<nFEObjects;i++) {
		memcpy(&FEObjects[i],lp,6);
		lp+=6;
		if (FEObjects[i].dataSize) FEObjects[i].data=(u_char*)malloc(FEObjects[i].dataSize);
		memcpy(FEObjects[i].data,lp,FEObjects[i].dataSize);
		lp+=FEObjects[i].dataSize;
	}
	
	remCoins=lastRemCoins=ncoins;
	coinCollected=(u_char*)calloc(ncoins,1); lastCoinCollected=(u_char*)calloc(ncoins,1);
	flappyPos.vx=spawnpoint[0]<<4; flappyPos.vy=spawnpoint[1]<<4|9;
	vacc=0; fflags=0; flappyAng.vy=0; pos.vy=999;
	levelExitCode=0; frame=0;
	
	free(lvl);
}

void unLoadLvl() {
	u_char i;
	free(dcoin);
	for (i=0;i<nFEObjects;i++) {
		if (FEObjects[i].dataSize) free(FEObjects[i].data);
	}
	free(FEObjects);
	free(coinCollected); free(lastCoinCollected);
}

void updateGameWorldPos() {
	pos.vx=(flappyPos.vx>400)?-400:(flappyPos.vx<-416)?416:-flappyPos.vx;
	if (gamePadDown(0)) pos.vy--;
	else if (gamePadUp(0)) pos.vy++;
	else if (fflags&2 && frame&1 && (pos.vy+flappyPos.vy)<8) pos.vy++;
	
	if ((pos.vy+flappyPos.vy)>32) {
		pos.vy=-flappyPos.vy+32;
	}else if ((pos.vy+flappyPos.vy)<-20) {
		pos.vy=-flappyPos.vy-20;
	}
	if (pos.vy<-420) pos.vy=-420;
	if (pos.vy>436) pos.vy=436;
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
	
	DrawPrim(&tp[0]);	//blocking
	
	//Coins
	scoinp=&scoin[(frame>>2)&7];
	for (i=0;i<ncoins;i++) if (!coinCollected[i] && abs(dcoin[i<<1]+pos.vx)<128 && abs(dcoin[(i<<1)+1]+pos.vy)<128) {
		if (abs(dcoin[i<<1]-flappyPos.vx+4)<10 && abs(dcoin[(i<<1)+1]-flappyPos.vy+4)<12) {	//coin touched?
			PLAYSFX(SFX_COIN);
			coinCollected[i]=1; remCoins--;
		} else {	//if not, just paint it
			coinVertex.vx=dcoin[i<<1]; coinVertex.vy=dcoin[(i<<1)+1];
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
	if (!lvlnum) return;
	scoin[1].x0=300; scoin[1].y0=9;
	DrawPrim(&scoin[1]);
	if (remCoins) printNum(287,8,remCoins);
	else DrawPrim(&tik);
}

void checkPauseAndAlert() {
	if (alertmsg) {
		SpuSetCommonCDVolume(0x0fff, 0x0fff);
		PLAYSFX(SFX_SWSH);
		fullScreenBlack.r0=64; fullScreenBlack.g0=64; fullScreenBlack.b0=64;
		DrawPrim(&fullScreenBlack);
		DrawPrim(&alertSprt);
		print(96,90,0,0,0,0,alertmsg);
		nextFrameFlipBuff();
		while (gamePad[0].ex && gamePad[0].start) VSync(0);
		while (!gamePad[0].ex || !gamePad[0].start) VSync(0);
		SpuSetCommonCDVolume(0x3fff, 0x3fff);
		alertmsg=0;
	} else if (lvlnum && !gamePad[0].start) {
		static u_char selection;
		SpuSetCommonCDVolume(0x0fff, 0x0fff);
		PLAYSFX(SFX_SWSH);
		fullScreenBlack.r0=0; fullScreenBlack.g0=0; fullScreenBlack.b0=64;
		fflags|=32; selection=0;
		while (!gamePad[0].start) VSync(0);
		while (gamePad[0].start && gamePad[0].ex) {
			static struct s_gamePad lastGamePad;
			DrawPrim(&tp[1]);
			DrawPrim(&fullScreenBlack);
			PRINTFMT(8,224,128,255,255,0,"<< %s >>",songnames[audioChannel]);
			print(128,104,192,255,128,0,"Continue\n\nExit level");
			if ((!gamePad[0].down && lastGamePad.down) || (!gamePad[0].up && lastGamePad.up)) {selection=(selection+1)&1; PLAYSFX(SFX_WING);}
			switch (selection)  {
				case 0: print(114+(rsin(VSync(-1)<<7)>>10),104,128,128,255,0,">");
				break;
				case 1: print(114+(rsin(VSync(-1)<<7)>>10),120,128,128,255,0,">");
				break;
			}
			if (!gamePad[0].right && lastGamePad.right) {
				audioChannel=(audioChannel+1)&7;
				Sound_CD_XAChangeChannel(audioChannel);
			} else if (!gamePad[0].left && lastGamePad.left) {
				audioChannel=(audioChannel-1)&7;
				Sound_CD_XAChangeChannel(audioChannel);
			}
			memcpy(&lastGamePad,&gamePad[0],sizeof(struct s_gamePad));
			nextFrameFlipBuff();
			printGame();
		}
		while (!gamePad[0].start || !gamePad[0].ex) VSync(0);
		fflags&=0xDF;
		PLAYSFX(SFX_SWSH);
		SpuSetCommonCDVolume(0x3fff, 0x3fff);
		if (selection==1) levelExitCode=1;
	}
}

void fdie() {
	u_char i,j;
	flappyAng.vz=1024;
	PLAYSFX(SFX_HIT);
	nextFrameFlipBuff();
	printGame();
	PLAYSFX(SFX_DIE);
	for (i=0;i<8;i++) {
		nextFrameFlipBuff();
		printGame();
		if ((flappyPos.vy&15)<=8 || !b((flappyPos.vy>>4)+33,(flappyPos.vx>>4)+32)) flappyPos.vy++;	//Not ground
		else STOPSFX(SFX_DIE);
		if (!(fflags&16)) updateGameWorldPos();
	}
	for (i=0;i<32;i++) {	//the same with fullScreenBlack
		nextFrameFlipBuff();
		printGame();
		fullScreenBlack.r0=i<<3; fullScreenBlack.g0=i<<3; fullScreenBlack.b0=i<<3;
		DrawPrim(&fullScreenBlack);
		if ((flappyPos.vy&15)<=8 || !b((flappyPos.vy>>4)+33,(flappyPos.vx>>4)+32)) flappyPos.vy++;	//Not ground
		else STOPSFX(SFX_DIE);
	}
	levelCustomCode[lvlnum].onDie();
	//Restart level
	flappyPos.vx=spawnpoint[0]<<4; flappyPos.vy=spawnpoint[1]<<4|9;
	vacc=0; fflags=0; flappyAng.vy=0; pos.vy=999;
	
	memcpy(coinCollected,lastCoinCollected,ncoins); remCoins=lastRemCoins;
}


void updatePlayer() {
	if (!gamePad[0].ex && vacc>=0)  {	//jump button
		vacc=-19;
		PLAYSFX(SFX_WING);
	}
	flappyPos.vy+=vacc>>3;
	if ((flappyPos.vy&15)>8 && b((flappyPos.vy>>4)+33,(flappyPos.vx>>4)+32)) {	//ground
		switch (b((flappyPos.vy>>4)+33,(flappyPos.vx>>4)+32)) {
			case 1:	//normal ground
			if (gamePadLeft(0)) {flappyPos.vx--; fflags|=1;}
			else if (gamePadRight(0)) {flappyPos.vx++; fflags&=0xFE;}
			flappyAng.vy=(fflags&1)?2048:0;
			break;
			case 2:	//ice ground
			if ((flappyPos.vx&15)==8 && (b((flappyPos.vy>>4)+32,(flappyPos.vx>>4)+33) || b((flappyPos.vy>>4)+32,(flappyPos.vx>>4)+31))) {	//wallhit
				if (gamePadLeft(0)) fflags|=1;
				else if (gamePadRight(0)) fflags&=0xFE;
				flappyAng.vy=(fflags&1)?2048:0;
			}
			if (fflags&1) flappyPos.vx--; else flappyPos.vx++;
			break;
			default: //obstacle
			fflags|=8;
			return;
		}
		if (!(fflags&2)) {	//hit floor first time, to stay
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
		if (gamePadLeft(0)) {flappyPos.vx--; fflags|=1;}
		if (gamePadRight(0)) {flappyPos.vx++; fflags&=0xFE;}
		flappyAng.vy=(fflags&1)?2048:0;
	} else {	//fly
		if ((flappyPos.vy&15)<7 && b((flappyPos.vy>>4)+31,(flappyPos.vx>>4)+32)) {	//ceilhit
			fflags|=8;
			return;
		}
		if ((!(fflags&1)) && (flappyPos.vx&15)>8 && b((flappyPos.vy>>4)+32,(flappyPos.vx>>4)+33)	//Wallhit
		|| (fflags&1) && (flappyPos.vx&15)<8 && b((flappyPos.vy>>4)+32,(flappyPos.vx>>4)+31)
		|| (!(fflags&1)) && (flappyPos.vx&15)>8 && (flappyPos.vy&15)<8 && b((flappyPos.vy>>4)+31,(flappyPos.vx>>4)+33)
		|| (!(fflags&1)) && (flappyPos.vx&15)>8 && (flappyPos.vy&15)>9 && b((flappyPos.vy>>4)+33,(flappyPos.vx>>4)+33)
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

u_char startLevel(u_char lvlnumparam) {
	lvlnum=lvlnumparam;
	sprintf(stringholder,"\\BKG%d.TIM;1",lvlnum);
	loadTimCD(stringholder);
	sprintf(stringholder,"\\LVL%d.LVL;1",lvlnum);
	loadLvlCD(stringholder);
	levelCustomCode[lvlnum].onStart();
	audioChannel=lvlnum;
	Sound_CD_XAPlay("\\MUSIC1.XA;1", lvlnum);
	while (!levelExitCode) {
		nextFrameFlipBuff();
		updatePlayer();
		if (!(fflags&16)) updateGameWorldPos();
		printGame();
		levelCustomCode[lvlnum].onFrame();
		if (fflags&8) {fdie(); continue;}
		checkPauseAndAlert();
		//PRINTFMT(8,8,255,255,255,0,"Fla pos: %d,%d [%d,%d] + %d,%d\nWld pos: %d,%d\nfflags: %02x vline: %d flost: %d",flappyPos.vx,flappyPos.vy,flappyPos.vx>>4,flappyPos.vy>>4,flappyPos.vx&15,flappyPos.vy&15,pos.vx,pos.vy,fflags,VSync(1),VSync(-1)-frame);
		frame++;
	}
	levelCustomCode[lvlnum].onEnd();
	unLoadLvl();
	Sound_CD_XAStop();
	return levelExitCode;
}
