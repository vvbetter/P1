
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <iostream>
bool ProcessData(double i)
{
	std::chrono::milliseconds duration(100);
	std::this_thread::sleep_for(duration);
	return true;
}

int main()
{
	const int samples = 1000;
	std::vector<double> data;
	std::atomic<int> result = 0;
	std::atomic<int> currentIndex = 0;
	std::vector<std::thread> threads;

	for (int i = 0; i < samples; i++) data.push_back(i);

	for (int i = 0; i < std::thread::hardware_concurrency(); i++)
	{
		threads.push_back(std::thread([&]()
		{
			while (1)
			{
				int index = currentIndex.fetch_add(1);
				if (index >= samples) break;
				if (ProcessData(data[index]))
				{
					result++;
				}
			}
		}));
	}

	for (std::thread& t : threads)
	{
		t.join();
	}

	std::cout << result << std::endl;

	return 0;
}