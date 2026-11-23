#ifndef __FOO_FILESTREAM_H__
#define __FOO_FILESTREAM_H__

#include "stream.h"

namespace NFoo
{
	class CFileWriter : public COutputStream
	{
		void *m_pFileHandle;
	public:
		enum
		{
			EBinary,
			EText,
			EBinaryAppend,
			ETextAppend
		};

		CFileWriter(void *pHandle);
		CFileWriter(const int8 *_pFilename=NULL, int32 _Mode=EBinary);
		virtual ~CFileWriter();

		bool Open(const int8 *_pFilename, int32 _Mode=EBinary);
		bool Close();

		// Various Functions
		bool virtual IsReady();
		bool virtual Flush();
		int32 virtual GetSize();

		// Write
		int32 virtual Write(const void *Buffer, int32 Size);

	private:
		bool operator =(CFileWriter &) { return false; }
	};

	class CFileReader : public CInputStream
	{
		int32 m_FileSize;
		void *m_pFileHandle;
		CString m_Filename;
	public:
		CFileReader(void *pHandle);
		CFileReader(const int8 *_pFilename=NULL);
		virtual ~CFileReader();

		bool Open(const int8 *_pFilename);
		bool Close();

		// Various Functions
		bool virtual IsReady();
		bool virtual EndOfStream();
		int32 virtual GetSize();
		int32 virtual GetPosition();

		bool virtual Seekable() { return true; }
		void virtual Seek(int32 _Offset,intr _Origin);

		bool virtual Clonable() { return true; }
		virtual CInputStream *Clone();

		// Read
		//int32 virtual Skip(int32 _Size);
		int32 virtual Read(void *Buffer, int32 Size);

	private:
		bool operator =(CFileReader &) { return false; }
	};

	extern CFileWriter g_Stdout;
	extern CFileWriter g_Stdin;

} // end namespace
#endif // __FOO_FILESTORAGE_H__
