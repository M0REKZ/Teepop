#include "filestream.h"
#include <cstdio>

using namespace std;
using namespace NFoo;

CFileWriter NFoo::g_Stdout((void *)stdout);
CFileWriter NFoo::g_Stdin((void *)stdin);


CFileWriter::CFileWriter(void *pHandle)
{
	m_pFileHandle = pHandle;
}

//
//
//
CFileWriter::CFileWriter(const int8 *_pFilename, int32 _Mode)
{
	m_pFileHandle = NULL;
	if(_pFilename)
		Open(_pFilename, _Mode);
}

//
//
//
bool CFileWriter::Open(const int8 *_pFilename, int32 _Mode)
{
	switch(_Mode)
	{
		case EText: m_pFileHandle = fopen((char*)_pFilename, "wt"); break;
		case EBinary: m_pFileHandle = fopen((char*)_pFilename, "wb"); break;
		case ETextAppend: m_pFileHandle = fopen((char*)_pFilename, "at"); break;
		case EBinaryAppend: m_pFileHandle = fopen((char*)_pFilename, "ab"); break;
		default:
			throw "(CFileWriter::Open): Invalid mode";
	}

	if(!m_pFileHandle)
		return false;

	m_Mime = CMime::MimeFromFileExt(_pFilename);

	return true;
}


//
//
//
bool CFileWriter::Close()
{
	if(m_pFileHandle)
	{
		fclose((FILE*)m_pFileHandle);
		m_pFileHandle = 0;
		return true;
	}
	return false;
}

//
//
//
CFileWriter::~CFileWriter()
{
	Close();
}

//
//
//
bool CFileWriter::IsReady()
{
	if(m_pFileHandle)
		return true;
	return false;
}

//
//
//
int32 CFileWriter::GetSize()
{
	return ftell((FILE*)m_pFileHandle);
}

//
//
//
bool CFileWriter::Flush()
{
	fflush((FILE*)m_pFileHandle);
	return true;
}

//
//
//
int32 CFileWriter::Write(const void *_pBuffer, int32 _Size)
{
	return fwrite(_pBuffer, 1, _Size, (FILE*)m_pFileHandle);
}





CFileReader::CFileReader(void *pHandle)
{
	m_pFileHandle = pHandle;
}


CFileReader::CFileReader(const int8 *_pFilename)
{
	m_pFileHandle = 0;
	if(_pFilename)
		Open(_pFilename);
}

CFileReader::~CFileReader()
{
	Close();
}

//
//
//
bool CFileReader::Open(const int8 *_pFilename)
{
	m_pFileHandle = fopen((char*)_pFilename, "rb");

	if(!m_pFileHandle)
		return false;

	m_Filename = _pFilename;

	fseek((FILE*)m_pFileHandle, 0, SEEK_END);
	m_FileSize = ftell((FILE*)m_pFileHandle);
	fseek((FILE*)m_pFileHandle, 0, SEEK_SET);

	m_Mime = CMime::MimeFromFileExt(_pFilename);

	return true;
}

//
//
//
CInputStream *CFileReader::Clone()
{
	return new CFileReader(m_Filename.GetData());
}

//
//
//
bool CFileReader::Close()
{
	if(m_pFileHandle)
	{
		fclose((FILE*)m_pFileHandle);
		m_pFileHandle = 0;
		return true;
	}
	return false;
}


bool CFileReader::IsReady()
{
	if(m_pFileHandle)
		return true;
	return false;
}

int32 CFileReader::GetSize()
{
	return m_FileSize;
}

int32 CFileReader::GetPosition()
{
	return ftell((FILE*)m_pFileHandle);
}

bool CFileReader::EndOfStream()
{
	if(ftell((FILE*)m_pFileHandle) >= m_FileSize)
		return true;
	return false;
}

void CFileReader::Seek(int32 _Offset,intr _Origin)
{
	Assert(m_pFileHandle);
	switch (_Origin)
	{
		case ESeekSet:
			fseek((FILE*)m_pFileHandle,_Offset,SEEK_SET);
			break;

		case ESeekCurrent:
			fseek((FILE*)m_pFileHandle,_Offset,SEEK_CUR);
			break;

		case ESeekEnd:
			fseek((FILE*)m_pFileHandle,_Offset,SEEK_END);
			break;

		default:
			DebugBreak();
	}
}

// Read
int32 CFileReader::Read(void *_pBuffer, int32 _Size)
{
	if(_Size <= 0)
		return 0;
	if(_pBuffer)
		return fread(_pBuffer, 1, _Size, (FILE*)m_pFileHandle);
	fseek((FILE*)m_pFileHandle, _Size, SEEK_CUR);
	return _Size;
}
