// LoadBalancingServer.cpp : 定义控制台应用程序的入口点。
//Serge xu 10.30

#include "stdafx.h"
#include "LoadBalancer.h"
#include "ServerInfo.h"
#include <iostream>
#include <Windows.h>
#include <fstream>

using namespace std;
// thread_1, serving_connection_from_server on the serve_server_port;

// thread_2, serving_connection_from_client on the serve_client_port;
// if the client "Login", pickAServer() and send the server information back
// else if the client "Quit", updateServerInfo(server_id) to minus the num_of_clients_serving
// else if the client "ChangeServer", pickAServer() and send the server information back, also update server information accordingly.
DWORD WINAPI serving_connection_from_client(LPVOID lpParameter);
DWORD WINAPI serving_connection_from_server(LPVOID lpParameter);
DWORD WINAPI updateServerData(LPVOID lpParameter);

void readConfig(void);
std::string& trim(std::string &s) ;


LoadBalancer loadBal;
CRITICAL_SECTION g_cs;
//配置文件项

std::string balanceServerIP="";
int clientListenPort=0;
int serverListenPort=0;

int _tmain(int argc, _TCHAR* argv[])
{
	//读取配置文件
	readConfig();
	//cout<<"the size of  is ServerInfo:"<<sizeof(ServerInfo)<<endl;
	InitializeCriticalSection(&g_cs);

	HANDLE hThread1;
	HANDLE hThread2;
	HANDLE hThread3;
	hThread1 = CreateThread(NULL,0,serving_connection_from_client,NULL,0,NULL);
	hThread2 = CreateThread(NULL,0,serving_connection_from_server,NULL,0,NULL);
	hThread3 = CreateThread(NULL,0,updateServerData,NULL,0,NULL);

	std::cout << "This is a test...\n";
	//std::system("pause");
	Sleep(-1);
	DeleteCriticalSection(&g_cs);

	//EnterCriticalSection(&g_cs);
	//LeaveCriticalSection(&g_cs);
	return 0;
}

DWORD WINAPI serving_connection_from_client(LPVOID lpParameter){
	/*
	winsock初始化
	*/
	WORD wVersionRequested; 
    WSADATA wsaData; 
    int err; 
    wVersionRequested = MAKEWORD(1,1); 
    err = WSAStartup(wVersionRequested,&wsaData); 
    if ( err != 0 ) { 
        return -1; 
    } 

    if ( LOBYTE( wsaData.wVersion ) != 1 || 
        HIBYTE( wsaData.wVersion ) != 1) { 
            WSACleanup( ); 
            return -1; 
    }

	SOCKADDR_IN banlanceServerAddr;
	SOCKET client_socket;    
	client_socket=socket(AF_INET, SOCK_DGRAM,0);
	if(client_socket==INVALID_SOCKET)
	{

		cout<<"serving_connection_from_client socket create fail\n";
		return false;
	}
    banlanceServerAddr.sin_family = AF_INET;
    //banlanceServerAddr.sin_port = htons(loadBal.serve_client_port); 
	banlanceServerAddr.sin_port = clientListenPort;
	banlanceServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(SOCKET_ERROR==bind(client_socket,(SOCKADDR*)&banlanceServerAddr,sizeof(banlanceServerAddr)))
	{
		cout<<"serving_connection_from_client bind error\n";
		return false;
	};
	//CString tempstr;
	ServerInfo tempServer;
	char recvBuf[200];
	char sendBuf[300];
	if(sizeof(ServerInfo)>300)
	{
		cout<<"the ServerInfo size is larger than send buffer, return error";
		return false;
	}
	char * pSendBuf;
	SOCKADDR_IN clientAddr;
	int len = sizeof(clientAddr);
	while(1)
	{
		cout<<"serving_connection_from_client start listen:\n";
		memset(recvBuf,0,200);
		recvfrom(client_socket,recvBuf,200,0,(SOCKADDR *)&clientAddr,&len);
		clientConnectRequst tempRecFromClient = *((clientConnectRequst*)recvBuf);
		//判断是否是连接请求
		if(1==tempRecFromClient.flag)
		{

			cout<<inet_ntoa(clientAddr.sin_addr)<<" send: "<<recvBuf<<endl;

			//开始发送信息
			memset(sendBuf,0,300);

			EnterCriticalSection(&g_cs);
			tempServer=loadBal.pickAServer(tempRecFromClient.bitRate,tempRecFromClient.frame_rate,tempRecFromClient.resolution);
			//*sendBuf=(string)(tempServer);
			LeaveCriticalSection(&g_cs);

			tempServer.showInfo();
			pSendBuf=(char *)(&tempServer);
			for(int i=0;i<sizeof(sendBuf);i++)
			{
				sendBuf[i]=pSendBuf[i];
			}
			sendto(client_socket,sendBuf,300,0,(SOCKADDR *)&clientAddr,sizeof(clientAddr));
			//清空recvBuf
			memset(recvBuf,0,200);
			memset(sendBuf,0,300);
		}
		//client exit
		else if(2==tempRecFromClient.flag)
		{
			EnterCriticalSection(&g_cs);
			loadBal.getServerInfoById(tempRecFromClient.serverId).minusClientsServing();
			LeaveCriticalSection(&g_cs);
			cout<<inet_ntoa(clientAddr.sin_addr)<<" client exit\n ";
		}
		
	}

	closesocket(client_socket);
	WSACleanup();
}


