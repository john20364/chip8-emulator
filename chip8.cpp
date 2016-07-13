#include <iostream>
#include <string.h>
#include <time.h>
#include "chip8.h"

using namespace std;

Chip8::Chip8() {
	cout << "Chip8 constructor" << endl;
	init();
	setOps();
}

Chip8::~Chip8() {
	cout << "Chip8 destructor" << endl;
}

void Chip8::loadFontset() {
}

void Chip8::emulateCycle() {
	opcode = 0xFFFF;
	(this->*ops[(opcode & 0xF000) >> 12])(opcode);
}

void Chip8::init() {
	memset(memory, 0, sizeof(memory));
	memset(V, 0, sizeof(V));
	memset(stack, 0, sizeof(stack));
	
	opcode = 0;
	I = 0;
	pc = 0;
	sp = 0;
	drawFlag = false;

	memset(keystate, 0, sizeof(keystate));
	memset(gfx, 0, sizeof(gfx));
		
	delay_timer = 0;
	sound_timer = 0;
}

void Chip8::setOps() {
	for (int i=0; i<0x10; i++)
		ops[i] = doNothing;
}

void Chip8::doNothing(unsigned short opcode) {
	printf("doNothing - opcode=0x%04X\n", opcode);
}

// 00** - Relay instructions.
void Chip8::oc0000(unsigned short opcode) {}

// 1NNN - Jumps to address NNN.
void Chip8::oc1000(unsigned short opcode) {}

// 2NNN - Calls subroutine at NNN.
void Chip8::oc2000(unsigned short opcode) {}

// 3XNN - Skips the next instruction if VX equals NN.
void Chip8::oc3000(unsigned short opcode) {}

// 4XNN - Skips the next instruction if VX doesn't equal NN.
void Chip8::oc4000(unsigned short opcode) {}

// 5XY0 - Skips the next instruction if VX equals VY.
void Chip8::oc5000(unsigned short opcode) {}

// 6XNN - Sets VX to NN.
void Chip8::oc6000(unsigned short opcode) {}

// 7XNN - Add NN to VX.
void Chip8::oc7000(unsigned short opcode) {}

// 8XY* - Relay instructions.
void Chip8::oc8000(unsigned short opcode) {}

// 9XY0 - Skips the instruction if VX doesn't equal VY.
void Chip8::oc9000(unsigned short opcode) {}

// ANNN - Sets I to address NNN.
void Chip8::ocA000(unsigned short opcode) {}

// BNNN - Jumps to the address NNN plus V0.
void Chip8::ocB000(unsigned short opcode) {}

// CXNN - Sets VX to the result of a bitwise AND operation on a 
// random number and NN.
void Chip8::ocC000(unsigned short opcode) {}

// DXYN - Draws a sprite at coordinate (VX, VY) that has a width
// of 8 pixels and a  height of N pixels. Each row of 8 pixels is
// read as bit-coded starting from memory location I; I value 
// doesn't change after the execution of this instruction. As 
// described above, VF is set to 1 if any screen pixels are flipped
// from set to unset when the sprite is drawn, and to 0 if that 
// doesn't happen.
void Chip8::ocD000(unsigned short opcode) {}

// EX** - Relay instructions.
void Chip8::ocE000(unsigned short opcode) {}

// FX** - Relay instructions.
void Chip8::ocF000(unsigned short opcode) {}





