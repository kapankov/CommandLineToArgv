#pragma once

#include <type_traits>

#ifdef _WIN32
#include <windows.h>
#else
#define MAX_PATH 260
#endif

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

	template<>
	int GetCurrentModuleFileName<char>(char* lpFilename, int nSize)
	{
#ifdef _WIN32
		return GetModuleFileNameA(nullptr, lpFilename, nSize);
#else
		return 0;
#endif
	}

	template<>
	int GetCurrentModuleFileName<wchar_t>(wchar_t* lpFilename, int nSize)
	{
#ifdef _WIN32
		return GetModuleFileNameW(nullptr, lpFilename, nSize);
#else
		return 0;
#endif
	}
} // end of detail namespace

#define L(x) L##x
#define TT( T, V ) (detail::tchar<T>(V, L(V)))


template<typename T>
void parse_cmdline(
	T* cmdline,
	T** argv,
	int* num_args,
	int* num_bytes
)
{
	T* p = cmdline;
	T* lpstr = nullptr;

	if (argv && num_args)
		lpstr = (T*)(((char*)argv) + *num_args * sizeof(T*));

	*num_bytes = 0;
	*num_args = 1;

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
		++*num_args;

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

template<typename T>
T** CommandLineToArgv(const T* lpCmdLine, int* pNumArgs)
{
	if (!pNumArgs)
		throw std::invalid_argument("Invalid arguments of the CommandLineToArgv function");

	T mod_name[MAX_PATH];
	T* cmd_start = (T*)lpCmdLine;

	if (!lpCmdLine || *lpCmdLine == TT(T, '\0'))
	{
		detail::GetCurrentModuleFileName(mod_name, sizeof(mod_name) / sizeof(T));
	}

	int num_bytes = 0;

	parse_cmdline<T>(cmd_start, nullptr, pNumArgs, &num_bytes);

	T** argv = (T**)::calloc((*pNumArgs + 1) * sizeof(T*) + num_bytes, 1);
	if (!argv)
		throw std::bad_alloc();

	parse_cmdline<T>(cmd_start, argv,
		pNumArgs, &num_bytes);

	return argv;
}
