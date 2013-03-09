#include "schip.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

SChip* SChip::Instance = 0 ;

SChip* SChip::CreateInstance( )
{
	if (!Instance)
	Instance = new SChip( );
	return Instance ;
}

SChip::SChip( )
{

}

SChip::~SChip( )
{
}

bool SChip::LoadROM(const char* file)
{
	CPUReset( ) ;
	ClearScreen( ) ;
	FILE *pFile;
	pFile = fopen(file,"rb");
	if (!pFile)
	{
		return false ;
	}
	int size;
	fseek(pFile, 0, SEEK_END);
	size = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	fread(&Memory[ROMBase], size, 1, pFile) ;
	fclose(pFile) ;
	IsEmulating = true;
	return true;
}

void SChip::CPUReset()
{
	const BYTE font4x5[] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, //0
		0x20, 0x60, 0x20, 0x20, 0x70, //1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
		0x90, 0x90, 0xF0, 0x10, 0x10, //4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
		0xF0, 0x10, 0x20, 0x40, 0x40, //7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
		0xF0, 0x90, 0xF0, 0x90, 0x90, //A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
		0xF0, 0x80, 0x80, 0x80, 0xF0, //C
		0xE0, 0x90, 0x90, 0x90, 0xE0, //D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
		0xF0, 0x80, 0xF0, 0x80, 0x80  //F
	};
	const BYTE font8x10[] = {
		0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, //0
		0x00, 0x08, 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3E, 0x00, //1
		0x00, 0x38, 0x44, 0x04, 0x08, 0x10, 0x20, 0x44, 0x7C, 0x00, //2
		0x00, 0x38, 0x44, 0x04, 0x18, 0x04, 0x04, 0x44, 0x38, 0x00, //3
		0x00, 0x0C, 0x14, 0x24, 0x24, 0x7E, 0x04, 0x04, 0x0E, 0x00, //4
		0x00, 0x3E, 0x20, 0x20, 0x3C, 0x02, 0x02, 0x42, 0x3C, 0x00, //5
		0x00, 0x0E, 0x10, 0x20, 0x3C, 0x22, 0x22, 0x22, 0x1C, 0x00, //6
		0x00, 0x7E, 0x42, 0x02, 0x04, 0x04, 0x08, 0x08, 0x08, 0x00, //7
		0x00, 0x3C, 0x42, 0x42, 0x3C, 0x42, 0x42, 0x42, 0x3C, 0x00, //8
		0x00, 0x3C, 0x42, 0x42, 0x42, 0x3E, 0x02, 0x04, 0x78, 0x00, //9
		0x00, 0x18, 0x08, 0x14, 0x14, 0x14, 0x1C, 0x22, 0x77, 0x00, //A
		0x00, 0x7C, 0x22, 0x22, 0x3C, 0x22, 0x22, 0x22, 0x7C, 0x00, //B
		0x00, 0x1E, 0x22, 0x40, 0x40, 0x40, 0x40, 0x22, 0x1C, 0x00, //C
		0x00, 0x78, 0x24, 0x22, 0x22, 0x22, 0x22, 0x24, 0x78, 0x00, //D
		0x00, 0x7E, 0x22, 0x28, 0x38, 0x28, 0x20, 0x22, 0x7E, 0x00, //E
		0x00, 0x7E, 0x22, 0x28, 0x38, 0x28, 0x20, 0x20, 0x70, 0x00  //F
	};

	memset(Registers,0,sizeof(Registers)) ;
	memset(Stack,0,sizeof(Stack)) ;
	memset(Memory,0,sizeof(Memory)) ;
	memset(KeyState,0,sizeof(KeyState)) ;
	memset(HP48Flags,0,sizeof(HP48Flags)) ;
	memcpy(Memory, font4x5, 80);
	memcpy(Memory+80, font8x10, 240-80);
	ProgramCounter = 0x200;
	ROMBase = ProgramCounter;
	DelayTimer = 0 ;
	SoundTimer = 0 ;
	index = 0;
	halt = false;
	StackPointer = Stack;
	srand(time(NULL));
	IsEmulating = false;
}

void SChip::ClearScreen()
{
	memset(ScreenData, 0, 128*64);

}

void SChip::KeyPressed	 ( int key )
{
	KeyState[key] = 1 ;

}

void  SChip::KeyReleased ( int key )
{
	KeyState[key] = 0 ;

}

int SChip::GetKey()
{	
	int res = 0;
	for (int i=0;i <16;i++){
		if (KeyState[i])return i ;
	}
	return res;
}

void SChip::Update()
{
	for (int i = 0 ; i < 6; i++) //opcodes per frame
	ExecOpcode();
	//render frame
}

