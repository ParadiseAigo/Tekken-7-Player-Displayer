#include "player-displayer.h"

HANDLE getProcessHandle(DWORD pid) {
	HANDLE processHandle;
	DWORD errorCode;
	processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, pid);
	if (processHandle == NULL) {
		errorCode = GetLastError();
		print(std::string("Error! OpenProcess failed! Code: ").append(std::to_string(errorCode)).append(std::string(". Closing program.\r\n")));
		_endthreadex(0);
	}
	return processHandle;
}

DWORD getProcessId(const std::wstring& programNameExe) {
	HANDLE processesSnapshot;   //will hold a snapshot of all processes...
	PROCESSENTRY32 processInfo; //this struct will contain the process id
	DWORD errorCode;
	ZeroMemory(&processInfo, sizeof(PROCESSENTRY32)); // set all the struct's members equal to zero
	processInfo.dwSize = sizeof(processInfo); // must set the size before using it / filling it
	processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL); // get snapshot of all processes
	if (processesSnapshot == INVALID_HANDLE_VALUE) {
		errorCode = GetLastError();
		print(std::string("Error! CreateToolhelp32Snapshot failed! Code: ").append(std::to_string(errorCode)).append(std::string(". Closing program.\r\n")));
		_endthreadex(0);
	}
	Process32First(processesSnapshot, &processInfo);
	if (!programNameExe.compare(processInfo.szExeFile)) { //if names are equal
		CloseHandle(processesSnapshot);
		return processInfo.th32ProcessID;
	}
	while (Process32Next(processesSnapshot, &processInfo)) {
		if (!programNameExe.compare(processInfo.szExeFile)) {
			CloseHandle(processesSnapshot);
			return processInfo.th32ProcessID;
		}
	}
	CloseHandle(processesSnapshot);
	return 0;
}

QWORD getDynamicPointer(HANDLE processHandle, void* basePointer, std::vector<DWORD> offsets) {
	QWORD resultPointer = 0, prevPointer;
	DWORD errorCode;
	if (offsets.size() == 0) {
		return (QWORD) basePointer;
	}
	prevPointer = (QWORD) basePointer;
	for (int i = 0; i < offsets.size(); i++) {
		if (ReadProcessMemory(processHandle, (void*) prevPointer, &resultPointer, sizeof(resultPointer), NULL) == 0) {
			errorCode = GetLastError();
			//print(std::string("Error! ReadProcessMemory in getDynamicPointer failed! Code: ").append(std::to_string(errorCode)).append(std::string(" .\r\n")));
			return 0;
		}
		prevPointer = resultPointer + offsets[i];
	}
	resultPointer += offsets[offsets.size() - 1];
	return resultPointer;
}

void writeDwordToMemory(HANDLE processHandle, void* address, DWORD newValue) {
	// void* address  holds values that look like this:  (void*) 0x03AFFE76
	DWORD errorCode;
	errorCode = WriteProcessMemory(processHandle, address, &newValue, sizeof(newValue), NULL);
	if (errorCode == 0) {
		errorCode = GetLastError();
		print(std::string("Error! WriteProcessMemory failed in WriteDwordToMemory. Code: ").append(std::to_string(errorCode)).append(std::string(" .\r\n")));
		system("PAUSE");
	}
}

void writeStringLimitedToMemory(HANDLE processHandle, void* address, char* newValue) {
	DWORD errorCode;
	int sizeAddress = getMaxSizeStringInMemory(processHandle, address) + 1; //to "undo" the -1 in strncpy_s
	int sizeNewValue = (int) strlen(newValue);
	int sizeNewValuePadded = sizeAddress;
	char* newValuePadded = (char*) malloc(sizeNewValuePadded * sizeof(char));
	memset(newValuePadded, 0, sizeNewValuePadded);
	strncpy_s(newValuePadded, sizeNewValuePadded, newValue, sizeNewValuePadded - 1); // 4th argument must be smaller than the 2nd
	errorCode = WriteProcessMemory(processHandle, address, newValuePadded, sizeNewValuePadded, NULL);
	if (errorCode == 0) {
		errorCode = GetLastError();
		print(std::string("Error! WriteProcessMemory failed in writeStringLimitedToMemory. Code: ").append(std::to_string(errorCode)).append(std::string(" .\r\n")));
	}
	free(newValuePadded);
}

void writeStringUnlimitedToMemory(HANDLE processHandle, void* address, char* newValue) {
	DWORD errorCode;
	int sizeNewValue = (int) strlen(newValue);
	errorCode = WriteProcessMemory(processHandle, address, newValue, sizeNewValue + 1, NULL);
	if (errorCode == 0) {
		errorCode = GetLastError();
		print(std::string("Error! WriteProcessMemory failed in writeStringUnlimitedToMemory. Code: ").append(std::to_string(errorCode)).append(std::string(" .\r\n")));
	}
}

