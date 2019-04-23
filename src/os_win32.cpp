#include <cc/os.h>

#include <Windows.h>

void cc::TryKeepConsoleOpen()
{
	if (IsDebuggerPresent())
	{
		system("pause");
	}
}