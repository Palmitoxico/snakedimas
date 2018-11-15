#include <cstring>
#include <errno.h>
#include "net.hpp"

namespace net
{
	NetTransfer::NetTransfer()
	{
		int num = 1;
		this->target.sin_family = AF_INET;
		this->conn_fd = socket(AF_INET, SOCK_STREAM, 0);
		setsockopt(this->conn_fd, SOL_SOCKET, SO_REUSEADDR, &num, sizeof(num));
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

		total_packets = ndata.data.size() / 4088;
		if ((ndata.data.size() % 4088) > 0)
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

		if (send(this->conn_fd, &npacket, sizeof(npacket), MSG_NOSIGNAL) < 0)
		{
			this->connected = false;
			close(this->conn_fd);
			return -1;
		}

		npacket.packet_id = data_packet;
		for (int32_t bytes_sent = 0; bytes_sent < ndata.data.size();)
		{
			int32_t bytes_remaining = ndata.data.size() - bytes_sent;
			int32_t bytes_to_send = bytes_remaining > 4088 ? 4088 : bytes_remaining;

			memcpy(npacket.data, &ndata.data[bytes_sent], bytes_to_send);
			npacket.data_len = bytes_to_send;

			if (send(this->conn_fd, &npacket, sizeof(npacket), MSG_NOSIGNAL) < 0)
			{
				this->connected = false;
				close(this->conn_fd);
				return -1;
			}

			bytes_sent += bytes_to_send;
		}
		return 0;
	}

	int NetTransfer::recv_netdata(NetObject &ndata, int timeout_ms)
	{
		NetPacket npacket;

		if (timeout_ms > 0)
		{
			struct timeval tv;
			tv.tv_sec = timeout_ms / 1000;
			tv.tv_usec = (timeout_ms % 1000) * 1000;
			setsockopt(this->conn_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
		}

		if (recv(this->conn_fd, &npacket, sizeof(npacket), 0) == -1)
		{
			auto recv_status = errno;
			volatile char error_str[512];
			if (recv_status == ETIMEDOUT || recv_status == EAGAIN)
			{
				return -2;
			}
			else
			{
				this->connected = false;
				close(this->conn_fd);
				return -1;
			}
		}

		if (npacket.packet_id != control_packet)
		{
			this->connected = false;
			close(this->conn_fd);
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
				auto recv_status = errno;
				volatile char error_str[512];
				if (recv_status == ETIMEDOUT || recv_status == EAGAIN)
				{
					return -2;
				}
				else
				{
					this->connected = false;
					close(this->conn_fd);
					return -1;
				}
			}

			if (npacket.packet_id != data_packet)
			{
				this->connected = false;
				close(this->conn_fd);
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
		int num = 1;
		this->target.sin_family = AF_INET;
		this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &num, sizeof(num));
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
