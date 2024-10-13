#include <iostream>

#include "CommandLineToArgv.h"

void print_args(int argc, _TCHAR** argv)
{
	while (argc--)
#ifdef _UNICODE
		std::wcout << L"[" << *argv++ << L"]\n";
#else
		std::cout << "[" << *argv++ << "]\n";
#endif
}

int _tmain(int argc, _TCHAR** argv)
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