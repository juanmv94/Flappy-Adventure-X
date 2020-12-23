#define IS_RGB24	1	// 0:16-bit playback, 1:24-bit playback (recommended for quality)
#define RING_SIZE	32	// Ring Buffer size (32 sectors seems good enough)

#if IS_RGB24==1
	#define PPW			3/2	// pixels per short word
	#define DCT_MODE	3	// Decode mode for DecDCTin routine
#else
	#define PPW			1
	#define DCT_MODE    2
#endif

typedef struct {				//Decode environment
	u_long	*VlcBuff_ptr[2];	//Pointers to the VLC buffers
	u_short	*ImgBuff_ptr[2];	//Pointers to the frame slice buffers
	RECT	rect[2];			//VRAM parameters on where to draw the frame data to
	RECT	slice;				//Frame slice parameters for loading into VRAM
	int		VlcID;				//Current VLC buffer ID
	int		ImgID;				//Current slice buffer ID
	int 	RectID;				//Current video buffer ID
	int		FrameDone;			//Frame decode completion flag
} STRENV;

static STRENV strEnv;
static int strScreenWidth,strScreenHeight,strNumFrames;
static char* filename;
static int strPlayDone=0;	//Playback completion flag

static void strCallback() {	//Callback routine which is called whenever a slice has finished decoding. All it does is transfer the decoded slice into VRAM.
	RECT TransferRect = strEnv.slice;
	int id = strEnv.ImgID;
	
	#if IS_RGB24==1		//In 24-bit color, StCdInterrupt must be called in every callback
	extern StCdIntrFlag;
	if (StCdIntrFlag) {
		StCdInterrupt();
		StCdIntrFlag = 0;
	}
	#endif
	
	strEnv.ImgID = strEnv.ImgID? 0:1;	// Switch slice buffers
	strEnv.slice.x += strEnv.slice.w;	// Step to next slice
	if (strEnv.slice.x < strEnv.rect[strEnv.RectID].x + strEnv.rect[strEnv.RectID].w) {	//Frame not yet decoded completely?
		DecDCTout(strEnv.ImgBuff_ptr[strEnv.ImgID], strEnv.slice.w*strEnv.slice.h/2);	// Prepare for next slice
	} else { // Frame has been decoded completely
		strEnv.FrameDone = 1;					//Set the FrameDone flag
		strEnv.RectID = strEnv.RectID? 0: 1;	// Switch display buffers
		strEnv.slice.x = strEnv.rect[strEnv.RectID].x;
		strEnv.slice.y = strEnv.rect[strEnv.RectID].y;
	}
	LoadImage(&TransferRect, (u_long *)strEnv.ImgBuff_ptr[id]);		//Transfer the slice into VRAM
}

static void strSync() {		//Waits for the frame to finish decoding.
	u_long cnt = WAIT_TIME;
    while (strEnv.FrameDone == 0) {
        if (--cnt == 0) { //Timeout handler. If a timeout occurs, force switching buffers (not sure why)
            strEnv.FrameDone = 1;
            strEnv.RectID = strEnv.RectID? 0: 1;
            strEnv.slice.x = strEnv.rect[strEnv.RectID].x;
            strEnv.slice.y = strEnv.rect[strEnv.RectID].y;
        }
    }
    strEnv.FrameDone = 0;
}

static u_long* strNext() {	//Grabs a frame of video from the stream.
	u_long		*addr;
	StHEADER	*sector;
	int			cnt = WAIT_TIME;

	while (StGetNext((u_long **)&addr,(u_long **)&sector)) if (--cnt == 0) return(0);	//Grab a frame, with timeout handler
	if (sector->frameCount >= strNumFrames) strPlayDone = 1;	//frame's number has reached number of frames the video has
	if (strScreenWidth != sector->width || strScreenHeight != sector->height) printf("Wrong res: %dx%d",sector->width,sector->height);
	return addr;
}

static void strNextVlc() {	//Performs VLC decoding and grabs a frame from the stream.
	int		cnt=WAIT_TIME;
	u_long* next;

	while ((next = strNext()) == 0) if (--cnt == 0) return;		//Grab a frame from the stream. Return if timeout.
	strEnv.VlcID = strEnv.VlcID? 0: 1;							//Switch VLC buffers
	DecDCTvlc(next, strEnv.VlcBuff_ptr[strEnv.VlcID]);			//Decode the VLC
	StFreeRing(next);											//Free the ring buffer
}