void SChip::ExecOpcode()
{
	if(!IsEmulating) return;

	WORD op = (Memory[ProgramCounter+0]<<8) | Memory[ProgramCounter+1];
	if (DelayTimer > 0) { DelayTimer--; return; } 
	switch((op&0xF000)>>12)
	{
	case 0x0: Op0(op); break;
	case 0x1: //jump to nnn
		ProgramCounter= (op&0x0FFF)-2;
		break;
	case 0x2: //call nnn
		Push(ProgramCounter);
		ProgramCounter = (op&0x0FFF)-2;
		break;
	case 0x3: if (Registers[(op&0x0F00)>>8] == (op&0x00FF)) ProgramCounter+=2; break; //skip if vx==kk
	case 0x4: //skip if vx!=kk
		if (Registers[(op&0x0F00)>>8] != (op&0x00FF)) ProgramCounter+=2;
		break;
	case 0x5: //skip if vx==vy
		if (Registers[(op&0x0F00)>>8] == Registers[(op&0x00F0)>>4]) ProgramCounter+=2;
		break;
	case 0x6: //vx = kk
		Registers[(op&0x0F00)>>8] = op&0x00FF;
		break;
	case 0x7: //vx = vx + kk
		Registers[(op&0x0F00)>>8] += op&0x00FF;
		break;
	case 0x8: Op8(op); break;
	case 0x9: //skip if vx!=vy
		if (Registers[(op&0x0F00)>>8] != Registers[(op&0x00F0)>>4])ProgramCounter+=2;
		break;
	case 0xA: //I = nnn
		index = op&0x0FFF;
		break;
	case 0xB: //jump nnn+v0
		op = (op&0x0FFF)+Registers[0x0]-2;
		break;
	case 0xC: //vx = random and/mod kk
		Registers[(op&0x0F00)>>8] = rand() & (op&0x00FF);
		break;
	case 0xD: OpD(op); break; //draw 8xN sprite at vx vy, collision
	case 0xE: //check keys
		switch (op&0x00FF)
		{
		case 0x9E: //skip instruction if key vx pressed
			if (KeyState[Registers[(op&0x0F00)>>8]]) ProgramCounter+=2;
			break;
		case 0xA1: //skip instruction if key vx not pressed
			if (!KeyState[Registers[(op&0x0F00)>>8]]) ProgramCounter+=2;
			break;
		}
		break;
	case 0xF: OpF(op); break;
	}

	ProgramCounter +=2;
}

void SChip::Push(WORD value)
{
	*StackPointer++ = value;
}

WORD SChip::Pop()
{
	return *--StackPointer;
}

void SChip::Op0(WORD opcode)
{
	int i,j,n;

	if ((opcode & 0x00F0)>>4 == 0xC) //scroll display N lines down *SCHIP*
	{
	n = opcode&0x000F;
	for (j=63; j>=n; j--)
	{
		for (i=0; i<128; i++)
		{
			ScreenData[j][i] =ScreenData[j-n][i];
		}
	}
	memset(ScreenData, 0, 128*n); //clear
	}

	switch (opcode&0x00FF)
	{
		case 0xE0: //clear screen
			memset(ScreenData, 0, 128*64);
			break;
		case 0xEE: //return from instruction
			ProgramCounter = Pop();
			break;
		case 0xFB: //scroll 4 pixels right  *SCHIP*
			for (j=0; j<64; j++)
			{
				for (i=127; i>4; i--)
				{
					ScreenData[j][i] = ScreenData[j][i-5];
				}
			}
			for (j=0; j<64; j++) //clear
			{
				for (i=0; i<=4; i++)
				{
					ScreenData[j][i] = 0;
				}
			}
			break;
		case 0xFC: //scroll 4 pixels left  *SCHIP*
			for (j=0; j<64; j++)
			{
				for (i=0; i<128-4; i++)
				{
					ScreenData[j][i] =ScreenData[j][i+4];
				}
			}
			for (j=0; j<64; j++) //clear
			{
				for (i=128-4; i<128; i++)
				{
					ScreenData[j][i] = 0;
				}
			}
			break;
		case 0xFD: //quit
			halt = true;
			break;
		case 0xFE: //chip8 graphics mode *SCHIP*
			schip_mode = false;
			memset(ScreenData, 0, 128*64);
			break;
		case 0xFF: //schip graphics mode *SCHIP*
			schip_mode = true;
			memset(ScreenData, 0, 128*64);
			break;
	}
}

