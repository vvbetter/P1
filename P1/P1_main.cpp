#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <functional> 
#include <typeinfo>
#include <thread>
using namespace std;

typedef function<bool(int *)> CallbackFunction;

class Test
{
public:
	bool t1(int *) { cout << "this is t1 " << endl; return false; }
};


int main(int argc, char* argv[])
{

	cout << (LONG_MAX ^ (1 << 1)) << endl;
	return 0;
}

