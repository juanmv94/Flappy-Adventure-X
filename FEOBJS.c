#include "models.h"

void alert(struct FEObject* feo) {
	printFEModel(model_alert, feo->dx, feo->dy);
	if ((!(fflags&8)) && (!feo->data[0]) && abs(feo->dx-flappyPos.vx)<10 && abs(feo->dy-flappyPos.vy)<12) {
		feo->data[0]=1;
		alertmsg=&feo->data[1];
	}
}

void yflag(struct FEObject* feo) {
	printFEModel(model_yflag, feo->dx, feo->dy);
	if (abs(feo->dx-flappyPos.vx)<10 && (feo->dy-flappyPos.vy)<32 && (feo->dy-flappyPos.vy)>0) {
		if (!feo->data[0] && (feo->dx-flappyPos.vx)) {
			PLAYSFX(SFX_CHCK);
			memcpy(lastCoinCollected,coinCollected,ncoins);
			lastRemCoins=remCoins;
			spawnpoint[0]=feo->dx>>4; spawnpoint[1]=(feo->dy>>4)-1;
		}
		feo->data[0]=1;
	} else {
		feo->data[0]=0;
	}
}

void platform(struct FEObject* feo) {
	short my = (feo->data[1]) ? rsin((frame<<feo->data[2])+(feo->data[3]<<4))/feo->data[1] : 0;	//animation desp y
	short mx = (feo->data[4]) ? rsin((frame<<feo->data[5])+(feo->data[6]<<4))/feo->data[4] : 0;	//animation desp x
	short x1=feo->dx-feo->data[0]+mx,x2=feo->dx+feo->data[0]+mx,y1=feo->dy-4+my,y2=feo->dy+4+my;
	SVECTOR pvertex[8]={{x1,y1,0},{x2,y1,0},{x1,y2,0},{x2,y2,0},{x1,y1,persp+16},{x2,y1,persp+16},{x1,y2,persp+16},{x2,y2,persp+16}};
	static long scrfrontpoints[4];
	RotTransPers4(&pvertex[0],&pvertex[1],&pvertex[2],&pvertex[3],(long*)&scrfrontpoints[0],(long*)&scrfrontpoints[1],(long*)&scrfrontpoints[2],(long*)&scrfrontpoints[3],&dmy,&flg);
	if (((short*)&scrfrontpoints[0])[1]>120) {	//draw top
		f4.r0=255; f4.g0=128; f4.b0=128;
		RotTransPers(&pvertex[4],(long*)&f4.x0,&dmy,&flg); RotTransPers(&pvertex[5],(long*)&f4.x1,&dmy,&flg);
		*(long*)&f4.x2=scrfrontpoints[0]; *(long*)&f4.x3=scrfrontpoints[1];
		DrawPrim(&f4);
	} else if (((short*)&scrfrontpoints[2])[1]<120) {	//draw bottom
		f4.r0=160; f4.g0=100; f4.b0=100;
		RotTransPers(&pvertex[6],(long*)&f4.x0,&dmy,&flg); RotTransPers(&pvertex[7],(long*)&f4.x1,&dmy,&flg);
		*(long*)&f4.x2=scrfrontpoints[2]; *(long*)&f4.x3=scrfrontpoints[3];
		DrawPrim(&f4);
	}
	if (((short*)&scrfrontpoints[0])[0]>160) {	//draw left
		f4.r0=128; f4.g0=64; f4.b0=64;
		RotTransPers(&pvertex[4],(long*)&f4.x0,&dmy,&flg); RotTransPers(&pvertex[6],(long*)&f4.x1,&dmy,&flg);
		*(long*)&f4.x2=scrfrontpoints[0]; *(long*)&f4.x3=scrfrontpoints[2];
		DrawPrim(&f4);
	} else if (((short*)&scrfrontpoints[1])[0]<160) {	//draw right
		f4.r0=144; f4.g0=80; f4.b0=80;
		RotTransPers(&pvertex[5],(long*)&f4.x0,&dmy,&flg); RotTransPers(&pvertex[7],(long*)&f4.x1,&dmy,&flg);
		*(long*)&f4.x2=scrfrontpoints[1]; *(long*)&f4.x3=scrfrontpoints[3];
		DrawPrim(&f4);
	}
	f4.r0=255; f4.g0=160; f4.b0=160;
	*(long*)&f4.x0=scrfrontpoints[0]; *(long*)&f4.x1=scrfrontpoints[1]; *(long*)&f4.x2=scrfrontpoints[2]; *(long*)&f4.x3=scrfrontpoints[3];
	DrawPrim(&f4);
	
	//Collision
	if (flappyPos.vx>x1 && flappyPos.vx<x2) {
		short diff=flappyPos.vy-y1;
		if (diff>=-7 && diff<-3) {	//top
			short nextmy = (feo->data[1]) ? rsin((1+frame<<feo->data[2])+(feo->data[3]<<4))/feo->data[1] : 0;	//animation desp y
			short nextmx = (feo->data[4]) ? rsin((1+frame<<feo->data[5])+(feo->data[6]<<4))/feo->data[4] : 0;	//animation desp x
			fflags|=4;
			flappyPos.vy=feo->dy-11+nextmy;
			flappyPos.vx+=(nextmx-mx);
		}
		else if (diff>=-3 && diff<13) {	//bottom
			short nextmy=rsin((frame+1)<<4)/70;
			fflags|=8;
			return;
		}
	}
}

