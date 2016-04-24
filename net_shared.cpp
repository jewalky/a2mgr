#include "net_shared.h"

void CPacket::FromPacket(uint8_t* data)
{
	uint32_t size = *(uint32_t*)(data);
	this->origin = *(uint32_t*)(data + 4);
	this->SetAllData(data+8, size);
	this->ResetPosition();
}

uint32_t CPacket::GetOrigin()
{
	return this->origin;
}