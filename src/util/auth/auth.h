/* For our purposes auth system is required
 * If password is set and user isnt authed only auth cmd will work
 * If password isn't set (by default) user can use all the functions without any restrictions
 * Note that password in stored unencrypted
 * So this system has weight only with esp-idf flash encryption */

#ifndef UTIL_AUTH_H
#define UTIL_AUTH_H

#include "iobase/fileio/fileio.h"

/* Path to store password */
#define AUTH_PWD_PATH "/fat/auth_secret_file"
#define AUTH_PWD_LENGTH 32

class CAuth{
public:
	/* Get instance since there is singleton pattern */
	static CAuth *Instance();

	/* Create new password if there is no password yet
	 * Once you set password you cannot change or remove it */
	int StorePass(const char* pwd);

	/* Compare passwords, on match make user authed */
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
