#include "chip8.h"

// Constants definitions
const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;

// Fontset definition
uint8_t fontset[FONTSET_SIZE] =
{
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

uint8_t randomByte() {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<int> dist(0, 255);
    return dist(gen);
}


void CHIP8::LoadROM(string file_address){
    ifstream rom(file_address,ios::binary);
    if(!rom.is_open()){
        cout<<"ERROR WHILE OPENING THE FILE\n";
        return;
    }

    char ch;
    int i = 0;

    while(rom.get(ch)){
        memory[START_ADDRESS + i++] = ch;
    }
    rom.close();

}

void CHIP8::Initialize(){
    //Program Starts at 0x200
    pc= START_ADDRESS;
    index=0;
    sp=0;
    opcode=0;

    delayTimer=0;
    SoundTimer=0;

    //Clear registers,memory,stack,keypad,video
    memset(registers, 0, sizeof(registers));
    memset(memory, 0, sizeof(memory));
    memset(stack, 0, sizeof(stack));
    memset(keypad, 0, sizeof(keypad));
    memset(video, 0, sizeof(video));
    
    //LOADING FONTS
    for(unsigned int i =0; i<FONTSET_SIZE; i++) memory[FONTSET_START_ADDRESS+i] = fontset[i];
    //each font is 5 pixels tall and 4 pixels wide hence total 5*16=80 address required to store all hexadecimal
    //data of fonts

};

void CHIP8::OP_00E0(){
    memset(video, 0, sizeof(video));
}

void CHIP8::OP_00EE(){
    sp--;
    pc = stack[sp];
}

void CHIP8::OP_1nnn(){

    uint16_t address = opcode & 0x0FFF; //opcode = 0x1ABC, opcode & 0x0FFF = 0x0ABC
    pc = address;
}

void CHIP8::OP_2nnn(){
    uint16_t address = opcode & 0x0FFF;
    stack[sp] = pc;
    sp++; //moving to a new empty address 
    pc = address;
}

void CHIP8::OP_3xkk(){
    uint8_t Vx = (opcode & 0x0F00) >> 8; //shifiting by 8 bits as 32,16,8,4 each hexadecimal represents 4 bits so since x is given by 3rd position we gotta shift by 8 bits to left to get position
    uint8_t byte = opcode & 0x00FF;
    
    if(registers[Vx]==byte){
        pc+=2; //moving to the next instruction
    }
}

void CHIP8::OP_4xkk(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;

    if(registers[Vx]!=byte){
        pc+=2;
    }
}

void CHIP8::OP_5xy0(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    if(registers[Vx]==registers[Vy]){
        pc+=2;
    }
}

void CHIP8::OP_6xkk(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = (opcode & 0x00FF);

    registers[Vx] = byte;
}

void CHIP8::OP_7xkk(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = (opcode & 0x00FF);

    registers[Vx]+=byte;
}

void CHIP8::OP_8xy0(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    registers[Vx] = registers[Vy];
}

void CHIP8::OP_8xy1(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    registers[Vx] = registers[Vx] | registers[Vy];
}

void CHIP8::OP_8xy2(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    registers[Vx] = registers[Vx] & registers[Vy];
}

void CHIP8::OP_8xy3(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    registers[Vx] = registers[Vx] ^ registers[Vy];
}

void CHIP8::OP_8xy4(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    uint16_t sum = registers[Vx]+registers[Vy];

    if(sum>>8){
        registers[15]=1;
    }
    else{
        registers[15]=0;
    }

    registers[Vx] = sum & 0xFF; //Only lower 8bits are stored! 
}


void CHIP8::OP_8xy5(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;
    
    if(registers[Vx]>registers[Vy]){
        registers[15] = 1;
    }
    else{
        registers[15] = 0;
    }

    registers[Vx]-=registers[Vy];

}

void CHIP8::OP_8xy6(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    registers[15] = (registers[Vx] & 0x1);

    registers[Vx] >>=1; //Divide by 2
}

void CHIP8::OP_8xy7(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    if(registers[Vy]>registers[Vx]){
        registers[15] = 1;
    }
    else{
        registers[15] = 0;
    }

    registers[Vx] = registers[Vy]-registers[Vx];
}

void CHIP8::OP_8xyE(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;

    registers[15] = (registers[Vx] & 0x80) >>7;
// abcd efgh ->Vx
// 1000 0000 ->0x80
// a000 0000 now right shift by 7 to get msb!

    registers[Vx] <<=1; //multiplied by 2
}

void CHIP8::OP_9xy0(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t Vy = (opcode & 0x00F0) >> 4;

    if(registers[Vx]!=registers[Vy]){
        pc+=2;
    }
}

void CHIP8::OP_Annn(){
    uint16_t address = opcode & 0x0FFF; //only need the first 12 bits!

    index = address;
}

void CHIP8::OP_Bnnn(){
    uint16_t address = opcode & 0x0FFF;

    pc = registers[0] + address;
}

void CHIP8::OP_Cxkk(){
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    uint8_t byte = opcode & 0x00FF;

    registers[Vx] = randomByte() & byte;
}

void CHIP8::OP_Dxyn(){
    uint8_t Vx = (opcode & 0x0F00) >>8;
    uint8_t Vy = (opcode & 0x00F0) >>4;
    uint8_t height = opcode & 0x000F;

    //wrap if going beyound screen boundaries
    uint8_t xPos = registers[Vx] % 64;
	uint8_t yPos = registers[Vy] % 32;

    registers[15] = 0;

    for(unsigned int row=0; row<height; row++){
        uint8_t sprite_byte = memory[index + row];

        for(unsigned int col=0; col<8; col++){
            uint8_t sprite_pixel = sprite_byte & (0x80 >> col);
            uint32_t* screenPixel = &video[(yPos + row) * 64 + (xPos + col)];

            if(sprite_pixel){
                if(*screenPixel == 0xFFFFFFFF){
                    registers[15] = 1;
                }
                *screenPixel = *screenPixel^0xFFFFFFFF;
            }

        }
    }
}

void CHIP8::OP_Ex9E(){
    uint8_t Vx = (opcode & 0x0F00)>>8;
    uint8_t key = registers[Vx];

    if(keypad[key]){
        pc+=2;
    }
}

void CHIP8::OP_ExA1(){
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t key = registers[Vx];

	if (!keypad[key]){
		pc += 2;
	}
}

void CHIP8::OP_Fx07(){
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[Vx] = delayTimer;
}

void CHIP8::OP_Fx0A(){
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	if (keypad[0])
	{
		registers[Vx] = 0;
	}
	else if (keypad[1])
	{
		registers[Vx] = 1;
	}
	else if (keypad[2])
	{
		registers[Vx] = 2;
	}
	else if (keypad[3])
	{
		registers[Vx] = 3;
	}
	else if (keypad[4])
	{
		registers[Vx] = 4;
	}
	else if (keypad[5])
	{
		registers[Vx] = 5;
	}
	else if (keypad[6])
	{
		registers[Vx] = 6;
	}
	else if (keypad[7])
	{
		registers[Vx] = 7;
	}
	else if (keypad[8])
	{
		registers[Vx] = 8;
	}
	else if (keypad[9])
	{
		registers[Vx] = 9;
	}
	else if (keypad[10])
	{
		registers[Vx] = 10;
	}
	else if (keypad[11])
	{
		registers[Vx] = 11;
	}
	else if (keypad[12])
	{
		registers[Vx] = 12;
	}
	else if (keypad[13])
	{
		registers[Vx] = 13;
	}
	else if (keypad[14])
	{
		registers[Vx] = 14;
	}
	else if (keypad[15])
	{
		registers[Vx] = 15;
	}
	else
	{
		pc -= 2;
	}
}

void CHIP8::OP_Fx15(){
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	delayTimer = registers[Vx];
}

void CHIP8::OP_Fx18(){
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	SoundTimer = registers[Vx];
}

void CHIP8::OP_Fx1E(){
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	index += registers[Vx];
}

void CHIP8::OP_Fx29(){
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t digit = registers[Vx];

	index = FONTSET_START_ADDRESS + (5 * digit);
}

void CHIP8::OP_Fx33(){
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t value = registers[Vx];

	//1s place
	memory[index + 2] = value % 10;
	value /= 10;

	//10s place
	memory[index + 1] = value % 10;
	value /= 10;

	//100s place
	memory[index] = value % 10;
}

void CHIP8::OP_Fx55(){
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	for (uint8_t i = 0; i <= Vx; ++i){
		memory[index + i] = registers[i];
	}
}

void CHIP8::OP_Fx65(){
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	for (uint8_t i = 0; i <= Vx; ++i){
		registers[i] = memory[index + i];
	}
}

void CHIP8::Cycle(){
    opcode = (memory[pc]<< 8)  | memory[pc+1];
    //incrementing program counter before execution

    pc+=2;

    switch (opcode & 0xF000)
    {
        case 0x0000:
            switch (opcode & 0x00FF)
            {
                case 0xE0: OP_00E0(); break; // CLS
                case 0xEE: OP_00EE(); break; // RET
                default: break;
            }
            break;

        case 0x1000: OP_1nnn(); break;
        case 0x2000: OP_2nnn(); break;
        case 0x3000: OP_3xkk(); break;
        case 0x4000: OP_4xkk(); break;
        case 0x5000: OP_5xy0(); break;
        case 0x6000: OP_6xkk(); break;
        case 0x7000: OP_7xkk(); break;

        case 0x8000:
            switch (opcode & 0x000F)
            {
                case 0x0: OP_8xy0(); break;
                case 0x1: OP_8xy1(); break;
                case 0x2: OP_8xy2(); break;
                case 0x3: OP_8xy3(); break;
                case 0x4: OP_8xy4(); break;
                case 0x5: OP_8xy5(); break;
                case 0x6: OP_8xy6(); break;
                case 0x7: OP_8xy7(); break;
                case 0xE: OP_8xyE(); break;
                default: break;
            }
            break;

        case 0x9000: OP_9xy0(); break;
        case 0xA000: OP_Annn(); break;
        case 0xB000: OP_Bnnn(); break;
        case 0xC000: OP_Cxkk(); break;
        case 0xD000: OP_Dxyn(); break;

        case 0xE000:
            switch (opcode & 0x00FF)
            {
                case 0x9E: OP_Ex9E(); break;
                case 0xA1: OP_ExA1(); break;
                default: break;
            }
            break;

        case 0xF000:
            switch (opcode & 0x00FF)
            {
                case 0x07: OP_Fx07(); break;
                case 0x0A: OP_Fx0A(); break;
                case 0x15: OP_Fx15(); break;
                case 0x18: OP_Fx18(); break;
                case 0x1E: OP_Fx1E(); break;
                case 0x29: OP_Fx29(); break;
                case 0x33: OP_Fx33(); break;
                case 0x55: OP_Fx55(); break;
                case 0x65: OP_Fx65(); break;
                default: break;
            }
            break;

        default:
            break;
    }

    if(delayTimer>0){
        delayTimer--;
    }
    
    if(SoundTimer>0){
        SoundTimer--;
    }

}

void SAVE_STATE(){
    
}