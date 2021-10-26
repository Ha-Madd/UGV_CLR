#using <System.dll>

#include <conio.h>

#include <SMObject.h>
#include <smstructs.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

value struct UGVProcesses
{
	String^ ModuleName;
	int Critical;
	int CrashCount;
	int CrashCountLimit;
	Process^ ProcessName;
};

int main()
{
	// Tele-Operation
	// Declarations + Initializations
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));
	array<UGVProcesses>^ ProcessList = gcnew array<UGVProcesses>
	{
		{"Laser",    1, 0, 10, gcnew Process},
		{ "Display", 1, 0, 10, gcnew Process },
		{ "Vehicle", 1, 0, 10, gcnew Process },
		{ "Gps",	 0, 0, 10, gcnew Process },
		{ "Camera",  0, 0, 10, gcnew Process },

	};

	__int64 Counter, Frequency;
	double PMTimeStamp;
	int Count;
	

	//array<String^>^ ModuleList = gcnew array<String^>{"Laser", "Display", "Vehicle", "GPS", "Camera"};
	//array<int>^ Critical = gcnew array<int>(ModuleList->Length){1, 1, 1, 0, 0};
	//array<int>^ CrashCount = gcnew array<int>(ModuleList->Length);
	//array<int>^ CrashCountLimit = gcnew array<int>(ModuleList->Length) { 10, 10, 10, 10, 10 };
	//array<Process^>^ ProcessList = gcnew array<Process^>(ModuleList->Length);

	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);

	//SM Creation and seeking access
	PMObj.SMCreate();
	PMObj.SMAccess();

	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;



	for (int i = 0; i < ProcessList->Length; i++)
	{
		if (Process::GetProcessesByName(ProcessList[i].ModuleName)->Length == 0)
		{
			ProcessList[i].ProcessName = gcnew Process;
			ProcessList[i].ProcessName->StartInfo->WorkingDirectory = "C:\\Users\\Blank\\source\\repos\\UGV_CLR 1.1\\Executable";
			ProcessList[i].ProcessName->StartInfo->FileName = ProcessList[i].ModuleName;
			ProcessList[i].ProcessName->Start();
			Console::WriteLine("The process " + ProcessList[i].ModuleName + ".exe started");
		}
	}


	// Main loop
	while (!_kbhit())
	{	
		// PMTime Stamp
		QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
		PMData->PMTimeStamp = PMTimeStamp = (double)Counter / (double)(Frequency) * 1000.0; //ms
		PMData->Ready = true;

	// Checking for heartbeats
		// iterate through all the processes(for loop)
		for (int i = 0; i < ProcessList->Length; i++)
		{
			// is the heatbeat bit of process [i] up? (if statment)
			// True -> Put the bit of process[i] down & reset CrashCount
		
			
			if (PMData->Heartbeat.Flags.GPS == 1)
			{
				PMData->Heartbeat.Flags.GPS = 0; ProcessList[i].CrashCount = 0;
			}

			// False-> Increment the counter (heartbeat lost counter)
			else
			{
				ProcessList[i].CrashCount ++;
				// is the counter passed the limit for process[i] (if statment)
				// True-> is process[i] critical(if statment)
				if (ProcessList[i].CrashCount > ProcessList[i].CrashCountLimit)
				{
					// True-> Shutdown all 
					if (ProcessList[i].Critical == 1)
					{
						PMData->Shutdown.Status = 0xFF; break;
					}
					// False-> Has process[i] exites the operating system (HasExited())(if statment)
					else
					{
						if (ProcessList[i].ProcessName->HasExited)
						{
							ProcessList[i].ProcessName->Start();
						}
						//re_start GPS

						else
						{
							ProcessList[i].ProcessName->Kill();
							ProcessList[i].ProcessName->Start();
							//Kill GPS
							//Restart GPS
						}
					}
				}
			}
		}
								
								// False-> Has process[i] exites the operating system (HasExited())(if statment)
									// True->Start();									
									// False-> Kill(); start();
							// False-> incerment counter for process[i]

		Thread::Sleep(1000);
	}

	PMData->Shutdown.Status = 0xFF;
	// Clearing and shutdown


	Console::ReadKey();

	return 0;
}