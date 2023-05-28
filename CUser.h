#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "sha1.h"

const std::string hash_to_string(uint*);
uint* hash_from_string(const char*);
uint* hash_from_string(const std::string&);

//========================================================================================
class CUser {
public:
	CUser() = default;
	explicit CUser(const char*);
	CUser(const CUser&) = delete;
	~CUser() {
		if (pwd_hash)
			delete[] pwd_hash;
	};
	CUser& operator=(const CUser&) = delete;

	void set_name(const std::string& nm) {
		name = nm; 
	}
	void set_pwd(const char*);
	void set_pwd(const std::string&);
	void set_stored_pwd(const std::string&);

	std::string get_name() const { return name; }
	unsigned int *get_pwd_hash() const { return pwd_hash; }
	bool operator==(const CUser& rhs);
	bool operator!=(const CUser& rhs);
	

	friend std::fstream& operator<<(std::fstream& out, const CUser&);
private:
	std::string name{};
	unsigned int* pwd_hash{nullptr};
};
