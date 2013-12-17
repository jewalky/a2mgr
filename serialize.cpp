#include "serialize.h"

#include <cstring>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

CArchive::CArchive()
{
    myData.clear();
    myPosRead = 0;
    myPosWrite = 0;
    myFail = false;
}

CArchive::~CArchive()
{
    myData.clear();
}

void CArchive::AppendData(uint8_t* data, uint32_t count)
{
    for(uint32_t i = 0; i < count; i++)
    {
        myData.push_back(data[i]);
        myPosWrite++;
    }
}

void CArchive::GetData(uint8_t* data, uint32_t count)
{
    uint32_t oldPosRead = myPosRead;
    for(uint32_t i = 0; i < count; i++)
    {
        if(myPosRead >= myData.size())
        {
            for(uint32_t j = 0; j < count; j++)
                data[j] = 0;
            myFail = true;
            break;
        }
        data[i] = myData[oldPosRead+i];
        myPosRead++;
    }
}

CArchive& CArchive::operator << (uint8_t data)
{
    AppendData(&data, 1);
    return *this;
}

CArchive& CArchive::operator << (uint16_t data)
{
    AppendData((uint8_t*)&data, 2);
    return *this;
}

CArchive& CArchive::operator << (uint32_t data)
{
    AppendData((uint8_t*)&data, 4);
    return *this;
}

CArchive& CArchive::operator << (uint64_t data)
{
    AppendData((uint8_t*)&data, 8);
    return *this;
}

CArchive& CArchive::operator << (int8_t data)
{
    AppendData((uint8_t*)&data, 1);
    return *this;
}

CArchive& CArchive::operator << (int16_t data)
{
    AppendData((uint8_t*)&data, 2);
    return *this;
}

CArchive& CArchive::operator << (int32_t data)
{
    AppendData((uint8_t*)&data, 4);
    return *this;
}

CArchive& CArchive::operator << (int64_t data)
{
    AppendData((uint8_t*)&data, 8);
    return *this;
}

CArchive& CArchive::operator << (const std::string& data)
{
    uint32_t strsiz = data.length();
    AppendData((uint8_t*)&strsiz, 4);
    for(uint32_t i = 0; i < data.length(); i++)
    {
        if(!data[i]) break;
        AppendData((uint8_t*)&data[i], 1);
    }
    return *this;
}

CArchive& CArchive::operator << (const char* data)
{
    uint32_t strsiz = strlen(data);
    AppendData((uint8_t*)&strsiz, 4);
    AppendData((uint8_t*)data, strsiz);
    return *this;
}

CArchive& CArchive::operator << (bool data)
{
    uint8_t what = (uint8_t)(data & 1);
    AppendData(&what, 1);
    return *this;
}

CArchive& CArchive::operator >> (uint8_t &data)
{
    GetData(&data, 1);
    return *this;
}

CArchive& CArchive::operator >> (uint16_t &data)
{
    GetData((uint8_t*)&data, 2);
    return *this;
}

CArchive& CArchive::operator >> (uint32_t &data)
{
    GetData((uint8_t*)&data, 4);
    return *this;
}

CArchive& CArchive::operator >> (uint64_t &data)
{
    GetData((uint8_t*)&data, 8);
    return *this;
}

CArchive& CArchive::operator >> (int8_t &data)
{
    GetData((uint8_t*)&data, 1);
    return *this;
}

CArchive& CArchive::operator >> (int16_t &data)
{
    GetData((uint8_t*)&data, 2);
    return *this;
}

CArchive& CArchive::operator >> (int32_t &data)
{
    GetData((uint8_t*)&data, 4);
    return *this;
}

CArchive& CArchive::operator >> (int64_t &data)
{
    GetData((uint8_t*)&data, 8);
    return *this;
}

CArchive& CArchive::operator >> (std::string& data)
{
    uint32_t strsiz;
    GetData((uint8_t*)&strsiz, 4);
    data.resize(strsiz);
    for(uint32_t i = 0; i < strsiz; i++)
        GetData((uint8_t*)&data[i], 1);
    return *this;
}

CArchive& CArchive::operator >> (char*& data)
{
    uint32_t strsiz;
    GetData((uint8_t*)&strsiz, 4);
    data = new char[strsiz+1];
    data[strsiz] = 0;
    GetData((uint8_t*)data, strsiz);
    return *this;
}

CArchive& CArchive::operator >> (bool& data)
{
    uint8_t wh;
    GetData(&wh, 1);
    data = wh;
    return *this;
}


CArchive::operator bool ()
{
    return !myFail;
}

void CArchive::GetAllData(uint8_t*& buf, uint32_t& count)
{
    count = myData.size();
    buf = new uint8_t[count];
    for(uint32_t i = 0; i < count; i++)
        buf[i] = myData[i];
}

void CArchive::SetAllData(uint8_t* buf, uint32_t count)
{
    myData.clear();

    for(uint32_t i = 0; i < count; i++)
        myData.push_back(buf[i]);
}

void CArchive::SaveToFile(string filename)
{
    ofstream f_temp;
    f_temp.open(filename.c_str(), ios::out | ios::binary);
    if(!f_temp.is_open())
    {
        myFail = true;
        return;
    }
    SaveToStream(f_temp);
    f_temp.close();
}

void CArchive::LoadFromFile(string filename)
{
    ifstream f_temp;
    f_temp.open(filename.c_str(), ios::in | ios::binary);
    if(!f_temp.is_open())
    {
        myFail = true;
        return;
    }
    LoadFromStream(f_temp);
    f_temp.close();
}

void CArchive::SaveToStream(ostream& stream)
{
    uint8_t* buf = NULL;
    uint32_t siz = 0;
    GetAllData(buf, siz);
    stream.write((char*)buf, siz);
    delete buf;
}

void CArchive::LoadFromStream(istream& stream)
{
    uint8_t* buf = NULL;
    stream.seekg(0, ios::end);
    uint32_t siz = stream.tellg();
    stream.seekg(0, ios::beg);
    buf = new uint8_t[siz];
    stream.read((char*)buf, siz);

    SetAllData(buf, siz);

    delete buf;
}

void CArchive::Reset()
{
    SetAllData(NULL, 0);
    myPosRead = 0;
    myPosWrite = 0;
}

void CArchive::ResetPosition()
{
	myPosRead = 0;
}