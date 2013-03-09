#pragma once

typedef unsigned char BYTE ;
typedef unsigned short int WORD ;

class SChip{
public:
	~SChip();
	static  SChip*   CreateInstance		( ) ;
	bool LoadROM     (const char* file);
	void Update();
	void KeyPressed	 ( int key ) ;
	void KeyReleased ( int key ) ;
	BYTE ScreenData[64][128]; 
private:
	static  SChip*   Instance ;
	SChip	( ) ;
	BYTE  Memory[0x1000];
	BYTE Registers[16];
	BYTE KeyState[16];
	BYTE HP48Flags[8];	// for Fx75, Fx85 opcodes
	BYTE DelayTimer;
	BYTE SoundTimer;
	WORD ProgramCounter;
	WORD Stack[16];
	WORD *StackPointer;
	WORD index;

	WORD ROMBase;
	bool schip_mode;
	bool halt;
	bool IsEmulating;
	
	int GetKey();
	void CPUReset();
	void ClearScreen();
	void ExecOpcode();
	void Push(WORD value);
	WORD Pop();
	void Op0(WORD opcode);
	void Op8(WORD opcode);
	void OpD(WORD opcode);
	void OpF(WORD opcode);
};