#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <string>
#include <vector>

#include <fstream>

extern std::ofstream _LOG_FILE;
extern std::string	 _LOG_NAME;

void _stdcall log_format(char *s, ...);
void _stdcall log_format2(char *s, ...);

std::vector<std::string> ParseCommandLine(std::string raw);

// string-related
std::string Format(const std::string format, ...);
std::vector<std::string> Explode(const std::string& what, const std::string& separator);
bool IsWhitespace(char what);
std::string TrimLeft(const std::string& what, bool (callback)(char) = IsWhitespace);
std::string TrimRight(const std::string& what, bool (callback)(char) = IsWhitespace);
std::string Trim(const std::string& what, bool (callback)(char) = IsWhitespace);
std::string ToLower(const std::string& what);
std::string ToUpper(const std::string& what);
unsigned long StrToInt(const std::string& what);
double StrToFloat(const std::string& what);
bool StrToBoolean(const std::string& what);
bool CheckInteger(const std::string& what);

// file related
bool FileExists(const std::string& file);
std::string Basename(const std::string& file);
std::string Dirname(const std::string& filename);
std::string FixSlashes(const std::string& filename);
std::string TruncateSlashes(const std::string& filename);

// helper
unsigned long _stdcall _call_func(unsigned long addr, unsigned long countp = 0, ...);
unsigned long _call_member_func(unsigned long addr, unsigned long cptr, unsigned long countp = 0, ...);
unsigned long _call_virtual_func(unsigned long cptr, unsigned long offset, unsigned long countp = 0, ...);
unsigned long _get_this();
#define _fst(count) __asm add esp, count

// directory search
struct CDirectoryEntry
{
    std::string name;
};

class CDirectory
{
public:
    CDirectory();
    ~CDirectory();

    bool Open(const std::string& what);
    bool Read(CDirectoryEntry& where);
    void Close();

protected:
    //DIR* directory;
};


#endif // UTILS_H_INCLUDED
