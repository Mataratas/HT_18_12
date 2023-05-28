#include "CUser.h"
#pragma warning(disable : 4996)
//========================================================================================
CUser::CUser(const char* nm):pwd_hash(nullptr){
	std::string stmp(nm);
	size_t pos = stmp.find(',');
	if (pos != std::string::npos) {
		name = stmp.substr(0, pos);
	}
	else
		name = nm;
}
//---------------------------------------------------------------------------------------
bool CUser::operator==(const CUser& rhs) {
	bool cmpHashes = !memcmp(pwd_hash, rhs.get_pwd_hash(), SHA1HASHLENGTHBYTES);
	return name == rhs.get_name() && cmpHashes;
}
//---------------------------------------------------------------------------------------
bool CUser::operator!=(const CUser& rhs) {
	bool cmpHashes = memcmp(pwd_hash, rhs.get_pwd_hash(), SHA1HASHLENGTHBYTES);
	return name != rhs.get_name() || cmpHashes;
}
//---------------------------------------------------------------------------------------
std::fstream& operator<<(std::fstream& out, const CUser& usr) {
	out << usr.get_name() << ',' << hash_to_string(usr.get_pwd_hash()) << std::endl;
	return out;
}
//---------------------------------------------------------------------------------------
void CUser::set_pwd(const char* pwd) {
	if (pwd_hash)
		delete[] pwd_hash;
	pwd_hash = sha1((char*)pwd, strlen(pwd));
}
//---------------------------------------------------------------------------------------
void CUser::set_pwd(const std::string& pwd) {
	if (pwd_hash)
		delete[] pwd_hash;
	pwd_hash = sha1((char*)pwd.c_str(), pwd.size());
}
//---------------------------------------------------------------------------------------
void CUser::set_stored_pwd(const std::string& pwd) {
	pwd_hash = hash_from_string(pwd);
}
//---------------------------------------------------------------------------------------
const std::string hash_to_string(uint* hash) {
	char buf[48]{ (char)'/0' };
	char tmp[10]{ (char)'/0' };
	for (int i = 0; i < SHA1HASHLENGTHUINTS; i++) {
		sprintf(tmp, "%X-", hash[i]);
		if(!i)
			strcpy(buf, tmp);
		else
			strcat(buf, tmp);
	}	
	std::string res{ buf };
	return res.substr(0, res.size() - 1);
}
//---------------------------------------------------------------------------------------
uint* hash_from_string(const char* s) {
	uint* digest = new uint[SHA1HASHLENGTHUINTS]{};
	std::string t{ s };
	size_t sz = t.find_first_of('-');
	for (size_t i = 0; i < SHA1HASHLENGTHUINTS; i++){
		digest[i] = (uint)strtoul(t.substr(0, sz).c_str(), NULL, 16);
		if (sz == std::string::npos)
			break;
		t = t.substr(sz + 1);
		sz = t.find_first_of('-');
	}
	return digest;
}
//---------------------------------------------------------------------------------------
uint* hash_from_string(const std::string& s) {
	return hash_from_string(s.c_str());
}
//---------------------------------------------------------------------------------------
