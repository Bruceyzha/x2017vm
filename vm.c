#include <stdio.h>
#include "x2017.h"


int read_file(unsigned char* buffer,char *file_name, unsigned char *len)
{
    if (file_name != NULL)
    {
        FILE *fp = fopen(file_name, "rb+");
        if (fp == NULL)
        {
            return -1;
        }
        fseek(fp, 0, SEEK_END);
        *len = ftell(fp);
        rewind(fp);
        if(fread(buffer, *len, 1, fp) == 0){
            return -1;
        }
        fclose(fp);
    }
    return 0;
}

int decoder(unsigned char *buffer, int len, int *program_len,uint24 *program_code)
{
    if (buffer != NULL)
    {
        *program_len = 0;
        len *= 8;
        for (int i = 0; i < len; i++)
        {
            if ((len - i) < 9)
            {
                return 1;
            }
            unsigned char function_len = 0;
            unsigned char symbol[32] = {0};
            unsigned char num = 0;
            for (int j = 0; j < 5; j++)
            {
                write_bit(buffer, len - i - 1 - j, &function_len, 7 - j);
            }
            if (function_len == 0)
            {
                printf("The number of function is uncorrect!\n");
                return -1;
            }
            i += 5;
            for (int j = *program_len; j < function_len + *program_len; j++)
            {
                argument ins_arg;
                ins_arg.buffer = buffer;
                ins_arg.len = len;
                ins_arg.index = i;
                i += write_instruction(&program_code[j], ins_arg);
            }
            for (int j = function_len + *program_len - 1; j > *program_len - 1; j--)
            {
                for (int pos = 1; pos > -1; pos--)
                {
                    if (((program_code[j].uint8[0] & (0x03 + pos * 9)) >> 2 * pos) > VT_REG)
                    {
                        int tmp = num;
                        for (int exist = 0; exist < tmp; exist++)
                        {
                            if (program_code[j].uint8[2 - pos] == symbol[exist])
                            {
                                program_code[j].uint8[2 - pos] = exist;
                                break;
                            }
                            else if (program_code[j].uint8[2 - pos] != symbol[exist] && exist == num - 1)
                            {
                                symbol[num] = program_code[j].uint8[2 - pos];
                                program_code[j].uint8[2 - pos] = num;
                                num++;
                            }
                        }
                        if (num == 0)
                        {
                            symbol[num] = program_code[j].uint8[2 - pos];
                            program_code[j].uint8[2 - pos] = num;
                            num++;
                        }
                    }
                    else if (((program_code[j].uint8[0] & (0x03 + pos * 9)) >> 2 * pos) == VT_REG)
                    {
                        if(program_code[j].uint8[2 - pos] > R_R3 && program_code[j].uint8[2 - pos] < R_PC)
                        {
                            printf("Regsiter Permission denied\n");
                            return -1;
                        }
                    }
                }
            }
            program_code[function_len + *program_len].uint8[0] = 0b00010000;
            for (int j = 0; j < 3; j++)
            {
                write_bit(buffer, len - i - 1 - j, &program_code[function_len + *program_len].uint8[0], 7 - j);
            }
            i += 2;
            *program_len = function_len + *program_len + 1;
        }
    }
    return 1;
}

void write_bit(unsigned char *buffer, int buf_pos, unsigned char *target, int tar_pos)
{
    if (TestBit(buffer, buf_pos))
    {
        SetBit(*target, tar_pos);
    }
    else
    {
        ClearBit(*target, tar_pos);
    }
}

