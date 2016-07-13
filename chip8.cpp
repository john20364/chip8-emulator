#include <iostream>
#include <string.h>
#include <time.h>
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
}

void Chip8::emulateCycle() {
	opcode = memory[pc] << 8 | memory[pc + 1];
	(this->*ops[(opcode & 0xF000) >> 12])(opcode);
}

void Chip8::init() {
	memset(memory, 0, sizeof(memory));
	memset(V, 0, sizeof(V));
	memset(stack, 0, sizeof(stack));
	
	pc = 0x200; // Starting point of program
	opcode = 0;
	I = 0;
	sp = 0;
	drawFlag = false;

	memset(keystate, 0, sizeof(keystate));
	memset(gfx, 0, sizeof(gfx));
		
	delay_timer = 0;
	sound_timer = 0;
	
	// random seed
	srand(time(NULL));
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
	ops00[0xEE] = oc00E0;

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
	opsFX[0x07] = ocFX07;
	opsFX[0x07] = ocFX07;
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
	memset(gfx, 0, sizeof(0));
	drawFlag = true;
}

// 00EE - Returns from a subroutine.
void Chip8::oc00EE(unsigned short opcode) {
	pc = stack[--sp];
	pc += 2;
}

// 1NNN - Jumps to address NNN.
void Chip8::oc1000(unsigned short opcode) {
	pc = opcode & 0xFFF;
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
	V[0xF] = (V[X(opcode)] >> 7);
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
	pc = (opcode & 0x0FFF) + V[0];
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
void Chip8::ocD000(unsigned short opcode) {}

// EX** - Relay instructions.
void Chip8::ocE000(unsigned short opcode) {
	(this->*opsEX[opcode & 0x00FF])(opcode);
}

void Chip8::ocEX9E(unsigned short opcode) {}
void Chip8::ocEXA1(unsigned short opcode) {}

// FX** - Relay instructions.
void Chip8::ocF000(unsigned short opcode) {
	(this->*opsFX[opcode & 0x00FF])(opcode);
}

void Chip8::ocFX07(unsigned short opcode) {}
void Chip8::ocFX0A(unsigned short opcode) {}
void Chip8::ocFX15(unsigned short opcode) {}
void Chip8::ocFX18(unsigned short opcode) {}
void Chip8::ocFX1E(unsigned short opcode) {}
void Chip8::ocFX29(unsigned short opcode) {}
void Chip8::ocFX33(unsigned short opcode) {}
void Chip8::ocFX55(unsigned short opcode) {}
void Chip8::ocFX65(unsigned short opcode) {}




