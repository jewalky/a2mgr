#include "utils.hpp"
#include "stdint.h"

using namespace std;

bool CheckBool(const string& what)
{
    string wh2 = ToLower(Trim(what));
    if(wh2 == "true" || wh2 == "false" || wh2 == "yes" || wh2 == "no" || wh2 == "y" || wh2 == "n" || wh2 == "0" || wh2 == "1")
        return true;
    return false;
}

bool StrToBool(const string& what)
{
	string cr = Trim(ToLower(what));
	if(cr == "yes" || cr == "true" || cr == "1" || cr == "y")
		return true;
	return false;
}