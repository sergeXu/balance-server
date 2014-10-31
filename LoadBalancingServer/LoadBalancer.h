//Serge xu 10.30
#pragma once
#include "ServerInfo.h"
#include <vector>
class LoadBalancer
{
public:
	LoadBalancer(void);
	~LoadBalancer(void);

	// Load from config file
	void loadFromConfig();		

	// when new server connects, add server to the list
	void addServertoList(serverConnectRequst server);	

	// when m_heartbeat < 0, remove the server from the list
	void removeServerFromList(int server_id);

	// get server information with m_server_id
	ServerInfo getServerInfo(int server_id);

	// choose a server according to required bit_rate, frame_rate, or resolution
	ServerInfo pickAServer(int bit_rate, int frame_rate, int resolution);

	// whether the server list is empty
	bool isServerListEmpty();

	// when a client quit, update the num_of_clients_serving of the server
	void clientDisconnect(int server_id);
	//maintain the alive server
	void updateServerInfo();
	void showDetail();

	ServerInfo & getServerInfoById(int server_id);

public:
	std::string load_balancing_server_ip;
	int serve_server_port;
	int serve_client_port;
private:
	vector<ServerInfo> server_list;
	//vector <ServerInfo>::iterator Iter;
};

