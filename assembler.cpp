#include <iostream>

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include "assembler.hpp"

namespace comp {
	constexpr std::uint8_t OP_SENTINEL = std::uint8_t(-200);
	
	enum IMMEDIATE_TYPES { IB = -1, IW = -2, ID = -3 };
	enum SLASHES {
		SLASH_ZERO = -4, SLASH_ONE = -5, SLASH_TWO = -6,
		SLASH_THREE = -7, SLASH_FOUR = -8, SLASH_FIVE = -9,
		SLASH_SIX = -10, SLASH_SEVEN = -11,
		SLASH_R = -12
	};
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

	static const char err_message[50] = "";

	struct instr { const char* full_str; std::uint8_t op[15]{}; };
	static const instr instr_list[58] = {
		{ "aaa,imm8,imm8", { 0x37, OP_SENTINEL } },
		{ "aad,imm8,imm8", { 0xD5, 0x0A, OP_SENTINEL }},
		{ "aam,imm8,imm8", { 0xD4, 0x0A, OP_SENTINEL }},
		{ "aas,imm8,imm8", { 0x3F, OP_SENTINEL }},

		{ "adc,al,imm8", { 0x14, IB, OP_SENTINEL }},
		{ "adc,ax,imm16", { 0x15, IW, OP_SENTINEL }},
		{ "adc,eax,imm32", { 0x15, ID, OP_SENTINEL }},

		{ "add,al,imm8", { 0x04, IB, OP_SENTINEL }},
		{ "add,ax,imm16", { 0x05, IW, OP_SENTINEL }},
		{ "add,eax,imm32", { 0x05, ID, OP_SENTINEL }},
		{ "add,rm8,imm8", { 0x80, SLASH_ZERO, IB, OP_SENTINEL }},
		{ "add,rm16,imm16", { 0x81, SLASH_ZERO, IW, OP_SENTINEL }},
		{ "add,rm32,imm32", { 0x81, SLASH_ZERO, ID, OP_SENTINEL }},
		{ "add,rm16,imm8", { 0x83, SLASH_ZERO, IB, OP_SENTINEL }},
		{ "add,rm32,imm8", { 0x83, SLASH_ZERO, IB, OP_SENTINEL }},
		{ "add,rm8,r8", { 0x00, SLASH_R, OP_SENTINEL }},
		{ "add,rm16,r16", { 0x01, SLASH_R, OP_SENTINEL }},
		{ "add,rm32,r32", { 0x01, SLASH_R, OP_SENTINEL }},
		{ "add,r8,rm8", { 0x02, SLASH_R, OP_SENTINEL }},
		{ "add,r16,rm16", { 0x03, SLASH_R, OP_SENTINEL }},
		{ "add,r32,rm32", { 0x03, SLASH_R, OP_SENTINEL }},

		{ "sub,al,imm8", { 0x2C, IB, OP_SENTINEL }},
		{ "sub,ax,imm16", { 0x2D, IW, OP_SENTINEL }},
		{ "sub,eax,imm32", { 0x2D, ID, OP_SENTINEL }},
		{ "sub,rm8,imm8", { 0x80, SLASH_FIVE, IB, OP_SENTINEL }},
		{ "sub,rm16,imm16", { 0x81, SLASH_FIVE, IW, OP_SENTINEL }},
		{ "sub,rm32,imm32", { 0x81, SLASH_FIVE, ID, OP_SENTINEL }},

		{ "int,imm8,imm8", { 0xCD, IB, OP_SENTINEL }},

		{ "mov,rm8,r8", { 0x88, SLASH_R, OP_SENTINEL }},
		{ "mov,rm16,r16", { 0x89, SLASH_R, OP_SENTINEL }},
		{ "mov,rm32,r32", { 0x89, SLASH_R, OP_SENTINEL }},
		{ "mov,r8,rm8", { 0x8A, SLASH_R, OP_SENTINEL }},
		{ "mov,r16,rm16", { 0x8B, SLASH_R, OP_SENTINEL }},
		{ "mov,r32,rm32", { 0x8B, SLASH_R, OP_SENTINEL }},

		{ "mov,al,imm8", { 0xB0 + 0, DAT_IMM8, OP_SENTINEL}},
		{ "mov,cl,imm8", { 0xB0 + 1, OP_SENTINEL }},
		{ "mov,dl,imm8", { 0xB0 + 2, OP_SENTINEL }},
		{ "mov,bl,imm8", { 0xB0 + 3, OP_SENTINEL }},
		{ "mov,ah,imm8", { 0xB0 + 4, OP_SENTINEL }},
		{ "mov,ch,imm8", { 0xB0 + 5, OP_SENTINEL}},
		{ "mov,dh,imm8", { 0xB0 + 6, OP_SENTINEL}},
		{ "mov,bh,imm8", { 0xB0 + 7, OP_SENTINEL}},

		{ "mov,ax,imm16", { 0xB8 + 0, OP_SENTINEL}},
		{ "mov,cx,imm16", { 0xB8 + 1, OP_SENTINEL}},
		{ "mov,dx,imm16", { 0xB8 + 2, OP_SENTINEL}},
		{ "mov,bx,imm16", { 0xB8 + 3, OP_SENTINEL}},
		{ "mov,sp,imm16", { 0xB8 + 4, OP_SENTINEL}},
		{ "mov,bp,imm16", { 0xB8 + 5, OP_SENTINEL}},
		{ "mov,si,imm16", { 0xB8 + 6, OP_SENTINEL}},
		{ "mov,di,imm16", { 0xB8 + 7, OP_SENTINEL}},

		{ "mov,eax,imm32", { 0xB8 + 0, OP_SENTINEL}},
		{ "mov,ecx,imm32", { 0xB8 + 1, OP_SENTINEL}},
		{ "mov,edx,imm32", { 0xB8 + 2, OP_SENTINEL}},
		{ "mov,ebx,imm32", { 0xB8 + 3, OP_SENTINEL}},
		{ "mov,esp,imm32", { 0xB8 + 4, OP_SENTINEL}},
		{ "mov,ebp,imm32", { 0xB8 + 5, OP_SENTINEL}},
		{ "mov,esi,imm32", { 0xB8 + 6, OP_SENTINEL}},
		{ "mov,edi,imm32", { 0xB8 + 7, OP_SENTINEL}},
	};

