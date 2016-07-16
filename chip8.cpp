#include <iostream>
#include <string.h>
#include <time.h>
#include <map>
#include "chip8.h"

using namespace std;

#define X(a) ((a & 0x0F00) >> 8)
#define Y(a) ((a & 0x00F0) >> 4)

Chip8::Chip8() {
	cout << "Chip8 constructor" << endl;
	init();
	setOps();
}

Chip8::~Chip8() {
	cout << "Chip8 destructor" << endl;
}

void Chip8::loadFontset() {
	unsigned char fontset[80] =  {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	// Store the font data starting at memory location 0x0000
	for (int i=0; i<80; i++) {
		memory[i] = fontset[i];
	}
}

bool Chip8::timeToDraw() {
	bool result = drawFlag;
	drawFlag = false;
	return result;
}

void Chip8::loadGame(string title) {
	FILE *fp = fopen(title.c_str(), "rb");
	if (fp == NULL) return;

	fseek(fp, 0, SEEK_END);
	int nbytes = ftell(fp);
	rewind(fp);

	int cnt = fread(memory + 0x200, 1, nbytes, fp);
	cout << cnt << endl;
	fclose(fp);
}

void Chip8::emulateCycle() {
	static int tick=0;
	currentCount = SDL_GetPerformanceCounter();
	double result = currentCount - startCount;
	result *= 600; // 600 Hz
	result /= SDL_GetPerformanceFrequency();

	if (result < 1.0) return;

	startCount = currentCount;

	opcode = memory[pc] << 8 | memory[pc + 1];
	(this->*ops[(opcode & 0xF000) >> 12])(opcode);

	++tick %= 10; // 60 Hz
	if (tick == 0) {
		if (delay_timer > 0)
			delay_timer--;
	
		if (sound_timer > 0) {
			if (sound_timer == 1)
				cout << "Beep" << endl;
			sound_timer--;
		}
	}
}

void Chip8::init() {
	// Clear memory
	memset(memory, 0, sizeof(memory));

	// Clear V-registers
	memset(V, 0, sizeof(V));

	//Clear the stack
	memset(stack, 0, sizeof(stack));
	
	pc     = 0x200;	// Set Program counter to the start location
	opcode = 0;     // Reset current opcode
	I      = 0;     // Reset index register
	sp     = 0;     // Reset stack pointer
	
	drawFlag = false; // Set drawFlag to false

	// Clear keystate
	memset(keystate, 0, sizeof(keystate));

	// Clear the display
	memset(gfx, 0, sizeof(gfx));
		
	// Reset the timers
	delay_timer = 0;
	sound_timer = 0;
	
	// random seed
	srand(time(NULL));

	// Load Fontset in memory
	loadFontset();

	// Map keyboard
	// 
	// +---+---+---+---+       +---+---+---+---+ 
	// | 1 | 2 | 3 | 4 |       | 1 | 2 | 3 | C | 
	// +---+---+---+---+       +---+---+---+---+ 
	// | Q | W | E | R |       | 4 | 5 | 6 | D | 
	// +---+---+---+---+  ===> +---+---+---+---+ 
	// | A | S | D | F |       | 7 | 8 | 9 | E | 
	// +---+---+---+---+       +---+---+---+---+ 
	// | Z | X | C | V |       | A | 0 | B | F | 
	// +---+---+---+---+       +---+---+---+---+ 
	keymap[SDLK_1] = 0x1;	
	keymap[SDLK_2] = 0x2;	
	keymap[SDLK_3] = 0x3;	
	keymap[SDLK_4] = 0xC;	

	keymap[SDLK_q] = 0x4;	
	keymap[SDLK_w] = 0x5;	
	keymap[SDLK_e] = 0x6;	
	keymap[SDLK_r] = 0xD;	

	keymap[SDLK_a] = 0x7;	
	keymap[SDLK_s] = 0x8;	
	keymap[SDLK_d] = 0x9;	
	keymap[SDLK_f] = 0xE;	

	keymap[SDLK_z] = 0xA;	
	keymap[SDLK_x] = 0x0;	
	keymap[SDLK_c] = 0xB;	
	keymap[SDLK_v] = 0xF;	

	startCount = SDL_GetPerformanceCounter();
}

void Chip8::setKeystate(Uint32 sdl_keycode, unsigned char state) {
	if (keymap.count(sdl_keycode) == 1)
		keystate[keymap[sdl_keycode]] = state;
}

void Chip8::setOps() {
	for (int i=0; i<0x10; i++) {
		ops[i] = doNothing;
		ops8XY[i]= doNothing;
	}

	for (int i=0; i<0x100; i++) {
		ops00[i] = doNothing;
		opsEX[i]= doNothing;
		opsFX[i]= doNothing;
	}

	ops[0x0] = oc0000;
	ops[0x1] = oc1000;
	ops[0x2] = oc2000;
	ops[0x3] = oc3000;
	ops[0x4] = oc4000;
	ops[0x5] = oc5000;
	ops[0x6] = oc6000;
	ops[0x7] = oc7000;
	ops[0x8] = oc8000;
	ops[0x9] = oc9000;
	ops[0xA] = ocA000;
	ops[0xB] = ocB000;
	ops[0xC] = ocC000;
	ops[0xD] = ocD000;
	ops[0xE] = ocE000;
	ops[0xF] = ocF000;

	ops00[0xE0] = oc00E0;
	ops00[0xEE] = oc00EE;

	ops8XY[0x0] = oc8XY0;
	ops8XY[0x1] = oc8XY1;
	ops8XY[0x2] = oc8XY2; 
	ops8XY[0x3] = oc8XY3; 
	ops8XY[0x4] = oc8XY4; 
	ops8XY[0x5] = oc8XY5; 
	ops8XY[0x6] = oc8XY6; 
	ops8XY[0x7] = oc8XY7; 
	ops8XY[0xE] = oc8XYE; 

	opsEX[0x9E] = ocEX9E;
	opsEX[0xA1] = ocEXA1;

	opsFX[0x07] = ocFX07;
	opsFX[0x0A] = ocFX0A;
	opsFX[0x15] = ocFX15;
	opsFX[0x18] = ocFX18;
	opsFX[0x1E] = ocFX1E;
	opsFX[0x29] = ocFX29;
	opsFX[0x33] = ocFX33;
	opsFX[0x55] = ocFX55;
	opsFX[0x65] = ocFX65;
}

void Chip8::doNothing(unsigned short opcode) {
	printf("doNothing - opcode=0x%04X\n", opcode);
}

// 00** - Relay instructions.
void Chip8::oc0000(unsigned short opcode) {
	(this->*ops00[opcode & 0x00FF])(opcode);
}

// 00E0 - Clears the screen.
void Chip8::oc00E0(unsigned short opcode) {
	memset(gfx, 0, sizeof(gfx));
	drawFlag = true;
	pc += 2;
}

// 00EE - Returns from a subroutine.
void Chip8::oc00EE(unsigned short opcode) {
	pc = stack[--sp];
	pc += 2;
}

// 1NNN - Jumps to address NNN.
void Chip8::oc1000(unsigned short opcode) {
	pc = opcode & 0x0FFF;
}

// 2NNN - Calls subroutine at NNN.
void Chip8::oc2000(unsigned short opcode) {
	stack[sp++] = pc;
	pc = opcode & 0x0FFF;
}

// 3XNN - Skips the next instruction if VX equals NN.
void Chip8::oc3000(unsigned short opcode) {
	if (V[X(opcode)] == (opcode & 0x00FF))
		pc += 2;
	pc += 2;
}

// 4XNN - Skips the next instruction if VX doesn't equal NN.
void Chip8::oc4000(unsigned short opcode) {
	if (V[X(opcode)] != (opcode & 0x00FF))
		pc += 2;
	pc += 2;
}

// 5XY0 - Skips the next instruction if VX equals VY.
void Chip8::oc5000(unsigned short opcode) {
	if (V[X(opcode)] == V[Y(opcode)])
		pc += 2;
	pc += 2;
}

// 6XNN - Sets VX to NN.
void Chip8::oc6000(unsigned short opcode) {
	V[X(opcode)] = opcode & 0x00FF;
	pc += 2;
}

// 7XNN - Add NN to VX.
void Chip8::oc7000(unsigned short opcode) {
	V[X(opcode)] += opcode & 0x00FF;
	pc += 2;
}

// 8XY* - Relay instructions.
void Chip8::oc8000(unsigned short opcode) {
	(this->*ops8XY[opcode & 0x000F])(opcode);
}

// 8XY0 - Sets VX to the value of VY.
void Chip8::oc8XY0(unsigned short opcode) {
	V[X(opcode)] = V[Y(opcode)];
	pc += 2;
}

// 8XY1 - Sets VX to VX OR VY.
void Chip8::oc8XY1(unsigned short opcode) {
	V[X(opcode)] |= V[Y(opcode)];
	pc += 2;
}

// 8XY2 - Sets VX to VX AND VY.
void Chip8::oc8XY2(unsigned short opcode) {
	V[X(opcode)] &= V[Y(opcode)];
	pc += 2;
}

// 8XY3 - Sets VX to VX XOR VY.
void Chip8::oc8XY3(unsigned short opcode) {
	V[X(opcode)] ^= V[Y(opcode)];
	pc += 2;
}

// 8XY4 - Adds VY to VX. VF is set to 1 when there's a carry, 
// and to 0 when there isn't.
void Chip8::oc8XY4(unsigned short opcode) {
	if (V[Y(opcode)] > (0xFF - V[X(opcode)]))
		V[0xF] = 1;
	else
		V[0xF] = 0;
	V[X(opcode)] += V[Y(opcode)];
	pc += 2;
}

// 8XY5 - VY is subtracted from VX. VF is set to 0 when there's 
// a borrow, and 1 when there isn't.
void Chip8::oc8XY5(unsigned short opcode) {
	if (V[Y(opcode)] > V[X(opcode)])
		V[0xF] = 0;
	else
		V[0xF] = 1;
	V[X(opcode)] -= V[Y(opcode)];
	pc += 2;
}

// 8XY6 - Shifts VX right by one. VF is set to the value of the
// least significant bit of VX before the shift.
// On the original interpreter, the value of VY is shifted, and
// the result is stored into VX. On current implementations, 
// Y is ignored.
void Chip8::oc8XY6(unsigned short opcode) {
	V[0xF] = V[X(opcode)] & 0x01;
	V[X(opcode)] >>= 1;
	pc += 2;
}

// 8XY7 - Sets VX to VY minus VX. VF is set to 0 when there's
// a borrow, and 1 when there isn't.
void Chip8::oc8XY7(unsigned short opcode) {
	if (V[X(opcode)] > V[Y(opcode)])
		V[0xF] = 0;
	else
		V[0xF] = 1;
	V[X(opcode)] = V[Y(opcode)] - V[X(opcode)];
	pc += 2;
}

// 8XYE - Shifts VX left by one. VF is set to the value of the
// most significant bit of VX before the shift.
// On the original interpreter, the value of VY is shifted, and
// the result is stored into VX. On current implementations, 
// Y is ignored.
void Chip8::oc8XYE(unsigned short opcode) {
	V[0xF] = V[X(opcode)] >> 7;
	V[X(opcode)] <<= 1;
	pc += 2;
}

// 9XY0 - Skips the instruction if VX doesn't equal VY.
void Chip8::oc9000(unsigned short opcode) {
	if (V[X(opcode)] != V[Y(opcode)])
		pc += 2;
	pc += 2;
}

// ANNN - Sets I to address NNN.
void Chip8::ocA000(unsigned short opcode) {
	I = opcode & 0x0FFF;
	pc += 2;
}

// BNNN - Jumps to the address NNN plus V0.
void Chip8::ocB000(unsigned short opcode) {
	pc = (opcode & 0x0FFF) + V[0x0];
}

// CXNN - Sets VX to the result of a bitwise AND operation on a 
// random number and NN.
void Chip8::ocC000(unsigned short opcode) {
	V[X(opcode)] = (rand() % 0x100) & (opcode & 0x00FF);
	pc += 2;
}

// DXYN - Draws a sprite at coordinate (VX, VY) that has a width
// of 8 pixels and a  height of N pixels. Each row of 8 pixels is
// read as bit-coded starting from memory location I; I value 
// doesn't change after the execution of this instruction. As 
// described above, VF is set to 1 if any screen pixels are flipped
// from set to unset when the sprite is drawn, and to 0 if that 
// doesn't happen.
void Chip8::ocD000(unsigned short opcode) {
	int coord_x = V[X(opcode)];
	int coord_y = V[Y(opcode)];
	unsigned char sprite_height = opcode & 0x000F;
	unsigned char sprite_byte;
	int xpos, ypos;
	int gfx_index;

	// Reset VF
	V[0x0F] = 0;

	for (int y=0; y<sprite_height; y++) {
		sprite_byte = memory[I + y];
		for (int x=0; x<8; x++) {
			xpos = coord_x + x;
			ypos = coord_y + y;

			// When sprite is drawn outside display,
			// wrap it around!
			// This test is turned of, because of on conflict with the
			// game BLITZ. Instead now there is an extra line added to
			// the gfx-display
			//
//			if (xpos > 63)
//				xpos -= 64;
//			if (xpos < 0)
//				xpos += 64;
//
//			if (ypos > 31)
//				ypos -= 32;
//			if (ypos < 0)
//				ypos += 32;

			gfx_index = xpos + (ypos * 64);

			// If sprite pixel is set and gfx pixel is set
			// then VF is set.
			if ((sprite_byte & (0x80 >> x)) != 0) {
				if (gfx[gfx_index] == 1)
					V[0x0F] = 1;
				// gfx pixel XOR 1
				gfx[gfx_index] ^= 1;
			}
		}
	}

	drawFlag = true;
	pc += 2;
}

// EX** - Relay instructions.
void Chip8::ocE000(unsigned short opcode) {
	(this->*opsEX[opcode & 0x00FF])(opcode);
}

// EX9E - Skips the next instruction if the key stored in VX 
// is pressed.
void Chip8::ocEX9E(unsigned short opcode) {
	if (keystate[V[X(opcode)]] != 0)
			pc += 2;
	pc += 2;
}

// EXA1 - Skips the next instruction if the key stored in VX
// isn't pressed.
void Chip8::ocEXA1(unsigned short opcode) {
	if (keystate[V[X(opcode)]] == 0)
		pc += 2;
	pc += 2;
}

// FX** - Relay instructions.
void Chip8::ocF000(unsigned short opcode) {
	(this->*opsFX[opcode & 0x00FF])(opcode);
}

// FX07 - Sets VX to the value of the delay timer.
void Chip8::ocFX07(unsigned short opcode) {
	V[X(opcode)] = delay_timer;
	pc += 2;
}

// FX0A - A key press is awaited, and then stored
// in VX.
void Chip8::ocFX0A(unsigned short opcode) {
	bool keypress = false;
	for (int i=0; i<0x10; i++) {
		if (keystate[i] != 0) {
			keypress = true;
			V[X(opcode)] = i;
			break;
		}
	}
	if (keypress)
		pc += 2;
}

// FX15 - Sets the delay timer to VX.
void Chip8::ocFX15(unsigned short opcode) {
	delay_timer = V[X(opcode)];
	pc += 2;
}

// FX18 - Sets the sound timer to VX.
void Chip8::ocFX18(unsigned short opcode) {
	sound_timer = V[X(opcode)];
	pc += 2;
}

// FX1E - Adds VX to I.
// VF is set to 1 when range overflow (I+VX>0xFFF),
// and 0 when there isn't. This is undocumented feature
// of the CHIP-8 and used by Spacefight 2091! game.
void Chip8::ocFX1E(unsigned short opcode) {
	I += V[X(opcode)];
	if (I > 0xFFF)
		V[0x0F] = 1;
	else
		V[0x0F] = 0;
	pc += 2;
}

// FX29 - Sets I to the location off the sprite for the
// character in VX. Chracters 0-F (in hexadecimal) are
// represented by a 4x5 font.
void Chip8::ocFX29(unsigned short opcode) {
	I = V[X(opcode)] * 0x5;
	pc += 2;
}

// FX33 - Stores the binary-coded decimal (BCD) representation
// of VX, with the most significant of three digits at the address 
// in I, the middle digit at I plus 1, and the least significant 
// digit at I plus 2. (In other words, take the decimal representation
// of VX, place the hundreds digit in memory at location in I,
// the tens digit at location I+1, nd the ones digit at location I+2.
void Chip8::ocFX33(unsigned short opcode) {
	memory[I] = V[X(opcode)] / 100;
	memory[I + 1] = (V[X(opcode)] / 10) % 10;
	memory[I + 2] = (V[X(opcode)] % 100) % 10;
	pc += 2;
}

// FX55 - Stores V0 to VX (including VX) in memory starting at
// address I.
// On the original interpreters, when the operation is done,
// I=I+X+1. On current implementations, I is left unchanged.
void Chip8::ocFX55(unsigned short opcode) {
	for (int i=0; i<=X(opcode); i++)
		memory[I + i] = V[i];
	I += X(opcode) + 1;
	pc += 2;
}

// FX65 - Fills V0 to VX (incuding VX) with values from memory 
// starting at address I.
// On the original interpreters, when the operation is done,
// I=I+X+1. On current implementations, I is left unchanged.
void Chip8::ocFX65(unsigned short opcode) {
	for (int i=0; i<=X(opcode); i++)
		V[i] = memory[I + i];
	I += X(opcode) + 1;
	pc += 2;
}




