#ifndef __CHIP8_H__
#define __CHIP8_H__
#include <map>
#include <SDL2/SDL.h>

class Chip8 {
	private:
		Uint64 startCount;			  // SDL_GetPerformanceCounter
		Uint64 currentCount;		  // SDL_GetPerformanceCounter
		unsigned char memory[4096];	  // 4K memory
		unsigned char V[16];		  // General purpose registers
		
		unsigned short opcode;		  // Operation code
		unsigned short I;			  // Index register
		unsigned short pc;			  // Program counter
		unsigned short stack[16];	  // 16 levels of stack
		unsigned short sp;			  // Stack pointer
		bool drawFlag;				  // Set to true when time to draw

		unsigned char keystate[16];	  // Keystate for HEX keys
		std::map<Uint32, int> keymap; // keyboard mapping

		unsigned char delay_timer;
		unsigned char sound_timer;

		void (Chip8::*ops[0x10])(unsigned short);
		void (Chip8::*ops00[0x100])(unsigned short);
		void (Chip8::*ops8XY[0x10])(unsigned short);
		void (Chip8::*opsEX[0x100])(unsigned short);
		void (Chip8::*opsFX[0x100])(unsigned short);

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

		void oc00E0(unsigned short opcode);
		void oc00EE(unsigned short opcode);

		void oc8XY0(unsigned short opcode);
		void oc8XY1(unsigned short opcode);
		void oc8XY2(unsigned short opcode);
		void oc8XY3(unsigned short opcode);
		void oc8XY4(unsigned short opcode);
		void oc8XY5(unsigned short opcode);
		void oc8XY6(unsigned short opcode);
		void oc8XY7(unsigned short opcode);
		void oc8XYE(unsigned short opcode);

		void ocEX9E(unsigned short opcode);
		void ocEXA1(unsigned short opcode);

		void ocFX07(unsigned short opcode);
		void ocFX0A(unsigned short opcode);
		void ocFX15(unsigned short opcode);
		void ocFX18(unsigned short opcode);
		void ocFX1E(unsigned short opcode);
		void ocFX29(unsigned short opcode);
		void ocFX33(unsigned short opcode);
		void ocFX55(unsigned short opcode);
		void ocFX65(unsigned short opcode);
	public:
		unsigned char gfx[64*32];	// Graphics 64 x 32 pixels
		Chip8();
		~Chip8();
		void setKeystate(Uint32 sdl_keycode, unsigned char state);
		bool timeToDraw();
		void emulateCycle();
		void loadGame(std::string title);
	public:
};
#endif
