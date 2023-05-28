#pragma once
#include "CMessage.h"
#include "CUser.h"
#include <map>
#include <unordered_map>
#include <algorithm>
#include <random>
//-----------------------------------------------------------------------------------------
enum MainMenuItem {
	eSendToUser,
	eSendToAll,
	eNothing,
	eLogOut,
	eQuit
};

using MMI = MainMenuItem;
//-----------------------------------------------------------------------------------------
struct cHasher{
	std::size_t operator()(const std::string& uname) const noexcept {
		size_t sum{};
		for (const auto& c : uname)
			sum += static_cast<size_t>(c);		
		return (sum % mem_size)<<1;// 
	}
	const size_t mem_size{ 64 };
};
//-----------------------------------------------------------------------------------------
class CSession {
public:
	CSession();
	~CSession() noexcept;
	auto is_running() -> bool;
	auto send(const std::string&) -> MMI;
	auto broadcast() -> MMI;

private:
	void init();
	void close();
	void save_user_data() const;
	auto login() -> bool;
	auto re_login() -> bool;
	auto show_main_menu() -> MMI;
	auto show_msg_menu(const std::string&) -> MMI;
	auto show_user_msg()->void;
	auto hash_func(const std::string&) -> size_t;
	auto is_valid_user(const std::string&, std::shared_ptr<CUser>&) -> bool;
	auto is_valid_user_pwd(const std::shared_ptr<CUser>&, const std::string&) -> bool;
	auto send_msg(const std::string&, const std::string&, const std::string&) -> bool;
	auto get_current_user() -> std::shared_ptr<CUser> const { return current_user; }
	auto get_user(const std::string&) -> std::shared_ptr<CUser> const;
	CSession(const CSession&);

	std::shared_ptr<CUser> current_user{ nullptr };
	std::unordered_map<size_t,std::shared_ptr<CUser>> um_users;
	std::map<std::time_t, std::shared_ptr<CMessage>> msg_pool;

	const char* user_fn{ "users.txt" };
	const char* msg_fn{ "msg.txt" };
	bool first_launch{ false };
	bool is_new_user{ false };
};

