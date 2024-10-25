#include <iostream>

#include "CommandLineToArgv.h"

#ifdef _WIN32
#include <tchar.h>
#else
#include <unistd.h>
#include <fcntl.h>

/**
 * \brief Gets the argv and argc values for the current process.
 *
 * Reads the /proc/self/cmdline file and returns argv - an array of 
 * pointers to command line arguments, and argc - a counter of such 
 * arguments.
 *
 * \param argc Pointer to an int that receives the number of array
 * elements.
 * \return a pointer to an array of T* values (argv).
 */
char** get_argv_argc(int* argc)
{
	if (!argc)
		return nullptr;

	char** argv = nullptr;

	int fd = open("/proc/self/cmdline", O_RDONLY);
	if (fd != -1)
	{
		const size_t STACK_PATH_SIZE = 0x4FF;
		char buf[STACK_PATH_SIZE] { 0 };
		int num_readed = read(fd, buf, STACK_PATH_SIZE);
		if (num_readed > 0)
		{
			size_t num_bytes = 0;
			while(!argv)
			{
				char* p_src = buf;
				char* p_dst = nullptr;
				int n = num_readed;
				bool in_arg = false;
				if (num_bytes)
				{
					argv = (char**)calloc(sizeof(char*) * (*argc + 1) + num_bytes, 1);
					p_dst = (char*)(argv + (*argc + 1));
				}
				*argc = 0;
				while (n--)
				{
					if (*p_src)
					{
						if (!in_arg)
						{
							in_arg = true;
							if (argv)
								argv[*argc] = p_dst;
							++(*argc);
						}
						if (p_dst)
							*p_dst++ = *p_src;
						++num_bytes;
					}
					else if (in_arg)
					{
						in_arg = false;
						if (p_dst)
							*p_dst++ = *p_src;
						++num_bytes;
					}
					++p_src;
				}
			}
		}
		close(fd);
	}

	return argv;
}

typedef char TCHAR;
typedef char* LPTSTR;
#define _tmain main

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
int _tmain(int argc, TCHAR** argv)
{
	std::cout << "original:\n";

	print_args(argc, argv);

	std::cout << "implementation:\n";

	int arg_c = 0;
	LPTSTR* arg_v =
#ifdef _WIN32
		CommandLineToArgv(::GetCommandLine(), &arg_c);
#else
		get_argv_argc(&arg_c);
#endif

	print_args(arg_c, arg_v);

	::free(arg_v);

	return 0;
}

