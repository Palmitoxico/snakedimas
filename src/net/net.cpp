#include <cstring>
#include "net.hpp"

namespace net
{
	NetTransfer::NetTransfer()
	{
		this->target.sin_family = AF_INET;
		this->conn_fd = socket(AF_INET, SOCK_STREAM, 0);
	}

	NetTransfer::NetTransfer(const struct sockaddr_in* target, int conn_fd)
	{
		this->target = *target;
		this->conn_fd = conn_fd;
		this->connected = true;
	}

	int NetTransfer::open_conn(const std::string &ip_addr, int port)
	{
		this->ip_addr = ip_addr;
		this->target.sin_port = htons(port);
		inet_aton(this->ip_addr.c_str(), &(this->target.sin_addr));
		if (connect(this->conn_fd, (struct sockaddr*)&(this->target), sizeof(sockaddr_in)) != 0)
		{
			this->connected = false;
			return -1;
		}
		this->connected = true;
		return 0;
	}

	int NetTransfer::send_netdata(const NetObject &ndata)
	{
		NetPacket npacket;
		int32_t total_packets;

		total_packets = ndata.data_len / 4088;
		if ((ndata.data_len % 4088) > 0)
		{
			total_packets += 1;
		}

		npacket.packet_id = control_packet;
		int8_t obj_id = ndata.id;
		npacket.data_len = 5;
		npacket.data[0] = obj_id;
		npacket.data[1] = total_packets & 0xFF;
		npacket.data[2] = (total_packets >> 8) & 0xFF;
		npacket.data[3] = (total_packets >> 16) & 0xFF;
		npacket.data[4] = (total_packets >> 24) & 0xFF;

		if (send(this->conn_fd, &npacket, (sizeof(npacket) - 4088) + npacket.data_len, 0) == -1)
		{
			this->connected = false;
			return -1;
		}

		npacket.packet_id = data_packet;
		for (int32_t bytes_sent = 0; bytes_sent < ndata.data_len;)
		{
			int32_t bytes_remaining = ndata.data_len - bytes_sent;
			int32_t bytes_to_send = bytes_remaining > 4088 ? 4088 : bytes_remaining;

			memcpy(npacket.data, &ndata.data[bytes_sent], bytes_to_send);
			npacket.data_len = bytes_to_send;

			if (send(this->conn_fd, &npacket, (sizeof(npacket) - 4088) + npacket.data_len, 0) == -1)
			{
				this->connected = false;
				return -1;
			}

			bytes_sent += bytes_to_send;
		}
		return 0;
	}

	int NetTransfer::recv_netdata(NetObject &ndata)
	{
		NetPacket npacket;
		if (recv(this->conn_fd, &npacket, sizeof(npacket), 0) == -1)
		{
			this->connected = false;
			return -1;
		}

		if (npacket.packet_id != control_packet)
		{
			this->connected = false;
			return -1;
		}

		ndata.id = static_cast<ObjectID>(npacket.data[0]);
		ndata.data.clear();

		int32_t total_packets = 0;

		total_packets = npacket.data[1] | (npacket.data[2] << 8) |
			(npacket.data[3] << 16) | (npacket.data[4] << 24);

		for (int32_t recv_packets = 0; recv_packets < total_packets; recv_packets++)
		{
			if (recv(this->conn_fd, &npacket, sizeof(npacket), 0) == -1)
			{
				this->connected = false;
				return -1;
			}

			if (npacket.packet_id != data_packet)
			{
				this->connected = false;
				return -1;
			}

			for (int32_t i = 0; i < npacket.data_len; i++)
			{
				ndata.data.push_back(npacket.data[i]);
			}
		}
		return 0;
	}

	int NetTransfer::get_ipaddr(std::string& ipstr)
	{
		char ips[32];
		if (inet_ntop(AF_INET, &this->target.sin_addr, ips, INET_ADDRSTRLEN) == NULL)
		{
			return -1;
		}
		ipstr = ips;
		return 0;
	}

	bool NetTransfer::is_connected()
	{
		return this->connected;
	}

	NetTransfer::~NetTransfer()
	{
		close(this->conn_fd);
	}

	NetServer::NetServer()
	{
		this->target.sin_family = AF_INET;
		this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	}

	int NetServer::open_conn(const std::string &ip_addr, int port)
	{
		this->ip_addr = ip_addr;
		this->target.sin_port = htons(port);
		inet_aton(this->ip_addr.c_str(), &(this->target.sin_addr));
		if (bind(this->socket_fd, (struct sockaddr*)&this->target, sizeof(struct sockaddr_in)) != 0)
		{
			return -1;
		}
		return 0;
	}

	std::shared_ptr<NetTransfer> NetServer::wait_conn()
	{
		struct sockaddr_in client;
		socklen_t client_size = (socklen_t)sizeof(client);

		listen(socket_fd, 2);
		int client_fd = accept(this->socket_fd, (struct sockaddr*)&client, &client_size);
		std::shared_ptr<NetTransfer> ntrans(new NetTransfer(&client, client_fd));
		return ntrans;
	}

	NetServer::~NetServer()
	{
		close(this->socket_fd);
	}
}
