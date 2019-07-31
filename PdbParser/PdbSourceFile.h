#pragma once

class PdbSourceFile : public IPdbSourceFile
{
public:
	//	Interface
	const wstring&			GetSourceFileName();
	DWORD					GetUniqueId();
	CheckSumType			GetCheckSumType() const;
	const char*				GetCheckSum() const;

	//	Internal
	PdbSourceFile( IPdbModule*, IDiaSourceFile* );
	virtual ~PdbSourceFile();

private:
	IPdbModule*			m_pIPdbModule;
	IDiaSourceFile*		m_pIDiaSourceFile;

	//	Source name
	wstring		m_fileName;

	//	Check Sum
	CheckSumType	m_checkSumType;
	char			m_checkSum[32+1];
};