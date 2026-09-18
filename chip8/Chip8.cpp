#include <iostream>
#include <fstream>
#include <iomanip>
#include "chip8.h"

Chip8::Chip8()
{
	for (std::size_t i{ 0 }; i < font.size(); i++)
	{
		mem[FONT_START + i] = font[i];
	}
}

bool Chip8::loadROM(const std::string& filepath)
{
	std::ifstream file{ filepath, std::ios::binary };

	if (!file)
	{
		std::cout << "File not found or unable to open file.\n";
		return false;
	}

	file.seekg(0, std::ios::end);
	const std::streamsize size{ file.tellg() };
	file.seekg(0, std::ios::beg);

	if (size > static_cast<std::streamsize>(mem.size() - ROM_START))
	{
		std::cout << "File size too big.\n";
		return false;
	}

	file.read(reinterpret_cast<char*>(&mem[ROM_START]), size);

	if (file.gcount() < size)
	{
		std::cout << "File read failed.\n";
		return false;
	}

	return true;
}

void Chip8::dumpMemory(uint16_t pos, int count) const
{
	for (int i{ 0 }; i < count; i++)
	{
		std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(mem[pos + i]) << ' ';
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