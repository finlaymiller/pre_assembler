/*
- Pre-assembler for the X-Makina Project
- ECED3403 Assignment 1
- Headerfile for the pre-assembler's conversion class
- Finlay Miller B00675696
- 03 June 2018
*/

#ifndef CONVERSION_H
#define CONVERSION_H

#define TABLE_SIZE 56

#include "stdio.h"
#include <iostream>
#include <string>
#include <vector>

class conversion
{
private:
	std::string instruction;
	std::string emulation;
	int argType;
	std::string argL;
	std::string argR;
public:
	conversion(std::string i, std::string e, int a, std::string l, std::string r);
	std::string getInstruction() { return instruction; }
	std::string getEmulation() { return emulation; }
	int getArgType() { return argType; }
	std::string getArgL() { return argL; }
	std::string getArgR() { return argR; }
	void write(std::ostream& out) const;
};

std::vector<conversion> generate_table(void);

#endif // !CONVERSION_H