#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib,"Ws2_32.lib")
#include <cstdio>
#include <winsock2.h>
#include <string>
#include <iostream>
#include <process.h>

using namespace std;

bool in = true;

void client_send(LPVOID client_socket)
{
   auto socket = *(SOCKET*)client_socket;

   string message;
   getline(cin, message);
   cin.clear();

   char content[UCHAR_MAX];
   strncpy_s(content, message.c_str(), sizeof(content) - 1);
   auto responce = send(socket, content, UCHAR_MAX, 0);
   if (string(content) == "exit")
   {
      in = false;
      return;
   }
   if (responce == SOCKET_ERROR)
   {
      in = false;
   }
}

void client_receive(LPVOID client_socket)
{
   auto socket = *(SOCKET*)client_socket;

   char content[UCHAR_MAX];
   auto responce = recv(socket, content, UCHAR_MAX, 0);
   if (string(content) == "Server shutdown")
   {
      cout << "Server shutdown" << endl;
      in = false;
      return;
   }
   if (string(content) == "Chat if full")
   {
      cout << "Chat if full" << endl;
      in = false;
      return;
   }
   if (responce == SOCKET_ERROR)
   {
      in = false;
      return;
   }
   if (strlen(content) < UCHAR_MAX)
   {
      cout << content << endl;
   }
}


int main()
{
   WSADATA wsaData;
   WSAStartup(MAKEWORD(2, 2), &wsaData);

   //Создаем сокет
   auto clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (clientSock == SOCKET_ERROR)
   {
      cout << "Unable to create socket" << endl;
      WSACleanup();
      system("pause");
      return 1;
   }
   string ip;
   cout << "ip>";
   cin >> ip;
   cin.ignore();

   SOCKADDR_IN serverInfo;
   serverInfo.sin_family = AF_INET;
   serverInfo.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
   serverInfo.sin_port = htons(2009);

   //Пытаемся присоединится к серверу по ip и port

   auto responce = connect(clientSock, reinterpret_cast<LPSOCKADDR>(&serverInfo), sizeof(serverInfo));
   if (responce == SOCKET_ERROR)
   {
      cout << "Unable to connect" << endl;
      WSACleanup();
      system("pause");
      return SOCKET_ERROR;
   }
   cout << "Connection made successfully" << endl;
   cout << "Enter your nickname: " << endl;
   string nickname;
   cin >> nickname;
   cin.ignore();

   responce = send(clientSock, nickname.c_str(), SCHAR_MAX, 0);
   if (responce == SOCKET_ERROR)
   {
      cout << "Unable to send" << endl;
      WSACleanup();
      system("pause");
      return SOCKET_ERROR;
   }

   cout << "To leave the chat enter 'exit'" << endl;

   while (in)
   {
      _beginthread(client_send, NULL, &clientSock);
      _beginthread(client_receive, NULL, &clientSock);
   }

   closesocket(clientSock);
   system("pause");
   return 0;
}
