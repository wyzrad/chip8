#include <iostream>
#include "chip8.h"

Chip8::Chip8()
{
	for (std::size_t i{ 0 }; i < font.size(); i++)
	{
		mem[FONT_START + i] = font[i];
	}
}

void Chip8::debugDump() const
{
	std::cout << "pc:" << std::hex << pc << std::dec << '\n';
	std::cout << "address register: " << std::hex << I << std::dec << '\n';
	std::cout << "stack pointer: " << static_cast<int>(sp) << '\n';

	for (std::size_t i{ 0 }; i < V.size(); i++)
	{
		std::cout << "V[" << i << "]: " << static_cast<int>(V[i]) << '\n';
	}

	std::cout << "st: " << static_cast<int>(st) << '\n';
	std::cout << "dt:" << static_cast<int>(dt) << '\n';
}