#pragma once

#include <string>

struct CEMapObject
{
	CEMapObject();
	virtual ~CEMapObject();

	std::string Nick;
	std::string Clan;
};

CEMapObject* _stdcall CEMapObject_Create();