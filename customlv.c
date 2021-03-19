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

SPRT_16 l1_ball={r0:128, g0:128, b0:128, u0:54, v0:240};
SVECTOR l1_ballpos;
signed char l1_ballvel[2];
u_char l1_buttonPressedFrameCounter;
u_char l1_firstDieMsgShown;

void level1_onStart() {
	l1_buttonPressedFrameCounter=0; l1_firstDieMsgShown=0;
	//Final challenge
	setSprt16(&l1_ball);
	l1_ballpos.vx=-202; l1_ballpos.vy=144; l1_ballvel[0]=-1; l1_ballvel[1]=1;
}

void level1_onFrame() {
	if (flappyPos.vx>-190 || flappyPos.vy<0) {		//Normal level code
		if (FEObjects[13].data[1]) {l1_buttonPressedFrameCounter=80; STOPSFX(SFX_TTAC);}
		else {
			if (!(fflags&40) && l1_buttonPressedFrameCounter) {
				if (l1_buttonPressedFrameCounter==80) PLAYSFX(SFX_TTAC);
				l1_buttonPressedFrameCounter--;
				if (!l1_buttonPressedFrameCounter) STOPSFX(SFX_TTAC);
			}
		}
		FEObjects[14].data[0]=FEObjects[15].data[0]=FEObjects[13].data[0]=	//open doors if button pressed, or block on button
				l1_buttonPressedFrameCounter || (!(FEObjects[13].dy-FEObjects[16].dy) && (abs(FEObjects[13].dx-FEObjects[16].dx)<8));
		FEObjects[44].data[0]=FEObjects[43].data[0];	//opens door if button pressed
	} else {		//Final challenge level code
		FEObjects[44].data[0]=0;	 //close door
		fflags|=16;
		if (pos.vx<262 && !(fflags&40)) pos.vx++;
		if (pos.vy>-145 && !(fflags&40)) pos.vy--; else if (pos.vy<-145 && !(fflags&40)) pos.vy++;
		if (pos.vx==262 && pos.vy==-145) {	//custom camera ok
			if (flappyPos.vx<-348) {levelExitCode=2; return;}
			RotTransPers(&l1_ballpos,(long*)&l1_ball.x0,&dmy,&flg);
			DrawPrim(&l1_ball);
			if (fflags&40) return;	//don't update ball position if paused/death
			if (frame&7) l1_ballpos.vx+=l1_ballvel[0];
			if (l1_ballpos.vx>-202) {
				PLAYSFX(SFX_ABS1);
				l1_ballvel[0]=-l1_ballvel[0];
				l1_ballpos.vx+=l1_ballvel[0];
			} else if (l1_ballpos.vx<-336) {
				PLAYSFX(SFX_ABS1);
				l1_ballvel[0]=-l1_ballvel[0];
				l1_ballpos.vx+=l1_ballvel[0];
			} else if (level[32+(l1_ballpos.vy>>4)][32+(l1_ballpos.vx+8>>4)]) {
				PLAYSFX(SFX_ABS1);
				level[32+(l1_ballpos.vy>>4)][32+(l1_ballpos.vx+8>>4)]=0;
				l1_ballvel[0]=-l1_ballvel[0];
				l1_ballpos.vx+=l1_ballvel[0];
			} else if (level[32+(l1_ballpos.vy>>4)][32+(l1_ballpos.vx>>4)]) {
				PLAYSFX(SFX_ABS1);
				level[32+(l1_ballpos.vy>>4)][32+(l1_ballpos.vx>>4)]=0;
				l1_ballvel[0]=-l1_ballvel[0];
				l1_ballpos.vx+=l1_ballvel[0];
			}
			l1_ballpos.vy+=l1_ballvel[1];
			if (l1_ballpos.vy>200) {
				l1_ballvel[0]=0;
				l1_ballvel[1]=0;
			} else if (abs(flappyPos.vx-l1_ballpos.vx)<16 && (flappyPos.vy-l1_ballpos.vy)<8) {
				PLAYSFX(SFX_ABS1);
				l1_ballvel[1]=-1;
				l1_ballvel[0]=(flappyPos.vx>l1_ballpos.vx)?-1:1;
				l1_ballpos.vy=flappyPos.vy-8;
			} else if (level[32+(l1_ballpos.vy>>4)][32+(l1_ballpos.vx>>4)]) {
				PLAYSFX(SFX_ABS1);
				if (level[32+(l1_ballpos.vy>>4)][32+(l1_ballpos.vx>>4)]==2) level[32+(l1_ballpos.vy>>4)][32+(l1_ballpos.vx>>4)]=0;
				l1_ballvel[1]=-l1_ballvel[1];
				l1_ballpos.vy+=l1_ballvel[1];
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
	if(!l1_firstDieMsgShown && remCoins!=lastRemCoins) {
		alertmsg="If you hit something,\nyou will die and lose\nall your coins since\nlast checkpoint. Check\ndistances before jump!";
		l1_firstDieMsgShown=1;
	}
	//Final challenge
	l1_ballpos.vx=-202; l1_ballpos.vy=144; l1_ballvel[0]=-1; l1_ballvel[1]=1;
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
		if (fflags&40) return;	//Don't do anything if paused/dead
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
u_char l3_ttakplaying;

void level3_onStart() {
	alertmsg="Flappy, you must get to\nthe checkpoints before\ncountdown gets 0.\nWill you reach the end?\nGood luck!";
	l3_reachedChck=0;
	l3_ttakplaying=0;
	l3_timer=l3_countdowns[l3_reachedChck]*50+49;
}

void level3_onFrame() {
	printNum(16,8,l3_timer/50);
	if (!l3_timer) fflags|=8;
	if (fflags&40) {STOPSFX(SFX_TTAC); l3_ttakplaying=0; return;}
	else l3_timer--;
	if (l3_timer<=299 && l3_ttakplaying==0) {PLAYSFX(SFX_TTAC); l3_ttakplaying=1;}
	
	if (FEObjects[8].data[0]) {							//checkpoint
		FEObjects[37].data[0]=0;						//deactivate button->door
		if (l3_reachedChck==0) {l3_reachedChck++; l3_timer=l3_countdowns[l3_reachedChck]*50+49; STOPSFX(SFX_TTAC);}
	}
	FEObjects[36].data[0]=FEObjects[37].data[0];		//button->door
	
	if (FEObjects[7].data[0]) {							//checkpoint
		FEObjects[6].data[0]=0;							//deactivate button->door
		if (l3_reachedChck==1) {l3_reachedChck++; l3_timer=l3_countdowns[l3_reachedChck]*50+49; STOPSFX(SFX_TTAC);}
	}
	FEObjects[5].data[0]=FEObjects[6].data[0];			//button->door
	
	if (FEObjects[16].data[0]) {						//checkpoint
		FEObjects[24].data[0]=0;						//deactivate button->door
		if (l3_reachedChck==2) {l3_reachedChck++; l3_timer=l3_countdowns[l3_reachedChck]*50+49; STOPSFX(SFX_TTAC);}
	}
	FEObjects[25].data[0]=FEObjects[24].data[0];		//button->door
	
	if (FEObjects[28].data[0]) {						//checkpoint
		FEObjects[29].data[0]=0;						//deactivate button->door
		if (l3_reachedChck==3) {l3_reachedChck++; l3_timer=l3_countdowns[l3_reachedChck]*50+49; STOPSFX(SFX_TTAC);}
	}
	FEObjects[30].data[0]=FEObjects[29].data[0];		//button->door
	
	if (flappyPos.vy<-496) {levelExitCode=2;STOPSFX(SFX_TTAC);}
}

void level3_onDie() {
	l3_timer=l3_countdowns[l3_reachedChck]*50+49;
	FEObjects[17].dx=-432;FEObjects[17].dy=368;		//restore block position
	FEObjects[37].data[0]=0;						//deactivate button->door
	FEObjects[6].data[0]=0;							//deactivate button->door
	FEObjects[24].data[0]=0;						//deactivate button->door
	FEObjects[29].data[0]=0;						//deactivate button->door
}

void level4_onFrame() {
	FEObjects[37].data[0]=FEObjects[35].data[0] & FEObjects[36].data[0];
	if (FEObjects[45].data[0]) {	//Final challenge button
		FEObjects[46].data[0]=0;
		level[10][39]=level[12][39]=level[10][46]=level[12][46]=level[10][52]=level[12][52]=0;
		if (flappyPos.vy<-496) levelExitCode=2;
	} else {
		FEObjects[46].data[0]=1;
		level[10][39]=level[12][39]=2;
		level[10][46]=level[12][46]= (FEObjects[40].data[0]=FEObjects[40].data[1]) ? 0 : 2;
		level[10][52]=level[12][52]= (FEObjects[41].data[0]=FEObjects[41].data[1]) ? 0 : 2;
		level[10][57]=level[12][57]= (FEObjects[42].data[0]=FEObjects[42].data[1]) ? 0 : 2;
	}
}

void level4_onDie() {
	FEObjects[35].data[0]=FEObjects[36].data[0]=0;	//buttons
	FEObjects[45].data[0]=0;						//final challenge button
	FEObjects[47].dx=192; FEObjects[48].dx=144;		//restore final challenge angry birds positions
}

SPRT l5_spaceship={w:38, h:24, u0:24, v0:214, y0:148};
SPRT l5_vader={w:23, h:16, u0:233, v0:0};
TILE l5_shoot={w:2,h:8,r0:255,g0:255,b0:255};
u_char l5_msgshown,l5_finalchready;
u_char l5_vadersx, l5_vadersy, l5_vadersr;
u_char l5_vadersbm[3];

void level5_onStart() {
	setSprt(&l5_spaceship);
	setShadeTex(&l5_spaceship,1);
	setSprt(&l5_vader);
	setShadeTex(&l5_vader,1);
	SetTile(&l5_shoot);
	l5_msgshown=0;
	l5_finalchready=0;
	l5_spaceship.x0=160;
	l5_shoot.x0=l5_spaceship.x0+18; l5_shoot.y0=0;
	l5_vadersx=16; l5_vadersy=0; l5_vadersr=1;
	l5_vadersbm[0]=l5_vadersbm[1]=l5_vadersbm[2]=0xFF;
}

void level5_onFrame() {
	if (frame&1 && !(fflags&46)) vacc--;
	if (vacc>15) vacc=15;
	if (flappyPos.vx>-238 || flappyPos.vy>-300) {		//Normal level code
		FEObjects[3].data[0]=FEObjects[2].data[0];		//button->door
	} else {											//Final challenge
		fflags|=16;
		if (!l5_finalchready) {							//Final challenge not ready yet = move camera
			if (fflags&40) return;						//Don't move camera if paused/dead
			FEObjects[25].data[0]=0;
			if (pos.vy<412) pos.vy++;
			else if (pos.vy>412) pos.vy--;
			if (pos.vx<330) pos.vx++;
			else if (flappyPos.vy>-362) {
				u_char i;
				for (i=6;i<17;i++) level[8][i]=2;
				l5_finalchready=1;
			}
		}
		else {											//Final challenge ready
			u_char i,j;
			if (!l5_msgshown) {
				alertmsg="Flappy, you have found a\nship that will bring you\nto Bird Island. You must\nprotect it from that\ninvaders!";
				l5_msgshown=1;
			}
			if (l5_vadersbm[0]|l5_vadersbm[1]|l5_vadersbm[2]) {	//Not win yet
				if ((l5_vadersbm[2] && l5_vadersy>=100) || (l5_vadersbm[1] && l5_vadersy>=110) || l5_vadersy>=120) fflags|=8;
				if (!(frame&7) && !(fflags&40)) {	// update vaders
					l5_vader.v0^=16;
					if (l5_vadersr) {
						if (l5_vadersx<56) l5_vadersx+=2;
						else {l5_vadersy+=10; l5_vadersr=0;}
					} else {
						if (l5_vadersx>16) l5_vadersx-=2;
						else {l5_vadersy+=10; l5_vadersr=1;}
					}
				}
				
				for (i=0;i<3;i++) {		//print vaders
					l5_vader.y0=l5_vadersy+i*20;
					l5_vader.x0=l5_vadersx;
					for (j=1;j!=0;j<<=1) {							//iterate vader bitmap
						short dx=l5_shoot.x0-l5_vader.x0,dy=l5_shoot.y0-l5_vader.y0;
						if (l5_vadersbm[i]&j) {						//vader alive
							DrawPrim(&l5_vader);
							if (dx>0 && dx<23 && dy>0 && dy<16) {	//collision
								l5_vadersbm[i]^=j;
								PLAYSFX(SFX_LSER);
								l5_shoot.x0=l5_spaceship.x0+18; l5_shoot.y0=l5_spaceship.y0;
							}
						}
						l5_vader.x0+=32;
					}
				}
				
				if (l5_shoot.y0<0) {	//Update shoot
					l5_shoot.x0=l5_spaceship.x0+18; l5_shoot.y0=l5_spaceship.y0;
					PLAYSFX(SFX_LSER);
				} else if (!(fflags&40)) {
					l5_shoot.y0-=2;
				}
				DrawPrim(&l5_shoot);
				
				//Update ship
				if ((FEObjects[26].data[0]=FEObjects[26].data[1]) && l5_spaceship.x0>8 && !(fflags&40)) l5_spaceship.x0--;
				if ((FEObjects[27].data[0]=FEObjects[27].data[1]) && l5_spaceship.x0<280 && !(fflags&40)) l5_spaceship.x0++;
				DrawPrim(&l5_spaceship);
			} else {		//win
				for (i=10;i<13;i++) level[8][i]=0;
				l5_spaceship.x0=10;
				DrawPrim(&l5_spaceship);
				if (flappyPos.vy<-400) levelExitCode=2;
			}
		}
	}
}

void level5_onDie() {
	u_char i;
	FEObjects[2].data[0]=0;	//disable button
	for (i=6;i<17;i++) level[8][i]=0;
	FEObjects[25].data[0]=1;						//Door from final challenge
	FEObjects[26].data[0]=FEObjects[27].data[0]=0;	//Buttons from final challenge
	
	l5_finalchready=0;
	l5_spaceship.x0=160;
	l5_shoot.x0=l5_spaceship.x0+18; l5_shoot.y0=0;
	l5_vadersx=16; l5_vadersy=0; l5_vadersr=1;
	l5_vadersbm[0]=l5_vadersbm[1]=l5_vadersbm[2]=0xFF;
}

struct l6_animstruct {
   signed char vx : 2;
   signed char vy : 2;
   u_char mod : 4;
   unsigned short time : 16;
};

SPRT l6_abird={w:32, h:32, v0:0};
short l6_inib_x=470, l6_inib_y=416, l6_b_x, l6_b_y;
u_char l6_animidx,l6_animsize=7;
unsigned short l6_animcnt;
struct l6_animstruct test={-1,0,1,255};
struct l6_animstruct l6_anim[]={{-1,0,1,214},{0,-1,3,128},{1,0,1,210},{0,-1,1,176},{-1,0,2,354},{0,-1,2,336},{-1,0,2,620},{0,0,15,999}};

void level6_onStart() {
	setSprt(&l6_abird); setShadeTex(&l6_abird,1);
	l6_b_x=l6_inib_x; l6_b_y=l6_inib_y;
	l6_animidx=l6_animcnt=0;
	alertmsg="Red: What?? You are next\nto pipe forest end in\nBird Island! I won't let\nyou get there, Flappy.\nI'll go faster than you\nand stop you! hahaha!";
}

void level6_onFrame() {
	SVECTOR v={l6_b_x,l6_b_y,persp};
	RotTransPers(&v,(long*)&l6_abird.x0,&dmy,&flg);
	l6_abird.u0=(l6_anim[l6_animidx].vx<0)?138:(l6_anim[l6_animidx].vx)?202:170;
	DrawPrim(&l6_abird);
	if (l6_animidx==l6_animsize) {fflags|=8; return;}
	/*if (flappyPos.vy<-130 && flappyPos.vx<140) {	//final camera
		fflags|=16;
	}*/
	if (!(fflags&40) && !(frame%l6_anim[l6_animidx].mod)) {		//next anim
		l6_b_x+=l6_anim[l6_animidx].vx; l6_b_y+=l6_anim[l6_animidx].vy;
		if (++l6_animcnt == l6_anim[l6_animidx].time) {
			l6_animcnt=0; l6_animidx++;
			if (l6_animidx==l6_animsize) alertmsg="Red: too slow, Flappy!\nhahaha!";
		}
		/*
		//DEBUG PATH
		fflags|=16;
		pos.vx=(l6_b_x>400)?-400:(l6_b_x<-416)?416:-l6_b_x;
		pos.vy=(l6_b_y>436)?-436:(l6_b_y<-420)?420:-l6_b_y;
		*/
	}
	if (flappyPos.vx<-500 || coinCollected[0]) levelExitCode=2;	//level end
}

void level6_onDie() {
	frame=0;	//needed for modulo calculations per frame to act always the same, and object timings equals each try
	FEObjects[14].dx=0;FEObjects[14].dy=144;	//block position
	l6_b_x=l6_inib_x; l6_b_y=l6_inib_y;
	l6_animidx=l6_animcnt=0;
}

void level6_onEnd() {
	if (levelExitCode==2) {
		u_char* creditstxt=readFromCD("\\GAME\\CREDITS.TXT;1");
		unsigned short scrollcredits=0;
		short creditspos=480;
		loadTimCD("\\GAME\\CREDITS.TIM;1");
		Sound_CD_XAPlay("\\GAME\\MUSIC.XA;1", 7);
		while (creditspos>-880) {
			if (!scrollcredits) {
				static RECT backgroundRect={x:704,y:272,w:320,h:240};
				MoveImage(&backgroundRect,0,cdb->draw.clip.y);
			} else {
				RECT backgroundRect1={x:704,y:272,w:scrollcredits,h:240};
				RECT backgroundRect2={x:704+scrollcredits,y:272,w:320-scrollcredits,h:240};
				MoveImage(&backgroundRect2,0,cdb->draw.clip.y);
				MoveImage(&backgroundRect1,320-scrollcredits,cdb->draw.clip.y);
			}
			scrollcredits=(scrollcredits+1)%320;
			DrawPrim(&tp[1]);
			print(5,(creditspos>>1)+1,0,0,0,0,creditstxt);
			print(4,(creditspos>>1),255,255,255,0,creditstxt);
			creditspos--;
			while (VSync(-1)&1);	//Delay
			nextFrameFlipBuff();
		}
		Sound_CD_XAStop();
		free(creditstxt);
		remCoins=0;	//Force level coin collection
	}
}

struct LvlFuncs levelCustomCode[]= {
	{&noLevFunc,&noLevFunc,&noLevFunc,&noLevFunc},
	{&level1_onStart,&level1_onFrame,&level1_onDie,&stopTtac},
	{&level2_onStart,&level2_onFrame,&level2_onDie,&noLevFunc},
	{&level3_onStart,&level3_onFrame,&level3_onDie,&stopTtac},
	{&noLevFunc,&level4_onFrame,&level4_onDie,&noLevFunc},
	{&level5_onStart,&level5_onFrame,&level5_onDie,&noLevFunc},
	{&level6_onStart,&level6_onFrame,&level6_onDie,&level6_onEnd}
};