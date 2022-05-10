#include "assembler.hpp"

int main() {
	std::FILE* dest = std::fopen("destasm.bin", "w");
	if (!dest) return -1;
	std::FILE* src = std::fopen("src.assem", "r");
	if (!src) { std::fclose(dest); return -1; }

	int ret_code = 0;
	if (comp::assemble(dest, src) == -1) ret_code = -1;

	std::fclose(dest);
	std::fclose(src);

	return ret_code;
}

