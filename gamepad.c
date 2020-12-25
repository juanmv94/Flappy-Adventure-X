#include <libpad.h>

struct s_gamePad {
  u_char status:8;

  u_char recvSize:4;
  u_char type:4;
  
  u_char select:1;
  u_char na2:1;
  u_char na1:1;
  u_char start:1;
  u_char up:1;
  u_char right:1;
  u_char down:1;
  u_char left:1;

  u_char l2:1;
  u_char r2:1;
  u_char l1:1;
  u_char r1:1;
  u_char triangle:1;
  u_char circle:1;
  u_char ex:1;
  u_char square:1;
  
  u_char an1h;
  u_char an1v;
  u_char an2h;
  u_char an2v;
  
  u_char padding[26];
};

struct s_gamePad gamePad[2];

u_char gamePadLeft(u_char pad) {
	if (!gamePad[pad].left) return 1;
	return ((gamePad[pad].type==5 || gamePad[pad].type==7) && gamePad[pad].an2h<64);
}

u_char gamePadRight(u_char pad) {
	if (!gamePad[pad].right) return 1;
	return ((gamePad[pad].type==5 || gamePad[pad].type==7) && gamePad[pad].an2h>192);
}

u_char gamePadUp(u_char pad) {
	if (!gamePad[pad].up) return 1;
	return ((gamePad[pad].type==5 || gamePad[pad].type==7) && gamePad[pad].an2v<64);
}

u_char gamePadDown(u_char pad) {
	if (!gamePad[pad].down) return 1;
	return ((gamePad[pad].type==5 || gamePad[pad].type==7) && gamePad[pad].an2v>192);
}