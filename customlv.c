typedef void (*LvlFunc)();

struct LvlFuncs {
	LvlFunc onStart;
	LvlFunc onFrame;
	LvlFunc onDie;
	LvlFunc onEnd;
};

void noLevFunc() {
	//Nothing here
}

void stopTtac() {
	STOPSFX(SFX_TTAC);
}

SPRT_16 ball={r0:128, g0:128, b0:128, u0:54, v0:240};
SVECTOR ballpos;
signed char ballvel[2];
u_char buttonPressedFrameCounter;
u_char firstDieMsgShown;

void level1_onStart() {
	buttonPressedFrameCounter=0; firstDieMsgShown=0;
	//Final challenge
	setSprt16(&ball);
	ballpos.vx=-202; ballpos.vy=144; ballvel[0]=-1; ballvel[1]=1;
}

void level1_onFrame() {
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
		FEObjects[44].data[0]=FEObjects[43].data[0];	//opens door if button pressed
	} else {		//Final challenge level code
		FEObjects[44].data[0]=0;	 //close door
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

void level1_onDie() {
	if ((FEObjects[13].dy-FEObjects[16].dy) || abs(FEObjects[13].dx-FEObjects[16].dx)>=8) {
		FEObjects[16].dx=4<<4;FEObjects[16].dy=9<<4;	//restore block position if not on button
	}
	FEObjects[36].dx=96;FEObjects[36].dy=-416;		//restore block position
	FEObjects[37].dx=244;FEObjects[37].dy=-336;		//restore block position
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

u_char l2_passStep,l2_correctPassMsgShown;
u_char l2_password[7]={0,6,1,5,2,4,3};

u_char l2_tetP[6][2][4]={{{1,1,1,0},{0,1,0,0}},{{1,1,1,1},{0,0,0,0}},{{1,1,1,0},{1,0,0,0}},{{1,1,0,0},{1,1,0,0}},{{1,1,0,0},{0,1,1,0}},{{1,0,0,0},{1,1,1,0}}};
u_char l2_tetMsgShown,l2_curTetP, l2_pPosx, l2_pPosy, l2_tetBlocked, l2_tetLines;

void level2_resetPassButtons() {
	u_char i;
	for (i=15;i<=21;i++) if (!FEObjects[i].data[1]) FEObjects[i].data[0]=0;
	l2_passStep=0;
}

void level2_addTP() {
	u_char i,j;
	for (i=0;i<4;i++) for (j=0;j<2;j++) if (l2_tetP[l2_curTetP][j][i]) level[l2_pPosy+i][l2_pPosx+j]=1;
}

void level2_removeTP() {
	u_char i,j;
	for (i=0;i<4;i++) for (j=0;j<2;j++) if (l2_tetP[l2_curTetP][j][i]) level[l2_pPosy+i][l2_pPosx+j]=((l2_pPosy+i)>1)?0:2;
}

u_char l2_canGoDownTP() {
	u_char i,j;
	for (i=0;i<2;i++) for (j=0;j<4;j++) if (l2_tetP[l2_curTetP][i][3-j]) if (level[4-j+l2_pPosy][i+l2_pPosx]==1) return 0;
	return 1;
}

void level2_checkLinesT() {
	u_char i,l=10;
	for (i=9;i>=2;i--) {
		if (!(level[i][1] && level[i][2] && level[i][3] && level[i][4] && level[i][5] && level[i][6])) {
			l--;
			if (l!=i) memcpy(&level[l][1],&level[i][1],6);
		}
		else {
			level[3+(l2_tetLines++)][0]=0;
		}
	}
	l--;
	while ((--l)>=2) {
		level[l][1]=level[l][2]=level[l][3]=level[l][4]=level[l][5]=level[l][6]=0;
	}
}

u_char l2_canGoLeftTP() {
	u_char i,j;
	if (l2_pPosx<2) return 0;
	for (j=0;j<4;j++) for (i=0;i<2;i++) if (l2_tetP[l2_curTetP][i][j]) if (level[j+l2_pPosy][i+l2_pPosx-1]==1) return 0;
	return 1;
}

u_char l2_canGoRightTP() {
	u_char i,j;
	if ((l2_pPosx>5) || (l2_curTetP!=1 && l2_pPosx>4)) return 0;
	for (j=0;j<4;j++) for (i=0;i<2;i++) if (l2_tetP[l2_curTetP][1-i][j]) if (level[j+l2_pPosy][2-i+l2_pPosx]==1) return 0;
	return 1;
}

void level2_onStart() {
	l2_passStep=0;
	l2_correctPassMsgShown=0; l2_tetMsgShown=0;
	l2_curTetP=5; l2_pPosy=0; l2_pPosx=3; l2_tetBlocked=0;
}

void level2_onFrame() {
	u_char i,j;
	if (flappyPos.vx>-400 || flappyPos.vy>-350) {	//Normal level code
		FEObjects[3].data[0]=FEObjects[6].data[0];		//opens door if button pressed
		FEObjects[11].data[0]=FEObjects[12].data[0]=(FEObjects[15].data[0] && FEObjects[16].data[0] && FEObjects[17].data[0] &&
				FEObjects[18].data[0] && FEObjects[19].data[0] && FEObjects[20].data[0] && FEObjects[21].data[0]);
		//Password
		if (l2_passStep<7) {
			if (FEObjects[15+l2_password[l2_passStep]].data[0]) l2_passStep++;
			for (i=6;i>l2_passStep;i--) {
				if (FEObjects[15+l2_password[i]].data[0]) {
					level2_resetPassButtons();
					break;
				}
			}
		} else {
			if (!l2_correctPassMsgShown) {
				alertmsg="Correct password!\nDoors open";
				l2_correctPassMsgShown=1;		
			}
		}
		//Sine wave coins
		for (i=70*2+1;i<81*2;i+=2) dcoin[i]=400+(rsin((frame<<5)+(i<<8))>>7);
		
	} else {	//final challenge
		if (flappyPos.vx<-490) levelExitCode=2;
		FEObjects[12].data[0]=0;	 //close door
		fflags|=16;
		if (pos.vx<416) pos.vx++;
		if (pos.vy>416) pos.vy--; else if (pos.vy<416) pos.vy++;
		if (pos.vx==416 && pos.vy==416 && !l2_tetBlocked) {	//custom camera ok
			static struct s_gamePad lastGamePad;
			if (!l2_tetMsgShown) {
				alertmsg="See that exit at top?\nMake lines to unfreeze\nthat ice. Use R1/L1 to\nmove blocks";
				l2_tetMsgShown=1;
			}
			level2_removeTP();
			if (!gamePad[0].l1 && lastGamePad.l1 && l2_canGoLeftTP()) l2_pPosx--;
			else if (!gamePad[0].r1 && lastGamePad.r1 && l2_canGoRightTP()) l2_pPosx++;
			if (!(frame&31)) {
				if (l2_canGoDownTP()) {
					l2_pPosy++;
				} else {
					if (l2_pPosy<4) {
						l2_tetBlocked=1;
					} else {
						level2_addTP();
						level2_checkLinesT();
						l2_pPosy=0; l2_pPosx=3; l2_curTetP=(l2_curTetP+1)%6;
						for (i=0;i<4;i++) for (j=0;j<2;j++) level[3+i][8+j]=(l2_tetP[(l2_curTetP+1)%6][j][i])?1:2;	//next piece HUD
					}
				}
			}
			level2_addTP();
			if (level[32+(flappyPos.vy>>4)][32+(flappyPos.vx>>4)]) fflags|=8;
			memcpy(&lastGamePad,&gamePad[0],sizeof(struct s_gamePad));
		}
	}
}

void level2_onDie() {
	u_char i,j;
	FEObjects[4].dx=80;FEObjects[4].dy=368;		//restore block position
	FEObjects[5].dx=304;FEObjects[5].dy=368;	//restore block position
	if (l2_passStep<7) level2_resetPassButtons();
	if (flappyPos.vx<=-400 || flappyPos.vy<=-350) {		//Final challenge
		l2_pPosy=0; l2_pPosx=3; l2_tetBlocked=0;
		for (j=1;j<7;j++) level[1][j]=2;
		for (i=2;i<10;i++) for (j=1;j<7;j++) level[i][j]=0;
		while (l2_tetLines) {level[2+l2_tetLines][0]=2; l2_tetLines--;}
	}
}

u_short l3_timer;
u_char l3_reachedChck;
u_char l3_countdowns[5]={25,35,99,30,20};

void level3_onStart() {
	alertmsg="Flappy, you must get to\nthe checkpoints before\ncountdown gets 0.\nWill you reach the end?\nGood luck!";
	l3_reachedChck=0;
	l3_timer=l3_countdowns[l3_reachedChck]*50;
}

void level3_onFrame() {
	printNum(300,210,--l3_timer/50+1);
	if (!l3_timer) fflags|=8;
	if (fflags&8) {STOPSFX(SFX_TTAC); return;}
	if (l3_timer==250) PLAYSFX(SFX_TTAC);
	
	if (FEObjects[8].data[0]) {							//checkpoint
		FEObjects[37].data[0]=0;						//deactivate button->door
		if (l3_reachedChck==0) {l3_reachedChck++; l3_timer=l3_countdowns[l3_reachedChck]*50; STOPSFX(SFX_TTAC);}
	}
	FEObjects[36].data[0]=FEObjects[37].data[0];		//button->door
	
	if (FEObjects[7].data[0]) {							//checkpoint
		FEObjects[6].data[0]=0;							//deactivate button->door
		if (l3_reachedChck==1) {l3_reachedChck++; l3_timer=l3_countdowns[l3_reachedChck]*50; STOPSFX(SFX_TTAC);}
	}
	FEObjects[5].data[0]=FEObjects[6].data[0];			//button->door
	
	if (FEObjects[16].data[0]) {						//checkpoint
		FEObjects[24].data[0]=0;						//deactivate button->door
		if (l3_reachedChck==2) {l3_reachedChck++; l3_timer=l3_countdowns[l3_reachedChck]*50; STOPSFX(SFX_TTAC);}
	}
	FEObjects[25].data[0]=FEObjects[24].data[0];		//button->door
	
	if (FEObjects[28].data[0]) {						//checkpoint
		FEObjects[29].data[0]=0;						//deactivate button->door
		if (l3_reachedChck==3) {l3_reachedChck++; l3_timer=l3_countdowns[l3_reachedChck]*50; STOPSFX(SFX_TTAC);}
	}
	FEObjects[30].data[0]=FEObjects[29].data[0];		//button->door
	
	if (flappyPos.vy<-496) {levelExitCode=2;STOPSFX(SFX_TTAC);}
}

void level3_onDie() {
	l3_timer=l3_countdowns[l3_reachedChck]*50;
	FEObjects[17].dx=-432;FEObjects[17].dy=368;		//restore block position
	FEObjects[37].data[0]=0;						//deactivate button->door
	FEObjects[6].data[0]=0;							//deactivate button->door
	FEObjects[24].data[0]=0;						//deactivate button->door
	FEObjects[29].data[0]=0;						//deactivate button->door
}

void level4_onFrame() {
	if (flappyPos.vx<-300 && flappyPos.vy<60) levelExitCode=2;
}

struct LvlFuncs levelCustomCode[]= {
	{&noLevFunc,&noLevFunc,&noLevFunc,&noLevFunc},
	{&level1_onStart,&level1_onFrame,&level1_onDie,&stopTtac},
	{&level2_onStart,&level2_onFrame,&level2_onDie,&noLevFunc},
	{&level3_onStart,&level3_onFrame,&level3_onDie,&stopTtac},
	{&noLevFunc,&level4_onFrame,&noLevFunc,&noLevFunc}
};