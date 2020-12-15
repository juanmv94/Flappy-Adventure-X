typedef void (*LvlFunc)();

struct LvlFuncs {
	LvlFunc onStart;
	LvlFunc onFrame;
};

void noLevFunc() {
	//Nothing here
}

//Level 0
void level0_onFrame() {
	static u_char buttonPressedFrameCounter=0;
	if (FEObjects[13].data[1]) buttonPressedFrameCounter=150;
	else if (buttonPressedFrameCounter) buttonPressedFrameCounter--;
	FEObjects[14].data[0]=FEObjects[13].data[0]=buttonPressedFrameCounter;	//open door if button pressed
	if (fflags&8 && remCoins!=lastRemCoins) {
		sprintf(stringholder,"You were %d coins left, \nbut since you died, you\nare now %d coins left",remCoins,lastRemCoins);
		alertmsg=stringholder;
	}
}

struct LvlFuncs levelCustomCode[1]= {
	{&noLevFunc,&level0_onFrame}
};