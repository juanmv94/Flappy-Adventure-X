#include "models.h"

void alert(struct FEObject* feo) {
	printFEModel(model_alert, feo->dx, feo->dy);
	if ((!(fflags&8)) && (!feo->data[0]) && abs(feo->dx-flappyPos.vx)<10 && abs(feo->dy-flappyPos.vy)<12) {
		feo->data[0]=1;
		alertmsg=&feo->data[1];
	}
}

void yflag(struct FEObject* feo) {
	static u_char wasPreviously=0;
	printFEModel(model_yflag, feo->dx, feo->dy);
	if (abs(feo->dx-flappyPos.vx)<10 && (feo->dy-flappyPos.vy)<32 && (feo->dy-flappyPos.vy)>0) {
		if (!wasPreviously && (feo->dx-flappyPos.vx)) {
			PLAYSFX(SFX_CHCK);
			memcpy(lastCoinCollected,coinCollected,ncoins);
			lastRemCoins=remCoins;
			spawnpoint[0]=feo->dx>>4; spawnpoint[1]=(feo->dy>>4)-1;
		}
		wasPreviously=1;
	} else {
		wasPreviously=0;
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

typedef void (*FEFunc)(struct FEObject*);

FEFunc FEFuncArray[7]= {&alert,&yflag,&platform,&pipe,&button,&door,&block};