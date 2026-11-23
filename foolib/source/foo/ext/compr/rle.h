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
		int32 m_UnitSize;

	public:
		CRLEEncoder(COutputStream *_pTarget=NULL, int32 _UnitSize=-1);

		int32 virtual GetSize();
		bool virtual IsReady();
		bool virtual Flush();
		int32 Write(const void *_pBuffer, int32 _Size);
	};

	//
	//
	//
	class CRLEDecoder : public CInputStream
	{
		CInputStream *m_pSource;
		int32 m_UnitSize;
		int32 m_ReadBytes;

	public:
		CRLEDecoder(CInputStream *_pSource=NULL, int32 _UnitSize=-1);

		int32 virtual GetSize();
		bool virtual IsReady();
		bool virtual EndOfStream();
		int32 virtual Read(void *_pBuffer, int32 _Size);
	};
}
