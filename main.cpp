#include <iostream>
#include "Akarin.h"

using namespace std;

int main()
{
	Akarin myCPU(
			"ini/DDR2_micron_16M_8b_x8_sg3E.ini",
			"system.ini",
			".",
			"hello_world",
			4096, /* Memory: 4GB */
			3.2e9 /* CPU: 3.2GHz */
		    );

	myCPU.doComputing(1000);
	cout << endl;

	cout << "[example 1] non-blocking writes with some locality" << endl;
	cout << endl;
	myCPU.accessMemory(0, 0, 0, 1, 3, true, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 2, 0, true, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 1, 8, true, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 2, 4, true, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 1, 1, true, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 2, 1, true, false);
	myCPU.doComputing(975);
	cout << endl;

	cout << "[example 2] non-blocking writes with no locality" << endl;
	cout << endl;
	myCPU.accessMemory(0, 0, 0, 3, 3, true, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 4, 0, true, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 5, 8, true, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 6, 4, true, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 7, 1, true, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 8, 1, true, false);
	myCPU.doComputing(975);

	cout << endl;
	cout << "[example 3] non-blocking read with some locality" << endl;
	cout << endl;
	myCPU.accessMemory(0, 0, 0, 9, 3, false, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 10, 0, false, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 9, 8, false, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 10, 4, false, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 9, 1, false, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 10, 1, false, false);
	myCPU.doComputing(975);
	cout << endl;

	cout << "[example 4] non-blocking read with no locality" << endl;
	cout << endl;
	myCPU.accessMemory(0, 0, 0, 11, 3, false, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 12, 0, false, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 13, 8, false, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 14, 4, false, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 15, 1, false, false);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 16, 1, false, false);
	myCPU.doComputing(975);
	cout << endl;

	cout << "[example 5] blocking read with some locality" << endl;
	cout << endl;
	myCPU.accessMemory(0, 0, 0, 17, 3, false, true);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 18, 0, false, true);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 17, 8, false, true);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 18, 4, false, true);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 17, 1, false, true);
	myCPU.doComputing(5);
	myCPU.accessMemory(0, 0, 0, 18, 1, false, true);
	myCPU.doComputing(975);
	cout << endl;

	return 0;
}
