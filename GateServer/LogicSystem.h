#pragma once
#include"const.h"

class HttpConnection;  //解决互引用
typedef std::function<void(std::shared_ptr<HttpConnection>)>HttpHandler; //函数处理器
class LogicSystem :public Singleton<LogicSystem> {
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem() {}
	bool HandleGet(std::string, std::shared_ptr<HttpConnection>);
	void RegGet(std::string,HttpHandler handler);
	void RegPost(std::string, HttpHandler handler);
	bool HandlePost(std::string, std::shared_ptr<HttpConnection>);
private:
	LogicSystem();
	std::map<std::string, HttpHandler> _post_handlers;
	std::map<std::string, HttpHandler> _get_handlers;//处理请求集合

};
