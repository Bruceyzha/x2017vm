#include <stdio.h>

#define SetBit(A,k)     ( A |= (1 << (7-k)) )
#define ClearBit(A,k)   ( A &= ~(1 << (7-k)) )
#define TestBit(A,k)    ( A[(k/8)] & (1 << (7-k%8)))
#define MAX_MEM 256
#define REG_NUM 8

enum{ //register typedef
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_xx, //unused register
    R_EBP,
    R_ESP,
    R_PC
};

enum { //opcode typedef
    OP_MOV = 0,
    OP_CAL,
    OP_RET,
    OP_REF,
    OP_ADD,
    OP_PRINT,
    OP_NOT,
    OP_EQU
};

enum { //arg type typedef
    VT_VAL = 0,
    VT_REG,
    VT_STK,
    VT_PTR
};

typedef struct{
    unsigned char uint8[3];
}uint24;
/*aligned instruction format
first byte: 000        0      00    00
            opcode label bit type1 type2
if the instruction is label, the label bit 
is 1. If the instruction do not have type 
or only contain 1, the others will be filled 
with 0.
The other two bytes is the value or filled with 0*/

typedef struct {
    unsigned char* buffer;
    int len;
    int index;
}argument;//common argument in several functions




int read_file(unsigned char* buffer,char *file_name, unsigned char *len);
int decoder(unsigned char *buffer, int len, int *program_len,uint24 *program_code);
void write_bit(unsigned char* buffer, int buf_pos, unsigned char* target, int tar_pos);
int write_instruction(uint24 *program_code, argument arg);
int vt_num(unsigned char value_type);
int op_num(unsigned char opcode);
int wirte_argument(uint24* program_code, argument arg, int num);
int reverse(uint24 *reverse_code,uint24 *program_code,int program_len, unsigned char *pc_start);

