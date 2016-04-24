#ifndef SERIALIZE_H_INCLUDED
#define SERIALIZE_H_INCLUDED

#include <vector>
#include <string>

#include "stdint.h"

class CArchive
{
    public:
        CArchive();
        ~CArchive();

        CArchive& operator << (uint8_t second);
        CArchive& operator << (uint16_t second);
        CArchive& operator << (uint32_t second);
        CArchive& operator << (uint64_t second);
        CArchive& operator << (int8_t second);
        CArchive& operator << (int16_t second);
        CArchive& operator << (int32_t second);
        CArchive& operator << (int64_t second);
        CArchive& operator << (const std::string& second);
        CArchive& operator << (const char* second);
        CArchive& operator << (bool second);

        CArchive& operator >> (uint8_t& second);
        CArchive& operator >> (uint16_t& second);
        CArchive& operator >> (uint32_t& second);
        CArchive& operator >> (uint64_t& second);
        CArchive& operator >> (int8_t& second);
        CArchive& operator >> (int16_t& second);
        CArchive& operator >> (int32_t& second);
        CArchive& operator >> (int64_t& second);
        CArchive& operator >> (std::string& second);
        CArchive& operator >> (char*& second);
        CArchive& operator >> (bool& second);

        operator bool ();

        void AppendData(uint8_t* buf, uint32_t count);
        void GetData(uint8_t* buf, uint32_t count);

        void GetAllData(uint8_t*& buf, uint32_t& count);
        void SetAllData(uint8_t* buf, uint32_t count);

        void SaveToFile(std::string filename);
        void LoadFromFile(std::string filename);

        void SaveToStream(std::ostream& stream);
        void LoadFromStream(std::istream& stream);

        void Reset();
		void ResetPosition();

    protected:
        uint32_t myPosRead;
        uint32_t myPosWrite;

        bool myFail;

        std::vector<uint8_t> myData;
};

#endif // SERIALIZE_H_INCLUDED
