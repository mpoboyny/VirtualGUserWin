
// appLin.inl

bool CApp::GetCurrWorkDir(Str &dir)
{
	char tmpPath[MP_MAX_PATH] = {0};
	if (getcwd(tmpPath, MP_MAX_PATH)) {
		dir = StrAToStr(StrA(tmpPath));
		return true;
	}
	return false;
}

bool CApp::SetCurrWorkDir(const TCHAR *dir)
{
	return !!chdir(StrToStrA(Str(dir)).c_str());
}
