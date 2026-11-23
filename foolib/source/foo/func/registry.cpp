#include <foo/base/log.h>
#include <foo/base/string.h>
#include "registry.h"
#include "stream.h"

using namespace NFoo;

//
//
//
CRegistry::~CRegistry()
{
	m_lValues.DeleteAll();
}

//
//
//
CRegistry *CRegistry::GetChild(const int8 *pName)
{
	for(int32 i = 0; i < m_lValues.Len(); i++)
		if(m_lValues[i]->m_Name == pName)
			return m_lValues[i];
	return NULL;
}

//
//
//
CRegistry *CRegistry::GetChild(const int8 *pName, const int8 *pValue)
{
	for(int32 i = 0; i < m_lValues.Len(); i++)
		if(m_lValues[i]->m_Name == pName && m_lValues[i]->m_Value == pValue)
			return m_lValues[i];
	return NULL;
}

//
//
//
void CRegistry::Dump(int32 Indent)
{
	for(int32 i = 0; i < Indent; i++)
		NLog::Write("\t");
	NLog::WriteF("%s %s\n", m_Name.GetData(), m_Value.GetData());

	for(int32 i = 0; i < m_lValues.Len(); i++)
		m_lValues[i]->Dump(Indent+1);
}

//
//
//
bool CRegistry::Read(CInputStream &rStream)
{
	enum { ERead, EExtract, EProcess };

	CRegistry *pLastChild = NULL;
	CRegistry *pStackChild[32] = {0};
	int32 Level = 0;
	int32 State = ERead;
	
	int8 *pLine = NULL;
	int8 *pName = NULL;
	int8 *pValue = NULL;
	int32 LineLevel = 0;

	pStackChild[0] = this;

	while(1)
	{
		switch(State)
		{
		case ERead:
			{
				if(rStream.EndOfStream())
					return true;
				pLine = rStream.ReadLine();
				int32 len = StringLength(pLine);
				if(len == 0)
					break; // empty line, read again

				int8 *p = pLine;
				while(*p && (*p == ' ' || *p == '\t'))
				{
					p++;
					len--;
				}
				if(len == 0)
					break;

				State = EExtract;
			}
		case EExtract:
			{
				LineLevel = 0;
				while(pLine[LineLevel] == '\t')
					LineLevel++;

				pName = pLine+LineLevel;
				pValue = NULL;
				pLine += LineLevel;

				while(*pLine && *pLine != ' ')
					pLine++;

				pValue = pLine;
				if(*pLine)
				{
					*pLine = 0;
					pValue = pLine+1;
				}

				State = EProcess;
			}
		case EProcess:
			{
				if(LineLevel < Level)
				{
					Level = LineLevel;
					State = EProcess;
				}
				else if(LineLevel == Level)
				{
					CRegistry *pChild = new CRegistry;
					pChild->m_Name = pName;
					pChild->m_Value = pValue;
					pStackChild[Level]->m_lValues.Add(pChild);
					pLastChild = pChild;
					State = ERead;
				}
				else if(LineLevel == Level+1)
				{
					if(pLastChild)
					{
						Level++;
						pStackChild[Level] = pLastChild;
						State = EProcess;
					}
					else
						Assert(0);
				}
				else
					Assert(0);
			}
		}
	}
}
