#ifndef __FOO_STREAM_H__
#define __FOO_STREAM_H__

#include <foo/base/base.h>
#include "string.h"
#include "dynamicarray.h"
#include "mime.h"

namespace NFoo
{

	//
	// Output Stream
	//
	class COutputStream
	{
	protected:
		CMime m_Mime;

	public:
		COutputStream(CMime &rMime=m_NullMime) { m_Mime = rMime; }
		virtual ~COutputStream() {}

		const CMime &GetMime() { return m_Mime; }
		
		// Various Functions
		bool virtual IsReady() pure;
		bool virtual Flush() { return true; }
		int32_t virtual GetSize() { return -1; }

		// Write
		int32_t virtual Write(const void *_pBuffer, int32_t _Size) pure;

		// NE = Native endian (No byte swapping), this was added because of collisions with Write(void*,int32_t)
		// LE = Little endian (Intel, x86 order)
		// ME = Big endian (IBM, PPC order)
		template<typename T> inline void WriteNE(T _Value) { Write(&_Value, sizeof(T)); }
		template<typename T> inline void WriteLE(T _Value) { WriteNE(ByteSwapCPUtoLE(_Value)); }
		template<typename T> inline void WriteBE(T _Value) { WriteNE(ByteSwapCPUtoBE(_Value)); }

		// Write pascal (uint16)
		int32_t Write_p8(CString Str); // 8 bit, max length = 255
		int32_t Write_p16(CString Str); // 16 bit, max length = 64 kib
	};

	//
	// Input Stream
	//
	class CInputStream
	{
	protected:
		CMime m_Mime;
	public:
		enum
		{
			ESeekSet,
			ESeekCurrent,
			ESeekEnd
		};

		CInputStream(CMime &rMime=m_NullMime) { m_Mime = rMime; }
		virtual ~CInputStream() {}

		void SetMime(const CMime Mime) { m_Mime = Mime; }
		const CMime &GetMime() { return m_Mime; }

		// Various Functions
		bool virtual IsReady() pure;
		bool virtual EndOfStream() pure;
		int32_t virtual GetSize() { return -1; }
		int32_t virtual GetPosition() { return -1; }

		bool virtual Seekable() { return false; }
		void virtual Seek(int32_t _Offset,intr _Origin) { }

		bool virtual Clonable() { return false; }
		virtual CInputStream *Clone() { return 0; }

		// if count == -1 then it will read until eof
		int32_t Transfer(COutputStream &rOutput, int32_t Count=-1, int32_t BufferSize=4092);
		
		// Read
		int32_t virtual Read(void *Buffer, int32_t Size) pure; // NULL as buffer will result in a skip

		// NE = Native endian (No byte swapping), this was added because of collisions with Read(void*,int32_t)
		// LE = Little endian (Intel, x86 order)
		// ME = Big endian (IBM, PPC order)
		template<typename T> inline T ReadNE() { T v; Read(&v, sizeof(T)); return v; } 
		template<typename T> inline T ReadLE() { return ByteSwapLEtoCPU(ReadNE<T>()); }
		template<typename T> inline T ReadBE() { return ByteSwapBEtoCPU(ReadNE<T>()); }

		template<typename T> inline T &ReadNE(T &v) { Read(&v, sizeof(T)); return v; }
		template<typename T> inline T &ReadLE(T &v) { ReadNE(v); v = ByteSwapLEtoCPU(v); return v; }
		template<typename T> inline T &ReadBE(T &v) { ReadNE(v); v = ByteSwapBEtoCPU(v); return v; }

		void Read_sz(int8 *_pDst);
		int8 *ReadLine(int8 *_pDst=NULL); // pass null to use a internal buffer of 4092

		// Read pascal string (uint16)
		CString Read_p8(); // 8 bit, max length = 255
		CString Read_p16(); // 16 bit, max length = 64 kib
	};

	//
	//
	//
	class CChainedInputStream : public CInputStream
	{
	protected:
		CInputStream &m_rSource;
	public:
		CChainedInputStream(CInputStream &rSource)
		: m_rSource(rSource)
		{}

		// Various Functions
		bool virtual IsReady() { return m_rSource.IsReady(); }
		bool virtual EndOfStream() { return m_rSource.EndOfStream(); }
		int32_t virtual GetSize() { return m_rSource.GetSize(); }
		int32_t virtual GetPosition() { return m_rSource.GetPosition(); }

		bool virtual Seekable() { return m_rSource.Seekable(); }
		void virtual Seek(int32_t Offset, intr Origin) { m_rSource.Seek(Offset, Origin); }

		bool virtual Clonable() { return false; }
		virtual CInputStream *Clone() { return 0; }

		// Read
		int32_t virtual Read(void *Buffer, int32_t Size) { return m_rSource.Read(Buffer, Size); }
	};

	//
	//
	//
	class CBlockReader : public CChainedInputStream
	{
		uint8 *m_pBuffer;
		int32_t m_BufferSize;
		int32_t m_Pos;
		int32_t m_BufferLeft;

		void FillBuffer();

	public:
		CBlockReader(CInputStream &rSource, int32_t BufferSize=32*1024);
		virtual ~CBlockReader();

		bool virtual EndOfStream() { return (m_rSource.EndOfStream() && !m_BufferLeft); }

		// Read
		virtual int32_t Read(void *pOutput, int32_t Size);
	};

} // name space end
#endif // __FOO_STREAM_H__


 
