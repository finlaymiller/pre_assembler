/*
	- Pre-assembler for the X-Makina Project
	- ECED3403 Assignment 1
	- 
	- Finlay Miller B00675696
	- 3 June 2018
*/

#include "preassembler.h"

/* function definitions */
vector<string> get_input(string);
vector<string> format_record(const char *);
int binary_search(vector<conversion>, string);
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
	int bs_result, i = 0;
	size_t sz = v_rec.size();


	if (v_rec[0][0] = ';')
	{
		cout << v_rec[0][0];
		cout << "\nLine found to be entirely comment.\n";
		r.setComment(v_rec[i]);
		v_rec.erase(v_rec.begin());
		sz = v_rec.size();
	}
	else cout << "Attempted to add comment to line but one was already present.\n";

	if (sz > 2)
	{
		/* if a comment has already been removed and the size of the vector is greater than two */
		/* then the record's first token must be a lable */
		r.setLabel(v_rec[0]);
		v_rec.erase(v_rec.begin());
		sz = v_rec.size();
	}
	else
	{
		bs_result = binary_search(v_conv, v_rec[0]);
		/* check to see if match was found */
		if (bs_result = -1)
		{
			/* match not found, must be label */
			r.setLabel(v_rec[0]);
			v_rec.erase(v_rec.begin());
			sz = v_rec.size();
		}
		/* match found, has args */
		else if (v_conv[bs_result].doesHaveArgs())
		{
			if (sz = 1)
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
			if (sz = 2)
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
     Purpose:    Uppercases and tokenizes raw record string
     Input:      Pointer to current raw record
     Output:     Vector of strings - formatted record
     */
    
    string raw = p_rv;
	string uppercased;
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
    
    /* uppercased */
	for (i = 0; i < raw.size(); i++) 
	{
		if (raw[i] >= 'A' && raw[i] <= 'Z') 
		{
			raw[i] -= ('Z' - 'z');
		}
	}
    
    /* tokenized */
	result = split(raw, "	 ");
    
    return result;
}

int binary_search(vector<conversion> vect, string inst)
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
