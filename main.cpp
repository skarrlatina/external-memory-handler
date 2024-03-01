#include "memory.h"

int main()
{
	memory mem;

	if (mem.AttachProcess("name.exe"))
	{
		MessageBoxA(NULL, "Successfully attached to the process.", "Success", MB_OK | MB_ICONINFORMATION);
	}
	//..
}