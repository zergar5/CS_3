#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment (lib,"Ws2_32.lib")
#include <cstdio>
#include <winsock2.h>
#include <string>
#include <iostream>
#include <process.h>

using namespace std;

void client_send(LPVOID client_socket)
{
   while (true)
   {
      auto socket = *static_cast<SOCKET*>(client_socket);

      string message;
      getline(cin, message);
      cin.clear();

      char content[UCHAR_MAX];
      strncpy_s(content, message.c_str(), sizeof(content) - 1);
      send(socket, content, UCHAR_MAX, 0);
      if (string(content) == "exit")
      {
         exit(0);
      }
   }
}

void client_receive(LPVOID client_socket)
{
   while (true)
   {
      auto socket = *static_cast<SOCKET*>(client_socket);

      char content[UCHAR_MAX];
      auto responce = recv(socket, content, UCHAR_MAX, 0);
      if (string(content) == "Server shutdown")
      {
         cout << "Server shutdown" << endl;
         break;
      }
      if (string(content) == "Chat if full")
      {
         cout << "Chat if full" << endl;
         break;
      }
      if (responce == SOCKET_ERROR)
      {
         break;
      }
      if (strlen(content) < UCHAR_MAX)
      {
         cout << content << endl;
      }
   }
}

int main()
{
   WSADATA wsaData;
   WSAStartup(MAKEWORD(2, 2), &wsaData);

   auto clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (clientSock == SOCKET_ERROR)
   {
      cout << "Unable to create socket" << endl;
      WSACleanup();
      system("pause");
      return SOCKET_ERROR;
   }
   string ip;
   cout << "ip>";
   cin >> ip;
   cin.ignore();

   u_short port_number = 0;
   cout << "Enter port number: ";
   cin >> port_number;
   cin.ignore();

   SOCKADDR_IN serverInfo;
   serverInfo.sin_family = AF_INET;
   serverInfo.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
   serverInfo.sin_port = htons(port_number);

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

   auto s_thread = _beginthread(client_send, NULL, &clientSock);
   auto r_thread = _beginthread(client_receive, NULL, &clientSock);

   WaitForSingleObject(reinterpret_cast<HANDLE>(s_thread), INFINITE);
   WaitForSingleObject(reinterpret_cast<HANDLE>(r_thread), INFINITE);

   closesocket(clientSock);
   system("pause");
   return 0;
}