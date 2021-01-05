#include <sys/file.h>
#include <libmcrd.h>

#define ICON_NOICON 0x00
#define ICON_1FRAME 0x11
#define ICON_2FRAME 0x12
#define ICON_3FRAME 0x13

struct MCHeader {
	char magic[2];
	char iconFlag;
	char blockNumber;
	char title[64];
	char reserved1[12];
	char PocketStationFrames1[2];
	char PocketStationId[4];
	char PocketStationFrames2[2];
	char reserved2[8];
	char CLUT[32];	//16 color
	//128 bytes to here
	char icons[128*3];	//3x(16x16 4BPP)
	char saveData[128*60];
};

struct MCHeader mCardData={
		magic:{53,43},	//"SC" magic
		iconFlag: ICON_3FRAME,
		blockNumber: 1,
		title: "FLAPPY ADVENTURE X",
		CLUT: {0x00, 0x00, 0x1C, 0x16, 0x5F, 0x47, 0xFF, 0x00, 0x9A, 0x67, 0xEA, 0x20, 0xDF, 0x1A, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		icons: {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x55, 0x55, 0x05, 0x00, 0x00, 0x00, 0x50, 0x25, 0x22, 0x75, 0x57, 0x00, 0x00, 0x00, 0x25, 0x62, 0x56, 0x77, 0x77, 0x05, 0x00, 0x55, 0x55, 0x66, 0x56, 0x74, 0x57, 0x57, 0x00, 0x77, 0x77, 0x65, 0x56, 0x74, 0x57, 0x57, 0x00, 0x77, 0x77, 0x57, 0x66, 0x45, 0x77, 0x57, 0x00, 
				0x72, 0x77, 0x52, 0x66, 0x56, 0x55, 0x55, 0x05, 0x25, 0x22, 0x65, 0x66, 0x35, 0x33, 0x33, 0x53, 0x50, 0x55, 0x11, 0x51, 0x53, 0x55, 0x55, 0x05, 0x50, 0x11, 0x11, 0x11, 0x35, 0x33, 0x33, 0x05, 0x00, 0x55, 0x11, 0x11, 0x51, 0x55, 0x55, 0x00, 0x00, 0x00, 0x55, 0x55, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x55, 0x55, 0x05, 0x00, 0x00, 0x00, 0x50, 0x25, 0x22, 0x75, 0x57, 0x00, 0x00, 0x00, 0x25, 0x62, 0x56, 0x77, 0x77, 0x05, 0x00, 0x50, 0x62, 0x66, 0x56, 0x74, 0x57, 0x57, 0x00, 0x65, 0x66, 0x66, 0x56, 0x74, 0x57, 0x57, 0x00, 0x55, 0x55, 0x65, 0x66, 0x45, 0x77, 0x57, 0x00, 
				0x77, 0x77, 0x57, 0x66, 0x56, 0x55, 0x55, 0x05, 0x72, 0x77, 0x52, 0x66, 0x35, 0x33, 0x33, 0x53, 0x55, 0x55, 0x15, 0x51, 0x53, 0x55, 0x55, 0x05, 0x50, 0x11, 0x11, 0x11, 0x35, 0x33, 0x33, 0x05, 0x00, 0x55, 0x11, 0x11, 0x51, 0x55, 0x55, 0x00, 0x00, 0x00, 0x55, 0x55, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x55, 0x55, 0x05, 0x00, 0x00, 0x00, 0x50, 0x25, 0x22, 0x75, 0x57, 0x00, 0x00, 0x00, 0x25, 0x62, 0x56, 0x77, 0x77, 0x05, 0x00, 0x50, 0x62, 0x66, 0x56, 0x74, 0x57, 0x57, 0x00, 0x65, 0x66, 0x66, 0x56, 0x74, 0x57, 0x57, 0x00, 0x65, 0x66, 0x66, 0x66, 0x45, 0x77, 0x57, 0x00, 
				0x55, 0x55, 0x65, 0x66, 0x56, 0x55, 0x55, 0x05, 0x72, 0x77, 0x52, 0x66, 0x35, 0x33, 0x33, 0x53, 0x77, 0x77, 0x15, 0x51, 0x53, 0x55, 0x55, 0x05, 0x77, 0x52, 0x11, 0x11, 0x35, 0x33, 0x33, 0x05, 0x55, 0x55, 0x11, 0x11, 0x51, 0x55, 0x55, 0x00, 0x00, 0x00, 0x55, 0x55, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		},
		saveData: "TODO"
	};
	
void loadMemCard() {
	long cmds;
	long result;
	MemCardInit(0);
	MemCardStart();
	MemCardSync(0, &cmds, &result);
	result=MemCardCreateFile(0,"FLADVX",1);
	switch (result) {
		case McErrNone: printf("file created\n"); break;
		case McErrCardNotExist: printf("card not connected\n"); break;
		case McErrCardInvalid: printf("communication error\n"); break;
		case McErrNotFormat: printf("card not formatted\n"); break;
		case McErrAlreadyExist: printf("file already exists\n"); break;
		case McErrBlockFull: printf("memcard is full\n"); break;
		default: printf("unknown error\n");
	}
	if (result==McErrNone) {
		MemCardWriteFile(0,"FLADVX",(u_long*)&mCardData,0,128*4);
		MemCardSync(0, &cmds, &result);
		switch (result) {
			case McErrNone: printf("write card OK\n"); break;
			case McErrCardNotExist: printf("card not connected\n"); break;
			case McErrCardInvalid: printf("communication error\n"); break;
			case McErrNewCard: printf("new card (card swapped)\n"); break;
			case McErrFileNotExist: printf("file not found\n"); break;
			default: printf("unknown error\n");
		}
	}
	MemCardStop();
}