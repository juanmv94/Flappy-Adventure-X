#include <sys/types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include "FEPSX.h"

static void read_pad(SVECTOR* ang, VECTOR* pos) {
	u_long pad = PadRead(1);
	if(pad & PADRleft) ang->vy+=8;
	if(pad & PADRright) ang->vy-=8;
	if(pad & PADRup) ang->vx+=8;
	if(pad & PADRdown) ang->vx-=8;
	//if(pad & PADL1) ang->vz-=8;
	//if(pad & PADR1) ang->vz+=8;
	if(pad & PADLup) {pos->vz-=rcos(ang->vy)>>9; pos->vx+=rsin(ang->vy)>>9;}
	if(pad & PADLdown) {pos->vz+=rcos(ang->vy)>>9; pos->vx-=rsin(ang->vy)>>9;}
	if(pad & PADLleft) {pos->vz+=rsin(ang->vy)>>9; pos->vx+=rcos(ang->vy)>>9;}
	if(pad & PADLright) {pos->vz-=rsin(ang->vy)>>9; pos->vx-=rcos(ang->vy)>>9;}
	if(pad & PADL2) pos->vy-=4;
	if(pad & PADR2) pos->vy+=4;
}

int main() {
	DRAWENV	draw;	//Drawing environment
	DISPENV	disp;	//Display environment
	//u_long ot;		//Ordering table
	RECT rect={0,0,320,240};
	POLY_F4	f4; POLY_F3 f3;
	
	SVECTOR ang={64,0,0};	//angle (512=45?)
	VECTOR pos = {0, 128, 256};	//translation vector (position)
	MATRIX m; //work matrix
	long dmy,flg;	/* Work for RotTransPers */

	PadInit(0);	//Initialise the 1st controller
	InitGeom();	//initialise geometry subsystem
	SetGeomOffset(160, 120);	//geom screen center
	SetGeomScreen(256);			//distance from viewpoint to screen
	ResetGraph(0);		//Start graphics
    SetVideoMode(1); //PAL MODE
	SetDefDrawEnv(&draw, 0,   0, 320, 240);
	PutDrawEnv(&draw);
	SetDefDispEnv(&disp, 0, 0, 320, 240);
	PutDispEnv(&disp);
	SetDispMask(1);
	
	SetPolyF4(&f4); SetPolyF3(&f3);
	FntLoad(960, 256);	
	FntOpen(8, 8, 320, 64, 0, 512);

	while(1) {
		u_char* p=FEPSX;
		u_char i,polyCount=*(p++);
		read_pad(&ang,&pos);		//modify angle with joypad

		/* Geometry calculations */
		RotMatrix(&ang, &m); //Get a rotation matrix from the vector
		TransMatrix(&m, &pos);	//Sets the translation
		SetRotMatrix(&m);
		SetTransMatrix(&m);

		ClearImage(&rect,100,100,255);
		DrawSync(0);
		
		for (i=0;i<polyCount;i++) {
			long d;
			u_char flags=*(p++);
			if (flags&0x01) {	//quad
				SVECTOR x[4];
				//memcpy(&f4.r0,p,3);
				memcpy(&x[1],p+3,6);memcpy(&x[0],p+9,6);memcpy(&x[2],p+15,6);memcpy(&x[3],p+21,6);
				if (((d=TransRotPers(&x[0],(long*)&f4.x0,&dmy,&flg)) > 16) + (TransRotPers(&x[1],(long*)&f4.x1,&dmy,&flg) > 16) +
					(TransRotPers(&x[2],(long*)&f4.x2,&dmy,&flg) > 16) + (TransRotPers(&x[3],(long*)&f4.x3,&dmy,&flg) > 16))
					if (flags&0x02 || NormalClip(*(long*)&f4.x0,*(long*)&f4.x1,*(long*)&f4.x2)>0) {
						long c;
						c=(d>>2)+p[0]; f4.r0=(c>255)?255:c;
						c=(d>>2)+p[1]; f4.g0=(c>255)?255:c;
						c=(d>>2)+p[2]; f4.b0=(c>255)?255:c;
						SetSemiTrans(&f4,d>768);
						DrawPrim(&f4);
					}
				p+=27;
			} else {	//triangle
				SVECTOR x[3];
				//memcpy(&f3.r0,p,3);
				memcpy(&x[1],p+3,6);memcpy(&x[0],p+9,6);memcpy(&x[2],p+15,6);
				if (((d=TransRotPers(&x[0],(long*)&f3.x0,&dmy,&flg)) > 16) + (TransRotPers(&x[1],(long*)&f3.x1,&dmy,&flg) > 16) +
					(TransRotPers(&x[2],(long*)&f3.x2,&dmy,&flg) > 16))
					if (flags&0x02 || NormalClip(*(long*)&f3.x0,*(long*)&f3.x1,*(long*)&f3.x2)>0) {
						long c;
						c=(d>>2)+p[0]; f3.r0=(c>255)?255:c;
						c=(d>>2)+p[1]; f3.g0=(c>255)?255:c;
						c=(d>>2)+p[2]; f3.b0=(c>255)?255:c;
						SetSemiTrans(&f3,d>768);
						DrawPrim(&f3);
					}
				p+=21;
			}
		}
		FntPrint("Flappy Engine PSX v0.2 - Juanmv94\n\n");
		FntPrint("ang: %03d', %03d', %03d'\n\n",((ang.vx&4095)*45)>>9,((ang.vy&4095)*45)>>9,((ang.vz&4095)*45)>>9);
		FntPrint("pos: x:%06d, y:%06d, z:%06d\n\n",pos.vx,pos.vy,pos.vz);
		FntPrint("System load:%d",VSync(1));
        FntFlush(-1);
		DrawSync(0);
		VSync(0);
    }
	return 0;
}