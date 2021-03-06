int abs(int n) {
	return (n<0)?-n:n;
}

void nextFrameFlipBuff() {
	cdb=(cdb==db)?db+1:db;
	VSync(0);
	PutDrawEnv(&cdb->draw);
	PutDispEnv(&cdb->disp);
}
void loadTim(long tim) {
	int palsize;
	if (*(long*)tim!=16) return;	//Not a TIM file
	LoadImage((RECT*)(tim+12), (u_long*)(tim+20));
	if (*(long*)(tim+4)&8) {		//Has palette?
		palsize=(*(short*)(tim+16)) * (*(short*)(tim+18)) *2;
		LoadImage((RECT*)(tim+palsize+24), (u_long*)(tim+palsize+32));
	}
}

void loadTimCD(char* file) {
	u_char* tim=readFromCD(file);
	if (tim) {
		loadTim((long)tim);
		DrawSync(0);
		free(tim);
	}
}

void introScreen(u_char num) {
	static char* filename="\\INTRO$.TIM;1";
	RECT introRect={x:320,y:272,w:320,h:240};
	u_char i;
	filename[6]=num+'0';
	loadTimCD(filename);
	PLAYSFX(SFX_ABS1);
	for (i=63;i>0;i--) {
		DrawPrim(&tp[0]);
		MoveImage(&introRect,0,cdb->draw.clip.y);
		fullScreenBlack.r0=i<<2; fullScreenBlack.g0=i<<2; fullScreenBlack.b0=i<<2;
		DrawPrim(&fullScreenBlack);
		nextFrameFlipBuff();
		if (!gamePad[0].start) break;
	}
	for (i=0;i<64;i++) {
		MoveImage(&introRect,0,cdb->draw.clip.y);
		DrawSync(0);
		nextFrameFlipBuff();
	}
}

void print(u_short dx, u_short dy, u_char r0, u_char g0, u_char b0, u_char st, char *text) {
	u_short x=0,y=0;
	charSprt.r0=r0; charSprt.g0=g0; charSprt.b0=b0;
	SetSemiTrans(&charSprt,st);
	while(*text!=0) {
		if (*text=='\n') {x=0; y+=8;} else {
			char c=*text-0x20;
			charSprt.u0=(c&31)<<3; charSprt.v0=174+((c>>5)<<3);
			charSprt.x0=x+dx; charSprt.y0=y+dy;
			DrawPrim(&charSprt);
			x+=8;
		}
		text++;
	}
}

#define PRINTFMT(DX, DY, R0, G0, B0, ST, ARGS...) \
{\
	char stringholder[128];\
	sprintf(stringholder, ARGS);\
	print(DX,DY,R0,G0,B0,ST,stringholder);\
} 

void printNum(u_short dx, u_short dy,u_short num) {
	numSprt.x0=dx; numSprt.y0=dy;
	if (num==0) {
		numSprt.u0=136;
		DrawPrim(&numSprt);
	} else while(num!=0) {
		u_short nl=num%10;
		numSprt.u0=136+12*nl;
		DrawPrim(&numSprt);
		num/=10;
		numSprt.x0-=13;
	}
}

void printCd(u_char cdn) {
	static SVECTOR cdvert[4]={{-256,0,256},{-256,0,-256},{256,0,256},{256,0,-256}};
	POLY_FT4 *ccdp=&cdp[cdn];
	static MATRIX m;
	SVECTOR cdang={1024,VSync(-1)<<5,0};
	VECTOR cdpos={360,200,persp+500};
	RotMatrix(&cdang, &m); //Get a rotation matrix from the vector
	TransMatrix(&m, &cdpos);	//Sets the translation
	SetRotMatrix(&m);
	SetTransMatrix(&m);
	RotTransPers4(&cdvert[0],&cdvert[1],&cdvert[2],&cdvert[3],(long*)&ccdp->x0,(long*)&ccdp->x1,(long*)&ccdp->x2,(long*)&ccdp->x3,&dmy,&flg);
	DrawPrim(ccdp);
	RotMatrix(&ang, &m); //Get a rotation matrix from the vector
	TransMatrix(&m, &pos);	//Sets the translation
	SetRotMatrix(&m);
	SetTransMatrix(&m);
}

void printFEModel(u_char *p, short dx, short dy) {
	u_char i,j,polyCount=*(p++);
	for (i=0;i<polyCount;i++) {
		u_char flags=*(p++);
		if (flags&0x01) {	//quad
			SVECTOR x[4];
			memcpy(&f4.r0,p,3);
			memcpy(&x[1],p+3,6);memcpy(&x[0],p+9,6);memcpy(&x[2],p+15,6);memcpy(&x[3],p+21,6);
			for (j=0;j<4;j++) {x[j].vx+=dx; x[j].vy+=dy; x[j].vz+=persp;}
			RotTransPers4(&x[0],&x[1],&x[2],&x[3],(long*)&f4.x0,(long*)&f4.x1,(long*)&f4.x2,(long*)&f4.x3,&dmy,&flg);
			if (flags&0x02 || NormalClip(*(long*)&f4.x0,*(long*)&f4.x1,*(long*)&f4.x2)>=0) DrawPrim(&f4);
			p+=27;
		} else {	//triangle
			SVECTOR x[3];
			memcpy(&f3.r0,p,3);
			memcpy(&x[1],p+3,6);memcpy(&x[0],p+9,6);memcpy(&x[2],p+15,6);
			for (j=0;j<3;j++) {x[j].vx+=dx; x[j].vy+=dy; x[j].vz+=persp;}
			RotTransPers3(&x[0],&x[1],&x[2],(long*)&f3.x0,(long*)&f3.x1,(long*)&f3.x2,&dmy,&flg);
			if (flags&0x02 || NormalClip(*(long*)&f3.x0,*(long*)&f3.x1,*(long*)&f3.x2)>=0) DrawPrim(&f3);
			p+=21;
		}
	}
}

void printFEModelInvertedY(u_char *p, short dx, short dy) {
	u_char i,j,polyCount=*(p++);
	for (i=0;i<polyCount;i++) {
		u_char flags=*(p++);
		if (flags&0x01) {	//quad
			SVECTOR x[4];
			memcpy(&f4.r0,p,3);
			memcpy(&x[1],p+3,6);memcpy(&x[0],p+9,6);memcpy(&x[2],p+15,6);memcpy(&x[3],p+21,6);
			for (j=0;j<4;j++) {x[j].vx+=dx; x[j].vy=dy-x[j].vy; x[j].vz+=persp;}
			RotTransPers4(&x[0],&x[1],&x[2],&x[3],(long*)&f4.x0,(long*)&f4.x1,(long*)&f4.x2,(long*)&f4.x3,&dmy,&flg);
			if (flags&0x02 || NormalClip(*(long*)&f4.x0,*(long*)&f4.x2,*(long*)&f4.x1)>=0) DrawPrim(&f4);
			p+=27;
		} else {	//triangle
			SVECTOR x[3];
			memcpy(&f3.r0,p,3);
			memcpy(&x[1],p+3,6);memcpy(&x[0],p+9,6);memcpy(&x[2],p+15,6);
			for (j=0;j<3;j++) {x[j].vx+=dx; x[j].vy=dy-x[j].vy; x[j].vz+=persp;}
			RotTransPers3(&x[0],&x[1],&x[2],(long*)&f3.x0,(long*)&f3.x1,(long*)&f3.x2,&dmy,&flg);
			if (flags&0x02 || NormalClip(*(long*)&f3.x0,*(long*)&f3.x2,*(long*)&f3.x1)>=0) DrawPrim(&f3);
			p+=21;
		}
	}
}