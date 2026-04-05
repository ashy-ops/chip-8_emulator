#pragma once

#include <bits/stdc++.h>
#include <SDL2/SDL.h>

using namespace std;

// Constants (declarations only)
extern const unsigned int START_ADDRESS;
extern const unsigned int FONTSET_SIZE;
extern const unsigned int FONTSET_START_ADDRESS;

// External declarations
extern uint8_t fontset[80];
uint8_t randomByte();


struct SAVE_STATE{
    //CPU STATE
    uint8_t registers[16]={}; //VX (x-> 0 to F)
    uint8_t memory[4096]={}; //4KB of ram only upto so 4096 adressess each of 1 byte(aka 8 bits)
    uint16_t index; //index register is 16bit aka 2 bytes
    uint16_t pc; //program counter 16 bit
    
    //STACK
    uint16_t stack[16]={};
    uint8_t sp; //points to a location in the above stack

    //TIMER
    uint8_t delayTimer;
    uint8_t SoundTimer;

    //INPUT + VIDEO
    uint8_t keypad[16]={};
    uint32_t video[64*32]={};

    //OPCODE
    uint16_t opcode;
};

struct CHIP8{
    //CPU STATE
    uint8_t registers[16]={}; //VX (x-> 0 to F)
    uint8_t memory[4096]={}; //4KB of ram only upto so 4096 adressess each of 1 byte(aka 8 bits)
    uint16_t index; //index register is 16bit aka 2 bytes
    uint16_t pc; //program counter 16 bit
    
    //STACK
    uint16_t stack[16]={};
    uint8_t sp; //points to a location in the above stack

    //TIMER
    uint8_t delayTimer;
    uint8_t SoundTimer;

    //INPUT + VIDEO
    uint8_t keypad[16]={};
    uint32_t video[64*32]={};

    //OPCODE
    uint16_t opcode;

    //MEMBER FUNCTIONS
    void Initialize();
    void LoadROM(const string file_adress);
    void Cycle();

    //INSTRUCTIONS:
    void OP_00E0(); //CLS
    void OP_00EE(); //RET
    void OP_1nnn(); //JUMP ADDER
    void OP_2nnn(); //CALL ADDER
    void OP_3xkk(); //GO TO NEXT INSTRUCTION IF VALUE AT REGISTER VX == kk
    void OP_4xkk(); //GO TO NEXT INSTRUCTION IF VALUE AT REGISTER VX != kk
    void OP_5xy0();
    void OP_6xkk();
    void OP_7xkk();
    void OP_8xy0();
    void OP_8xy1();
    void OP_8xy2();
    void OP_8xy3();
    void OP_8xy4();
    void OP_8xy5();
    void OP_8xy6();
    void OP_8xy7();
    void OP_8xyE();
    void OP_9xy0();
    void OP_Annn();
    void OP_Bnnn();
    void OP_Cxkk();
    void OP_Dxyn();
    void OP_Ex9E();
    void OP_ExA1();
    void OP_Fx07();
    void OP_Fx0A();
    void OP_Fx15();
    void OP_Fx18();
    void OP_Fx1E();
    void OP_Fx29();
    void OP_Fx33();
    void OP_Fx55();
    void OP_Fx65();
    void SAVE_STATE();

};
