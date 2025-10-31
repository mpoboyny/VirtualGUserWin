
// appWin.inl

bool CApp::GetCurrWorkDir(Str &dir)
{
	TCHAR tmpPath[MP_MAX_PATH] = {0};
	if (_tgetcwd(tmpPath, MP_MAX_PATH)) {
		dir = tmpPath;
		return true;
	}
	return false;
}

bool CApp::SetCurrWorkDir(const TCHAR *dir)
{
	return !!_tchdir(dir);
}