void SChip::Op8(WORD opcode)
{
	int t = 0;
	switch (opcode&0x000F)
	{
	case 0x0: //vx = vy
		Registers[(opcode&0x0F00)>>8] = Registers[(opcode&0x00F0)>>4];
		break;
	case 0x1: //vx = vx or vy
		Registers[(opcode&0x0F00)>>8] |= Registers[(opcode&0x00F0)>>4];
		break;
	case 0x2: //vx = vx and vy
		Registers[(opcode&0x0F00)>>8] &= Registers[(opcode&0x00F0)>>4];
		break;
	case 0x3: //vx = vx xor vy
		Registers[(opcode&0x0F00)>>8] ^= Registers[(opcode&0x00F0)>>4];
		break;
	case 0x4: //vx = vx + vy, carry
		t = Registers[(opcode&0x0F00)>>8];
		Registers[(opcode&0x0F00)>>8] += Registers[(opcode&0x00F0)>>4];
		Registers[0xF] = t > Registers[(opcode&0x0F00)>>8];
		break;
	case 0x5: //vx = vx - vy, not borrow
		Registers[0xF] = Registers[(opcode&0x0F00)>>8] >= Registers[(opcode&0x00F0)>>4];
		Registers[(opcode&0x0F00)>>8] -= Registers[(opcode&0x00F0)>>4];
		break;
	case 0x6: //vx = vx >> 1, carry
		Registers[0xF] = Registers[(opcode&0x0F00)>>8]&0x0001;
		Registers[(opcode&0x0F00)>>8] >>= 1;
		break;
	case 0x7: //vx = vy - vx, not borrow
		Registers[0xF] = Registers[(opcode&0x00F0)>>4] >= Registers[(opcode&0x0F00)>>8];
		Registers[(opcode&0x0F00)>>8] = Registers[(opcode&0x00F0)>>4] - Registers[(opcode&0x0F00)>>8];
		break;
	case 0xE: //vx = vx << 1, carry
		Registers[0xF] = (Registers[(opcode&0x0F00)>>8]&0x80)>0;
		Registers[(opcode&0x0F00)>>8] <<= 1;
		break;
	}
}

void SChip::OpD(WORD opcode)
{
	int x, y, height, x_pos, y_pos,t,i,j;
	height = opcode&0x000F;
	if (height == 0) height = 16;
	Push(index);
	Registers[0xF] = 0;
	if (!schip_mode)
	{
		x = Registers[(opcode&0x0F00)>>8]%64;
		y = Registers[(opcode&0x00F0)>>4]%32;
		if (height==0) height = 16;
		for (j=0; j<height; j++)
		{
			for (i=0; i<8; i++)
			{
				t = (Memory[index]&(0x80>>i))>>(7-i);
				x_pos = ((x+i)%64)<<1;
				y_pos = ((y+j)%32)<<1;
				Registers[0xF] += (ScreenData[y_pos][x_pos]>0) && (t>0);

				ScreenData[y_pos+0][x_pos+0] ^= t;
				ScreenData[y_pos+1][x_pos+0] ^= t;
				ScreenData[y_pos+1][x_pos+1] ^= t;
				ScreenData[y_pos+0][x_pos+1] ^= t;
			}
			index++;
		}
	}
	else
	{
		for (j=0; j<height; j++)
		{
			for (i=0; i<((height==16)?16:8); i++)
			{
				if (i==8) index++;
				t = (Memory[index]&(0x80>>(i%8)))>>(7-(i%8));
				x_pos = (Registers[(opcode&0x0F00)>>8]+i)%128;
				y_pos = (Registers[(opcode&0x00F0)>>4]+j)%64;
				Registers[0xF] += (ScreenData[y_pos][x_pos]>0) && (t>0);

				ScreenData[y_pos][x_pos] ^= t;
			}
			index++;
		}
	}
	Registers[0xF] = Registers[0xF]>0;
	index = Pop();
}

void SChip::OpF(WORD opcode)
{
	switch (opcode &0x00FF)
	{
	case 0x07: //vx = delay timer
		Registers[(opcode&0x0F00)>>8] = DelayTimer;
		break;
	case 0x0A: //waits for a keypress and stores in vx
		Registers[(opcode&0x0F00)>>8] = GetKey();
		break;
	case 0x15: //delay timer = vx
		DelayTimer = Registers[(opcode&0x0F00)>>8];
		break;
	case 0x18: //sound timer = vx
		SoundTimer = Registers[(opcode&0x0F00)>>8];
		break;
	case 0x1E: //I = I + vx
		index += Registers[(opcode&0x0F00)>>8];
		break;
	case 0x29: //I = 4x5 font character of vx
		index = Registers[(opcode&0x0F00)>>8] * 5;
		break;
	case 0x30: //I = 8x10 font charater of vx *SCHIP*
		index = 80 + Registers[(opcode&0x0F00)>>8] * 10;
		break;
	case 0x33: //bcd of vx in mem(I) ... mem(I+2)
		Memory[index+0] = Registers[(opcode&0x0F00)>>8] / 100;
		Memory[index+1] = (Registers[(opcode&0x0F00)>>8] % 100) / 10;
		Memory[index+2] = Registers[(opcode&0x0F00)>>8] % 10;
		break;
	case 0x55: //store v0 ... vx starting at mem(I);
		memcpy(Memory+index, Registers, ((opcode&0x0F00)>>8)+1);
		break;
	case 0x65: //load v0 ... vx starting at mem(I);
		memcpy(Registers, Memory+index, ((opcode&0x0F00)>>8)+1);
		break;
	case 0x75:	//save v0...vx (X<8) in the HP48 flags *SCHIP*
		memcpy(HP48Flags,Registers,((opcode & 0x0F00)>>8));
		break;
	case 0x85:	//load v0...vX (X<8) from the HP48 flags *SCHIP*
		memcpy(Registers,HP48Flags,((opcode & 0x0F00)>>8));
		break;
	}
}