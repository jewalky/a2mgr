#ifndef NET_SHARED_H_INCLUDED
#define NET_SHARED_H_INCLUDED

#include "serialize.h"

class CPacket : public CArchive
{
    public:
		void FromPacket(uint8_t* data);
		uint32_t GetOrigin();

	protected:
		uint32_t origin;
};

#endif // NET_SHARED_H_INCLUDED
