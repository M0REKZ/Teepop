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
		CFileWriter(const char *_pFilename=NULL, int32_t _Mode=EBinary);
		virtual ~CFileWriter();

		bool Open(const char *_pFilename, int32_t _Mode=EBinary);
		bool Close();

		// Various Functions
		bool virtual IsReady();
		bool virtual Flush();
		int32_t virtual GetSize();

		// Write
		int32_t virtual Write(const void *Buffer, int32_t Size);

	private:
		bool operator =(CFileWriter &) { return false; }
	};

	class CFileReader : public CInputStream
	{
		int32_t m_FileSize;
		void *m_pFileHandle;
		CString m_Filename;
	public:
		CFileReader(void *pHandle);
		CFileReader(const char *_pFilename=NULL);
		virtual ~CFileReader();

		bool Open(const char *_pFilename);
		bool Close();

		// Various Functions
		bool virtual IsReady();
		bool virtual EndOfStream();
		int32_t virtual GetSize();
		int32_t virtual GetPosition();

		bool virtual Seekable() { return true; }
		void virtual Seek(int32_t _Offset,intr _Origin);

		bool virtual Clonable() { return true; }
		virtual CInputStream *Clone();

		// Read
		//int32_t virtual Skip(int32_t _Size);
		int32_t virtual Read(void *Buffer, int32_t Size);

	private:
		bool operator =(CFileReader &) { return false; }
	};

	extern CFileWriter g_Stdout;
	extern CFileWriter g_Stdin;

} // end namespace
#endif // __FOO_FILESTORAGE_H__