int write_instruction(uint24 *program_code, argument arg)
{
    int i = 0;
    int op_control = 0;
    program_code->uint8[0] = 0;
    for (; i < 3; i++)
    {
        write_bit(arg.buffer, arg.len - i - arg.index - 1, &program_code->uint8[0], 7 - i);
    }
    program_code->uint8[1] = 0;
    program_code->uint8[2] = 0;
    op_control = op_num(program_code->uint8[0]);
    program_code->uint8[0] <<= 5;
    if (op_control == 0)
    {
        return i;
    }
    for (int j = 0; j < op_control; j++)
    {
        argument arg_arg;
        arg_arg.buffer = arg.buffer;
        arg_arg.len = arg.len;
        arg_arg.index = arg.index + i;
        i = i + wirte_argument(program_code, arg_arg, j + 1);
    }
    return i;
}

int op_num(unsigned char opcode)
{
    if (opcode == OP_MOV || opcode == OP_REF || opcode == OP_ADD)
    {
        return 2;
    }
    else if (opcode == OP_RET)
    {
        return 0;
    }
    else
    {
        return 1;
    }
    return 0;
}

int vt_num(unsigned char value_type)
{
    if (value_type == 0)
    {
        return 8;
    }
    else if (value_type == 1)
    {
        return 3;
    }
    return 5;
}

int wirte_argument(uint24 *program_code, argument arg, int num)
{
    int i = 0;
    unsigned char value_type = 0;
    for (; i < 2; i++)
    {
        write_bit(arg.buffer, arg.len - i - arg.index - 1, &value_type, 7 - i);
    }
    int vt_control = vt_num(value_type);
    if (num == 1)
    {
        value_type <<= 2;
    }
    program_code->uint8[0] |= value_type;
    for (int j = 0; j < vt_control; j++)
    {
        write_bit(arg.buffer, arg.len - i - arg.index - 1 - j, &program_code->uint8[num], 7 - j);
    }
    i += vt_control;
    return i;
}

int reverse(uint24 *reverse_code,uint24 *program_code,int program_len, unsigned char *pc_start)
{
    int ret_check = 0;
    for (int i = 0; i < program_len; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            program_code[i].uint8[j] = reverse_code[program_len - i - 1].uint8[j];
        }
        if (program_code[i].uint8[0] == 0b00010000 && pc_start != NULL)
        {
            *pc_start = i;
        }
        switch (program_code[i].uint8[0] >> 5)
		{
		case OP_MOV:
		{
			if (((program_code[i].uint8[0] & 0x0c) >> 2) == VT_VAL && (program_code[i].uint8[0] >>4) != 1)
			{
                printf("The instruction can not support this type.\n");
                return 1;
			}
		}
		break;

		case OP_CAL:
		{
			if ((program_code[i].uint8[0] & 0x0f) != VT_VAL)
			{
                printf("The instruction can not support this type.\n");
                return 1;
			}
		}
		break;
		case OP_RET:
		{
			ret_check = 1;
            if ((program_code[i].uint8[0] & 0x0f) != VT_VAL)
			{
                printf("The instruction can not support this type.\n");
                return 1;
			}
		}
		break;
		case OP_REF:
		{
			if ((program_code[i].uint8[0] & 0x03) < VT_STK)
			{
                printf("The instruction can not support this type.\n");
                return 1;
			}
		}
		break;
		case OP_ADD:
		{
            if ((program_code[i].uint8[0] & 0x0f) != 0b0101)
			{
                printf("The instruction can not support this type.\n");
                return 1;
			}
		}
		break;
		case OP_PRINT:
		{
            if ((program_code[i].uint8[0] & 0x03) != 0)
			{
                printf("The instruction can not support this type.\n");
                return 1;
			}
		}
		break;
		case OP_NOT:
		{
            if ((program_code[i].uint8[0] & 0x03) != 0)
			{
                printf("The instruction can not support this type.\n");
                return 1;
			}
		}
		break;
		case OP_EQU:
		{
			if ((program_code[i].uint8[0] & 0x03) != 0)
			{
                printf("The instruction can not support this type.\n");
                return 1;
			}
		}
		break;

		default:
			break;
		}
    }
    if(ret_check == 0)
    {
        printf("Function no return.\n");
        return 1;
    }
    return 0;
}