static void strKickCD(CdlLOC *loc) {	//Begins CD streaming
	u_char param=CdlModeSpeed;
	do {
		while (CdControl(CdlSetloc, (u_char *)loc, 0) == 0);		//Seek to the STR file to play. We do "while==0" because it can fail?
		while (CdControl(CdlSetmode, &param, 0) == 0);				//double speed
		VSync(3);  													//Wait for 3 screen cycles after changing drive speed
	} while (CdRead2(CdlModeStream|CdlModeSpeed|CdlModeRT) == 0);	//Start streaming. If it fails, try again
}

int PlayStr(int xres, int yres, char* filename, u_long numframes) {	//Main STR playback routine: -1:Playback fail 0:Playback interrupted. 1:Playback finished.
	int id;			// Display buffer ID
	DISPENV disp;	// Display environment
	CdlFILE file;	// File info of video file
	
	// Buffers initialized here so we won't waste too much memory for playing FMV. Need at least 192KB of free memory
	u_long	RingBuff[RING_SIZE*SECTOR_SIZE];	// Ring buffer
	u_long	VlcBuff[2][xres/2*yres];	// VLC buffers
	u_short	ImgBuff[2][16*PPW*yres];		// Frame 'slice' buffers
	
	strNumFrames=numframes;
	strScreenWidth=xres;
	strScreenHeight=yres;
	strPlayDone=0;
	
	if (CdSearchFile(&file, filename) == 0) return -1;
	SetDispMask(0); //Set display mask so we won't see garbage while the stream is being prepared
	
	// Setup the buffer pointers
	strEnv.VlcBuff_ptr[0] = &VlcBuff[0][0];
	strEnv.VlcBuff_ptr[1] = &VlcBuff[1][0];
	strEnv.VlcID     = 0;
	strEnv.ImgBuff_ptr[0] = &ImgBuff[0][0];
	strEnv.ImgBuff_ptr[1] = &ImgBuff[1][0];
	strEnv.ImgID     = 0;
	
	// Setup the display buffers on VRAM
	strEnv.rect[0].x = 0;	// First page
	strEnv.rect[0].y = 0;
	strEnv.rect[0].w = strScreenWidth*PPW;
	strEnv.rect[0].h = strScreenHeight;
	strEnv.rect[1].x = 0;	// Second page
	strEnv.rect[1].y = strScreenHeight;
	strEnv.rect[1].w = strScreenWidth*PPW;
	strEnv.rect[1].h = strScreenHeight;
	strEnv.RectID    = 0;
	
	// Set the parameters for uploading frame slices
	strEnv.slice.x = 0;
	strEnv.slice.y = 0;
	strEnv.slice.w = 16*PPW;
	strEnv.slice.h = strScreenHeight;
	strEnv.FrameDone = 0;
	
	DecDCTReset(0);								//Reset the MDEC
	DecDCToutCallback(strCallback); 			//Set callback routine
	StSetRing(RingBuff, RING_SIZE);				//Set ring buffer
	StSetStream(IS_RGB24, 1, 0xffffffff, 0, 0);	//Set streaming parameters
	strKickCD(&file.pos);						//Begin streaming
	strNextVlc();						//Load the first frame of video before entering main loop
	while (1) {
		DecDCTin(strEnv.VlcBuff_ptr[strEnv.VlcID], DCT_MODE);								//Decode the compressed frame data
		DecDCTout(strEnv.ImgBuff_ptr[strEnv.ImgID], strEnv.slice.w*strEnv.slice.h/2);		//Prepare to receive the decoded image data from the MDEC
		strNextVlc();	//Get the next frame
		strSync();	//Wait for the frame to finish decoding
				
		// Switch between the display buffers per frame
		id = strEnv.RectID? 0: 1;
		SetDefDispEnv(&disp, 0, strScreenHeight*id, strScreenWidth*PPW, strScreenHeight);
		
		// Set parameters for 24-bit color mode
		#if IS_RGB24 == 1
		disp.isrgb24 = IS_RGB24;
		disp.disp.w = disp.disp.w*2/3;
		#endif
		disp.screen.y=32;

		VSync(0);			// VSync to avoid screen tearing
		PutDispEnv(&disp);	// Apply the video parameters
		SetDispMask(1);		// Remove the display mask
		
		if(strPlayDone == 1) break;
		if(!gamePad[0].start) break;  //stop button pressed exit animation routine
	}
	DecDCToutCallback(0);	//Shutdown streaming
	StUnSetRing();
	CdControlB(CdlPause, 0, 0);
	while (gamePad[0].start);
	return strPlayDone;
}
