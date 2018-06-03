// PreAssembler.cpp : Defines the entry point for the console application.
//

/* required libraries */
#include "stdafx.h"
#include "stdio.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <locale>
#include <algorithm>
#include <iterator>

/* global definitions */
#define IFILE_PATH "A:/Documents/GitHub/PreAssembler/input - easy.txt"
#define OFILE_PATH "A:/Documents/GitHub/PreAssembler/output - easy.txt"
#define TABLE_SIZE 56

using namespace std;

/* class definitions*/
class conversion
{
private:
	string instruction;
	string emulation;
	bool hasArgs;
	string argL;
	string argR;
public:
	conversion(string i, string e, bool a, string l, string r)
	{
		instruction = i;
		emulation = e;
		hasArgs = a;
		argL = l;
		argR = r;
	}
	string getInstruction() { return instruction; }
	string getEmulation() { return emulation; }
	bool doesHaveArgs() { return hasArgs; }
	string getArgL() { return argL; }
	string getArgR() { return argR; }
	void write(ostream& out) const;
};

class record
{
private:
	string label;
	string instruction;
    string emulation;
	string argument;
	string comment;
public:
	string getLabel() { return label; }
	string getInstruction() { return instruction; }
	string getArgument() { return argument; }
	string getComment() { return comment; }
	void setLabel(string l) { this->label = l; }
	void setInstruction(string i) { this->instruction = i; }
    void setEmulation(string e) { this->emulation = e; }
	void setArgument(string a) { this->argument = a; }
	void setComment(string c) { this->comment = c; }
	void write(ostream& out) const;
};

/* function definitions */
vector<string> get_input(string);
vector<string> format_record(const char *);
int binary_search(vector<conversion>, string);
void error(string);
vector<conversion> generate_table(void);
vector<string> split(const string&, const string&);
record convert(vector<string>, vector<conversion>);

int main(void)
{
	vector<string> active_r, record_vector_raw;
	vector<conversion> conversion_table = generate_table();
	record rec;
	const char * p_recvec;

	record_vector_raw = get_input(IFILE_PATH);
	size_t s = record_vector_raw.size();

	for (unsigned int current_record_index = 0; current_record_index < s; current_record_index++)
	{
		p_recvec = record_vector_raw[current_record_index].data();
		active_r = format_record(p_recvec);
		rec = convert(active_r, conversion_table);
		rec.write(cout);
		cout << "\n";
	}
	
	getchar();
	return 0;
}

record convert(vector<string> v_rec, vector<conversion> v_conv)
{
	/*
	Purpose:    Contains the logic flow for determining what each token of input string is
	Input:      The record currently being examined, the conversion table (as a vector)
	Output:     Record to be printed
	*/

	record r;
	int bs_result;
	size_t sz = v_rec.size();
	string temp, cmt;
	__int64 j = 0, cmt_found = -1;

	for (vector<string>::iterator i = v_rec.begin(); i != v_rec.end(); i++)
	{
		temp = *i;
		if (temp[0] == ';')
		{
			j = i - v_rec.begin();
			cmt_found = j;
			if (&i == 0) cout << "\nLine found to be entirely comment.\n";
			cout << "Comment Found \n";
			while (j != v_rec.size())
			{
				cmt += v_rec[j] + " ";
				j++;
			}
			r.setComment(cmt);
		}
	}

	if (cmt_found >= 0)
	{

		cout << cmt_found << "\n";
		while (cmt_found < v_rec.size())
		{
			v_rec.pop_back();
		}
		cout << "Size after comment removal is : " << v_rec.size() << "\n";
	}

	if (sz > 2)
	{
		/* if a comment has already been removed and the size of the vector is greater than two */
		/* then the record's first token must be a label */
		r.setLabel(v_rec[0]);
		v_rec.erase(v_rec.begin());
		sz = v_rec.size();
	}
	else
	{
		bs_result = binary_search(v_conv, v_rec[0]);
		/* check to see if match was found */
		if (bs_result == -1)
		{
			/* match not found, must be label */
			r.setLabel(v_rec[0]);
			v_rec.erase(v_rec.begin());
			sz = v_rec.size();
		}
		/* match found, has args */
		else if (v_conv[bs_result].doesHaveArgs())
		{
			if (sz == 1)
			{
				error(v_conv[bs_result].getInstruction());
			}
			r.setInstruction(v_conv[bs_result].getInstruction());
			r.setEmulation(v_conv[bs_result].getEmulation());
			r.setArgument(v_rec[1]);
		}
		/* match found but doesn't have args */
		else
		{
			if (sz == 2)
			{
				error(v_conv[bs_result].getInstruction());
			}

			r.setInstruction(v_conv[bs_result].getInstruction());
			r.setEmulation(v_conv[bs_result].getEmulation());
		}
	}

