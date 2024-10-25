# C++ implementation of the CommandLineToArgv function

WinAPI has a function **CommandLineToArgvW** (but there is no version for ANSI strings - CommandLineToArgvA), which according to the documentation ‘*Parses a Unicode command line string and returns an array of pointers to the command line arguments, along with a count of such arguments, in a way that is similar to the standard C run-time argv and argc values*’.

This function returns ‘*a pointer to an array of LPWSTR values, similar to argv*’.

## Using

**CommandLineToArgv** is implemented in C++14\STL via a template, without the use of third-party libraries, and works in the same manner as CommandLineToArgvW. Unlike CommandLineToArgvW, it returns the same result as the arguments of the main(int argc, char** argv) function and can work in both _UNICODE and ANSI versions.
```
template<typename T>
T** CommandLineToArgv(const T* lpCmdLine, int* pNumArgs)
```
You only need the CommandLineToArgv.h file

`#include "CommandLineToArgv.h"`

You can use it like this:
```
int argc = 0;
LPTSTR* argv = CommandLineToArgv(::GetCommandLine(), &argc);
```
To free the memory, use a single call to the **free** function (Not LocalFree).

## Motivation

CommandLineToArgvW works correctly in most cases. But the following command line (to be honest, it is extremely untypical and not quite correct):

`myapp.exe "abc"" ABC"`

gives different result in main(int argc, char** argv) and in CommandLineToArgvW.

Here is a small code to test this problem (works only with _UNICODE):
```
#include <iostream>

#include <windows.h>
#include <tchar.h>

void print_args(int argc, _TCHAR** argv)
{
	while (argc--)
		std::wcout << L"[" << *argv++ << L"]\n";
}

int _tmain(int argc, _TCHAR** argv)
{
	std::wcout << L"from main:\n";

	print_args(argc, argv);

	std::wcout << L"from CommandLineToArgvW:\n";

	int arg_c = 0;

	LPTSTR* arg_v = ::CommandLineToArgvW(::GetCommandLine(), &arg_c);

	print_args(arg_c, arg_v);

	::LocalFree(arg_v);

	return 0;
}
```
I haven't checked this error on all versions of Windows, but it is definitely present in Windows 11 Pro 23H2 (you can check this yourself in other versions)
