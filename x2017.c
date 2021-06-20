#include <stdio.h>
#include "x2017.h"

unsigned char ram[MAX_MEM];//RAM - Contains 28 addresses of 1 byte each
unsigned char reg[REG_NUM];//Register Bank - 8 registers of 1 byte each

int write_stack(unsigned char index, unsigned char value)//The ram memory write and control reg
{
	ram[reg[R_EBP] + index] = value;
	if (reg[R_EBP] + index > reg[R_ESP])
	{
		reg[R_ESP]++;
	}
	return 0;
}

int main(int argc, char *argv[])
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
	//parse binary and get a reverse program code
	if(decoder(buffer, len, &program_len,reverse_code) == -1)
	{
		return 1;
	}
	uint24 program_code[256]; 
	reg[R_PC] = 255;
	//reverse program code and error handle
	if(reverse(reverse_code,program_code,program_len, &reg[R_PC]) == 1)
	{
		return 1;
	}
	if(reg[R_PC] == 255){
		printf("No entry point!\n");
		return 1;
	}
	while (1)
	{
		if (reg[R_ESP] == 255)
		{
			printf("out of memory: stack overflow\n");
			break;
		}
		reg[R_PC]++;
		if (reg[R_PC] >= program_len)
		{
			printf("Program conter value overflow\n");
			break;
		}
		switch (program_code[reg[R_PC]].uint8[0] >> 5)
		{
		case OP_MOV:
		{
			if ((program_code[reg[R_PC]].uint8[0] & 0x03) == VT_REG)
			{
				if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_REG)
				{
					reg[program_code[reg[R_PC]].uint8[1]] = reg[program_code[reg[R_PC]].uint8[2]];
				}
				else if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_STK)
				{
					write_stack(program_code[reg[R_PC]].uint8[1], reg[program_code[reg[R_PC]].uint8[2]]);
				}
				else if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_PTR)
				{
					ram[ram[program_code[reg[R_PC]].uint8[1] + reg[R_EBP]]] = reg[program_code[reg[R_PC]].uint8[2]];
				}
			}
			else if ((program_code[reg[R_PC]].uint8[0] & 0x03) == VT_STK)
			{
				if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_REG)
				{
					reg[program_code[reg[R_PC]].uint8[1]] = ram[reg[R_EBP] + program_code[reg[R_PC]].uint8[2]];
				}
				else if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_STK)
				{
					write_stack(program_code[reg[R_PC]].uint8[1], ram[reg[R_EBP] + program_code[reg[R_PC]].uint8[2]]);
				}
				else if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_PTR)
				{
					ram[ram[program_code[reg[R_PC]].uint8[1] + reg[R_EBP]]] = ram[reg[R_EBP] + program_code[reg[R_PC]].uint8[2]];
				}
			}
			else if ((program_code[reg[R_PC]].uint8[0] & 0x03) == VT_PTR)
			{
				if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_REG)
				{
					reg[program_code[reg[R_PC]].uint8[1]] = ram[ram[reg[R_EBP] + program_code[reg[R_PC]].uint8[2]]];
				}
				else if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_STK)
				{
					write_stack(program_code[reg[R_PC]].uint8[1], ram[ram[reg[R_EBP] + program_code[reg[R_PC]].uint8[2]]]);
				}
				else if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_PTR)
				{
					ram[ram[program_code[reg[R_PC]].uint8[1] + reg[R_EBP]]] = ram[ram[reg[R_EBP] + program_code[reg[R_PC]].uint8[2]]];
				}
			}
			else
			{
				if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_REG)
				{
					reg[program_code[reg[R_PC]].uint8[1]] = program_code[reg[R_PC]].uint8[2];
				}
				else if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_STK)
				{
					write_stack(program_code[reg[R_PC]].uint8[1], program_code[reg[R_PC]].uint8[2]);
				}
				else if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_PTR)
				{
					ram[ram[program_code[reg[R_PC]].uint8[1]]] = program_code[reg[R_PC]].uint8[2];
				}
			}
		}
		break;

		case OP_CAL:
		{
			for (int i = 0; i < program_len; i++)
			{
				if ((program_code[i].uint8[0] - 16) == program_code[reg[R_PC]].uint8[1])
				{
					write_stack(reg[R_ESP] + 1 - reg[R_EBP], reg[R_PC]);
					write_stack(reg[R_ESP] + 1 - reg[R_EBP], reg[R_EBP]);
					reg[R_ESP]++;
					reg[R_EBP] = reg[R_ESP];
					reg[R_PC] = i;
					break;
				}
				if(i == program_len - 1){
					printf("Can not find cal function.\n");
					return 1;
				}
			}
		}
		break;
		case OP_RET:
		{
			if (reg[R_EBP] == 0)
			{
				return 0;
			}
			reg[R_ESP] = reg[R_EBP] - 1;
			reg[R_EBP] = ram[reg[R_ESP]];
			reg[R_PC] = ram[reg[R_ESP] - 1];
			reg[R_ESP] -= 2;
		}
		break;
		case OP_REF:
		{
			if ((program_code[reg[R_PC]].uint8[0] & 0x03) == VT_STK)
			{
				if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_REG)
				{
					reg[program_code[reg[R_PC]].uint8[1]] = reg[R_EBP] + program_code[reg[R_PC]].uint8[2];
				}
				else if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_STK)
				{
					write_stack(program_code[reg[R_PC]].uint8[1], reg[R_EBP] + program_code[reg[R_PC]].uint8[2]);
				}
				else if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_PTR)
				{
					ram[ram[program_code[reg[R_PC]].uint8[1] + reg[R_EBP]]] = reg[R_EBP] + program_code[reg[R_PC]].uint8[2];
				}
			}
			else if ((program_code[reg[R_PC]].uint8[0] & 0x03) == VT_PTR)
			{
				if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_REG)
				{
					reg[program_code[reg[R_PC]].uint8[1]] = ram[program_code[reg[R_PC]].uint8[2] + reg[R_EBP]];
				}
				else if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_STK)
				{
					write_stack(program_code[reg[R_PC]].uint8[1], ram[program_code[reg[R_PC]].uint8[2] + reg[R_EBP]]);
				}
				else if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_PTR)
				{
					ram[ram[program_code[reg[R_PC]].uint8[1] + reg[R_EBP]]] = ram[program_code[reg[R_PC]].uint8[2] + reg[R_EBP]];
				}
			}
		}
		break;
		case OP_ADD:
		{
			reg[program_code[reg[R_PC]].uint8[1]] = reg[program_code[reg[R_PC]].uint8[1]] + reg[program_code[reg[R_PC]].uint8[2]];
		}
		break;
		case OP_PRINT:
		{
			if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_REG)
			{
				printf("%d\n", reg[program_code[reg[R_PC]].uint8[1]]);
			}
			else if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_STK)
			{
				printf("%d\n", ram[reg[R_EBP] + program_code[reg[R_PC]].uint8[1]]);
			}
			else if (((program_code[reg[R_PC]].uint8[0] & 0x0c) >> 2) == VT_PTR)
			{
				printf("%d\n", ram[ram[reg[R_EBP] + program_code[reg[R_PC]].uint8[1]]]);
			}
			else
			{
				printf("%d\n", program_code[reg[R_PC]].uint8[1]);
			}
		}
		break;
		case OP_NOT:
		{
			reg[program_code[reg[R_PC]].uint8[1]] = ~reg[program_code[reg[R_PC]].uint8[1]];
		}
		break;
		case OP_EQU:
		{
			if (reg[program_code[reg[R_PC]].uint8[1]] == 0)
			{
				reg[program_code[reg[R_PC]].uint8[1]] = 1;
			}
			else
			{
				reg[program_code[reg[R_PC]].uint8[1]] = 0;
			}
		}
		break;

		default:
			break;
		}
	}
	return 1;
}