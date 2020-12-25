typedef void (*LvlFunc)();

struct LvlFuncs {
	LvlFunc onStart;
	LvlFunc onFrame;
	LvlFunc onDie;
};

void noLevFunc() {
	//Nothing here
}

//Level 0
SPRT_16 ball={r0:128, g0:128, b0:128, u0:54, v0:240};
SVECTOR ballpos;
signed char ballvel[2];
u_char buttonPressedFrameCounter;
u_char firstDieMsgShown;

void level0_onStart() {
	buttonPressedFrameCounter=0; firstDieMsgShown=0;
	//Final challenge
	setSprt16(&ball);
	ballpos.vx=-202; ballpos.vy=144; ballvel[0]=-1; ballvel[1]=1;
}

void level0_onFrame() {
	if (flappyPos.vx>-190 || flappyPos.vy<0) {		//Normal level code
		if (FEObjects[13].data[1]) {buttonPressedFrameCounter=80; STOPSFX(SFX_TTAC);}
		else {
			if (buttonPressedFrameCounter) {
				if (buttonPressedFrameCounter==80) PLAYSFX(SFX_TTAC);
				buttonPressedFrameCounter--;
				if (!buttonPressedFrameCounter) STOPSFX(SFX_TTAC);
			}
		}
		FEObjects[14].data[0]=FEObjects[15].data[0]=FEObjects[13].data[0]=	//open doors if button pressed, or block on button
				buttonPressedFrameCounter || (!(FEObjects[13].dy-FEObjects[16].dy) && (abs(FEObjects[13].dx-FEObjects[16].dx)<8));
		FEObjects[41].data[0]=FEObjects[40].data[0];	//opens door if button pressed
	} else {		//Final challenge level code
		FEObjects[41].data[0]=0;	 //close door
		fflags|=16;
		if (pos.vx<262) pos.vx++;
		if (pos.vy>-145) pos.vy--; else if (pos.vy<-145) pos.vy++;
		if (pos.vx==262 && pos.vy==-145) {	//custom camera ok
			if (flappyPos.vx<-348) {levelExitCode=2; return;}
			RotTransPers(&ballpos,(long*)&ball.x0,&dmy,&flg);
			DrawPrim(&ball);
			if (frame&7) ballpos.vx+=ballvel[0];
			if (ballpos.vx>-202) {
				PLAYSFX(SFX_ABS1);
				ballvel[0]=-ballvel[0];
				ballpos.vx+=ballvel[0];
			} else if (ballpos.vx<-336) {
				PLAYSFX(SFX_ABS1);
				ballvel[0]=-ballvel[0];
				ballpos.vx+=ballvel[0];
			} else if (level[32+(ballpos.vy>>4)][32+(ballpos.vx+8>>4)]) {
				PLAYSFX(SFX_ABS1);
				level[32+(ballpos.vy>>4)][32+(ballpos.vx+8>>4)]=0;
				ballvel[0]=-ballvel[0];
				ballpos.vx+=ballvel[0];
			} else if (level[32+(ballpos.vy>>4)][32+(ballpos.vx>>4)]) {
				PLAYSFX(SFX_ABS1);
				level[32+(ballpos.vy>>4)][32+(ballpos.vx>>4)]=0;
				ballvel[0]=-ballvel[0];
				ballpos.vx+=ballvel[0];
			}
			ballpos.vy+=ballvel[1];
			printf("%d %d, %d %d\n",ballpos.vx,ballpos.vy,flappyPos.vx,flappyPos.vy);
			if (ballpos.vy>200) {
				ballvel[0]=0;
				ballvel[1]=0;
			} else if (abs(flappyPos.vx-ballpos.vx)<16 && (flappyPos.vy-ballpos.vy)<8) {
				PLAYSFX(SFX_ABS1);
				ballvel[1]=-1;
				ballvel[0]=(flappyPos.vx>ballpos.vx)?-1:1;
				ballpos.vy=flappyPos.vy-8;
			} else if (level[32+(ballpos.vy>>4)][32+(ballpos.vx>>4)]) {
				PLAYSFX(SFX_ABS1);
				if (level[32+(ballpos.vy>>4)][32+(ballpos.vx>>4)]==2) level[32+(ballpos.vy>>4)][32+(ballpos.vx>>4)]=0;
				ballvel[1]=-ballvel[1];
				ballpos.vy+=ballvel[1];
			}
		}
	}
}