	static int categorize_number(long num) {
		if (num <= UINT8_MAX)
			return DAT_IMM8;
		else if (num <= UINT16_MAX)
			return DAT_IMM16;
		else if (num <= UINT32_MAX)
			return DAT_IMM32;
		else
			return DAT_ERR;
	}
	static int dat_type(const char* dat) {
		if (!dat) return DAT_ERR;

		if (std::strcmp(dat, "al") == 0) return DAT_AL;
		else if (std::strcmp(dat, "cl") == 0) return DAT_CL;
		else if (std::strcmp(dat, "dl") == 0) return DAT_DL;
		else if (std::strcmp(dat, "ah") == 0) return DAT_AH;
		else if (std::strcmp(dat, "ch") == 0) return DAT_CH;
		else if (std::strcmp(dat, "dh") == 0) return DAT_DH;
		else if (std::strcmp(dat, "bh") == 0) return DAT_BH;

		else if (std::strcmp(dat, "ax") == 0) return DAT_AX;

		else if (std::strcmp(dat, "eax") == 0) return DAT_EAX;
		else if (std::strcmp(dat, "ecx") == 0) return DAT_ECX;
		else if (std::strcmp(dat, "edx") == 0) DAT_EDX;
		else if (std::strcmp(dat, "ebx") == 0) DAT_EBX;
		else if (std::strcmp(dat, "esp") == 0) DAT_ESP;
		else if (std::strcmp(dat, "ebp") == 0) DAT_EBP;
		else if (std::strcmp(dat, "esi") == 0) DAT_ESI;
		else if (std::strcmp(dat, "edi") == 0) DAT_EDI;

		
		else if (std::strcmp(dat, "ax") == 0) return DAT_AX;
		else if (std::strcmp(dat, "al") == 0) return DAT_AL;
		else if (std::strcmp(dat, "ebx") == 0 ||
				std::strcmp(dat, "ecx") == 0 ||
				std::strcmp(dat, "edx") == 0 ||
				std::strcmp(dat, "ebp") == 0 ||
				std::strcmp(dat, "esp") == 0 ||
				std::strcmp(dat, "esi") == 0 ||
				std::strcmp(dat, "edi") == 0)
				return DAT_R32;
		else if (std::strcmp(dat, "bx") == 0 ||
			std::strcmp(dat, "cx") == 0 ||
			std::strcmp(dat, "dx") == 0 ||
			std::strcmp(dat, "bp") == 0 ||
			std::strcmp(dat, "sp") == 0 ||
			std::strcmp(dat, "si") == 0 ||
			std::strcmp(dat, "di") == 0)
			return DAT_R16;
		else if (std::strcmp(dat, "ah") == 0 ||
			std::strcmp(dat, "bh") == 0 ||
			std::strcmp(dat, "ch") == 0 ||
			std::strcmp(dat, "dh") == 0 ||
			std::strcmp(dat, "bl") == 0 ||
			std::strcmp(dat, "cl") == 0 ||
			std::strcmp(dat, "dl") == 0)
			return DAT_R8;
		else if (std::isdigit(dat[0]))
			return categorize_number(std::strtol(dat, nullptr, 0));
		else
			return DAT_ERR;
	}

