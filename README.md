# x2017vm
使用C语言实现一个名为x2017的虚拟机。该虚拟机分为两部分，解码器与虚拟机。解码器接收一个二进制文件模式的汇编代码，并且通过相应的按位操作将2进制文件转化成为一个24位的汇编数组，并且确定main函数的起始位置进行PC的定位。
虚拟机部分由3部分组成：一个256字节的RAM，8个一字节的寄存器，以及由解码之后的机器码。虚拟机可实现8种不同的指令，包括MOV，CAL，RET, ADD, PRINT, NOT, EQU。通过指令集即可实现复杂的逻辑结构。RAM的使用实现了虚拟机中栈的功能，8个寄存器当中拥有4个通用寄存器，PC寄存器，EBP与ESP寄存器将在内存中实现栈帧，以及一个备用寄存器作为后续功能引入的备用。
通过Makefile的优化使得该虚拟机的可执行文件仅有20个字节，非常小巧。
## 指令集介绍
* MOV：000 - [MOV A B] - Copies the value at some point B in memory to another point A in memory
(register or stack). The destination may not be value typed.
* CAL: 001 - [CAL A] - Calls another function the first argument is a single byte (using the VALUE type)
containing the label of the calling function.
* RET: 010 - [RET] - Terminates the current function, this is guaranteed to always exist at the end of each
function. There may be more than one RET in a function. If this function is the entry-point,
then the program terminates.
* REF: 011 - [REF A B] - Takes a stack symbol B and stores its corresponding stack address in A.
* ADD: 100 - [ADD A B] - Takes two register addresses and ADDs their values, storing the result in the first
listed register.
* PRINT: 101 - [PRINT A] - Takes any address type and prints the contents to a new line of standard output
as an unsigned integer.
* NOT: 110 - [NOT A] - Takes a register address and performs a bitwise not operation on the value at that
address. The result is stored in the same register
* EQU: 111 - [EQU A] - Takes a register address and tests if it equals zero. The value in the register will be
set to 1 if it is 0, or 0 if it is not. The result is stored in the same register.
## 使用规范
借助makefile快速编译。
* 解码：```make objdump_x2017```
* 虚拟机：```make vm_x2017```
* 测试：```make tests```, ```make run_tests```
借助编译工具与strip工具，vm_x2017仅有20字节。
## 测试
提供自助编写测试工具，writetest.c,同时提供部分测试用例，详见testinfo.txt文件