void pipe(struct FEObject* feo) {
	if (*feo->data) printFEModelInvertedY(model_pipe, feo->dx, feo->dy-32);
	else printFEModel(model_pipe, feo->dx, feo->dy);
	if (abs(feo->dx-flappyPos.vx+8)<15 && feo->dy-flappyPos.vy<40 && feo->dy-flappyPos.vy>-8) {
		if (fflags&2) {
			if ((feo->dx-flappyPos.vx)<-8) flappyPos.vx=feo->dx+23; else flappyPos.vx=feo->dx-7;
		} else {
			fflags|=8;
		}
	}
}

void button(struct FEObject* feo) {
	if (feo->data[0]) printFEModel(model_button2, feo->dx, feo->dy);
	else printFEModel(model_button1, feo->dx, feo->dy);
	if (abs(feo->dx-flappyPos.vx+8)<10 && feo->dy-flappyPos.vy<8 && feo->dy-flappyPos.vy>0) {
		if (!feo->data[0] && !feo->data[1]) {
			PLAYSFX(SFX_ABS1);
			feo->data[0]=feo->data[1]=1;
		}
	} else {
		feo->data[1]=0;
	}
}

void door(struct FEObject* feo) {
	if (feo->data[0]) printFEModel(model_door2, feo->dx, feo->dy);
	else {
		printFEModel(model_door1, feo->dx, feo->dy);
		if (abs(feo->dx-flappyPos.vx)<10 && (feo->dy-flappyPos.vy)<16 && (feo->dy-flappyPos.vy)>=0) {
			if (fflags&2) {
				if (feo->dx<flappyPos.vx) flappyPos.vx=feo->dx+10; else flappyPos.vx=feo->dx-10;
			} else {
				fflags|=8;
			}
		}
	}
}

void block(struct FEObject* feo) {
	u_char falling=0;
	printFEModel(model_block, feo->dx, feo->dy);
	if (!(feo->dx&15)) {	//Can fall?
		if (!b((feo->dy>>4)+32,(feo->dx>>4)+32)) {falling=1; if (!(fflags&32)) feo->dy++;}
	}
	if (abs(feo->dx-flappyPos.vx+8)<14 && (feo->dy-flappyPos.vy)<24 && (feo->dy-flappyPos.vy)>=0) {	//collision
		if (fflags&2 && (feo->dy-flappyPos.vy)<23) {		//on-ground collision
			if (feo->dx<flappyPos.vx) {
				flappyPos.vx=feo->dx+22;
				if (!falling && !b((feo->dy>>4)+31,(feo->dx>>4)+32)) feo->dx--;
			} else {
				flappyPos.vx=feo->dx-6;
				if (!falling && !b((feo->dy>>4)+31,(feo->dx>>4)+33)) feo->dx++;
			}
		} else {			//on-air collision
			if ((feo->dy-flappyPos.vy)<16) {			//down part collision
				fflags|=8;
			} else {		//up part collision
				fflags|=4;
				flappyPos.vy=feo->dy-23;
			}
		}
	}
}