	static int category_to_str(char* dest, int dat) {
		if (!dest) return -1;
		switch (dat) {
		case DAT_ERR:
			return -1;
		case DAT_R8:
			std::strcpy(dest, "r8");
			break;
		case DAT_R16:
			std::strcpy(dest, "r16");
			break;
		case DAT_R32:
			std::strcpy(dest, "r32");
			break;
		case DAT_RM8:
			std::strcpy(dest, "rm8");
			break;
		case DAT_RM16:
			std::strcpy(dest, "rm16");
			break;
		case DAT_RM32:
			std::strcpy(dest, "rm32");
			break;
		case DAT_IMM8:
			std::strcpy(dest, "imm8");
			break;
		case DAT_IMM16:
			std::strcpy(dest, "imm16");
			break;
		case DAT_IMM32:
			std::strcpy(dest, "imm32");
			break;
		case DAT_EAX:
			std::strcpy(dest, "eax");
			break;
		case DAT_AX:
			std::strcpy(dest, "ax");
			break;
		case DAT_AL:
			std::strcpy(dest, "al");
			break;
		default:
			return -1;
		}
		return 0;
	}

	static int combine_instr(char* dest, const char* op, int dat1, int dat2) {
		if (!dest || !op) return -1;

		char dat1_str[50];
		char dat2_str[50];

		if (category_to_str(dat1_str, dat1) == -1) return -1;
		if (category_to_str(dat2_str, dat2) == -1) return -1;

		if (std::snprintf(dest, 500, "%s,%s,%s", op, dat1_str, dat2_str) < 0) return -1;
		return 0;
	}

	static std::size_t find_instr(const char* str) {
		if (!str) return -1;
		for (std::size_t i = 0; i < sizeof(instr_list) / sizeof(instr); i++)
			if (std::strcmp(instr_list[i].full_str, str) == 0) return i;
		return -1;
	}

	static int combine_find(size_t* index, const char* op, int dat1, int dat2) {
		if (!index || !op) return -1;
		char monster[500];
		if (combine_instr(monster, op, dat1, dat2) == -1) return -1;
		*index = find_instr(monster);
		return 0;
	}

	/* Returns a nonzero number if dat is a register */
	static bool is_register(int dat) {
		switch (dat) {
		case DAT_R8:
		case DAT_R16:
		case DAT_R32:
			return true;
		default:
			return false;
		}
	}

	/* Checks if type is a immediate */
	static bool is_immediate(int dat) {
		switch (dat) {
		case DAT_IMM8:
		case DAT_IMM16:
		case DAT_IMM32:
			return true;
		default:
			return false;
		}
	}