DWORD WINAPI serving_connection_from_server(LPVOID lpParameter){
	printf("serving_connection_from_server start:\n");
	/*
	winsock初始化
	*/
	WORD wVersionRequested; 
    WSADATA wsaData; 
    int err; 
    wVersionRequested = MAKEWORD(1,1); 
    err = WSAStartup(wVersionRequested,&wsaData); 
    if ( err != 0 ) { 
        return -1; 
    } 

    if ( LOBYTE( wsaData.wVersion ) != 1 || 
        HIBYTE( wsaData.wVersion ) != 1) { 
            WSACleanup( ); 
            return -1; 
    }

	SOCKADDR_IN banlanceServerAddr;
	SOCKET server_socket;    
	server_socket=socket(AF_INET, SOCK_DGRAM,0);
	if(server_socket==INVALID_SOCKET)
	{

		cout<<"serving_connection_from_server socket create fail\n";
		return false;
	}
    banlanceServerAddr.sin_family = AF_INET;
	banlanceServerAddr.sin_port = serverListenPort;
	banlanceServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(SOCKET_ERROR==bind(server_socket,(SOCKADDR*)&banlanceServerAddr,sizeof(banlanceServerAddr)))
	{
		cout<<"serving_connection_from_server bind error\n";
		return false;
	};
	char recvBuf[200];
	char sendBuf[100];
	SOCKADDR_IN serverAddr;
	int len = sizeof(serverAddr);
	while(1)
	{
		cout<<"serving_connection_from_client start listen:\n";
		memset(recvBuf,0,200);
		recvfrom(server_socket,recvBuf,200,0,(SOCKADDR *)&serverAddr,&len);
		serverConnectRequst tempServer = *((serverConnectRequst*)recvBuf);
		std::cout<<"receive info from server\n";
		EnterCriticalSection(&g_cs);
		loadBal.addServertoList(tempServer);
		LeaveCriticalSection(&g_cs);
	}
	closesocket(server_socket);
	WSACleanup();
}
DWORD WINAPI updateServerData(LPVOID lpParameter){
	while(1)
	{
		//更新服务器数据
		EnterCriticalSection(&g_cs);
	
		loadBal.updateServerInfo();

		LeaveCriticalSection(&g_cs);
		Sleep(1000);
	}
	return false;
}
//配置项文件 serge 2014.10.16 
//裁剪字符串两边空格
std::string& trim(std::string &s)   
{  
    if (s.empty())   
    {  
        return s;  
    }  
  
    s.erase(0,s.find_first_not_of(" "));  
    s.erase(s.find_last_not_of(" ") + 1);  
    return s;  
}  
//读取配置文件
void readConfig(void)
{
	ifstream finC;
	finC.open("config.cfg");

	string tempStr="";

	if(finC.is_open())
	{
		
		while(getline(finC,tempStr))
		{
			if(trim(tempStr).size()<=0)
			{ continue; }
			else if(trim(tempStr).size()>0)
			{
				if(tempStr[0]=='#')
					{
						//输出配置注释
					//	cout<<"notation :"<<tempStr<<endl;
						continue ;
					}
				string temp1=tempStr;
				string temp2=tempStr;
				temp1.erase(temp1.find_first_of(" "));  
				temp2.erase(0,temp2.find_first_of(' ')); 
				trim(temp1);
				trim(temp2);
				//cout<<temp2<<endl;
				//在此添加配置项
				//设置服务器IP
				if(temp1 == "BALANCE_SERVER_IP")
				{
					 balanceServerIP= temp2;
					cout<<"BALANCE_SERVER_IP = "<<balanceServerIP<<endl;
				}
				if(temp1 == "CLIENT_LISTEN_PORT")
				{
					clientListenPort = atoi(temp2.data());
					cout<<"SET CLIENT_LISTEN_PORT = "<<clientListenPort<<endl;
				}
				if(temp1 == "SERVER_LISTEN_PORT")
				{
					serverListenPort = atoi(temp2.data());
					cout<<"SET SERVER_LISTEN_PORT = "<<serverListenPort<<endl;
				}
			}
		}

		finC.close();
		//检测读取文件是否正常
		if(balanceServerIP=="")
			cout<<"read config error,banlanceServerIP is blank\n";
		if(clientListenPort==0)
			cout<<"read config error,clientListenPort is 0\n";
		if(serverListenPort==0)
			cout<<"read config error,serverListenPort is 0\n";
	}
	else
	{
		cout<<"can not open config\n";
		return;
	}
}


