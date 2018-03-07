#include "auth.h"
#include "string.h"

CAuth::CAuth(){
	m_Authed = !IsPassFileExists();
}
CAuth::~CAuth(){

}

int CAuth::TryAuth(const char* pwd){
	CFile file(AUTH_PWD_PATH, FM_READ);
	if(!file.IsOpened())
		return -1;

	char *truePwd = (char*)malloc(AUTH_PWD_LENGTH);

	file.GetString(truePwd, AUTH_PWD_LENGTH);

	int res;
	if(!strcmp(truePwd, pwd)){
		res = 0;
		m_Authed = true;
	}
	else res = -2;

	free(truePwd);
	file.Close();

	return res;
}

int CAuth::StorePass(const char* pwd){
	if(strlen(pwd) > AUTH_PWD_LENGTH)
		return -3;

	CFile file(AUTH_PWD_PATH, FM_READ);
	if(file.IsOpened()){
		file.Close();
		return -1;
	}

	CFile file2(AUTH_PWD_PATH, FM_WRITE);
	if(!file2.IsOpened())
		return -2;

	file2 << pwd << endl;
	file2.Close();
	return 0;
}

bool CAuth::IsPassFileExists(){
	CFile file(AUTH_PWD_PATH, FM_READ);
	if(file.IsOpened()){
		file.Close();
		return true;
	}

	return false;
}

CAuth *CAuth::m_Instance = NULL;
CAuth *CAuth::Instance(){
	if(!CAuth::m_Instance){
		CAuth::m_Instance = new CAuth();
		return CAuth::m_Instance;
	} else return CAuth::m_Instance;
}
