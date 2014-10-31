//Serge xu 10.30
#include "StdAfx.h"
#include "ServerInfo.h"
// Server
ServerInfo::ServerInfo(void)
{
}

ServerInfo::ServerInfo(int id, std::string localIp, std::string publicIp, int data_port, 
	int command_port, int retransmit_port, int bit_rate, int frame_rate,
	int resolution){
		m_server_id = id;
		this->m_local_ip = localIp;
		this->m_public_ip = publicIp;
		m_data_port = data_port;
		m_command_port = command_port;
		m_retransmit_port = retransmit_port;
		m_bit_rate = bit_rate;
		m_frame_rate = frame_rate;
		m_resolution = resolution;
		m_num_of_clients_serving = 0;
		m_heart_beat = HEART_BEATS;
}


ServerInfo::~ServerInfo(void)
{

}

std::string ServerInfo::getServerPublicIP(){
	return this->m_public_ip;
}
std::string ServerInfo::getServerLocalIP(){
	return this->m_local_ip;
}
int ServerInfo::getServerID(){
	return this->m_server_id;
}
int ServerInfo::getServerDataPort(){
	return this->m_data_port;
}

int ServerInfo::getServerCommandPort(){
	return this->m_command_port;
}

int ServerInfo::getServerRetransmitPort(){
	return this->m_retransmit_port;
}

int ServerInfo::getBitRate(){
	return this->m_bit_rate;
}

int ServerInfo::getFrameRate(){
	return this->m_frame_rate;
}

int ServerInfo::getResolution(){
	return this->m_resolution;
}

int ServerInfo::getNumOfClientsServing(){
	return this->m_num_of_clients_serving;
}

int ServerInfo::getHeartBeat(){
	return this->m_heart_beat;
}

void ServerInfo::plusClientsServing(){
	this->m_num_of_clients_serving += 1;
}

void ServerInfo::minusClientsServing(){
	this->m_num_of_clients_serving -= 1;
	if(m_num_of_clients_serving<0)
	{
		std::cout<<"m_num_of_clients_serving < 0 , error\n";
	}
}

void ServerInfo::plusHeartBeat(){
	this->m_heart_beat = HEART_BEATS;
}

void ServerInfo::minusHeartBeat(){
	this->m_heart_beat-=1;
}
void ServerInfo::showInfo()
{
	std::cout<<" ServerID£º"<<m_server_id<< "  ServerPublicIP£º"<<this->m_public_ip;
	std::cout<<"\n ServerID£º"<<m_server_id<< "  ServerPublicIP£º"<<m_public_ip;
	std::cout<<"\n ServerDataPort£º"<<m_data_port <<"  command_port: "<<m_command_port;
	std::cout<<"\nretransmit_port:"<<m_retransmit_port<<"  bit_rate:"<<m_bit_rate<<"\nnum_of_clients_serving:"<<m_num_of_clients_serving<<std::endl;
}