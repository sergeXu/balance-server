//Serge xu 10.30
#include "StdAfx.h"
#include "LoadBalancer.h"
#include <WinSock2.h>
//////////////////////////////////////
// Load Balancer /////////////////////
//////////////////////////////////////
static int serverIdCount;

LoadBalancer::LoadBalancer(void)
{
	//预留5个位置
	this->server_list.reserve(5);
	serverIdCount=0;
}


LoadBalancer::~LoadBalancer(void)
{
	delete &server_list;
}

void LoadBalancer::loadFromConfig(){

}

void LoadBalancer::addServertoList(serverConnectRequst serverRq)
{
	ServerInfo temp=ServerInfo(serverIdCount,"",serverRq.serverIp,serverRq.serverDataPort,serverRq.serverCommandPort,serverRq.serverRetranPort,serverRq.bitRate,serverRq.frame_rate,serverRq.resolution);
	if(server_list.size()==0)
	{
		server_list.push_back(temp);
		serverIdCount++;
	}
	else
	{
		int i;
		for (i=0 ; i < server_list.size(); i++ )
		{
			if(server_list[i].getServerPublicIP()==temp.getServerPublicIP())
			{
				server_list[i].plusHeartBeat();			
				break;
			}
		}
		if(i>=server_list.size())
		{
			cout<<"add a server to list\n";
			server_list.push_back(temp);
			serverIdCount++;
		}
	}
}
void LoadBalancer::removeServerFromList(int server_id)
{
	for (int i=0 ; i < server_list.size(); i++ )
	{
		if(server_id==server_list[i].getServerID())
		{
			server_list.erase(server_list.begin()+i);
			return ;
		}	
	}
	std::cout<<"remove Server From List fail\n";
}

ServerInfo LoadBalancer::getServerInfo(int server_id)
{
	ServerInfo server;
	for(int i=0;i<server_list.size();i++)
	{
		if(server_id==server_list[i].getServerID())
		{
			server=server_list[i];
			return server;
		}
	}
	std::cout<<"can not find the server\n";
	return server;
}
ServerInfo LoadBalancer::pickAServer(int bit_rate, int frame_rate, int resolution)
{
	ServerInfo server;
	if(server_list.size()==0)
	{
		std::cout<<"not member of servers can be choose\n";
		return server;
	}
	else{
		//最小连接数选择	
		int index=0;
		int minConnect=server_list[0].getNumOfClientsServing();
		for(int i=0;i<server_list.size();i++)
		{
			if(server_list[i].getNumOfClientsServing()<minConnect)
			{
				minConnect=server_list[i].getNumOfClientsServing();
				index=i;
			}
		}
		server=server_list[index];
		server_list[index].plusClientsServing();
		return server;
	}
}

bool LoadBalancer::isServerListEmpty()
{
	if(server_list.size()==0)
		return true;
	else	
		return false;
}

void LoadBalancer::updateServerInfo(){
	for(int i=0;i<server_list.size();i++)
	{
		server_list[i].minusHeartBeat();

		if(server_list[i].getHeartBeat()<0)
		{
			server_list.erase(server_list.begin()+i);
		}
	}
	this->showDetail();
}
void LoadBalancer::clientDisconnect(int server_id){
	ServerInfo & temp=getServerInfoById(server_id);
	temp.minusClientsServing();
}

ServerInfo & LoadBalancer::getServerInfoById(int server_id){
	for(int i=0;i<server_list.size();i++)
	{
		if(server_id==server_list[i].getServerID())
		{
			return server_list[i];
		}
	}
}
void LoadBalancer::showDetail()
{
	std::cout<<"********* Server num =  "<<server_list.size()<<std::endl;
	for(int i=0;i<server_list.size();i++)
	{
		std::cout<<server_list[i].getServerPublicIP()<<"    "<<server_list[i].getHeartBeat()<<"  NUM: "<< server_list[i].getNumOfClientsServing()<<endl;
	}
}