	return r;
}

vector<string> get_input(string filepath)
{
	/*
	Purpose:    Reads records from input file
	Input:      Input file
	Output:     Vector of strings. One entry for each line in file
	*/

	ifstream ifile(filepath);
	string record;
	vector<string> rv;

	while (getline(ifile, record)) {
		rv.push_back(record);
	}
	
	ifile.close();

	return rv;
}

vector<string> format_record(const char * p_rv)
{
    /*
     Purpose:    Tokenizes raw record string
     Input:      Pointer to current raw record
     Output:     Vector of strings - formatted record
     */
    
    string raw = p_rv;
    vector<string> result;
	unsigned int i;
    string comment;
    
    /* check to see if line is just a comment */
    if (raw[0] == ';')
    {
        result.push_back(raw);
        return result;
    }
    
    comment = raw.substr(raw.find(";"));
    
    /* tokenized using spaces and tabs as delimiters*/
	result = split(raw, "	 ");
    
    return result;
}

int binary_search(vector<conversion> vect, string s)
{
	/*
	Purpose:    Binary search function
	Input:      Table of values to search, string to search for
	Output:     Integer location of item in table
	*/

    int start = 0,
        end = TABLE_SIZE - 1,
        middle,
        curr_pos = -1;
    bool found = false;
	string inst = uppercaser(s);

	cout << "Searching conversion table...\n";

	for (start; start < end; start++) 
	{
		vect[start].write(cout);
	};
    
    while (!found && start <= end)
    {
        middle = (start + end)/2;
        /* instruction is at the middle of the table */
        if (vect[middle].getInstruction() == inst)
        {
            found = true;
            curr_pos = middle;
        }
        /*  instruction is in bottom half of table */
        else if ( vect[middle].getInstruction() > inst) end = middle - 1;
        /* instruction is in upper half of table */
        else start = middle + 1;
	};
    
    if (!found)
    {
        return -1;
    }

	cout << "Match found at position %i of table.", curr_pos;
    
    return curr_pos;
}
                          
void error(string instruction)
{
	/*
	Purpose:    Writes errors to screen
	Input:      Instruction being examined at time of error
	Output:     None
	*/

    cout << "Error found due to record for instruction " << instruction << "\n";
    return;
}

string uppercaser(string s)
{
	for (unsigned int i = 0; i < s.size(); i++)
	{
		if ((s[i] > 96) && (s[i] < 123))
			s[i] = s[i] - 32;
	}
}

vector<string> split(const string& input, const string& delims)
{
	/*
	Purpose:    Splits string based on delimiters
	Input:      String to split, string of delimiters
	Output:     Vector of string tokens
	*/

	vector<string> tokens;
	size_t start = input.find_first_not_of(delims), end = 0;

	while ((end = input.find_first_of(delims, start)) != string::npos)
	{
		tokens.push_back(input.substr(start, end - start));
		start = input.find_first_not_of(delims, end);
	}
	if (start != string::npos)
		tokens.push_back(input.substr(start));

	return tokens;
}

void conversion::write(ostream& out) const
{
	/*
	Purpose:    Write method for conversion type
	Input:      output stream
	Output:     None
	*/

	out << instruction << " ";
	out << emulation << " ";
	out << hasArgs << " ";
	out << argL << " ";
	out << argR << "\n";
}

void record::write(ostream& out) const
{
	/*
	Purpose:    Write method for record type
	Input:      output stream
	Output:     None
	*/

	out << "Label is: " << label << "\n";
	out << "Instruction is: " << instruction << "\n";
	out << "Emulation is: " << emulation << "\n";
	out << "Argument is: " << argument << "\n";
	out << "Comment is: " << comment << "\n";
}

