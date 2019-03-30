#include <iostream>
#include <istream>
#include <ostream>
#include <string>
using namespace std;

int main(int argc, char* argv[])
{
	int64_t a = UINT_MAX;

	unsigned int b = static_cast<unsigned int>(a + 1);

	cout << a << "#" << b << endl;
	return 0;
}