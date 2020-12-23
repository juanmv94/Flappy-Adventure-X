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

struct LvlFuncs levelCustomCode[1]= {
	{&level0_onStart,&level0_onFrame,&level0_onDie}
};