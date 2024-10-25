/**
 * \file CommandLineToArgv.h
 *
 * \brief C++ implementation of the CommandLineToArgv function
 *
 * \author Konstantin A. Pankov
 * Contact: explorus@mail.ru
 *
 * \copyright Copyright (c) 2024 Konstantin A. Pankov
 *
 * \license  Licensed under the MIT License.
 */
#ifndef COMMANDLINETOARGV_H
#define COMMANDLINETOARGV_H

#include <type_traits>

#ifdef _WIN32
#include <windows.h>
#else
#define MAX_PATH 260
#endif // _WIN32

namespace detail
{
	// You are not expected to understand this.
	template<typename T>
	T tchar(char c, wchar_t W);

	template<>
	char tchar<char>(char c, wchar_t w)	{ return c; }

	template<>
	wchar_t tchar<wchar_t>(char c, wchar_t w) { return w; }

	template<typename T>
	int GetCurrentModuleFileName(T* lpFilename, int nSize);
	int GetCurrentModuleFileName(T* lpFilename, int nSize);

	template<>
	int GetCurrentModuleFileName<char>(char* lpFilename, int nSize)
	int GetCurrentModuleFileName<char>(char* lpFilename, int nSize)
	{
#ifdef _WIN32
		return GetModuleFileNameA(nullptr, lpFilename, nSize);
#else
		// not implemented
		return 0;
#endif // _WIN32
	}

	template<>
	int GetCurrentModuleFileName<wchar_t>(wchar_t* lpFilename, int nSize)
	int GetCurrentModuleFileName<wchar_t>(wchar_t* lpFilename, int nSize)
	{
#ifdef _WIN32
		return GetModuleFileNameW(nullptr, lpFilename, nSize);
#else
		// not implemented
		return 0;
#endif // _WIN32
	}
} // detail namespace

#define L(x) L##x
#define TT( T, V ) (detail::tchar<T>(V, L(V)))

/**
 * \brief Parses the command line.
 *
 * Parses the command line and if argv and argc arguments are specified,
 * returns the command line arguments, otherwise calculates the amount
 * of memory to allocate for argv.
 *
 * \param cmdline [in] Command string (pointer to char or wchar_t).
 * \param argv [int, out] Pointer to an array of arguments (strings).
 * \param argc [in, out] Number of arguments on the command line.
 * \param num_bytes [out] Number of bytes required for argv.
 * \return No return.
 */
template<typename T>
void parse_cmdline(
	T* cmdline,
	T** argv,
	int* argc,
	int* num_bytes
)
{
	T* p = cmdline;
	T* lpstr = nullptr;

	if (argv && argc)
		lpstr = (T*)(((char*)argv) + *argc * sizeof(T*));

	*num_bytes = 0;
	*argc = 1;

	if (argv)
		*argv++ = lpstr;

	if (*p == TT(T, '\"'))
	{
		++p;
		do
		{
			*num_bytes += sizeof(T);
			if (*p == TT(T, '\"') || *p == TT(T, '\0'))
			{
				if (lpstr)
					*(lpstr++) = TT(T, '\0');
				if (*p == TT(T, '\"'))
					++p;
				break;
			}
			if (lpstr)
				*(lpstr++) = *p;
			++p;
		} while (1);
	}
	else
	{
		do {
			*num_bytes += sizeof(T);
			if (*p <= TT(T, ' '))
			{
				if (lpstr)
					*(lpstr++) = TT(T, '\0');
				break;
			}
			if (lpstr)
				*(lpstr++) = *p;
			++p;
		} while (1);
	}

	bool inquote = false;

	do
	{
		while (*p == TT(T, ' ') || *p == TT(T, '\t'))
			++p;

		if (*p == TT(T, '\0'))
			break;

		if (argv)
			*argv++ = lpstr;
		++*argc;

		do
		{
			bool copy_char = true;
			short num_slash = 0;

			while (*p == TT(T, '\\'))
			{
				++num_slash;
				++p;
			}

			if (*p == TT(T, '\"'))
			{
				if (!(num_slash & 1))
				{
					if (inquote && p[1] == TT(T, '\"'))
						++p;
					else
					{
						copy_char = false;
						inquote = !inquote;
					}
				}
				num_slash >>= 1; // num_slash /= 2;
			}

			while (num_slash--)
			{
				if (lpstr)
					*lpstr++ = TT(T, '\\');
				*num_bytes += sizeof(T) * num_slash;
			}

			if (*p == TT(T, '\0') || (!inquote && (*p == TT(T, ' ') || *p == TT(T, '\t'))))
				break;
			else if (copy_char)
			{
				if (lpstr)
					*lpstr++ = *p;
				*num_bytes += sizeof(T);
			}
			++p;
		} while (1);

		if (lpstr)
			*lpstr++ = TT(T, '\0');
		*num_bytes += sizeof(T);
	} while (1);

}

/**
 * \brief Gets argv and argc values from the command line.
 *
 * Parses a Unicode command line string and returns an array of pointers
 * to the command line arguments, along with a count of such arguments,
 * in a way that is similar to the standard C run-time argv and argc values.
 *
 * \param lpCmdLine Pointer to a null-terminated Unicode string that
 * contains the full command line. If this parameter is an empty string
 * the function returns the path to the current executable file
 * \param pNumArgs Pointer to an int that receives the number of array
 * elements returned, similar to argc.
 * \return A pointer to an array of T* values, similar to argv.
 */
template<typename T>
T** CommandLineToArgv(const T* lpCmdLine, int* pNumArgs)
{
	if (!pNumArgs)
		throw std::invalid_argument("Invalid arguments of the CommandLineToArgv function");
	if (!pNumArgs)
		throw std::invalid_argument("Invalid arguments of the CommandLineToArgv function");

	T mod_name[MAX_PATH];
	T* cmd_start = (T*)lpCmdLine;

	if (!lpCmdLine || *lpCmdLine == TT(T, '\0'))
	{
		detail::GetCurrentModuleFileName(mod_name, sizeof(mod_name) / sizeof(T));
		detail::GetCurrentModuleFileName(mod_name, sizeof(mod_name) / sizeof(T));
	}

	int num_bytes = 0;
	int num_bytes = 0;

	parse_cmdline<T>(cmd_start, nullptr, pNumArgs, &num_bytes);

	T** argv = (T**)::calloc((*pNumArgs + 1) * sizeof(T*) + num_bytes, 1);
	if (!argv)
		throw std::bad_alloc();
	if (!argv)
		throw std::bad_alloc();

	parse_cmdline<T>(cmd_start, argv,
		pNumArgs, &num_bytes);

	return argv;
}

#endif // COMMANDLINETOARGV_H
