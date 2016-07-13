#ifndef __CHIP8_H__
#define __CHIP8_H__
class Chip8 {
	private:
		unsigned char memory[4096];	// 4K memory
		unsigned char V[16];		// General purpose registers
		
		unsigned short opcode;		// Operation code
		unsigned short I;			// Index register
		unsigned short pc;			// Program counter
		unsigned short stack[16];	// 16 levels of stack
		unsigned short sp;			// Stack pointer
		bool drawFlag;				// Set to true when time to draw

		unsigned char keystate[16];	// Keystate for HEX keys
		unsigned char gfx[64*32];	// Graphics 64 x 32 pixels

		unsigned char delay_timer;
		unsigned char sound_timer;

		void (Chip8::*ops[0x10])(unsigned short);

		void doNothing(unsigned short opcode);
		void setOps();
		void loadFontset();
		void init();

		void oc0000(unsigned short opcode);
		void oc1000(unsigned short opcode);
		void oc2000(unsigned short opcode);
		void oc3000(unsigned short opcode);
		void oc4000(unsigned short opcode);
		void oc5000(unsigned short opcode);
		void oc6000(unsigned short opcode);
		void oc7000(unsigned short opcode);
		void oc8000(unsigned short opcode);
		void oc9000(unsigned short opcode);
		void ocA000(unsigned short opcode);
		void ocB000(unsigned short opcode);
		void ocC000(unsigned short opcode);
		void ocD000(unsigned short opcode);
		void ocE000(unsigned short opcode);
		void ocF000(unsigned short opcode);
	public:
		Chip8();
		~Chip8();
		void emulateCycle();
	public:
};
#endif
