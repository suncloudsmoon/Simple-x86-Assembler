#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <cstdio>

/*
enum DAT_TYPE {
	DAT_ERR = -1, DAT_R8 = 1, DAT_R16 = 2, DAT_R32 = 3,
	DAT_RM8 = 4, DAT_RM16 = 5, DAT_RM32 = 6,
	DAT_IMM8 = 7, DAT_IMM16 = 8, DAT_IMM32 = 9,

	DAT_AL = 10, DAT_CL = 11, DAT_DL = 12, DAT_AH = 13,
	DAT_CH = 14, DAT_DH = 15, DAT_BH = 16,

	DAT_AX = 17, DAT_CX = 18, DAT_DX = 19,
	DAT_BX = 20, DAT_SP = 21, DAT_BP = 22,
	DAT_SI = 23, DAT_DI = 24,

	DAT_EAX = 25, DAT_ECX = 26, DAT_EDX = 27,
	DAT_EBX = 28, DAT_ESP = 29, DAT_EBP = 30,
	DAT_ESI = 31, DAT_EDI = 32
};
*/

namespace comp {
	int assemble(std::FILE* dest, std::FILE* src) noexcept;
	/* char array is 50 in length */
	const char* asmerr() noexcept;
}

#endif /* ASSEMBLER_HPP */