void level0_onDie() {
	if ((FEObjects[13].dy-FEObjects[16].dy) || abs(FEObjects[13].dx-FEObjects[16].dx)>=8) {
		FEObjects[16].dx=4<<4;FEObjects[16].dy=9<<4;	//restore block position if not on button
	}
	FEObjects[33].dx=112;FEObjects[33].dy=-416;		//restore block position
	FEObjects[34].dx=244;FEObjects[34].dy=-336;		//restore block position
	if(!firstDieMsgShown && remCoins!=lastRemCoins) {
		alertmsg="If you hit something,\nyou will die and lose\nall your coins since\nlast checkpoint. Check\ndistances before jump!";
		firstDieMsgShown=1;
	}
	//Final challenge
	ballpos.vx=-202; ballpos.vy=144; ballvel[0]=-1; ballvel[1]=1;
	if (flappyPos.vx<190 && flappyPos.vy>=0) {
		u_char i,j;
		for (i=37;i<41;i++) for (j=11;j<20;j++) level[i][j]=2;
	}
}

u_char l1_passStep,l1_correctPassMsgShown;
u_char l1_password[7]={0,6,1,5,2,4,3};

u_char l1_tetP[6][2][4]={{{1,1,1,0},{0,1,0,0}},{{1,1,1,1},{0,0,0,0}},{{1,1,1,0},{1,0,0,0}},{{1,1,0,0},{1,1,0,0}},{{1,1,0,0},{0,1,1,0}},{{1,0,0,0},{1,1,1,0}}};
u_char l1_tetMsgShown,l1_curTetP, l1_pPosx, l1_pPosy, l1_tetBlocked, l1_tetLines;

void level1_resetPassButtons() {
	u_char i;
	for (i=15;i<=21;i++) if (!FEObjects[i].data[1]) FEObjects[i].data[0]=0;
	l1_passStep=0;
}

void level1_addTP() {
	u_char i,j;
	for (i=0;i<4;i++) for (j=0;j<2;j++) if (l1_tetP[l1_curTetP][j][i]) level[l1_pPosy+i][l1_pPosx+j]=1;
}

void level1_removeTP() {
	u_char i,j;
	for (i=0;i<4;i++) for (j=0;j<2;j++) if (l1_tetP[l1_curTetP][j][i]) level[l1_pPosy+i][l1_pPosx+j]=((l1_pPosy+i)>1)?0:2;
}

u_char l1_canGoDownTP() {
	u_char i,j;
	for (i=0;i<2;i++) for (j=0;j<4;j++) if (l1_tetP[l1_curTetP][i][3-j]) if (level[4-j+l1_pPosy][i+l1_pPosx]==1) return 0;
	return 1;
}

void level1_checkLinesT() {
	u_char i,l=10;
	for (i=9;i>=2;i--) {
		if (!(level[i][1] && level[i][2] && level[i][3] && level[i][4] && level[i][5] && level[i][6])) {
			l--;
			if (l!=i) memcpy(&level[l][1],&level[i][1],6);
		}
		else {
			level[3+(l1_tetLines++)][0]=0;
		}
	}
	l--;
	while ((--l)>=2) {
		level[l][1]=level[l][2]=level[l][3]=level[l][4]=level[l][5]=level[l][6]=0;
	}
}

u_char l1_canGoLeftTP() {
	u_char i,j;
	if (l1_pPosx<2) return 0;
	for (j=0;j<4;j++) for (i=0;i<2;i++) if (l1_tetP[l1_curTetP][i][j]) if (level[j+l1_pPosy][i+l1_pPosx-1]==1) return 0;
	return 1;
}

u_char l1_canGoRightTP() {
	u_char i,j;
	if ((l1_pPosx>5) || (l1_curTetP!=1 && l1_pPosx>4)) return 0;
	for (j=0;j<4;j++) for (i=0;i<2;i++) if (l1_tetP[l1_curTetP][1-i][j]) if (level[j+l1_pPosy][2-i+l1_pPosx]==1) return 0;
	return 1;
}