	static std::size_t find_best_match(const char* op, int &dat1, int &dat2) {
		if (!op) return -1;

		std::size_t index = -1;
		if (combine_find(&index, op, dat1, dat2) == -1) return -1;
		if (index == -1) { 
			/* Repeat for finding matches for dat2 */
			switch (dat2) {
			case DAT_IMM8: {
				if (combine_find(&index, op, dat1, DAT_IMM16) == -1) return -1;
				dat2 = DAT_IMM16;
				if (index != -1) break;
			}
			case DAT_IMM16: {
				if (combine_find(&index, op, dat1, DAT_IMM32) == -1) return -1;
				dat2 = DAT_IMM32;
			} break;
			case DAT_R8: {
				if (combine_find(&index, op, dat1, DAT_RM8) == -1) return -1;
				dat2 = DAT_R8;
			} break;
			case DAT_R16: {
				if (combine_find(&index, op, dat1, DAT_RM16) == -1) return -1;
				dat2 = DAT_R16;
			} break;
			case DAT_R32: {
				if (combine_find(&index, op, dat1, DAT_RM32) == -1) return -1;
				dat2 = DAT_R32;
			} break;
			case DAT_AL: {
				int cate = DAT_R8;
				return find_best_match(op, dat1, cate);
			} break;
			case DAT_AX: {
				int cate = DAT_R16;
				return find_best_match(op, dat1, cate);
			} break;
			case DAT_EAX: {
				int cate = DAT_R32;
				return find_best_match(op, dat1, cate);
			} break;
			}
			if (index != (std::size_t) -1)
				return index;

			switch (dat1) {
			case DAT_IMM8: {
				if (combine_find(&index, op, DAT_IMM16, dat2) == -1) return -1;
				dat1 = DAT_IMM16;
				if (index != (std::size_t)-1) break;
			}
			case DAT_IMM16: {
				if (combine_find(&index, op, DAT_IMM32, dat2) == -1) return -1;
				dat1 = DAT_IMM32;
			} break;
			case DAT_R8: {
				if (combine_find(&index, op, DAT_RM8, dat2) == -1) return -1;
				if (index == (std::size_t) -1 && is_immediate(dat2)) { if (combine_find(&index, op, DAT_RM8, DAT_IMM8) == -1) return -1; dat2 = DAT_IMM8; }
				dat1 = DAT_R8;
			} break;
			case DAT_R16: {
				if (combine_find(&index, op, DAT_RM16, dat2) == -1) return -1;
				if (index == (std::size_t) -1 && is_immediate(dat2)) { if (combine_find(&index, op, DAT_RM16, DAT_IMM16) == -1) return -1; dat2 = DAT_IMM16; }
				dat1 = DAT_R16;
			} break;
			case DAT_R32: {
				if (combine_find(&index, op, DAT_RM32, dat2) == -1) return -1;
				if (index == (std::size_t) -1 && is_immediate(dat2)) { if (combine_find(&index, op, DAT_RM32, DAT_IMM32) == -1) return -1; dat2 = DAT_IMM32; }
				dat1 = DAT_R32;
			} break;
			case DAT_AL: {
				int cate = DAT_R8;
				return find_best_match(op, cate, dat2);
			} break;
			case DAT_AX: {
				int cate = DAT_R16;
				return find_best_match(op, cate, dat2);
			} break;
			case DAT_EAX: {
				int cate = DAT_R32;
				return find_best_match(op, cate, dat2);
			} break;
			}
		}
		return index;
	}

	static std::uint8_t reg_to_num(const char* reg) {
		if (!reg) return -1;
		/* There is a better way to do this in C++ (std::strings) */
		if (std::strcmp(reg, "al") == 0 ||
			std::strcmp(reg, "ax") == 0 ||
			std::strcmp(reg, "eax") == 0) return 0b000;
		else if (std::strcmp(reg, "bl") == 0 ||
			std::strcmp(reg, "bx") == 0 ||
			std::strcmp(reg, "ebx") == 0) return 0b011;
		else if (std::strcmp(reg, "cl") == 0 ||
			std::strcmp(reg, "cx") == 0 ||
			std::strcmp(reg, "ecx") == 0) return 0b001;
		else if (std::strcmp(reg, "dl") == 0 ||
			std::strcmp(reg, "dx") == 0 ||
			std::strcmp(reg, "edx") == 0) return 0b010;
		else if (std::strcmp(reg, "ah") == 0 ||
			std::strcmp(reg, "sp") == 0 ||
			std::strcmp(reg, "esp") == 0) return 0b100;
		else if (std::strcmp(reg, "ch") == 0 ||
			std::strcmp(reg, "bp") == 0 ||
			std::strcmp(reg, "ebp") == 0) return 0b101;
		else if (std::strcmp(reg, "dh") == 0 ||
			std::strcmp(reg, "si") == 0 ||
			std::strcmp(reg, "esi") == 0) return 0b110;
		else if (std::strcmp(reg, "bh") == 0 ||
			std::strcmp(reg, "di") == 0 ||
			std::strcmp(reg, "edi") == 0) return 0b111;
		else
			return -1;
	}

	static int process_slash(std::uint8_t &modrm, std::uint8_t slash_num, const char *reg) {
		if (!reg) return -1;
		/* [Mod] [\digit] [R/M] */
		modrm = 0b11'000'000;

		/* slash_num looks like this: 0000'0111 */
		/* So we need to shift the bits 3 to the left */
		slash_num <<= 3;
		modrm |= slash_num;
		
		std::uint8_t regnum = reg_to_num(reg);
		if (regnum == (std::uint8_t)-1) return -1;
		modrm |= regnum;

		return 0;
	}

