#include "stdafx.h"
#include "HttpClientBoost.h"
#include <iostream>

HttpClientBoost::HttpClientBoost(): reqQueue(1000)
{
}


HttpClientBoost::~HttpClientBoost()
{
}

unsigned int __stdcall HttpClientBoost::ExecuteThread(void * pParam)
{
	HttpClientBoost* pThis = (HttpClientBoost*)pParam;
	boost::asio::streambuf* pData = NULL;
	while (1)
	{
		pData = NULL;
		if (pThis->reqQueue.HasItem()) 
		{
			pData = pThis->reqQueue.GetItem();
			if (pData != NULL)
			{
				try
				{
					boost::system::error_code error = boost::asio::error::host_not_found;
					boost::asio::ip::address add;
					add = add.from_string(pThis->host, error);
					if (error)
					{
						std::cout << error.value() << std::endl;
						return false;
					}
					tcp::endpoint endpoint(add, pThis->port);
					tcp::socket socket(pThis->io_context);
					socket.connect(endpoint, error);
					if (error)
					{
						std::cout << "connect error " << error.value() << std::endl;
					}


					size_t nSize = socket.write_some((*pData).data(), error);
					//size_t nSize = boost::asio::write(socket, *pData, error);
					if (error)
					{
						std::cout << "send error " << error.value() << std::endl;
					}
					if (nSize != pData->size())
					{
						std::cout << "HttpClientBoost Send Size = " << pData->size() << ", Truth Size = " << nSize << std::endl;
					}
					else
					{
						std::cout << "send size " << nSize << std::endl;
					}
					delete pData;
					socket.close();
				}
				catch(std::exception& e)
				{
					std::cout << e.what() << std::endl;
				}
				
			}
		}
		else
		{
			Sleep(1000);
		}
	}
	return 0;
}

bool HttpClientBoost::Init(const std::string& host, USHORT port)
{
	try
	{
		this->host = host;
		this->port = port;

		_beginthreadex(NULL, 0, ExecuteThread, this, 0, NULL);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	return true;
}

bool HttpClientBoost::ReqData(boost::asio::streambuf * data)
{
	if (reqQueue.HasSpace())
	{
		reqQueue.AddItem(data);
		return true;
	}
	return false;
}

bool HttpClientBoost::ReqPlayerTotalCost(int64_t uid, double data)
{
	data = abs(data);
	boost::asio::streambuf* request = new boost::asio::streambuf();
	std::ostream request_stream(request);
	request_stream << "GET /data?cmd=subGameDataJson&data=" << "{\"" << uid << "\":\"" << data << "\"}"
				   << "&gameId=80002&token=01b90579-e9dc-4bca-900a-8234fd9d62fd"
				   << " HTTP/1.0\r\n";
	request_stream << "Host: " << host << ":" << port << "\r\n";
	request_stream << "Accept: */*\r\n";
	request_stream << "Connection: close\r\n\r\n";
	ReqData(request);
	std::cout << "data size is " << request->size() << std::endl;
	return true;
}
