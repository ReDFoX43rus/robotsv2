#ifndef UTIL_AUTH_H
#define UTIL_AUTH_H

#include "iobase/fileio/fileio.h"

#define AUTH_PWD_PATH "/fat/auth_secret_file"
#define AUTH_PWD_LENGTH 32

class CAuth{
public:
	static CAuth *Instance();

	int StorePass(const char* pwd);
	int TryAuth(const char* pwd);
	bool IsAuthed() {return m_Authed;}
private:
	CAuth();
	~CAuth();

	bool m_Authed;
	static CAuth *m_Instance;

	bool IsPassFileExists();
};

#endif /* end of include guard: UTIL_AUTH_H */
