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
	std::cout << '\n';
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

void Chip8::renderDisplay() const
{
	std::cout << "\033[2J\033[H";

	for (int row{ 0 }; row < 32; row++)
	{
		for (int col{ 0 }; col < 64; col++)
		{
			if (display[(row * 64) + col])
				std::cout << "##";
			else
				std::cout << "  ";
		}

		std::cout << '\n';
	}
}

void Chip8::cycle()
{
	uint16_t opcode{ static_cast<uint16_t>((mem[pc] * 256) + mem[pc + 1]) };
	uint8_t firstNibble{ static_cast<uint8_t>(opcode / 4096) };

	pc += 2;

	switch (firstNibble)
	{
	case 0:
		switch (opcode)
		{
		case 0x00E0:
			// clear screen
			for (bool& pixel : display) pixel = false; break;
		case 0x00EE:
			// return from subroutine
			// set pc to top value of stack, pop value from stack, decrement stack pointer

			if (sp == 0)
			{
				std::cout << "Warning: return from subroutine (stack pop) while stack pointer is at 0.\n";
				break;
			}

			pc = stack[sp - 1];
			sp--;
			break;
		default:
			// either unknown opcode or call machine-code routine opcode, unimplemented
			std::cout << "Unknown operation: " << std::hex << opcode << std::dec << '\n';
			break;
		}
		break;
	case 1:
		// jump, set program counter
		pc = opcode % 4096;
		break;
	case 6:
		// set register V[second hex digit] to last two hex digits
		V[(opcode / 256) % 16] = opcode % 256;
		break;
	case 7:
		// add constant value (last two hex digits) to value already in V[second hex digit]
		V[(opcode / 256) % 16] += opcode % 256;
		break;
	case 0xA:
		// set address register
		I = opcode % 4096;
		break;
	case 0xD:
	{
		// draw sprite to x y coordinate
		uint8_t	x{ V[(opcode / 256) % 16] };
		x = x % 64;
		uint8_t y{ V[(opcode / 16) % 16] };
		y = y % 32;

		// set special register used for flags to 0
		V[0xF] = 0;

		// N many rows, always 8 columns (8 bits)
		for (int row{ 0 }; row <= (opcode % 16) - 1; row++)
		{
			int py{ y + row };
			uint8_t placeValue{ 128 };

			for (int col{ 0 }; col <= 7; col++)
			{
				int px{ x + col };

				// extract the current bit from the byte at the memory index
				int bit{ (mem[I + row] / placeValue) % 2 };
				placeValue /= 2;

				// if off-screen
				if (px >= 64 || py >= 32)
					continue;

				// if a collission happened, bit at location on and draw sprite bit also on
				if (bit && display[(py) * 64 + (px)])
					V[0xF] = 1;

				display[(py) * 64 + (px)] ^= bit;
			}
		}

		break;
	}
	default:
		std::cout << "Unknown operation: " << std::hex << opcode << std::dec << '\n';
		break;
	}
}