/*
 * main.cpp
 *
 *  Created on: 2015年4月22日
 *      Author: cj
 */

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
//#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>

using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
//数据库
#include "Poco/Data/Session.h"
#include "Poco/Data/SessionPool.h"
#include "Poco/Data/Statement.h"
#include "Poco/Data/MySQL/Connector.h"
//#include "Poco/Data/MySQL/MySQLException.h"

using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
//#include<Poco/JSON/JSONException.h>
#include<iostream>

//using namespace Poco::JSON;
using namespace Poco;
using namespace std;
using namespace Poco::Data;

//int g_count = 0;
//Poco::Data::SessionPool* m_dbPool = NULL;		//数据库链接池

class UserRequestHandler: public HTTPRequestHandler {
public:
	UserRequestHandler() {
	}

	void handleRequest(HTTPServerRequest& request,
			HTTPServerResponse& response) {
		std::string uri = request.getURI();

		std::size_t pos = uri.find('?');
		std::string dir = uri.substr(0, pos);
		std::string param = uri.substr(pos + 1);

		pos = param.find('=') + 1;
		std::size_t size = param.find('&') - pos;
		std::string name = param.substr(pos, size);
		std::string age = param.substr(param.rfind('=') + 1);

		//std::cout << dir << ":   " << name << ":  " << age << std::endl;
		std::ostream& ostr = response.send();
		//g_count++;
		//std::cout << g_count << std::endl;
		ostr << "hello world";
	}

private:
};

class RequestHandlerFactory: public HTTPRequestHandlerFactory {
public:
	RequestHandlerFactory(const std::string& format) {
	}

	HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) {

		std::string uri = request.getURI();

		std::size_t pos = uri.find('?');
		std::string dir = uri.substr(0, pos);
		//	std::cout << dir << ":   " << name << ":  " << age << std::endl;
		if (dir == "/api10/ping") {
			return new UserRequestHandler();
		} else if (dir == "/api10/register") {

		} else if (dir == "/api10/taskrequest") {

		} else if (dir == "/api10/taskresult") {

		} else if (dir == "/api10/auth") {

		} else
			return 0;

	}
};

class PocoHttpServer: public Poco::Util::ServerApplication {
public:
	PocoHttpServer() :
			_helpRequested(false) {
	}

	~PocoHttpServer() {
	}

protected:
	void initialize(Application& self) {
		//loadConfiguration(); // load default configuration files, if present

		loadConfiguration("config.json", PRIO_APPLICATION);

		string server = config().getString("DEFAULT_DB.DEFAULT_DB_SERVER");
		cout << server << endl;
		ServerApplication::initialize(self);
	}

	void uninitialize() {
		ServerApplication::uninitialize();
	}

	void defineOptions(OptionSet& options) {
		ServerApplication::defineOptions(options);

		options.addOption(
				Option("help", "h",
						"display help information on command line arguments").required(
						false).repeatable(false));
	}

	void handleOption(const std::string& name, const std::string& value) {
		ServerApplication::handleOption(name, value);

		if (name == "help")
			_helpRequested = true;
	}

	void displayHelp() {
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader(
				"A web server that serves the current date and time.");
		helpFormatter.format(std::cout);
	}

	int main(const std::vector<std::string>& args) {
		if (_helpRequested) {
			displayHelp();
		} else {
			// get parameters from configuration file
			unsigned short port = (unsigned short) config().getInt(
					"HTTPTimeServer.port", 8888);
			std::string format(
					config().getString("HTTPTimeServer.format",
							DateTimeFormat::SORTABLE_FORMAT));
			int maxQueued = config().getInt("HTTPTimeServer.maxQueued", 1200);
			int maxThreads = config().getInt("HTTPTimeServer.maxThreads", 1200);
			ThreadPool::defaultPool().addCapacity(maxThreads);

			HTTPServerParams* pParams = new HTTPServerParams;
			pParams->setMaxQueued(maxQueued);
			pParams->setMaxThreads(maxThreads);

			// set-up a server socketyuhaiyang
			ServerSocket svs(port);
			// set-up a HTTPServer instance
			HTTPServer srv(new RequestHandlerFactory(format), svs, pParams);
			// start the HTTPServer
			srv.start();
			// wait for CTRL-C or kill
			waitForTerminationRequest();
			// Stop the HTTPServer
			srv.stop();
		}
		return Application::EXIT_OK;
	}

private:
	bool _helpRequested;
};

int main(int argc, char** argv) {
	PocoHttpServer app;
	return app.run(argc, argv);
}

