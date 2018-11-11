/*
 * Description
 */

#ifndef NET_HPP_
#define NET_HPP_

#include <string>
#include <vector>
#include <memory>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace net
{
	enum ObjectID
	{
		auth = 0,
		snake = 1,
		scenario = 2,
		input = 3,
	};

	/*
	 * Control Packets:
	 *
	 * Initialize the data exchange transaction, specify the number of
	 * data packets to be sent.
	 *
	 * Data Packet:
	 *
	 * Contains chunks of serialized data from NetObject.
	 */
	enum PacketID
	{
		control_packet = 0,
		data_packet = 1,
	};

	/*
	 * WARNNING:
	 *
	 * This struct has different memory layouts regarding the machine
	 * endianess, so it will not work when exchanging data between
	 * machines with different endianess.
	 */
	struct NetPacket
	{
		int32_t packet_id;
		int32_t data_len;
		uint8_t data[4088];
	} __attribute__((packed));

	struct NetObject
	{
		uint32_t data_len;
		ObjectID id;
		std::vector<uint8_t> data;
	};

	class NetTransfer
	{
	private:
		struct sockaddr_in target;
		int conn_fd;
		std::string ip_addr;
		bool connected = false;
	public:
		NetTransfer();
		NetTransfer(const struct sockaddr_in* target, int conn_fd);
		int open_conn(const std::string& ip_addr, int port);
		int send_netdata(const NetObject& ndata);
		int recv_netdata(NetObject& ndata);
		bool is_connected();
		~NetTransfer();
	};

	class NetServer
	{
	private:
		struct sockaddr_in target;
		std::string ip_addr;
		int socket_fd;
	public:
		NetServer();
		int open_conn(const std::string& ip_addr, int port);
		std::shared_ptr<NetTransfer> wait_conn();
		~NetServer();
	};
}

#endif