void hotfloor(struct FEObject* feo) {
	SVECTOR vertex[4]={{feo->dx,feo->dy,0},{feo->dx+16,feo->dy,0},{feo->dx,feo->dy,persp+16},{feo->dx+16,feo->dy,persp+16}};
	short val=rsin((frame<<feo->data[0])+(feo->data[1]<<4))>>5;
	f4.r0=(val==128)?255:128+val; f4.g0=0; f4.b0=0;
	RotTransPers4(&vertex[0],&vertex[1],&vertex[2],&vertex[3],(long*)&f4.x0,(long*)&f4.x1,(long*)&f4.x2,(long*)&f4.x3,&dmy,&flg);
	if (f4.y0>=f4.y2) DrawPrim(&f4);
	if ((feo->dy-flappyPos.vy)<8 && (feo->dy-flappyPos.vy)>0 && (feo->dx-flappyPos.vx)<0 && (feo->dx-flappyPos.vx)>-16) {
		if (val>96) fflags|=8;
	}
}

void sign(struct FEObject* feo) {
	SVECTOR vertex[4]={{feo->dx-6,feo->dy-21,persp+14},{feo->dx-6,feo->dy-9,persp+14},{feo->dx+6,feo->dy-21,persp+8},{feo->dx+6,feo->dy-9,persp+8}};
	POLY_FT4 sigt={u0:70,v0:240,u1:70,v1:255,u2:85,v2:240,u3:85,v3:255,tpage:tpages[0]};
	switch (feo->data[0]) {
		case 0:
		RotTransPers4(&vertex[3],&vertex[1],&vertex[2],&vertex[0],(long*)&sigt.x0,(long*)&sigt.x1,(long*)&sigt.x2,(long*)&sigt.x3,&dmy,&flg);
		break;
		case 1: //Left
		RotTransPers4(&vertex[1],&vertex[3],&vertex[0],&vertex[2],(long*)&sigt.x0,(long*)&sigt.x1,(long*)&sigt.x2,(long*)&sigt.x3,&dmy,&flg);
		break;
		case 2:	//Up
		RotTransPers4(&vertex[0],&vertex[1],&vertex[2],&vertex[3],(long*)&sigt.x0,(long*)&sigt.x1,(long*)&sigt.x2,(long*)&sigt.x3,&dmy,&flg);
		break;
		case 3:	//Down
		RotTransPers4(&vertex[1],&vertex[0],&vertex[3],&vertex[2],(long*)&sigt.x0,(long*)&sigt.x1,(long*)&sigt.x2,(long*)&sigt.x3,&dmy,&flg);
		break;
	}
	printFEModel(model_sign, feo->dx, feo->dy);
	SetPolyFT4(&sigt); setShadeTex(&sigt,1); DrawPrim(&sigt);
}

