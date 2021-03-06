/*
- Pre-assembler for the X-Makina Project
- ECED3403 Assignment 1
- Pre-assembler mainline
- Finlay Miller B00675696
- 05 June 2018
*/

#include "preassembler.h"
using namespace std;


int main(int argc, char *argv[])
{
	vector<string> active_r, record_vector_raw;
	vector<conversion> conversion_table = generate_table();
	vector<record> converted_records;
	record rec;
	string ofile_path;
	const char * p_recvec;
	bool written = false;

	if (argc < 2)
	{
		cout << "Not enough arguments. You probably forgot to drag'n'drop again.\n";
		getchar();
	}

	record_vector_raw = get_input(argv[1]);
	size_t s = record_vector_raw.size();
	cout << "Input file has " << s << " lines.\n";

	// this is where the work happens
	for (unsigned int current_record_index = 0; current_record_index < s; current_record_index++)
	{
		p_recvec = record_vector_raw[current_record_index].data();
		active_r = format_record(p_recvec);
		rec = convert(active_r, conversion_table);
		converted_records.push_back(rec);
	}

	// write results
	cout << "\nReady to write to output file.\nEnter desired output file location:\n";
	cin >> ofile_path;
	written = write_output(ofile_path, converted_records);
	if (!written)
	{
		error("write");
	}
	else cout << "\nDone.";

	// wait
	getchar();
	return 0;
}