	static int output_asm(std::FILE* dest, std::size_t list_index, 
				const char *dat1, const char *dat2, int dat1_info, int dat2_info) {
		if (!dest) return -1;
		const std::uint8_t* arr = instr_list[list_index].op;
		for (int i = 0; i < sizeof(instr_list[list_index].op) / sizeof(int); i++) {
			std::int8_t num = arr[i];
			switch (num) {
			case OP_SENTINEL:
				goto END;
			case IB: {
				/* MOD R / M Byte : [Mod] [Register] [R / M] [Immediate] */
				std::uint8_t num;
				if (dat1_info == DAT_IMM8)
					num = (std::uint8_t)std::strtol(dat1, nullptr, 0);
				else if (dat2_info == DAT_IMM8)
					num = (std::uint8_t)std::strtol(dat2, nullptr, 0);
				else
					return -1;
				std::fwrite(&num, sizeof(num), 1, dest);
			} break;
			case IW: {
				std::uint16_t num;
				if (dat1_info == DAT_IMM16)
					num = (std::uint16_t)std::strtol(dat1, nullptr, 0);
				else if (dat2_info == DAT_IMM16)
					num = (std::uint16_t)std::strtol(dat2, nullptr, 0);
				else
					return -1;
				std::fwrite(&num, sizeof(num), 1, dest);
				break;
			}
			case ID: {
				std::uint32_t num;
				if (dat1_info == DAT_IMM32)
					num = (std::uint32_t)std::strtol(dat1, nullptr, 0);
				else if (dat2_info == DAT_IMM32)
					num = (std::uint32_t)std::strtol(dat2, nullptr, 0);
				else
					return -1;
				std::fwrite(&num, sizeof(num), 1, dest);
			} break;
			case SLASH_R: {
				/* Means that Mod R/M [Mod] [Reg] [R/M] */
				/* Check which one is R8/R16/R32 */
				if (is_register(dat1_info)) {
					/* If dat1_info is a register, then dat2_info is not (according to intel specs) */
					std::uint8_t reg1 = reg_to_num(dat1) << 3;
					std::uint8_t reg2 = reg_to_num(dat2);
					if (reg1 == (std::uint8_t)-1 || reg2 == (std::uint8_t)-1) return -1;

					std::uint8_t modrm = 0b11'000'000;
					modrm |= reg1;
					modrm |= reg2;
					std::fwrite(&modrm, sizeof(modrm), 1, dest);
				}
				else if (is_register(dat2_info)) {
					std::uint8_t reg1 = reg_to_num(dat1);
					std::uint8_t reg2 = reg_to_num(dat2) << 3;
					if (reg1 == -1 || reg2 == -1) return -1;

					std::uint8_t modrm = 0b11'000'000;
					modrm |= reg1;
					modrm |= reg2;
					std::fwrite(&modrm, sizeof(modrm), 1, dest);
				}
				else {
					return -1;
				}
			} break;
			case SLASH_ZERO:
			case SLASH_ONE:
			case SLASH_TWO:
			case SLASH_THREE:
			case SLASH_FOUR:
			case SLASH_FIVE:
			case SLASH_SIX:
			case SLASH_SEVEN: {
				const char* reg = !is_immediate(dat1_info) ? dat1 : dat2;
				std::uint8_t modrm;
				process_slash(modrm, std::abs(num) - 4, reg);
				std::fwrite(&modrm, sizeof(modrm), 1, dest);
			} break;
			default: {
				std::fwrite(&num, sizeof(num), 1, dest);
				break;
			}
			}
		}
	END:
		return 0;
	}

	static int produce_bootloader(std::FILE* dest) {
		if (!dest) return -1;
		
		std::uint16_t boot_code = std::uint16_t(0xaa55);
		std::size_t bytes_written = std::ftell(dest);
		std::size_t writeb = 510 - bytes_written;
		std::uint8_t zeros[510] = { 0 };
		
		if (std::fwrite(zeros, sizeof(std::uint8_t), writeb, dest) < writeb) return -1;
		std::fwrite(&boot_code, sizeof(boot_code), 1, dest);

		std::cout << "Bytes written: " << bytes_written << std::endl;
		return 0;
	}

	int assemble(std::FILE* dest, std::FILE* src) noexcept {
		if (!dest || !src) return -1;
		do {
			/* Format: [opcode] [Mod R/M] [SIB (not supported yet)] [immediate] */
			char opcode[50];
			char dat1[50];
			char dat2[50];
			if (std::fscanf(src, "%49s %49s %49s", opcode, dat1, dat2) == EOF) break;

			int dat1_categor = dat_type(dat1);
			int dat2_categor = dat_type(dat2);

			std::size_t index;
			if ((index = find_best_match(opcode, dat1_categor, dat2_categor)) == (std::size_t) -1) return -1;
			if (output_asm(dest, index, dat1, dat2, dat1_categor, dat2_categor) == -1) return -1;
			
			std::cout << "Best match is " << index << "!\n";
		} while (true);

		if (produce_bootloader(dest) == -1) return -1;

		return 0;
	}
	const char* asmerr() noexcept {
		return err_message;
	}
}