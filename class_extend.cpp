#include <windows.h>
#include <map>
HANDLE AssocMutex = NULL;
std::map<void*, void*> AssocList;

void _stdcall AssociateObject(void* original_class, void* attached_class)
{
	if(!AssocMutex) AssocMutex = CreateMutex(NULL, TRUE, NULL);
	else WaitForSingleObject(AssocMutex, INFINITE);
	AssocList[original_class] = attached_class;
	ReleaseMutex(AssocMutex);
}

void* _stdcall GetAssociated(void* original_class)
{
	if(!AssocMutex) AssocMutex = CreateMutex(NULL, TRUE, NULL);
	else WaitForSingleObject(AssocMutex, INFINITE);
	void* obj = NULL;
	for(std::map<void*, void*>::iterator it = AssocList.begin(); it != AssocList.end(); ++it)
	{
		if(it->first == original_class)
		{
			obj = it->second;
			break;
		}
	}
	ReleaseMutex(AssocMutex);
	return obj;
}

void _stdcall FreeAssociated(void* original_class)
{
	if(!AssocMutex) AssocMutex = CreateMutex(NULL, TRUE, NULL);
	else WaitForSingleObject(AssocMutex, INFINITE);
	for(std::map<void*, void*>::iterator it = AssocList.begin(); it != AssocList.end(); ++it)
	{
		if(it->first == original_class)
		{
			delete it->second;
			AssocList.erase(it);
			break;
		}
	}
	ReleaseMutex(AssocMutex);
}