#ifndef ENGINE_RENDER_H
#define ENGINE_RENDER_H

#include "engine.h"

namespace NFoo
{
	namespace N3D
	{
		class CDevice;
		class C2DUtils;
	}

	namespace NSound
	{
		class CSoundDevice;
	}
}

namespace NEngine
{
	//
	//
	//
	class CRender : public CModule
	{
		NFoo::N3D::CDevice *m_pDevice;
		NFoo::N3D::C2DUtils *m_pUtils;
	protected:
		virtual bool Init();
		~CRender();
	public:
		NFoo::N3D::CDevice *GetDevice() { return m_pDevice; }
		NFoo::N3D::C2DUtils *Get2DUtils() { return m_pUtils; }
	};

	//
	//
	//
	class CSound : public CModule
	{
		NFoo::NSound::CSoundDevice *m_pDevice;
	protected:
		virtual bool Init();
		~CSound();
	public:

		NFoo::NSound::CSoundDevice *GetDevice() { return m_pDevice; }
	};

}

#endif // ENGINE_RENDER_H
