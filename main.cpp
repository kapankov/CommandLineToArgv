#include <iostream>

#include "CommandLineToArgv.h"

#ifdef _WIN32
#include <tchar.h>
#else
typedef char TCHAR;
typedef char* LPTSTR;
#endif

void print_args(int argc, TCHAR** argv)
{
	while (argc--)
#ifdef _UNICODE
		std::wcout << L"[" << *argv++ << L"]\n";
#else
		std::cout << "[" << *argv++ << "]\n";
#endif
}

int _tmain(int argc, TCHAR** argv)
{
	std::cout << "original:\n";

	print_args(argc, argv);

	std::cout << "implementation:\n";

	int arg_c = 0;
	LPTSTR* arg_v = CommandLineToArgv(::GetCommandLine(), &arg_c);

	print_args(arg_c, arg_v);

	::free(arg_v);

	return 0;
}
