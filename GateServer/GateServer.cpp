#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

#include"CServer.h"
#include"ConfigMgr.h"
#include"const.h"
#include"RedisMgr.h"


int main()
{

	auto & gCfgMgr = ConfigMgr::Inst();
	std::string gate_port_str=gCfgMgr["GateServer"]["Port"];
	unsigned short gate_port = atoi(gate_port_str.c_str());

	try {
		unsigned short port = static_cast<unsigned short>(5005);
		net::io_context ioc{ 1 };
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
			if (error) {
				return;
			}
			ioc.stop();
			});
		std::make_shared<CServer>(ioc, gate_port)->Start();//server启动后，监听链接
		std::cout << "Gate Server listen on port:" << gate_port << std::endl;
		ioc.run();
	}
	catch (std::exception const& e) {
		std::cerr << "error:" << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}