void level1_onStart() {
	l1_passStep=0;
	l1_correctPassMsgShown=0; l1_tetMsgShown=0;
	l1_curTetP=5; l1_pPosy=0; l1_pPosx=3; l1_tetBlocked=0;
}

void level1_onFrame() {
	u_char i,j;
	if (flappyPos.vx>-400 || flappyPos.vy>-350) {	//Normal level code
		FEObjects[3].data[0]=FEObjects[6].data[0];		//opens door if button pressed
		FEObjects[11].data[0]=FEObjects[12].data[0]=(FEObjects[15].data[0] && FEObjects[16].data[0] && FEObjects[17].data[0] &&
				FEObjects[18].data[0] && FEObjects[19].data[0] && FEObjects[20].data[0] && FEObjects[21].data[0]);
		//Password
		if (l1_passStep<7) {
			if (FEObjects[15+l1_password[l1_passStep]].data[0]) l1_passStep++;
			for (i=6;i>l1_passStep;i--) {
				if (FEObjects[15+l1_password[i]].data[0]) {
					level1_resetPassButtons();
					break;
				}
			}
		} else {
			if (!l1_correctPassMsgShown) {
				alertmsg="Correct password!\nDoors open";
				l1_correctPassMsgShown=1;		
			}
		}
	} else {	//final challenge
		if (flappyPos.vx<-490) levelExitCode=2;
		FEObjects[12].data[0]=0;	 //close door
		fflags|=16;
		if (pos.vx<416) pos.vx++;
		if (pos.vy>416) pos.vy--; else if (pos.vy<416) pos.vy++;
		if (pos.vx==416 && pos.vy==416 && !l1_tetBlocked) {	//custom camera ok
			static struct s_gamePad lastGamePad;
			if (!l1_tetMsgShown) {
				alertmsg="See that exit at top?\nMake lines to unfreeze\nthat ice. Use R1/L1 to\nmove blocks";
				l1_tetMsgShown=1;
			}
			level1_removeTP();
			if (!gamePad[0].l1 && lastGamePad.l1 && l1_canGoLeftTP()) l1_pPosx--;
			else if (!gamePad[0].r1 && lastGamePad.r1 && l1_canGoRightTP()) l1_pPosx++;
			if (!(frame&31)) {
				if (l1_canGoDownTP()) {
					l1_pPosy++;
				} else {
					if (l1_pPosy<4) {
						l1_tetBlocked=1;
					} else {
						level1_addTP();
						level1_checkLinesT();
						l1_pPosy=0; l1_pPosx=3; l1_curTetP=(l1_curTetP+1)%6;
						for (i=0;i<4;i++) for (j=0;j<2;j++) level[3+i][8+j]=(l1_tetP[(l1_curTetP+1)%6][j][i])?1:2;	//next piece HUD
					}
				}
			}
			level1_addTP();
			if (level[32+(flappyPos.vy>>4)][32+(flappyPos.vx>>4)]) fflags|=8;
			memcpy(&lastGamePad,&gamePad[0],sizeof(struct s_gamePad));
		}
	}
	
}

void level1_onDie() {
	u_char i,j;
	FEObjects[4].dx=80;FEObjects[4].dy=368;		//restore block position
	FEObjects[5].dx=304;FEObjects[5].dy=368;	//restore block position
	if (l1_passStep<7) level1_resetPassButtons();
	if (flappyPos.vx<=-400 || flappyPos.vy<=-350) {		//Final challenge
		l1_pPosy=0; l1_pPosx=3; l1_tetBlocked=0;
		for (j=1;j<7;j++) level[1][j]=2;
		for (i=2;i<10;i++) for (j=1;j<7;j++) level[i][j]=0;
		while (l1_tetLines) {level[2+l1_tetLines][0]=2; l1_tetLines--;}
	}
}

struct LvlFuncs levelCustomCode[]= {
	{&level0_onStart,&level0_onFrame,&level0_onDie},
	{&level1_onStart,&level1_onFrame,&level1_onDie}
};