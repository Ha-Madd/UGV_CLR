#using <System.dll>
#include <Windows.h>
#include <conio.h>

#include <SMObject.h>
#include <smstructs.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

int main()
{
	// Declaration
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	int TSCounter = 0;
	array<double>^ TSValues = gcnew array<double>(100);

	//SM Creation and seeking access
	double TimeStamp, TimeGap;
	__int64 Frequency, Counter, OldCounter;
	int Shutdown = 0x00;

	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);
	QueryPerformanceCounter((LARGE_INTEGER*)&OldCounter);

	PMObj.SMCreate();
	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;


	while (!PMData->Ready)
	{
		Thread::Sleep(25);
	}


	while (!PMData->Shutdown.Flags.Laser)
	{
		PMData->Heartbeat.Flags.Laser = 1; //set HeartBeat flag

		// extract gps data from receivers

		QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
		TimeStamp = (double)Counter / (double)(Frequency) * 1000.0; //ms
		TimeGap = (double)(Counter - OldCounter) / (double)Frequency * 1000; // ms
		OldCounter = Counter;

		if (TSCounter < 100)
			TSValues[TSCounter++] = TimeGap;


		// Did Pm put my flag down?(check PM heartbeat
		// True-> Put my flag up
		if (PMData->Heartbeat.Flags.Laser == 0)
		{
			PMData->Heartbeat.Flags.Laser = 1;
			// maybe Reset WaitAndSeeTime
		}
		// Flase-> Is the PM time stamp older by agreed time gap
		else
		{
			//Accumulate WaitAndSeeTime
			//PMData->PmTimeStamp
			if (TimeStamp - PMData->PMTimeStamp > 250)
			{
				//Request Shutdown all if critical process
				//if not critical attempt to restart
				PMData->Shutdown.Status = 0xFF;
			}
		}




		Console::WriteLine("Laser time Gap  : {0,12:F3} {1,12:X2}", TimeStamp, Shutdown);
		Thread::Sleep(25);

		if (PMData->Shutdown.Status)
			break;
		if (_kbhit())
			break;

	}

	for (int i = 0; i < 100; i++)
		Console::WriteLine("{0,12:F3}", TSValues[i]);

	//Console::ReadKey();
	//Console::ReadKey();

	return 0;
}