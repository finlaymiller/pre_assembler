/*
- Pre-assembler for the X-Makina Project
- ECED3403 Assignment 1
- Pre-assembler mainline
- Finlay Miller B00675696
- 03 June 2018
*/

#include "preassembler.h"
using namespace std;

/* function definitions */
vector<string> get_input(string);
int binary_search(vector<conversion>, string);
void error(string);
record convert(vector<string>, vector<conversion>);
bool write_output(string, vector<record>);

int main(int argc, char *argv[])
{
	vector<string> active_r, record_vector_raw;
	vector<conversion> conversion_table = generate_table();
	vector<record> converted_records;
	record rec;
	const char * p_recvec;
	bool written = false;

	record_vector_raw = get_input(IFILE_PATH);
	size_t s = record_vector_raw.size();
	cout << "Input file has " << s << " lines.\n";

	for (unsigned int current_record_index = 0; current_record_index < s; current_record_index++)
	{
		p_recvec = record_vector_raw[current_record_index].data();
		active_r = format_record(p_recvec);
		rec = convert(active_r, conversion_table);
		converted_records.push_back(rec);
	}

	written = write_output(OFILE_PATH, converted_records);
	if (!written) error("write");

	getchar();
	return 0;
}

record convert(vector<string> v_rec, vector<conversion> v_conv)
{
	/*
	Purpose:    Contains the logic flow for determining what each token of input string is
	Input:      The record currently being examined, the conversion table (as a vector)
	Output:     Record to be printed
	Process:	1.	Search record vector for a comment
					a.	If a comment is found, send it to comment field of record
				2.	If there are more than two other tokens in the vector then the first token MUST be a label. Send it to the record.
				3.	Else get the first token and perform a binary search
					a.	If match not found, 1st token must be a label
					b.	Else if match is found and the corresponding instruction has arguments
						i.		If no more tokens in vector throw error
						ii.		The next token must be an argument
						iii.	Send the instruction and argument to the record
					c.	Else if match is found and it doesn't have arguments
						i.		If there are more tokens throw error
						ii.		Send instruction to record
				4.	If there are tokens left over throw an error
	*/

	record r;
	int bs_result = -1;
	string temp, cmt;
	__int64 j = -1, cmt_start = -1;
	vector<string>::iterator i = v_rec.begin();

	// 1.	Search record vector for a comment	
	while (i != v_rec.end() && cmt_start < 0)
	{
		temp = *i;
		if (temp[0] == ';')
		{
			// a.	If a comment is found, send it to comment field of record 
			cmt_start = i - v_rec.begin();
			j = cmt_start;
			
			while ((unsigned) j < v_rec.size())
			{
				cmt += v_rec[j] + " ";
				j++;
			}
			r.setComment(cmt);
		}
		i++;
	}

	if (cmt_start >= 0)
	{
		// Delete all tokens after ';' if found 
		while (v_rec.size() > 0)
		{
			v_rec.pop_back();
		}
	}

	// Now we can start analyzing the rest of the tokens
	// Lasciate ogne speranza, voi ch'entrate
	if (v_rec.size() >= 2)
	{
		// 2. If there are more than two other tokens in the vector then 
		// the first token MUST be a label. Send it to the record. 
		r.setLabel(v_rec[0] + " ");
		v_rec.erase(v_rec.begin());
	}
	else
	{
		// 3. Get the first token and perform a binary search 
		bs_result = binary_search(v_conv, v_rec[0]);
		
		if (bs_result == -1)
		{
			// a. If match not found, 1st token must be a label 
			r.setLabel(v_rec[0] + " ");
			v_rec.erase(v_rec.begin());
		}
		else if (v_conv[bs_result].getArgType() > 0)
		{
			// b. Match is found and the corresponding instruction has arguments
			// i. If no more tokens in vector throw error
			if (v_rec.size() == 1) error(v_rec[0]);
			
			// ii. The next token must be an argument
			switch (v_conv[bs_result].getArgType())
			{
				// iii. Send the instruction and argument to the record
				case 1:	r.setArgumentL(v_rec[1]);
						break;
				case 2: r.setArgumentR(v_rec[1]);
						break;
				case 3: r.setArgumentL(v_rec[1]);
						r.setArgumentR(v_rec[1]);
						break;
			};
			
			r.setInstruction(v_rec[0]);
			r.setEmulation(v_conv[bs_result].getEmulation());
			v_rec.erase(v_rec.begin());
			v_rec.erase(v_rec.begin());
		}
		else if (v_conv[bs_result].getArgType() == 0)
		{
			// c. Match is found and it doesn't have arguments
			// i. If there are more tokens throw error
			if (v_rec.size() > 1) error(v_rec[0]);
			
			// ii. Send instruction to record
			r.setInstruction(v_conv[bs_result].getInstruction());
			r.setEmulation(v_conv[bs_result].getEmulation());
			v_rec.erase(v_rec.begin());
		}
	}
	
	if (v_rec.size() > 0)
	{
		// 4.	If there are tokens left over throw an error
		error("excess");
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

bool write_output(string filepath, vector<record> r)
{
	/*
	Purpose:    Writes record to output file
	Input:      Record
	Output:     Write status T|F
	*/

	ofstream ofile(filepath);

	if (ofile.is_open())
	{
		cout << "Writing " << r.size() << "  records to output file.\n";
		while (r.size() > 0)
		{
			//  file writes 
			ofile << r[0].getLabel();
			ofile << r[0].getEmulation();
			switch (r[0].getArgType())
			{
			case 0: break;
			case 1: ofile << r[0].getArgumentL();
					break;
			case 2: ofile << r[0].getArgumentR();
					break;
			case 3: ofile << r[0].getArgumentL();
					ofile << r[0].getArgumentR();
					break;
			}
			ofile << r[0].getComment();

			//  screen writes 
			cout << r[0].getLabel();
			cout << r[0].getEmulation();
			switch (r[0].getArgType())
			{
			case 0: break;
			case 1: cout << r[0].getArgumentL();
				break;
			case 2: cout << r[0].getArgumentR();
				break;
			case 3: cout << r[0].getArgumentL();
				cout << r[0].getArgumentR();
				break;
			}
			cout << r[0].getComment();

			r.erase(r.begin());
		}


		ofile.close();
		return true;
	}
	return false;
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

	//cout << "Searching conversion table...\n";
    
    while (!found && start <= end)
    {
        middle = (start + end)/2;
        //instruction is at the middle of the table 
        if (vect[middle].getInstruction() == inst)
        {
            found = true;
            curr_pos = middle;
        }
        // instruction is in bottom half of table 
        else if ( vect[middle].getInstruction() > inst) end = middle - 1;
        // instruction is in upper half of table
        else start = middle + 1;
	};
    
    if (!found)
    {
        return -1;
    }

	//cout << "Match found at position " << curr_pos <<" of table.\n";
    
    return curr_pos;
}

void error(string s)
{
	/*
	Purpose:    Writes errors to screen
	Input:      Instruction being examined at time of error
	Output:     None
	*/

	if (s == "write") cout << "\nError writing output to file.";
	if (s == "excess") cout << "\nExcess tokens detected. A comment might not have been fully deleted.";
	else cout << "\nError encountered trying to convert instruction " << s;
    return;
}