#include <iostream>

using namespace std;

extern "C" bool runTest()
{
	cout << "Evo me u test.cu\n";
	return true;
}