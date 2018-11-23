#include <iostream>
#include <thread>
#include <functional>
#include <sstream>
#include <memory>
#include <atomic>
#include <chrono>

#include "docopt.h"
#include "utils/msglog.hpp"
#include "net/net.hpp"
#include "model/model.hpp"
#include "serialize/serialize.hpp"

msglog::msglog logmsg;
using namespace std::chrono_literals;

/*
 * The GameAction class encapsulates the vector of snakes and the
 * scenario and provides thread-safe methods for interating with these
 */
class GameAction
{
	std::vector<std::shared_ptr<model::Snake>> snakes;
	model::Physics newton;
	model::Scenario scenario;
	std::mutex game_mutex;
public:
	int create_player()
	{
		std::lock_guard<std::mutex> lock(game_mutex);
		auto new_snake = std::make_shared<model::Snake>(10, 10, model::Right);
		snakes.push_back(new_snake);
		return new_snake->getUID();
	}

	void remove_dead_players()
	{
		std::lock_guard<std::mutex> lock(game_mutex);
		for (int i = 0; i < snakes.size(); i++)
		{
			if (snakes[i]->getLiviness() == true)
			{
				snakes.erase(snakes.begin() + i);
				i--;
			}
		}
	}

	void remove_player(int uid)
	{
		std::lock_guard<std::mutex> lock(game_mutex);
		for (int i = 0; i < snakes.size(); i++)
		{
			if (snakes[i]->getUID() == uid)
			{
				snakes.erase(snakes.begin() + i);
				break;
			}
		}
	}

	void run_interaction()
	{
		std::lock_guard<std::mutex> lock(game_mutex);
		newton.update_all(snakes, scenario);
	}

	void spaw_cash()
	{
		std::lock_guard<std::mutex> lock(game_mutex);
		this->scenario.generate_cash();
	}

	int get_num_players()
	{
		return this->snakes.size();
	}

	void serialize_all(std::vector<std::shared_ptr<net::NetObject>>& net_objs)
	{
		std::lock_guard<std::mutex> lock(game_mutex);
		serialize::Vector_Serializer snake_list_serializer;
		net_objs.clear();

		auto new_netobj = std::make_shared<net::NetObject>();
		this->scenario.serialize(*new_netobj);
		net_objs.push_back(std::move(new_netobj));

		auto snake_list_netobj = snake_list_serializer.serialize_snake_vector(snakes);
		net_objs.push_back(std::move(snake_list_netobj));
	}

	int change_direction(int uid, model::Direction dir)
	{
		std::lock_guard<std::mutex> lock(game_mutex);
		for (int i = 0; i < snakes.size(); i++)
		{
			if (snakes[i]->getUID() == uid)
			{
				snakes[i]->setDirection(dir);
				return 0;
			}
		}
		return -1;
	}
};

static const char _USAGE[] =
    R"(SnakeServer.
Usage:
  __PROGNAME__ [--port=<num> --debug-level=<num>]
  __PROGNAME__ (-h | --help)
  __PROGNAME__ --version
Options:
  -h --help            Show this screen.
  --version            Show version.
  --port=<num>         TCP port [default: 3001].
  --debug-level=<num>  Debug output verbosity level [default: 0].
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

	logmsg.set_debug_level(atoi(args["--debug-level"].asString().c_str()));

	net::NetServer netconn;

	int conn_stat = netconn.open_conn("127.0.0.1", atoi(args["--port"].asString().c_str()));
	if (conn_stat == -1)
	{
		std::stringstream err_str;
		err_str << "Couldn't open a new connection on the " << args["--port"].asString() << " port!";
		logmsg.error_msg(err_str.str());
		return 1;
	}

	std::vector<std::unique_ptr<std::thread>> client_threads;
	std::vector<std::shared_ptr<net::NetTransfer>> client_conns;
	GameAction game_act;
	auto ref_logmsg = std::ref(logmsg);

	/*
	 * Physics thread
	 */
	std::thread physics_thread(
		[ref_logmsg, &game_act, &client_conns]
			{
				uint64_t round = 0;
				while(1)
				{
					std::vector<std::shared_ptr<net::NetObject>> nobjs;
					game_act.run_interaction();
					game_act.remove_dead_players();
					game_act.serialize_all(nobjs);

					/*
					 * Send the serialized objects to every client in
					 * the connections vector
					 */
					for (int i = 0; i < client_conns.size(); i++)
					{
						for (int j = 0; j < nobjs.size(); j++)
						{
							if (client_conns[i]->send_netdata(*(nobjs[j].get())) < 0)
							{
								break;
							}
						}
					}

					/*
					 * Spaw cash every 5 rounds, proportionally to the
					 * number of active players
					 */
					if (round % 5 == 0)
					{
						for (int i = 0; i < game_act.get_num_players(); i++)
						{
							game_act.spaw_cash();
						}
					}

					std::this_thread::sleep_for(100ms);
					round++;
				}
			}
		);

	/*
	 * Main loop
	 */
	while(1)
	{
		/*
		 * Wait for a new player connection
		 */
		auto conn = netconn.wait_conn();

		std::stringstream debug_str;
		std::string ipstr;
		conn->get_ipaddr(ipstr);
		debug_str << "Client connected, ip = " << ipstr;
		logmsg.debug_msg(debug_str.str(), 0);

		/*
		 * Creates a new player and send its uid
		 */
		net::NetObject nobj;
		nobj.id = net::auth;
		int uid = game_act.create_player();
		nobj.data.push_back(uid & 0xFF);
		nobj.data.push_back((uid >> 8) & 0xFF);
		nobj.data.push_back((uid >> 16) & 0xFF);
		nobj.data.push_back((uid >> 24) & 0xFF);
		if (conn->send_netdata(nobj) == -1)
		{
			logmsg.debug_msg("Connection failed! [auth]", 0);
			continue;
		}
		client_conns.push_back(conn);

		/*
		 * Create a new thread to every player connected to handle
		 * keyboard events
		 */
		client_threads.push_back(
			std::make_unique<std::thread>(
				[uid, conn, ref_logmsg, &game_act]
					{
						while(1)
						{
							net::NetObject nobj;
							auto status = conn->recv_netdata(nobj);
							if (status != 0)
							{
								logmsg.debug_msg("Connection failed! [input]", 0);
								game_act.remove_player(uid);
								return;
							}

							if (nobj.id == net::input)
							{
								model::Direction dir = model::Up;
								switch (nobj.data[0])
								{
								case 0:
									dir = model::Up;
									break;
								case 1:
									dir = model::Down;
									break;
								case 2:
									dir = model::Right;
									break;
								case 3:
									dir = model::Left;
									break;
								default:
									dir = model::Up;
									break;
								}
								game_act.change_direction(uid, dir);
							}
						}
					}));
	}
	return 0;
}
