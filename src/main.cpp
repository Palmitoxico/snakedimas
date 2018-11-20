#include <iostream>
#include <unistd.h>
#include <thread>

#include "docopt.h"
#include "utils/msglog.hpp"
#include "display/display.hpp"
#include "model/model.hpp"
#include "input/input.hpp"
#include "net/net.hpp"
#include "serialize/serialize.hpp"

msglog::msglog logmsg;
using namespace display;
using namespace model;
using namespace input;
using namespace net;

static const char _USAGE[] =
    R"(SnakeServer.
Usage:
  __PROGNAME__ --server-ip ip [--port=<num> --debug-level=<num>]
  __PROGNAME__ (-h | --help)
  __PROGNAME__ --version
Options:
  -h --help            Show this screen.
  --version            Show version.
  --server-ip ip       Server IP
  --port=<num>         TCP port [default: 3001].
  --debug-level=<num>  Debug output verbosity level [default: -1].
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
						 "SnakeDimas V0.1"); // version string

	if (args["--debug-level"])
	{
		logmsg.set_debug_level(atoi(args["--debug-level"].asString().c_str()));
		logmsg.debug_msg("Verbose mode", 0);
	}
     
    std::string server_ip = args["--server-ip"].asString();
    
    auto conn = std::make_shared<NetTransfer>();
    auto net_obj = std::make_shared<NetObject>();
    
    int recv_status = conn->open_conn(server_ip, atoi(args["--port"].asString().c_str()));
    if(recv_status == -1)
	{
		logmsg.error_msg("Could not open a new connection");
		return 1;
	}
    recv_status = conn->recv_netdata(*net_obj);
	if(recv_status == -1)
	{
		logmsg.error_msg("The server has closed the connection unexpectedly");
		return 1;
	}
    
    int uid = 0;
    uid = net_obj->data[0];
    uid |= net_obj->data[1] << 8;
    uid |= net_obj->data[2] << 16;
    uid |= net_obj->data[3] << 24;
    
	auto screen = std::make_shared<Screen>();
    screen->init();
    
    auto camera = std::make_shared<Camera>(0,0);
    auto scenario = std::make_shared<Scenario>();
	static bool keep_running = true;
	static bool alive = true;
	auto keep_running_ref = std::ref(keep_running);
	auto alive_ref = std::ref(alive);

	/*
	 * Rendering thread
	 */
    std::thread conn_thread([uid, camera, conn, screen, scenario, keep_running_ref, &alive]
    {
        NetObject net_obj;
		int last_score = 0;
        while(keep_running_ref){
            std::vector<std::shared_ptr<Snake>> snakes;
            int conn_status = conn->recv_netdata(net_obj, 100);

			if (conn_status == -1) {
				return;
			}
            else if (conn_status == -2) {
				continue;
			}

            if(net_obj.id == ObjectID::snake_vector){
				serialize::Vector_Serializer snake_list_serializer;
				snakes.clear();
				snake_list_serializer.unserialize_snake_vector(net_obj, snakes);
				bool is_alive = false;
				for (int i = 0; i < snakes.size(); i++)
				{
					if (snakes[i]->getUID() == uid)
					{
						camera->setCam_x(snakes[i]->getHeadPosition_x() - (camera->getWidth() / 2));
						camera->setCam_y(snakes[i]->getHeadPosition_y() - (camera->getHeight() / 2));
						is_alive = true;
						last_score = snakes[i]->getScore();
						break;
					}
				}
				if (is_alive == false)
				{
					alive = false;
				}
				else alive = true;
            }
            else if(net_obj.id == ObjectID::scenario){
                scenario->unserialize(net_obj);
            }

			screen->clear_screen();
            screen->render_scenario(*scenario, *camera);
            screen->render_all_snakes(snakes, *camera);
			if (alive == false)
			{
				std::string str_dead;
				str_dead = "You've died. Your Score: " + std::to_string(last_score*100);
				screen->print_msg(str_dead, 4, 4);
				screen->print_msg("Press q to exit.", 4, 5);

			}
        }
    });


    while(keep_running){
        Keyboard keyboard;
		net::NetObject nobj;
        char input = keyboard.getChar();
		nobj.id = net::input;
		nobj.data.clear();
        switch(input){

            case 'w':
				nobj.data.push_back(0);
				if(alive)
					conn->send_netdata(nobj);
            break;

            case 'd':
				nobj.data.push_back(2);
				if(alive)
					conn->send_netdata(nobj);
            break;

            case 's':
				nobj.data.push_back(1);
				if(alive)
					conn->send_netdata(nobj);
            break;

            case 'a':
				nobj.data.push_back(3);
				if(alive)
					conn->send_netdata(nobj);
            break;

            case 'q':
            keep_running = false;
            break;
        }
    }

	conn_thread.join();
    screen->stop();
	return 0;
}
