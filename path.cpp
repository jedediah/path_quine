#include <string>
#include <list>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

void advance(list<string>::iterator* irow, string::size_type* col, int* rownum, int* colnum, int dir)
{
	switch (dir)
	{
		case 0: ++(*col); ++(*colnum); break;
		case 1: ++(*irow); ++(*rownum); break;
		case 2: --(*col); --(*colnum); break;
		case 3: --(*irow); --(*rownum); break;
	}
}

void expandmem(vector<int>* mem, int* memsize)
{
	int newsize = *memsize*2;;
	mem->reserve(newsize*2);

	while (*memsize < newsize)
	{
		(*mem)[(*memsize)++] = 0;
	}
}

int main(int argc, char** argv)
{
	ifstream f;
	list<string> rows;
	list<string>::iterator irow;
	string::size_type col, x;
	string s;
	char linebuf[1024];
	bool debugflag;
	int memsize = 0x100000, memptr, dir, rownum, colnum, i;
	int
		lurd[4] = {1,0,3,2},
		ruld[4] = {3,2,1,0};
	vector<int> mem(memsize);
	
	if (argc < 2)
	{
		cout << "jedpath <file>" << endl;
		return 1;
	}
	
	debugflag = false;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-' && argv[i][1] == 'd')
			debugflag = true;
	}
	
	f.open(argv[1],ifstream::in);

	if (!f)
	{
		cout << "can't open " << argv[1] << endl;
		return 1;
	}
	
	col = string::npos;
	rownum = colnum = dir = 0;

	while (!f.eof())
	{
		s.clear();

		do
		{
			f.clear();
			f.getline(linebuf,1024);

			if (debugflag)
				cout << linebuf << flush;

			s += linebuf;
		}
		while (f.fail() && !f.eof());

		if (debugflag)
			cout << endl << flush;

		rows.push_back(s);

		if ((x = s.find('$')) != string::npos)
		{
			colnum = col = x;
			irow = rows.end();
			--irow;
		}
		
		if (col == string::npos)
			++rownum;
	}
	
	f.close();

	if (col == string::npos)
	{
		cout << "Syntax error: no start char ($) found\n";
		return 1;
	}

	for (memptr = 0; memptr < memsize; memptr++)
		mem[memptr] = 0;
	
	memptr = 0;
		
	try
	{
		while ((*irow)[col] != '#')
		{
			if (debugflag)
				cout << '[' << colnum << ',' << rownum << ':' << dir << ']' << endl << flush;
	
			switch((*irow)[col])
			{
				case '!': advance(&irow,&col,&rownum,&colnum,dir); break;
				case '+': ++mem[memptr]; break;
				case '-': --mem[memptr]; break;
				case '>': if (++memptr >= memsize) expandmem(&mem,&memsize); break;
				case '<': if (--memptr < 0) throw "memory pointer out of bounds (less than 0)"; break;
				case '/': dir = ruld[dir]; break;
				case '\\': dir = lurd[dir]; break;
				case '.': cout.put(mem[memptr]); break;
				case ',': mem[memptr] = cin.get(); break;
				case '?': if (mem[memptr] == 0) advance(&irow,&col,&rownum,&colnum,dir); break;
			}
			
			if (rownum < 0 || colnum < 0) throw "instruction pointer out of bounds (less than 0)";
			advance(&irow,&col,&rownum,&colnum,dir);
			if (rownum < 0 || colnum < 0) throw "instruction pointer out of bounds (less than 0)";
		}
	}
	
	catch (char* msg)
	{
		cout << "Runtime error: " << msg << endl;
		return 1;
	}
	
	return 0;
}

