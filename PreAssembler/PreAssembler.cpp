/*
- Pre-assembler for the X-Makina Project
- ECED3403 Assignment 1
- Pre-assembler mainline
- Finlay Miller B00675696
- 03 June 2018
*/

#include "preassembler.h"
using namespace std;
FILE *infile;

/* function definitions */
vector<string> get_input(string);
int binary_search(vector<conversion>, string);
void error(string);
record convert(vector<string>, vector<conversion>);
bool write_output(string, vector<record>);

int main(int argc, char *argv[])
{
	vector<string> active_r;				/* tokenized record */
	vector<string> record_vector_raw;		/* input right from file, separated by newlines */
	vector<conversion> conversion_table = generate_table(); /* table of instructions and their corresponding */
															/* emulated instructions and arguments */
	vector<record> converted_records;		/* stores all converted records until final write */
	record rec;
	const char * p_recvec;
	bool written = false;

	/*if (argc < 2)
	{
		cout << "Not enough arguments.\n";
		getchar();
		return 0;
	}

	infile = fopen(argv[1], "r");

	if (infile != 0)
	{
		cout << "Error opening file: \n" << infile;
		getchar();
		return 1;
	}*/

	record_vector_raw = get_input(IFILE_PATH);
	size_t s = record_vector_raw.size();
	cout << "Input file has " << s << " lines.\n";

	for (unsigned int current_record_index = 0; current_record_index < s; current_record_index++)
	{
		cout << "Record #" << current_record_index + 1 << "\n";
		p_recvec = record_vector_raw[current_record_index].data();
		cout << "Raw data is : " << p_recvec << "\n";
		active_r = format_record(p_recvec);
		cout << "Formatted record is: \n";
		for (unsigned int i = 0; i < active_r.size(); i++) cout << active_r[i] << "	";
		cout << "\n";
		rec = convert(active_r, conversion_table);
		rec.write(cout);
		cout << "\n";
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
	*/

	record r;
	int bs_result = -1;
	string temp, cmt;
	__int64 j = 0, cmt_found = -1;

	/* Search record for comment */
	for (vector<string>::iterator i = v_rec.begin(); i != v_rec.end(); i++)
	{
		temp = *i;
		if (temp[0] == ';')
		{
			j = i - v_rec.begin();
			cmt_found = j;
			if (temp[1] == NULL) cout << "\nLine found to be entirely comment.";
			cout << "\nComment found at position " << cmt_found;
			cout << "\nCurrent vector size is " << v_rec.size();
			while ((unsigned) j != v_rec.size())
			{
				cmt += v_rec[j] + " ";
				j++;
			}
			r.setComment(cmt);
		}
	}
	if (cmt_found >= 0)
	{
		while (cmt_found < (signed) v_rec.size())
		{
			v_rec.pop_back();
		}
		cout << "\nSize after comment removal is : " << v_rec.size() << "\n";
	}

	if (v_rec.size() > 2)
	{
		/* if a comment has already been removed and the size of the vector is greater than two */
		/* then the record's first token must be a label */
		r.setLabel(v_rec[0]);
		v_rec.erase(v_rec.begin());
	}
	else if (v_rec.size() > 0)
	{
		bs_result = binary_search(v_conv, v_rec[0]);

		/* check to see if match was found */
		if (bs_result == -1)
		{
			/* match not found, must be label or unidentified command */
			r.setLabel(v_rec[0]);
			v_rec.erase(v_rec.begin());
		}
		/* match found, has args */
		else if (v_conv[bs_result].doesHaveArgs())
		{
			/* if the instruction has arguments but there is only 1 token (the instruction) */
			if (v_rec.size() == 1) error(v_conv[bs_result].getInstruction());

			r.setInstruction(v_conv[bs_result].getInstruction());
			r.setEmulation(v_conv[bs_result].getEmulation());
			
			if (v_conv[bs_result].getArgL() != "N/A") r.setArgumentL(v_rec[1]);
			if (v_conv[bs_result].getArgR() != "N/A") r.setArgumentR(v_rec[1]);
		}
		/* match found but doesn't have args */
		else
		{
			if (v_rec.size() == 2)
			{
				error(v_conv[bs_result].getInstruction());
			}

			r.setInstruction(v_conv[bs_result].getInstruction());
			r.setEmulation(v_conv[bs_result].getEmulation());
		}
	}
	else cout << "\nWat";

	/* check to ensure that all tokens have been identified. */
	/* print them if not identified */
	if (v_rec.size() > 0 && bs_result == -1)
	{
		cout << "\nRecord conversion ended with " << v_rec.size() << " tokens still in conversion vector.";
		/* reuse variables */
		j = 0;
		temp = "";
		while (j != v_rec.size())
		{
			temp += v_rec[j] + " ";
			v_rec.erase(v_rec.begin());
		}
		r.setArgumentL(temp);
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
			ofile << r[0].getLabel();
			ofile << r[0].getEmulation();
			ofile << r[0].getArgumentL();
			if (r[0].getArgumentL() != "N/A") ofile << ",";
			ofile << r[0].getArgumentR();
			if (r[0].getComment() != "N/A") ofile << "	";
			ofile << r[0].getComment();
			ofile << "\n";
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

	cout << "Searching conversion table...\n";
    
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

	cout << "Match found at position " << curr_pos <<" of table.\n";
    
    return curr_pos;
}

void error(string s)
{
	/*
	Purpose:    Writes errors to screen
	Input:      Instruction being examined at time of error
	Output:     None
	*/

	if (s == "write") cout << "Error writing output to file.\n";
	else cout << "Error found due to record for instruction " << s << "\n";
    return;
}