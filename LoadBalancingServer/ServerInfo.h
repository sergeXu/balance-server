//Serge xu 10.30
#pragma once
#include<iostream>
#include <string>
#define HEART_BEATS 100
class ServerInfo{
public:
	ServerInfo(void);
	ServerInfo(int id, std::string localIp, std::string publicIp, int data_port, int command_port, int retransmit_port, int bit_rate, int frame_rate, int resolution);
	~ServerInfo(void);

	std::string getServerPublicIP();
	std::string getServerLocalIP();
	int getServerDataPort();
	int getServerCommandPort();
	int getServerRetransmitPort();
	int getResolution();
	int getBitRate();
	int getFrameRate();
	int getNumOfClientsServing();
	int getHeartBeat();
	int getServerID();

	void plusClientsServing();
	void minusClientsServing();

	void plusHeartBeat();
	void minusHeartBeat();
	void showInfo();

private:
	int m_server_id;
	std::string m_public_ip;// IP Address in the public network
	std::string m_local_ip;
	int m_data_port;
	int m_command_port;
	int m_retransmit_port;
	int m_bit_rate;
	int m_frame_rate;
	int m_resolution;
	int m_num_of_clients_serving;
	int m_heart_beat;
};

struct clientConnectRequst
{
	int flag; //conmmad kind
	int bitRate;
	int resolution;
	int frame_rate;
	int serverId;
};
struct serverConnectRequst
{
	int flag; //conmmad kind
	std::string serverIp;
	//reserve to command parameter
	long param;
	int serverDataPort;
	int serverRetranPort;
	int serverCommandPort;
	int bitRate;
	int resolution;
	int frame_rate;
};