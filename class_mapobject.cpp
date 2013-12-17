#include "class_extend.h"
#include "class_mapobject.h"
#include "utils.h"

CEMapObject* _stdcall CEMapObject_Create()
{
	return new CEMapObject();
}

void CEMapObject_Copy(CEMapObject* first, CEMapObject* second)
{

}

CEMapObject::CEMapObject()
{
	log_format("object created.\n");
}

CEMapObject::~CEMapObject()
{
	log_format("object destroyed.\n");
}

// CMapObject:
// 464CC0 == constructor
// 464F02 == copy constructor
// 472D10 == destructor

// inject to: 464EEA
void _declspec(naked) iCMapObject_Construct()
{
	__asm
	{
		call	CEMapObject_Create
		push	eax
		push	eax
		mov		eax, [ebp-0x1C] // this
		push	eax
		call	AssociateObject
		mov		eax, [ebp+0x08]
		call	GetAssociated
		pop		ebx
		push	eax
		push	ebx
		call	CEMapObject_Copy
		mov		[ebp-0x04], 0xFFFFFFFF
		mov		eax, [ebp-0x1C]
		mov		ecx, [ebp-0x0C]
		mov		fs:[0], ecx
		mov		esp, ebp
		pop		ebp
		retn	4
	}
}

// inject to: 465380
void _declspec(naked) iCMapObject_CopyConstruct()
{
	__asm
	{
		/*call	CEMapObject_Create
		push	eax
		mov		eax, [ebp-0x14] // this
		push	eax
		call	AssociateObject*/
		mov		[ebp-0x04], 0xFFFFFFFF
		mov		eax, [ebp-0x14]
		mov		ecx, [ebp-0x0C]
		mov		fs:[0], ecx
		mov		esp, ebp
		pop		ebp
		retn
	}
}