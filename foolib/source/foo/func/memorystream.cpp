#include <foo/base/memory.h>
#include "memorystream.h"

using namespace NFoo;

CMemoryWriter::CMemoryWriter(void *_pAddress)
{
	Set(_pAddress);
}

CMemoryWriter::~CMemoryWriter()
{
}

void CMemoryWriter::Set(void *_pAddress)
{
	m_pAddress = (uint8*)_pAddress;
	m_Written = 0;
}

bool CMemoryWriter::IsReady()
{
	return m_pAddress != 0;
}

int32 CMemoryWriter::GetSize()
{
	return m_Written;
}

int32 CMemoryWriter::Write(const void *_pBuffer, int32 _Size)
{
	MemoryCopy(m_pAddress, _pBuffer, _Size);
	m_pAddress += _Size;
	m_Written += _Size;
	return _Size;
}

//
//
//
CMemoryReader::CMemoryReader(void *_pAddress, int32 _Size)
{
	Set(_pAddress, _Size);
}

CMemoryReader::~CMemoryReader()
{
}

void CMemoryReader::Set(void *_pAddress, int32 _Size)
{
	m_pAddress = (uint8*)_pAddress;
	m_Pos = 0;
	m_Size = _Size;
}

bool CMemoryReader::IsReady() { return m_pAddress != 0; }
int32 CMemoryReader::GetSize() { return m_Size; }
bool CMemoryReader::EndOfStream() {	return m_Pos >= m_Size; }

int32 CMemoryReader::Read(void *_pBuffer, int32 _Size)
{
	if(_pBuffer)
		MemoryCopy(_pBuffer, m_pAddress+m_Pos, _Size);
	m_Pos += _Size;
	return _Size;
}
