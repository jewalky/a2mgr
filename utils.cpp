#include "utils.h"

#include <cstdarg>
#include <cstdio>

using namespace std;

#include <windows.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

ofstream _LOG_FILE;
string   _LOG_NAME;

void _stdcall log_format(char *s, ...)
/* вывод в файл лога */
{
	char line[1256], l2[1256];
	va_list va;
	va_start(va, s);
	_vsnprintf(line, 1255, s, va);
	va_end(va);
	SYSTEMTIME tm;
	OemToChar(line, line);
	GetLocalTime(&tm);
	sprintf(l2,"[%d.%.2d.%.4d %d:%.2d:%.2d] %s", tm.wDay, tm.wMonth, tm.wYear, tm.wHour, tm.wMinute, tm.wSecond, /*tm.wMilliseconds, */line);
	_LOG_FILE.open(_LOG_NAME.c_str(), ios::out | ios::app);
	if(_LOG_FILE.is_open())
	{
		_LOG_FILE << l2;
		_LOG_FILE.flush();
		_LOG_FILE.close();
	}
}

void _stdcall log_format2(char *s, ...)
/* довывод в файл лога */
{
	char line[1256];
	va_list va;
	va_start(va, s);
	_vsnprintf(line, 1255, s, va);
	va_end(va);
	OemToChar(line, line);
	_LOG_FILE.open(_LOG_NAME.c_str(), ios::out | ios::app);
	if(_LOG_FILE.is_open())
	{
		_LOG_FILE << line;
		_LOG_FILE.flush();
		_LOG_FILE.close();
	}
}

vector<string> ParseCommandLine(string raw)
{
	for(size_t i = 0; i < raw.length(); i++)
		if(raw[i] == '\t') raw[i] = ' ';

	vector<string> parameters;
	parameters.push_back("");
	bool encaps = false;
    for (size_t i = 0; i < raw.size(); i++)
	{
		if (!encaps && !parameters.back().length() && IsWhitespace(raw[i]))
			continue;
		else if (raw[i] == '\\' && i+1 < raw.size())
		{
			parameters.back() += raw[i+1];
			i++;
		}
		else if (raw[i] == '"')
		{
			encaps = !encaps;
		}
		else if (!encaps && IsWhitespace(raw[i]))
		{
			parameters.push_back("");
		}
		else
		{
			parameters.back() += raw[i];
		}
	}

	if (!parameters.back().length())
		parameters.clear();

	return parameters;
}

/*
    utils::Format: функция, аналогичная sprintf(), но для STL строк.

    Возвращает отформатированную STL строку.

    format: формат
    ...: аргументы для формата
*/
std::string Format(const string format, ...)
{
    char line[2049];
    line[2048] = 0;
    va_list list;
    va_start(list, format);
    vsnprintf(line, 2048, format.c_str(), list);
    va_end(list);

    return string(line);
}

/*
    utils::Explode: функция для разбиения строки на подстроки.

    Возвращает массив из найденных подстрок.

    what: разбиваемая строка
    separator: разделитель
*/
vector<string> Explode(const string& what, const string& separator)
{
    string curstr;
    vector<string> retval;
    for(size_t i = 0; i < what.length(); i++)
    {
        if(what.find(separator, i) == i)
        {
            retval.push_back(curstr);
            curstr.assign("");
            i += separator.length()-1;
            continue;
        }

        curstr += what[i];
    }
    retval.push_back(curstr);
    return retval;
}

/*
    utils::TrimLeft, utils::TrimRight, utils::Trim: обрезать незначимые символы в начале строки (TrimLeft), в конце (TrimRight)
        или и там, и там (Trim).

    Возвращает обрезанную строку.

    what: обрезаемая строка
    callback: функция, определяющая значимость символа. См. IsWhitespace
*/
string TrimLeft(const string& what, bool (callback)(char))
{
    string ret = what;
    for(string::iterator i = ret.begin(); i != ret.end(); ++i)
    {
        if(callback((*i)))
            continue;
        ret.erase(ret.begin(), i);
        return ret;
    }
    return "";
}

string TrimRight(const string& what, bool (callback)(char))
{
    string ret = what;
    for(string::reverse_iterator i = ret.rbegin(); i != ret.rend(); ++i)
    {
        if(callback((*i)))
            continue;
        ret.erase(i.base(), ret.end());
        return ret;
    }
    return "";
}

string Trim(const string& what, bool (callback)(char))
{
    return TrimRight(TrimLeft(what, callback));
}

/*
    utils::IsWhitespace: вспомогательная функция для Trim*

    Возвращает true, если символ является пробелом и его нужно обрезать.

    what: проверяемый символ.
*/
bool IsWhitespace(char what)
{
    switch((unsigned char)what)
    {
        case ' ':
        case '\r':
        case '\n':
        case '\t':
        case 0xFF:
            return true;
        default:
            return false;
    }
}

/*
    utils::ToLower, utils::ToUpper: преобразование строки в верхний или нижний регистр.

    Возвращает преобразованную строку.

    what: Преобразовываемая строка.
*/
string ToLower(const string& what)
{
    string ret = what;
    for(string::iterator i = ret.begin(); i != ret.end(); ++i)
        (*i) = tolower((*i));
    return ret;
}

string ToUpper(const string& what)
{
    string ret = what;
    for(string::iterator i = ret.begin(); i != ret.end(); ++i)
        (*i) = toupper((*i));
    return ret;
}

unsigned long StrToInt(const string& what)
{
	unsigned long retval;
	sscanf(what.c_str(), "%u", &retval);
	return retval;
}

double StrToFloat(const string& what)
{
	double retval;
	sscanf(what.c_str(), "%f", &retval);
	return retval;
}

