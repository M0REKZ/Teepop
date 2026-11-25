#include <foo/func/stream.h>


namespace NFoo
{
	// TODO: these should be made more robust.. internal buffers etc

	//
	//
	//
	class CRLEEncoder : public COutputStream
	{
		COutputStream *m_pTarget;
		int32_t m_UnitSize;

	public:
		CRLEEncoder(COutputStream *_pTarget=NULL, int32_t _UnitSize=-1);

		int32_t virtual GetSize();
		bool virtual IsReady();
		bool virtual Flush();
		int32_t Write(const void *_pBuffer, int32_t _Size);
	};

	//
	//
	//
	class CRLEDecoder : public CInputStream
	{
		CInputStream *m_pSource;
		int32_t m_UnitSize;
		int32_t m_ReadBytes;

	public:
		CRLEDecoder(CInputStream *_pSource=NULL, int32_t _UnitSize=-1);

		int32_t virtual GetSize();
		bool virtual IsReady();
		bool virtual EndOfStream();
		int32_t virtual Read(void *_pBuffer, int32_t _Size);
	};
}