void writeStringSafeToMemory(HANDLE processHandle, void* address, char* newValue) {
	DWORD errorCode;
	int sizeAddress = getSizeStringInMemory(processHandle, address);
	int sizeNewValue = (int) strlen(newValue);
	int sizeNewValuePadded = sizeAddress;
	char* newValuePadded = (char*) malloc(sizeNewValuePadded * sizeof(char));
	memset(newValuePadded, 0, sizeNewValuePadded);
	strncpy_s(newValuePadded, sizeNewValuePadded, newValue, sizeNewValuePadded - 1); // 4th argument must be smaller than the 2nd
	errorCode = WriteProcessMemory(processHandle, address, newValuePadded, sizeNewValuePadded, NULL);
	if (errorCode == 0) {
		errorCode = GetLastError();
		print(std::string("Error! WriteProcessMemory failed in writeStringSafeToMemory. Code: ").append(std::to_string(errorCode)).append(std::string(" .\r\n")));
	}
	free(newValuePadded);
}

bool isMemoryReadable(HANDLE processHandle, void* address) {
	DWORD buffer = 0, errorCode;
	errorCode = ReadProcessMemory(processHandle, address, &buffer, sizeof(buffer), NULL);
	if (errorCode == 0) {
		return false;
	}
	else {
		return true;
	}
}

DWORD readDwordFromMemory(HANDLE processHandle, void* address) {
	DWORD result, errorCode;
	errorCode = ReadProcessMemory(processHandle, address, &result, sizeof(result), NULL);
	if (errorCode == 0) {
		errorCode = GetLastError();
		// make sure this is compiled into 64bit or you'll get the 299 error: trying to read x64 with x86 exe
		print(std::string("Error! ReadProcessMemory failed in readDwordFromMemory. Code: ").append(std::to_string(errorCode)).append(std::string(" .\r\n")));
	}
	return result;
}

QWORD readQwordFromMemory(HANDLE processHandle, void* address) {
	QWORD result;
	DWORD errorCode;
	errorCode = ReadProcessMemory(processHandle, address, &result, sizeof(result), NULL);
	if (errorCode == 0) {
		errorCode = GetLastError();
		// make sure this is compiled into 64bit or you'll get the 299 error: trying to read x64 with x86 exe
		print(std::string("Error! ReadProcessMemory failed in readQwordFromMemory. Code: ").append(std::to_string(errorCode)).append(std::string(" .\r\n")));
	}
	return result;
}

char* readStringFromMemory(HANDLE processHandle, void* address) {
	char* result;
	int sizeResult;
	DWORD errorCode;
	sizeResult = 50;
	result = (char*) malloc(sizeResult * sizeof(char));
	errorCode = ReadProcessMemory(processHandle, address, result, sizeResult, NULL);
	if (errorCode == 0) {
		errorCode = GetLastError();
		print(std::string("Error! ReadProcessMemory failed in readStringFromMemory. Code: ").append(std::to_string(errorCode)).append(std::string(" .\r\n")));
	}
	return result;
}

int getSizeStringInMemory(HANDLE processHandle, void* address) {
	int count;
	BYTE byteBuffer;
	DWORD errorCode;
	QWORD addressPlusOffset;
	count = 0;
	do {
		addressPlusOffset = (QWORD)address + (QWORD)(count * sizeof(BYTE));
		errorCode = ReadProcessMemory(processHandle, (void*)addressPlusOffset, &byteBuffer, sizeof(BYTE), NULL);
		if (errorCode == 0) {
			errorCode = GetLastError();
			print(std::string("Error! ReadProcessMemory failed in getSizeStringInMemory. Code: ").append(std::to_string(errorCode)).append(std::string(" .\r\n")));
		}
		count++;

	} while (byteBuffer != '\0');
	return count;
}

int getMaxSizeStringInMemory(HANDLE processHandle, void* address) {
	int count;
	BYTE byteBuffer;
	DWORD errorCode;
	QWORD addressPlusOffset;
	bool pastZero = false;
	count = 0;
	do {
		addressPlusOffset = (QWORD)address + (QWORD)(count * sizeof(BYTE));
		errorCode = ReadProcessMemory(processHandle, (void*)addressPlusOffset, &byteBuffer, sizeof(BYTE), NULL);
		if (errorCode == 0) {
			errorCode = GetLastError();
			print(std::string("Error! ReadProcessMemory failed in getMaxSizeStringInMemory. Code: ").append(std::to_string(errorCode)).append(std::string(" .\r\n")));
		}
		count++;
		if (byteBuffer == '\0') {
			pastZero = true;
		}
	} while (pastZero == false || byteBuffer == '\0');
	return count - 2;
}
