
// VirtuaGUserWinRecorder.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'pch.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CVirtuaGUserWinRecorderApp:
// Siehe VirtuaGUserWinRecorder.cpp für die Implementierung dieser Klasse
//

class CVirtuaGUserWinRecorderApp : public CWinApp
{
public:
	CVirtuaGUserWinRecorderApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CVirtuaGUserWinRecorderApp theApp;
