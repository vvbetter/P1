#include "pch.h"

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/steady_timer.hpp>
#include <string>
using namespace std;

void TimerFunction(boost::asio::steady_timer* timer)
{
	std::cout << "timer " << std::endl;
	timer->expires_after(std::chrono::seconds(5));
	timer->async_wait(boost::bind(&TimerFunction, timer));
}

int main()
{
	//
	//boost::asio::io_context io;
	//boost::asio::steady_timer timer(io);
	//
	//timer.expires_after(std::chrono::seconds(5));
	//timer.async_wait(boost::bind(&TimerFunction, &timer));
	//io.run();

	string a = "12345678";
	a.erase(0, 2);
	cout << a << endl;
	return 0;
}