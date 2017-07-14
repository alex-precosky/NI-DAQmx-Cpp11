#include "stdafx.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
using namespace std;


#include "NIDAQmx.h"



uInt8       writeDataZero[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uInt8       writeDataOne[8] = { 1, 1, 1, 1, 1, 1, 1, 1 };

int _tmain(int argc, _TCHAR* argv[])
{

	TaskHandle writeTaskHandle = 0;
	TaskHandle readTaskHandle = 0;
	uInt8 readArray[8];
	char errorStr[255];
	int32 errorCode;

	char ch;

	const int numIterations = 10000;

	std::cout << "DAQmxCreateTask write task\n";
	if (DAQmxCreateTask("", &writeTaskHandle))
	{

		std::cout << "Error creating DAQmx write task";
		std::cin >> ch;
		return -1;
	}

	std::cout << "DAQmxCreateTask read task\n";
	if (DAQmxCreateTask("", &readTaskHandle))
	{

		std::cout << "Error creating DAQmx read task";
		std::cin >> ch;
		return -1;
	}


	std::cout << "DAQmxCreateDOChan\n";
	if (DAQmxCreateDOChan(writeTaskHandle, "Dev3/port1/line0:3", "", DAQmx_Val_ChanForAllLines))
	{
		std::cout << "Error creating DO channel";
		std::cin >> ch;
		return -1;
	}


	std::cout << "DAQmxCreateDIChan\n";
	if (DAQmxCreateDIChan(readTaskHandle, "Dev3/port0/line0:3", "", DAQmx_Val_ChanForAllLines))
	{
		std::cout << "Error creating DI channel";
		std::cin >> ch;
		return -1;
	}

	std::cout << "DAQmxStartTasks\n";
	if (DAQmxStartTask(writeTaskHandle))
	{
		std::cout << "Error starting write task";
		std::cin >> ch;
		return -1;
	}
	
	if(DAQmxStartTask(readTaskHandle))
	{
		std::cout << "Error starting read task";
		std::cin >> ch;
		return -1;
	}


	std::cout << "Writing zero, then delaying for one second\n";
	if (DAQmxWriteDigitalLines(writeTaskHandle, 1, true, 10.0, DAQmx_Val_GroupByChannel, writeDataZero, NULL, NULL))
	{
		std::cout << "Error writing digital line";
		std::cin >> ch;
		return -1;
	}
	std::this_thread::sleep_for(std::chrono::seconds(1));


	std::cout << numIterations << " iterations of DAQmxWriteDigitalLines zeros\n";
	auto start = chrono::steady_clock::now();
	for (int i = 0; i < numIterations; i++)
	{
		DAQmxWriteDigitalLines(writeTaskHandle, 1, true, 10.0, DAQmx_Val_GroupByChannel, writeDataZero, NULL, NULL);
	}
	auto end = chrono::steady_clock::now();
	cout << chrono::duration <double, milli>(end-start).count() << " ms\n" << endl;


	std::cout << numIterations << " iterations of DAQmxWriteDigitalLines ones\n";
	start = chrono::steady_clock::now();
	for (int i = 0; i < numIterations; i++)
	{
		errorCode = DAQmxWriteDigitalLines(writeTaskHandle, 1, true, 10.0, DAQmx_Val_GroupByChannel, writeDataOne, NULL, NULL);
		if (errorCode)
		{
			DAQmxGetErrorString(errorCode, errorStr, 255);
			std::cout << "Error writing digital line ones\n";
			std::cout << errorStr;

			std::cin >> ch;
			return -1;
		}

	}
	end = chrono::steady_clock::now();
	cout << chrono::duration <double, milli>(end - start).count() << " ms\n" << endl;


	std::cout << numIterations << " iterations of DAQmxWriteDigitalLines zeros and ones alternating\n";
	start = chrono::steady_clock::now();
	for (int i = 0; i < numIterations; i++)
	{
		DAQmxWriteDigitalLines(writeTaskHandle, 1, true, 10.0, DAQmx_Val_GroupByChannel, writeDataZero, NULL, NULL);
		DAQmxWriteDigitalLines(writeTaskHandle, 1, true, 10.0, DAQmx_Val_GroupByChannel, writeDataOne, NULL, NULL);
	}
	end = chrono::steady_clock::now();
	cout << chrono::duration <double, milli>(end - start).count() << " ms\n" << endl;



	std::cout << numIterations << " iterations of DAQmxReadDigitalLines\n";
	start = chrono::steady_clock::now();
	for (int i = 0; i < numIterations; i++)
	{
		DAQmxReadDigitalLines(readTaskHandle, 1, 10.0, DAQmx_Val_GroupByChannel, readArray, 1, NULL, NULL, NULL);
	}
	end = chrono::steady_clock::now();
	cout << chrono::duration <double, milli>(end - start).count() << " ms\n" << endl;



	std::cout << numIterations << " iterations of looping back one pin to the next\n";
	start = chrono::steady_clock::now();
	readArray[0] = 0;
	for (int i = 0; i < numIterations; i++)
	{
		// Write a zero and wait until we read a zero on the loopback pin
		DAQmxWriteDigitalLines(writeTaskHandle, 1, true, 1.0, DAQmx_Val_GroupByChannel, writeDataZero, NULL, NULL);
		while ((readArray[0] & 0x01) != 0)
			DAQmxReadDigitalLines(readTaskHandle, 1, 1.0, DAQmx_Val_GroupByChannel, readArray, 1, NULL, NULL, NULL);


		// Write a one and wait until we read a one on the loopback pin
		DAQmxWriteDigitalLines(writeTaskHandle, 1, true, 1.0, DAQmx_Val_GroupByChannel, writeDataOne, NULL, NULL);
		while ((readArray[0] & 0x01) != 1)
			DAQmxReadDigitalLines(readTaskHandle, 1, 1.0, DAQmx_Val_GroupByChannel, readArray, 8, NULL, NULL, NULL);

	}
	end = chrono::steady_clock::now();
	cout << chrono::duration <double, milli>(end - start).count() << " ms\n" << endl;

	std::cin >> ch;

	return 0;
}

