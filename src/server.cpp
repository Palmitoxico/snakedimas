#include <iostream>
#include <thread>
#include <functional>

#include "docopt.h"
#include "utils/msglog.hpp"
#include "net/net.hpp"

msglog::msglog logmsg;

static const char _USAGE[] =
    R"(SnakeServer.
Usage:
  __PROGNAME__ [--port=<num> --debug-level=<num>]
  __PROGNAME__ (-h | --help)
  __PROGNAME__ --version
Options:
  -h --help           Show this screen.
  --version           Show version.
  --port=<num>        TCP port [default: 3001].
  --debug-level=<num> Debug output verbosity level [default: -1].
)";

void search_and_replace(std::string& str, const std::string& old_str, const std::string& new_str)
{
	std::string::size_type index = 0;
	std::string::size_type old_str_len = old_str.length();
	std::string::size_type new_str_len = new_str.length();
	while ((index = str.find(old_str, index)) != std::string::npos)
	{
		str.replace(index, old_str_len, new_str);
		index += new_str_len;
	}
}

int main(int argc, char *argv[])
{
	std::string USAGE = _USAGE;
	search_and_replace(USAGE, "__PROGNAME__", argv[0]);

	std::map<std::string, docopt::value> args
		= docopt::docopt(USAGE,
						 { argv + 1, argv + argc },
						 true,               // show help if requested
						 "SnakeServer V0.1"); // version string

	if (args["--debug-level"])
	{
		logmsg.set_debug_level(atoi(args["--debug-level"].asString().c_str()));
		logmsg.debug_msg("Verbose mode", 0);
	}

	net::NetServer netconn;

	netconn.open_conn("127.0.0.1", atoi(args["--port"].asString().c_str()));

	std::vector<std::unique_ptr<std::thread>> client_threads;

	while(1)
	{
		auto conn = netconn.wait_conn();
		auto ref_logmsg = std::ref(logmsg);

		client_threads.push_back(
			std::make_unique<std::thread>(
				[conn, ref_logmsg]
					{
						net::NetObject nobj;
						nobj.id = net::snake;
						nobj.data_len = 5;
						nobj.data.push_back('D');
						nobj.data.push_back('I');
						nobj.data.push_back('M');
						nobj.data.push_back('A');
						nobj.data.push_back('S');
						conn->send_netdata(nobj);
						logmsg.debug_msg("Dimas connected!", 0);
					}));
	}
	return 0;
}
