#pragma once
#include "SafeArray.h"
#include <string>
#include <thread>
#include <boost/asio.hpp>
#include <boost/lockfree/queue.hpp>
using boost::asio::ip::tcp;

class HttpClientBoost
{
public:
	HttpClientBoost();
	~HttpClientBoost();

	static unsigned int __stdcall ExecuteThread(void* pParam);
	bool Init(const std::string& host ,USHORT port);
	bool ReqData(boost::asio::streambuf* data);
	bool ReqPlayerTotalCost(int64_t uid, double data);
private:
	std::string host;
	USHORT port;
	boost::asio::io_context io_context;
	tcp::resolver::results_type endpoints;

	SafeArray<boost::asio::streambuf*> reqQueue;
};

