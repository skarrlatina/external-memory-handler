//MIT License
//
//Copyright(c) 2024 skarrlatina
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <string_view>
#include <comdef.h>

class memory
{
public:
	BOOL AttachProcess(std::string_view processName) noexcept
	{
		const HANDLE handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		::PROCESSENTRY32 entry = { };
		entry.dwSize = sizeof(PROCESSENTRY32);

		if (handle != INVALID_HANDLE_VALUE)
		{
			::Process32First(handle, &entry);
			do
			{
				if (!processName.compare((_bstr_t)entry.szExeFile))
				{
					_processId = entry.th32ProcessID;
					::CloseHandle(handle);
					_process = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, _processId);
					return TRUE;
				}
			} while (::Process32Next(handle, &entry));
		}
		return FALSE;
	}

	uintptr_t GetModuleAddress(std::string_view moduleName) const noexcept
	{
		const HANDLE module = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, _processId);
		::MODULEENTRY32 entry = { };
		entry.dwSize = sizeof(MODULEENTRY32);

		if (module != INVALID_HANDLE_VALUE)
		{
			::Module32First(module, &entry);
			do
			{
				if (!moduleName.compare((_bstr_t)entry.szModule))
				{
					::CloseHandle(module);
					return reinterpret_cast<std::uintptr_t>(entry.hModule);
				}
			} while (::Module32Next(module, &entry));
		}
		return NULL;
	}

	template<class T>
	T ReadMemory(const std::uintptr_t dwAddress) const noexcept
	{
		T _read;
		::ReadProcessMemory(_process, reinterpret_cast<const void*>(dwAddress), &_read, sizeof(T), NULL);
		return _read;
	}

	template<class T>
	void WriteMemory(const std::uintptr_t dwAddress, const T& value) const noexcept
	{
		::WriteProcessMemory(_process, reinterpret_cast<void*>(dwAddress), &value, sizeof(T), NULL);
	}

	void DetachProcess()
	{
		::CloseHandle(_process);
	}
private:
	void* _process = nullptr;
	std::uintptr_t _processId = NULL;
};