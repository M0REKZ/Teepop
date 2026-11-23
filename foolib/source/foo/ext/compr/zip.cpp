#include "zip.h"

using namespace NFoo;

//
// RFC 1950 for zlib format specification
//
CZReader::CZReader(CInputStream &rSource)
: m_rSource(rSource), m_DeflateReader(rSource)
{
	m_pError = NULL;
	m_Ready = true;
	int32 CMF = m_rSource.ReadNE<uint8>();
	int32 Flags = m_rSource.ReadNE<uint8>();
	int32 Method = CMF&0xF;
	int32 CompInfo = CMF>>4;
	int32 Check = Flags&0xf;
	bool Dict = (Flags>>5)&1;
	int32 Level = Flags>>6;

	if((CMF*256+Flags)%31)
		m_pError = "Unable to load. zlib header check failed";

	if(Dict)
		m_pError = "Unable to load. Don't know how to unpack when the dict flag is set";
	
	if(Method != 8)
		m_pError = "Unable to load. Unknown zlib compression method";

	if(m_pError)
		m_Ready = false;
}

bool CZReader::IsReady() { return m_Ready && m_rSource.IsReady(); }
bool CZReader::EndOfStream() { return m_rSource.EndOfStream(); }

int32 CZReader::Read(void *pBuffer, int32 Size)
{
	return m_DeflateReader.Read(pBuffer, Size);
}