void memcard(struct FEObject* feo) {
	static MATRIX m;
	static u_char timein=0,saved=0;
	static char* msg;
	SVECTOR mcardang={frame<<4,frame<<4,frame<<4};	//angle (512=45d)
	VECTOR mcardpos={pos.vx+feo->dx,pos.vy+feo->dy,persp+32};
	RotMatrix(&mcardang, &m); //Get a rotation matrix from the vector
	TransMatrix(&m, &mcardpos);	//Sets the translation
	SetRotMatrix(&m);
	SetTransMatrix(&m);
	printFEModel(model_memcard, 0, 0);
	RotMatrix(&ang, &m); //Get a rotation matrix from the vector
	TransMatrix(&m, &pos);	//Sets the translation
	SetRotMatrix(&m);
	SetTransMatrix(&m);
	if (saved==1) {
		msg=saveMemCard();
		VSync(0);
		PadStartCom();
		saved++;
	}
	if ((feo->dy-flappyPos.vy)<32 && (feo->dy-flappyPos.vy)>-24 && (feo->dx-flappyPos.vx)<24 && (feo->dx-flappyPos.vx)>-32) {	//collision
		TILE tile={x0:8,y0:8,w:timein*11,h:timein<<1,r0:timein<<3,g0:timein<<3,b0:timein<<2};
		SetTile(&tile); SetSemiTrans(&tile,1);
		DrawPrim(&tp[2]);
		DrawPrim(&tile);
		DrawPrim(&tp[0]);
		if (timein<14) timein++;
		if (!gamePad[0].start && !saved) {
			saved=1;
			PadStopCom();
			print(10,10,timein<<3,timein<<3,timein<<3,1,"saving...");
		} else {
			print(10,10,timein<<3,timein<<3,timein<<3,1,msg);
		}
	} else {
		timein=0; saved=0;
		msg="save game\n\n             >START";
	}
}

void lvlplatf(struct FEObject* feo) {
	static u_char timeinarr[6]={0};	//6 levels
	static char *levelNames[]={"Tedelche","The shortcut\n(Hellin)","Ice world\n(Riopar)","Feria GrandPrix\n(Albacete)",
			"Angry Birds\n(Ontur)","not implemented\n","not implemented\n"};	//6 levels
	static u_char totalCoins[]={0,89,81,9,31,0,0};	//6 levels
	printFEModel(model_lvlplatf, feo->dx, feo->dy);
	if (abs(feo->dx-flappyPos.vx)<12 && (feo->dy-flappyPos.vy)<12) {	//collision
		u_char timein=timeinarr[feo->data[0]];
		TILE tile={x0:8,y0:8,w:timein<<3,h:timein<<2,r0:timein<<3,g0:timein<<1,b0:timein<<3};
		SetTile(&tile); SetSemiTrans(&tile,1);
		DrawPrim(&tp[2]);
		DrawPrim(&tile);
		DrawPrim(&tp[0]);
		PRINTFMT(12,12,timein<<3,timein<<3,timein<<3,1,"LEVEL %d:\n\n%s\n\n%d/%d\n         >START",feo->data[0],levelNames[feo->data[0]],mCardData.saveData[feo->data[0]],totalCoins[feo->data[0]]);
		if (timein<16) timeinarr[feo->data[0]]++;
		fflags|=4;
		flappyPos.vy=feo->dy-11;
		if (!gamePad[0].start) levelExitCode=feo->data[0];
	} else {
		timeinarr[feo->data[0]]=0;
	}
}

void angrybird(struct FEObject* feo) {
	SPRT agb={w:44, h:40, u0:(feo->data[0])?168:212, v0:216};
	SVECTOR v={feo->dx,feo->dy,persp+4};
	short dify=feo->dy-flappyPos.vy;
	RotTransPers(&v,(long*)&agb.x0,&dmy,&flg);
	agb.x0-=22; agb.y0-=40;
	SetSprt(&agb); setShadeTex(&agb,1);
	DrawPrim(&agb);
	if (dify>0 && dify<32 && abs(feo->dx-flappyPos.vx)<(32-dify>>1)) fflags|=8;		//collision (triangle)
	if (frame&feo->data[1] && !(fflags&40)) {																	//Move
		if (feo->data[0]) {
			feo->dx--;
			if (b((feo->dy>>4)+31,(feo->dx-12>>4)+32)) feo->data[0]=0;
		} else {
			feo->dx++;
			if (b((feo->dy>>4)+31,(feo->dx+12>>4)+32)) feo->data[0]=1;
		}
	}
}

typedef void (*FEFunc)(struct FEObject*);

FEFunc FEFuncArray[12]= {&alert,&yflag,&platform,&pipe,&button,&door,&block,&hotfloor,&sign,&memcard,&lvlplatf,&angrybird};