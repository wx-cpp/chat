#pragma once
#include"const.h"

class HttpConnection:public std::enable_shared_from_this<HttpConnection>
{
public:
	friend class LogicSystem;
	//HttpConnection(tcp::socket socket);
	HttpConnection(boost::asio::io_context & ioc);
	tcp::socket& GetSocket() {
		return _socket;
	}
	void Start();//监听读写事件
private:
	void CheckDeadline();//处理超时
	void WriteResponse();//应答
	void HandleReq();//处理请求
	void PreParseGetParam();
	std::string _get_url;
	std::unordered_map<std::string, std::string> _get_params;
	tcp::socket _socket;
	beast::flat_buffer _buffer{ 8192 };//假设接收8k的信息
	http::request<http::dynamic_body> _request;
	http::response<http::dynamic_body> _response;
	net::steady_timer deadline_{
		_socket.get_executor(),std::chrono::seconds(60)//定时器
	};

};

