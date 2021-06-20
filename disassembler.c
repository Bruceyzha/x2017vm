#include <stdio.h>
#include "x2017.h"

void print_asm(uint24 *program_code, int len)
{
    for (int i = 0; i < len; i++)
    {
        if ((program_code[i].uint8[0] >> 4) == 1)
        {
            printf("FUNC LABEL %d\n", program_code[i].uint8[0] - 0x10);
            continue;
        }
        unsigned char opcode = program_code[i].uint8[0] >> 5;
        int op_control = op_num(opcode);
        printf("    ");
        if (opcode == OP_MOV)
        {
            printf("MOV ");
        }
        else if (opcode == OP_CAL)
        {
            printf("CAL ");
        }
        else if (opcode == OP_RET)
        {
            printf("RET");
        }
        else if (opcode == OP_REF)
        {
            printf("REF ");
        }
        else if (opcode == OP_ADD)
        {
            printf("ADD ");
        }
        else if (opcode == OP_PRINT)
        {
            printf("PRINT ");
        }
        else if (opcode == OP_NOT)
        {
            printf("NOT ");
        }
        else if (opcode == OP_EQU)
        {
            printf("EQU ");
        }
        if (op_control > 0)
        {
            if (((program_code[i].uint8[0] & 0x0c) >> 2) == VT_VAL)
            {
                printf("VAL %d", program_code[i].uint8[1]);
            }
            else if (((program_code[i].uint8[0] & 0x0c) >> 2) == VT_REG)
            {
                printf("REG %d", program_code[i].uint8[1]);
            }
            else if (((program_code[i].uint8[0] & 0x0c) >> 2) == VT_STK)
            {
                if (program_code[i].uint8[1] < 26)
                {
                    printf("STK %c", program_code[i].uint8[1] + 65);
                }
                else
                {
                    printf("STK %c", program_code[i].uint8[1] + 97);
                }
            }
            else if (((program_code[i].uint8[0] & 0x0c) >> 2) == VT_PTR)
            {
                if (program_code[i].uint8[1] < 26)
                {
                    printf("PTR %c", program_code[i].uint8[1] + 65);
                }
                else
                {
                    printf("PTR %c", program_code[i].uint8[1] + 97);
                }
            }
        }
        if (op_control == 2)
        {
            printf(" ");
            if ((program_code[i].uint8[0] & 0x03) == VT_VAL)
            {
                printf("VAL %d", program_code[i].uint8[2]);
            }
            else if ((program_code[i].uint8[0] & 0x03) == VT_REG)
            {
                printf("REG %d", program_code[i].uint8[2]);
            }
            else if ((program_code[i].uint8[0] & 0x03) == VT_STK)
            {
                if (program_code[i].uint8[2] < 26)
                {
                    printf("STK %c", program_code[i].uint8[2] + 65);
                }
                else
                {
                    printf("STK %c", program_code[i].uint8[2] + 97);
                }
            }
            else if ((program_code[i].uint8[0] & 0x03) == VT_PTR)
            {
                if (program_code[i].uint8[1] < 26)
                {
                    printf("PTR %c", program_code[i].uint8[2] + 65);
                }
                else
                {
                    printf("PTR %c", program_code[i].uint8[2] + 97);
                }
            }
        }
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    unsigned char len = 0;
    if (argc != 2)
    {
        printf("The number of arguement is uncorrect!\n");
        return 1;
    }
    unsigned char buffer[256];
	if (read_file(buffer,argv[1], &len) == -1)
	{
		printf("File name is uncorrect!\n");
		return 1;
	}
	int program_len = 0;
	uint24 reverse_code[256]; 
	if(decoder(buffer, len, &program_len,reverse_code) == -1){
		return 1;
	}
	uint24 program_code[256]; 
	if(reverse(reverse_code,program_code,program_len, NULL) == 1)
	{
		return 1;
	}
    print_asm(program_code, program_len);
    return 0;
}