#include "CSession.h"
//===================================================================================
CSession::CSession() {
	init();
}
//--------------------------------------------------------------------------------------
CSession::~CSession() {
	close();
}
//--------------------------------------------------------------------------------------
void CSession::init() {
	std::fstream in{ std::fstream(user_fn, std::ios::in)};
	if (!in) {
		in = std::fstream(user_fn, std::ios::in | std::ios::trunc); //create new file
		first_launch = true;
	}
	else {
		in.seekg(0, std::ios::beg);
		while (!in.eof()) {
			char arr_ch[256]{};
			in.getline(arr_ch, 256);
			if (arr_ch[0] == '\0')
				continue;

			std::string tmp = arr_ch;
			std::string nm = tmp.substr(0, tmp.find(','));
			auto uptr = std::make_shared<CUser>(nm.c_str());
			tmp = tmp.substr(tmp.find(',')+1);

			uptr->set_stored_pwd(tmp);
			um_users.emplace(std::make_pair(hash_func(nm), uptr));			
		}
	}
	in.close(); 

	in.open(msg_fn, std::ios::in);
	if (!in.is_open()) {
		in = std::fstream(msg_fn, std::ios::in | std::ios::trunc); //create new file
	}
	else {
		in.seekg(0, std::ios::beg);
		while (!in.eof()) {
			char arr_ch[128]{};
			in.getline(arr_ch, 128);
			if (arr_ch[0] == '\0')
				continue;
			auto mptr = std::make_shared<CMessage>(arr_ch);
			msg_pool.emplace(mptr->get_ts(), mptr);
		}
	}
	in.close();
}
//--------------------------------------------------------------------------------------
void CSession::close() {
	std::fstream out{ std::fstream(msg_fn, std::ios::app) };
	if (!out)
		out = std::fstream(msg_fn, std::ios::app | std::ios::trunc); // create file

	if (out) {
		for (const auto& p : msg_pool) {
			out << p.first << '|';
			out << p.second;
		}
		out.close();
	}
}
//--------------------------------------------------------------------------------------
auto CSession::show_main_menu()->MMI {
	std::cout << "Choose action: \n\twrite to user (r)\n\twrite to all (a)\n\tLog out (l)\n\tQuit (q):\n";
	std::string input;
	std::getline(std::cin, input, '\n');
	switch (input.at(0)) {
	case 'r': {
		if (um_users.size()) {
			std::cout << "Available users(type user name):\n";
			auto it = um_users.begin();
			while (it != um_users.end()) {
				if(it->second != current_user)
					std::cout << "\t" << it->second->get_name() << std::endl;
				it++;
			}
			return eSendToUser;
		}
		else {
			std::cout << "No available users:\n";
			return eNothing;
		}
		break;
	}
	case 'a': {
		if (um_users.size()) {
			return eSendToAll;
		}
		else {
			std::cout << "No available users:\n";
			return eNothing;
		}
	}
	case 'l': {
		return eLogOut;
	}
	case 'q': {
		return eQuit;
	}
	default:
		return eQuit;
	}
}
//--------------------------------------------------------------------------------------
void CSession::save_user_data() const {
	std::fstream out{ std::fstream(user_fn, std::ios::app)};
	if (!out)
		out = std::fstream(user_fn, std::ios::app | std::ios::trunc); // create file	
	if (out && current_user) {
		out << *current_user.get();
		out.close();
	}
}
//--------------------------------------------------------------------------------------
auto CSession::login()->bool {
	std::string u_name;
	std::string u_pwd;

	if (first_launch) {
		std::cout << "Welcome to the chat session. Choose Login (Enter to exit):\n";
		std::getline(std::cin, u_name, '\n');

		if (u_name.size()) {
			std::shared_ptr<CUser> usr{ nullptr };
			while (is_valid_user(u_name,usr)) {
				std::cout << "This login is used. Choose another:\n";
				std::getline(std::cin, u_name, '\n');
			}

			std::cout << "Choose Password:\n";
			std::getline(std::cin, u_pwd, '\n');

			while (!u_pwd.size()) {
				std::cout << "Empty password not allowed. Choose one:\n";
				std::getline(std::cin, u_pwd, '\n');
			}


			try {
				current_user = std::make_shared<CUser>();
			}
			catch (const std::exception& e) {
				std::cout << "Failed to create user instance:" << e.what() << std::endl;
				return false;
			}

			current_user->set_name(u_name);
			current_user->set_pwd(u_pwd);
			um_users.emplace(std::make_pair(hash_func(u_name), current_user));
			save_user_data();
			is_new_user = true;
			return true;
		}
		else
			return false;

	}
	else {
		std::string input;
		std::cout << "Welcome to the chat session. New user?(y/n. Enter to exit):\n";
		std::getline(std::cin, input, '\n');
		if (input.size()) {
			switch (input.at(0)) {
				case 'y': {
					std::cout << "Choose Login:\n";
					std::getline(std::cin, u_name, '\n');
					std::shared_ptr<CUser> usr{ nullptr };
					while (is_valid_user(u_name, usr)) {
						std::cout << "This login is used. Choose another:\n";
						std::getline(std::cin, u_name, '\n');
					}

					std::cout << "Choose Password:\n";
					std::getline(std::cin, u_pwd, '\n');

					while (!u_pwd.size()) {
						std::cout << "Empty password not allowed. Choose one:\n";
						std::getline(std::cin, u_pwd, '\n');
					}

					try {
						current_user = std::make_shared<CUser>();
					}
					catch (const std::exception& e) {
						std::cout << "Failed to create user instance:" << e.what() << std::endl;
						return false;
					}
					current_user->set_name(u_name);
					current_user->set_pwd(u_pwd);
					um_users.emplace(std::make_pair(hash_func(u_name), current_user));
					save_user_data();
					is_new_user = true;

					return true;

					break;
				}
				case 'n': {
					return re_login();
					break;
				}
				default: {
					std::cout << "Wrong character: type y or n \n";
					return login();
				}

			}
		}
		else {
			return false;
		}
	}
}
//--------------------------------------------------------------------------------------
auto CSession::re_login() -> bool {
	std::string u_name;
	std::string u_pwd;
	std::cout << "Login:\n";
	std::getline(std::cin, u_name, '\n');
	std::shared_ptr<CUser> found_user{ nullptr };
	is_new_user = false;
	if (is_valid_user(u_name, found_user)) {
		std::cout << "Password:\n";
		std::getline(std::cin, u_pwd, '\n');

		if (is_valid_user_pwd(found_user, u_pwd)) {
			std::cout << "You have succesfully logged in\n";
			current_user = found_user;
			return true;
		}
		else {
			std::cout << "Wrong password(3 more attempts):\n";
			bool ok(false);
			for (size_t i = 3; i > 0; i--) {
				std::getline(std::cin, u_pwd, '\n');
				if (is_valid_user_pwd(found_user, u_pwd)) {
					ok = true;
					break;
				}
				if (!ok && i > 1) {
					std::cout << "Wrong password.(" << i - 1 << " more attempts):\n";
				}
				else
					break;
			}

			if (ok) {
				std::cout << "You have succesfully logged in\n";
				current_user = found_user;
				return true;
			}
			else {
				std::cout << "Login failed.\n";
				return false;
			}
		}
	}
	else {
		std::cout << "Wrong user name.(3 more attempts):\n";
		for (size_t i = 3; i > 0; i--) {
			std::getline(std::cin, u_name, '\n');
			if (is_valid_user(u_name, found_user))
				break;

			if (i > 1) {
				std::cout << "Wrong user name.(" << i - 1 << " more attempts):\n";
			}
		}

		if (found_user) {
			std::cout << "Password:\n";
			std::getline(std::cin, u_pwd, '\n');

			if (is_valid_user_pwd(found_user, u_pwd)) {
				std::cout << "You have succesfully logged in\n";
				current_user = found_user;
				return true;
			}
			else {
				std::cout << "Wrong password(3 more attempts):\n";
				bool ok(false);
				for (size_t i = 3; i > 0; i--) {
					std::getline(std::cin, u_pwd, '\n');
					if (is_valid_user_pwd(found_user, u_pwd)) {
						ok = true;
						break;
					}
					if (!ok && i > 1) {
						std::cout << "Wrong password.(" << i - 1 << " more attempts):\n";
					}
					else
						break;
				}

				if (ok) {
					std::cout << "You have succesfully logged in\n";
					current_user = found_user;
					return true;
				}
				else {
					std::cout << "Login failed.\n";
					return false;
				}
			}
		}
		else {
			std::cout << "Login failed.\n";
			return false;
		}
	}
}
//--------------------------------------------------------------------------------------
auto CSession::send_msg(const std::string& msg, const std::string& sender, const std::string& rcv) -> bool {
	//иногда ,при отправке всем (broadcast), временные метки совпадают, и запись не добавляется так как такой ключ уже есть.
	//как вариант можно попробовать добавлять к метке рандомное количество секунд
	std::random_device rd;  // a seed source for the random number engine
	std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
	std::uniform_int_distribution<> distrib(1, 60);
	int rnd = distrib(gen);
	try {
		auto msg_ptr = std::make_shared<CMessage>(msg.c_str(), sender, rcv);
		msg_pool.emplace(msg_ptr->get_ts()+rnd, msg_ptr);
		return true;
	}
	catch (const std::exception& e) {
		std::cout << "Failed to send message:" << e.what() << std::endl;
		return false;
	}
}
//--------------------------------------------------------------------------------------
auto CSession::send(const std::string& uname) -> MMI {
	return show_msg_menu(uname);
}
//--------------------------------------------------------------------------------------
auto CSession::broadcast() -> MMI {
	std::string msg;
	std::cout << "Type your message to all:\n";
	std::getline(std::cin, msg);

	auto it = um_users.begin();
	while(it!= um_users.end()) {
		if (it->second != current_user)
			if (send_msg(msg, current_user->get_name(), it->second->get_name()))
				std::cout << "Message sent to: " << it->second->get_name() << "\n";		
		it++;
	}
	return eNothing;
}
//--------------------------------------------------------------------------------------
auto CSession::show_user_msg()->void {
	if (!first_launch && !is_new_user && um_users.size() > 0) {
		std::map<std::time_t, std::shared_ptr<CMessage>> usr_msgs;
		for (const auto& i : msg_pool) 
			if (i.second->get_recv() == current_user->get_name() && i.second->get_type()==eNew)
				usr_msgs.emplace(std::make_pair(i.first, i.second));

		if (usr_msgs.size()) {
			std::cout << "You have new messages:" << std::endl;
			for (auto& p : usr_msgs) {
				p.second->show();
				p.second->set_msg_read();
			}
		}
		else
			std::cout << "You have no new messages." << std::endl;
	}
	else {
		if (!is_new_user)
			std::cout << "You have no new messages." << std::endl;
	}

}
//--------------------------------------------------------------------------------------
auto CSession::is_valid_user(const std::string& uname,std::shared_ptr<CUser>& usr) -> bool {
	auto it = um_users.find(hash_func(uname));
	if (it != um_users.end() && it->second->get_name()== uname) {
		usr = it->second;
		return true;
	}else
		return false;
}
//--------------------------------------------------------------------------------------
auto CSession::is_valid_user_pwd(const std::shared_ptr<CUser>& usr, const std::string& pwd) -> bool {
	if (usr) {
		uint* digest = sha1((char*)pwd.c_str(), pwd.size());
		if (!memcmp(digest, usr->get_pwd_hash(), SHA1HASHLENGTHBYTES))
			return true;
		else
			return false;
	}
	return false;
}
//--------------------------------------------------------------------------------------
auto CSession::get_user(const std::string& uname) -> std::shared_ptr<CUser> const {
	auto found = um_users.find(hash_func(uname));
	if (found != um_users.end())
		return found->second;
	else
		return nullptr;
}
//--------------------------------------------------------------------------------------
auto CSession::is_running() -> bool {
	if (login()) {
		show_user_msg();
		MMI res = show_main_menu();
		while (res != eQuit) {
			std::cin.clear();
			std::cin.sync();
			switch (res) {
				case eSendToUser: {
					std::string u_rec;
					std::shared_ptr<CUser> uptr;
					std::getline(std::cin, u_rec);
					if (is_valid_user(u_rec,uptr)) {
						res = send(u_rec);
					}
					else
						std::cout << "Invalid user! Try again\n";
					break;
				}
				case eSendToAll: {
					res = broadcast();
					break;
				}
				case eNothing: {
					res = show_main_menu();
					break;
				}
				case eLogOut: {
					if (re_login()) {
						show_user_msg();
						res = show_main_menu();
					}else
						res = eQuit;
					break;
				}
			}
		}
		if (eQuit == res)
			return false;
		else
			return true;
	}
	else
		return false;

}
//--------------------------------------------------------------------------------------
auto CSession::show_msg_menu(const std::string& uname) -> MMI {
	std::string msg, cusr, choice;
	std::cout << "Type your message to " << uname << ":\n";
	std::getline(std::cin, msg, '\n');
	MMI res(eNothing);
	cusr = get_current_user()->get_name();
	if (send_msg(msg, cusr, uname)) {
		std::cout << "Message sent\n";
		std::cout << "Send another? (y/n):\n";
		std::getline(std::cin, choice, '\n');
		while (choice == "y") {
			std::cout << "Type your message:\n";
			std::getline(std::cin, msg, '\n');
			if (send_msg(msg, cusr, uname))
				std::cout << "Message sent\n";
			std::cout << "Send another? (y/n):\n";
			std::getline(std::cin, choice, '\n');
		}
		res = show_main_menu();
	}
	else {
		std::cout << "Failed to send message\n";
	}
	return res;
}
//--------------------------------------------------------------------------------------
auto CSession::hash_func(const std::string& user_name)->size_t {	
	return cHasher{}(user_name);
}
//---------------------------------------------------------------------------