vector<conversion> generate_table(void) 
{
	/*
	Purpose:    Generates conversion table - array of conversions
	Input:      None
	Output:     Pointer to table
	*/
	
	vector<conversion> v;

	conversion c_t[TABLE_SIZE] =
	{
		conversion( "ADC", "ADDC ", true, "#0", "" ),
		conversion( "ADC.B", "ADDC.B ", true, "#0", "" ),
		conversion( "ADC.W", "ADDC.W ", true, "#0", "" ),
		conversion( "CALL", "BL ", true, "", "" ),
		conversion( "CLR", "MOV ", true, "#0", "" ),
		conversion( "CLR.B", "MOV.B ", true, "#0", "" ),
		conversion( "CLR.W", "MOV.W ", true, "#0", "" ),
		conversion( "CLRC", "BIC #1,R6 ", false, "", "" ),
		conversion( "CLRN", "BIC #4,R6 ", false, "", "" ),
		conversion( "CLRZ", "BIC #2,R6 ", false, "", "" ),
		conversion( "DADC", "DADD ", true, "#0", "" ),
		conversion( "DADC.B", "DADD.B ", true, "#0", "" ),
		conversion( "DADC.W", "DADD.W ", true, "#0", "" ),
		conversion( "DEC", "SUB ", true, "#1", "" ),
		conversion( "DEC.B", "SUB.B ", true, "#1", "" ),
		conversion( "DEC.W", "SUB.W ", true, "#1", "" ),
		conversion( "DECD", "SUB ", true, "#2", "" ),
		conversion( "DECD.B", "SUB.B ", true, "#2", "" ),
		conversion( "DEC.W", "SUB.W ", true, "#2", "" ),
		conversion( "INC", "ADD ", true, "#1", "" ),
		conversion( "INC.B", "ADD.B ", true, "#1", "" ),
		conversion( "INC.W", "ADD.W ", true, "#1", "" ),
		conversion( "INCD", "ADD ", true, "#2", "" ),
		conversion( "INCD.B", "ADD.B ", true, "#2", "" ),
		conversion( "INCD.W", "ADD.W ", true, "#2", "" ),
		conversion( "INV", "XOR ", true, "#-1", "" ),
		conversion( "INV.B", "XOR.B ", true, "#-1", "" ),
		conversion( "INV.W", "XOR.W ", true, "#-1", "" ),
		conversion( "JUMP", "MOV ", true, "", "R7" ),
		conversion( "NOP", "MOV R6,R6", false, "", "" ),
		conversion( "PULL", "LD ", true, "R5+", "" ),
		conversion( "PUSH", "ST ", true, "-R5", "" ),
		conversion( "RET", "MOV R4,R7", false, "", "" ),
		conversion( "RLC", "ADDC ", true, "", "" ),
		conversion( "RLC.B", "ADDC.B ", true, "", "" ),
		conversion( "RLC.W", "ADDC.W ", true, "", "" ),
		conversion( "SBC", "SUBC ", true, "#0", "" ),
		conversion( "SBC.B", "SUBC.B ", true, "#0", "" ),
		conversion( "SBC.W", "SUBC.W ", true, "#0", "" ),
		conversion( "SETC", "BIS #1,R6", false, "", "" ),
		conversion( "SETN", "BIS #4,R6", false, "", "" ),
		conversion( "SETZ", "BIS #2,R6", false, "", "" ),
		conversion( "SLA", "ADD ", true, "", "" ),
		conversion( "SLA.B", "ADD.B ", true, "", "" ),
		conversion( "SLA.W", "ADD.W ", true, "", "" ),
		conversion( "SPL0", "MOVLZ #$0,R6", false, "", "" ),
		conversion( "SPL1", "MOVLZ #$20,R6", false, "", "" ),
		conversion( "SPL2", "MOVLZ #$40,R6", false, "", "" ),
		conversion( "SPL3", "MOVLZ #$60,R6", false, "", "" ),
		conversion( "SPL4", "MOVLZ #$80,R6", false, "", "" ),
		conversion( "SPL5", "MOVLZ #$A0,R6", false, "", "" ),
		conversion( "SPL6", "MOVLZ #$C0,R6", false, "", "" ),
		conversion( "SPL7", "MOVLZ #$E0,R6", false, "", "" ),
		conversion( "TST", "CMP ", true, "#0", "" ),
		conversion( "TST.B", "CMP.B ", true, "#0", "" ),
		conversion( "TST.W", "CMP.W ", true, "#0", "" ),
	};

	for (unsigned int i = 0; i < TABLE_SIZE; i++) v.push_back(c_t[i]);

	return v;
}