// SH4_Scn.cpp : This file contains the 'main' function. Program execution begins and ends there.
// 
// SH4EilRoom : Change Eileen's room scene (0 - 32)
// Works with GOG Release of Silent Hill 4: The Room
// by alanm1

#include <iostream>
#include <Windows.h>
#include <string>
using namespace std;

#define ReadMemInt(x)  ReadProcessMemory(handle, (PBYTE*)(x), &readTest, sizeof(readTest), 0)
#define WriteMemInt(x) WriteProcessMemory(handle, (PBYTE*)(x), &newValue, sizeof(newValue), 0)

int CheckAndChange(HANDLE handle, int addr, int expect, int newValue)
{
	int readTest = 0; // We store the Value we read from the Process here
	ReadMemInt(addr);
	int Found = 0;
	if (readTest == expect)
	{
		WriteMemInt(addr);
		ReadMemInt(addr);
		cout << "Set New Value 0x" << std::hex << readTest << endl;
		Found = 1;

	}
	else if (readTest == newValue)
	{
		cout << std::hex << readTest << endl;
		cout << "Already Patch: 0x" << std::hex << newValue << endl;
		Found = 1;
	}
	return Found;
}

int main() {
	int newValue = 5000;
	int readTest = 0; // We store the Value we read from the Process here

	HWND hwnd = FindWindowA(NULL, "SILENT HILL 4:The Room"); // HWND (Windows window) by Window Name

	// Check if HWND found the Window
	if (hwnd == NULL) {
		cout << "Can't find Process." << endl;
		Sleep(2000); // Sleep 2 seconds
		exit(-1); // Exit the program if it did not find the Window
	}
	else {
		DWORD procID; // A 32-bit unsigned integer, DWORDS are mostly used to store Hexadecimal Addresses
		GetWindowThreadProcessId(hwnd, &procID); // Getting our Process ID, as an ex. like 000027AC
		HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID); // Opening the Process with All Access

		if (procID == NULL) {
			cout << "Can't find Process." << endl;
			Sleep(1000); // Sleep 2 seconds
			exit(-1); // Exit the program if it did not find the Window
		}
		else {
			// Read the Process Memory, 03007640 is the Address, we read the Value from and save it in readTest
			//ReadProcessMemory(handle, (PBYTE*)0x004d72e6, &readTest, sizeof(readTest), 0);
			int addrJmpPeepNextDemoDecide = 0x004d8f65;
			int addrJmpGetPlayTime = 0x004d72ed;
			int addrJmpGetNextDemoIndex = 0x004d733f;
			int addrSetDemoIndex = 0x004d739c;
			const char* codeJmpGetPlayTime = "\xe9\x35\x00\x00\x00";
			const char* codeJmpGetIndex = "\xeb\x5b";
			char codeSetDemoIndex[] = "\xb8\x0A\x00\x00\x00";
			

			if (CheckAndChange(handle, addrJmpPeepNextDemoDecide, 0x0e77fa3b, 0x0eebfa3b))
			{
				// Found expected jmp location
				int sizeCode = 5;

				WriteProcessMemory(handle, (PBYTE*)(addrJmpGetPlayTime), codeJmpGetPlayTime, sizeCode, 0);
				WriteProcessMemory(handle, (PBYTE*)(addrJmpGetNextDemoIndex), codeJmpGetIndex, 2, 0);

				string input;
				while (true) {
					cout << "Enter Eileen Room Scene id (0 - 32 or empty string to quit): ";
					getline(cin, input);

					if (input.empty()) {
						break;
					}

					try {
						int number = stoi(input);
						if (number < 0 || number > 60)
							cout << "Id out of range.";
						else
						{
							codeSetDemoIndex[1] = (BYTE)number;

							WriteProcessMemory(handle, (PBYTE*)(addrSetDemoIndex), codeSetDemoIndex, sizeCode, 0);
							// read back what we set
							ReadMemInt(addrSetDemoIndex);
							int sceneId = ((char*)&readTest)[1];
							cout << "Scene id set to " << std::dec << sceneId << endl;

						}

					}
					catch (const std::invalid_argument& e) {
						cout << "Invalid input. Please enter a number." << endl;
					}
				}

			}
			else
				cout << "Cannot access the game.";
		}
	}
}