bool CheckInteger(const string& what)
{
	for(size_t i = 0; i < what.length(); i++)
		if(what[i] < 0x30 || 0x39 < what[i]) return false;
	return true;
}

bool StrToBoolean(const string& what)
{
	string cr = Trim(ToLower(what));
	if(cr == "yes" || cr == "true" || cr == "1" || cr == "y")
		return true;
	return false;
}

#include <fstream>

/*
    utils::FileExists: проверка на существование файла.

    Возвращает true, если файл существует.

    filename: название проверяемого файла.
*/
bool FileExists(const string& filename)
{
    ifstream f_temp;
    f_temp.open(filename.c_str(), ios::in | ios::binary);
    if(!f_temp.is_open()) return false;
    f_temp.close();
    return true;
}

/*
    utils::Basename: срезать путь к файлу, оставив только его название.

    Возвращаемое значение: обрезанная строка.

    filename: старое название файла.
*/
string Basename(const string& filename)
{
    //string ret = FixSlashes(filename);
	string ret = filename;
    unsigned long where = ret.find_last_of("/\\");
    if(where == string::npos) return ret;
    ret.erase(0, where+1);
    return ret;
}

string Dirname(const string& filename)
{
    //string ret = FixSlashes(filename);
	string ret = filename;
    unsigned long where = ret.find_last_of("/\\");
    if(where == string::npos) return ret;
    ret.erase(where);
    return ret;
}

/*
    utils::FixSlashes: сделать путь к файлу POSIX-совместимым (т.е. с человеческими слешами вместо backslash, используемого в DOS/Windows)

    Возвращаемое значение: исправленная строка.

    filename: старое название файла.
*/
string FixSlashes(const string& filename)
{
    string ret = filename;
    for(string::iterator i = ret.begin(); i != ret.end(); ++i)
        if((*i) == '\\') (*i) = '/';
    return ret;
}

/*
    utils::TruncateSlashes: удалить повторяющиеся слеши (напр. main//graphics/mainmenu//menu_.bmp).

    Возвращает исправленную строку.

    filename: старое название файла.
*/
string TruncateSlashes(const string& filename)
{
    string ret = filename;
    char lastchar = 0;
    char thischar = 0;
    for(string::iterator i = ret.begin(); i != ret.end(); ++i)
    {
        thischar = (*i);
        if((thischar == '/' || thischar == '\\') &&
           (lastchar == '/' || lastchar == '\\'))
        {
            ret.erase(i);
            i--;
        }
        lastchar = thischar;
    }
    return ret;
}

/*
    utils::CDirectory::Open: назначить папку, в которой будет производиться поиск.

    Возвращает true при успешном открытии папки.

    what: название папки.
*/
bool CDirectory::Open(const string& what)
{
    this->Close();
    /*this->directory = opendir(what.c_str());
    if(!this->directory) return false;*/
    return true;
}

/*
    utils::CDirectory::Read: прочитать следующий файл из папки.

    Возвращаемое значение: true если прочитан, false в противном случае.

    where: структура, куда будут помещены данные о файле.
*/
bool CDirectory::Read(CDirectoryEntry& where)
{
    /*if(!this->directory) return false;
    struct dirent* dp = readdir(this->directory);
    if(!dp) return false;
    if(dp->d_name[0] == '.') return this->Read(where);
    where.name.assign(dp->d_name);*/
    return true;
}

/*
    utils::CDirectory::Close: закрыть папку.
*/
void CDirectory::Close()
{
    //if(this->directory) closedir(this->directory);
}

// На всякий, собственно, пожарный
CDirectory::~CDirectory()
{
    this->Close();
}

CDirectory::CDirectory()
{
    //this->directory = NULL;
}

unsigned long _stdcall _call_func(unsigned long addr, unsigned long countp, ...)
{
	unsigned long xebp;
	__asm mov xebp, ebp;
	xebp += 16; // ebp + 8 = first parameter, ebp + 12 = second parameter, ebp + 16 = all remaining
	for(unsigned long i = 0; i < countp; i++)
	{
		__asm
		{
			mov		eax, [xebp]
			push	[eax]
		}
		xebp += 4;
	}
	__asm
	{
		mov		edx, [addr]
		call	edx
	}
}

unsigned long _stdcall _call_member_func(unsigned long addr, unsigned long cptr, unsigned long countp, ...)
{
	unsigned long xebp;
	__asm mov xebp, ebp;
	xebp += 24; // ebp + 8 = first parameter, ebp + 12 = second parameter, ebp + 16 = third parameter,
				// ebp + 24 = all remaining
	for(unsigned long i = 0; i < countp; i++)
	{
		__asm
		{
			mov		eax, [xebp]
			push	[eax]
		}
		xebp += 4;
	}
	__asm
	{
		mov		ecx, [cptr]
		mov		edx, [addr]
		call	edx
	}
}

unsigned long _stdcall _call_virtual_func(unsigned long cptr, unsigned long xoffset, unsigned long countp, ...)
{
	unsigned long xebp;
	__asm mov xebp, ebp;
	xebp += 24; // ebp + 8 = first parameter, ebp + 12 = second parameter, ebp + 16 = third parameter,
				// ebp + 24 = all remaining
	for(unsigned long i = 0; i < countp; i++)
	{
		__asm
		{
			mov		eax, [xebp]
			push	[eax]
		}
		xebp += 4;
	}
	__asm
	{
		mov		ecx, [cptr]
		mov		edx, [ecx]
		mov		eax, [xoffset]
		call	[edx+eax]
	}
}

unsigned long __declspec(naked) _get_this()
{
	__asm
	{
		mov		eax, ecx
		retn
	}
}