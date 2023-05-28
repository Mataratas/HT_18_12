#pragma once
#include <ctime>
#include "CUser.h"

enum MsgType {
	eNew,
	eOld
};

class CMessage {
public:
	CMessage(const char* msg, const std::string& snd, const std::string& rcv) :body(msg), reciever(rcv), sender(snd), timestamp(std::time(nullptr)) {};
	explicit CMessage(const char*);
	~CMessage() = default;
	CMessage(const CMessage&) = delete;
	CMessage& operator=(const CMessage&) = delete;
	friend std::fstream& operator<<(std::fstream& out, const CMessage&);
	friend std::fstream& operator<<(std::fstream& out, std::shared_ptr<CMessage>);
	template<class S> void set_body(S& b) { body = b; }
	auto get_recv() -> std::string const { return reciever; };
	auto get_sender() -> std::string const { return sender; };
	auto get_ts() -> std::time_t const { return timestamp; };
	auto get_body() -> std::string const { return body; };
	auto get_type() -> MsgType const { return type; };
	auto show_time() -> const std::string;
	void show();

	void set_recv(const std::string&);
	void set_sender(const std::string&);
	void set_msg_read();


private:
	std::time_t timestamp{};
	std::string sender;
	std::string reciever;
	std::string body;
	MsgType type{ eNew };
};
