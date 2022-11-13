#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment (lib,"Ws2_32.lib")
#include <winsock2.h>
#include <iostream> 
#include <string>
#include <vector>
#include <process.h>

using namespace std;

int current_number = 0;
vector<SOCKET> sockets;
vector<string> nicknames;

void server_chat(LPVOID client_socket)
{
   auto member_socket = *(SOCKET*)client_socket;
   char content[UCHAR_MAX];
   while (true)
   {
      auto responce = recv(member_socket, content, UCHAR_MAX, 0);
      int current_member = distance(sockets.cbegin(), find(sockets.cbegin(), sockets.cend(), member_socket));

      if (responce == SOCKET_ERROR)
      {
         sockets.erase(sockets.cbegin() + current_member);
         nicknames.erase(nicknames.cbegin() + current_member);
         current_number--;
         cout << "Current number of members in chat: " << current_number << endl;
         cout << "Unable to recv" << endl;
         closesocket(member_socket);
         return;
      }

      string message = content;

      if (message == "exit")
      {
         message = nicknames[current_member];
         message.append(" leave the chat");
         cout << message << endl;

         sockets.erase(sockets.cbegin() + current_member);
         nicknames.erase(nicknames.cbegin() + current_member);
         current_number--;

         cout << "Member disconnected" << endl;

         closesocket(member_socket);

         strncpy_s(content, message.c_str(), sizeof(content) - 1);

         for (auto socket : sockets)
         {
            if (socket != member_socket) {
               responce = send(socket, content, UCHAR_MAX, 0);
               if (responce == SOCKET_ERROR)
               {
                  cout << "Unable to send" << endl;
                  return;
               }
            }
         }

         cout << "Current number of members in chat: " << current_number << endl;

         return;
      }

      if (message == "kill")
      {
         strncpy_s(content, "Server shutdown", sizeof(content) - 1);
         for (auto socket : sockets)
         {
            send(socket, content, UCHAR_MAX, 0);
            closesocket(socket);
         }
         system("pause");
         exit(0);
      }

      if (content[0] != '\0');
      {
         string message;
         message.append(nicknames[current_member]);
         message.append(": ");
         message.append(content);
         if (strlen(content) < UCHAR_MAX)
         {
            cout << message << endl;

            strncpy_s(content, message.c_str(), sizeof(content) - 1);
            for (auto socket : sockets)
            {
               if (socket != member_socket)
               {
                  responce = send(socket, content, UCHAR_MAX, 0);
                  if (responce == SOCKET_ERROR)
                  {
                     cout << "Unable to send" << endl;
                     return;
                  }
               }
            }
         }
      }
   }
}

int main()
{
   WSADATA wsaData;
   WSAStartup(MAKEWORD(2, 2), &wsaData);

   auto servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

   if (servSock == INVALID_SOCKET)
   {
      cout << "Unable to create socket" << endl;
      WSACleanup();
      system("pause");
      return SOCKET_ERROR;
   }

   u_short port_number = 0;
   cout << "Enter port number: ";
   cin >> port_number;

   SOCKADDR_IN sin;
   sin.sin_family = AF_INET;
   sin.sin_port = htons(port_number);
   sin.sin_addr.s_addr = INADDR_ANY;

   auto responce = bind(servSock, (LPSOCKADDR)&sin, sizeof(sin));
   if (responce == SOCKET_ERROR)
   {
      cout << "Unable to bind" << endl;
      WSACleanup();
      system("pause");
      return SOCKET_ERROR;
   }

   char host[UCHAR_MAX];
   char HostName[1024];

   if (!gethostname(HostName, 1024))
      if (LPHOSTENT lphost = gethostbyname(HostName))
         strcpy_s(host, inet_ntoa(*(in_addr*)lphost->h_addr_list[0]));

   cout << "Server started at " << host << ", port " << htons(sin.sin_port) << endl;

   while (true)
   {
      responce = listen(servSock, 10);
      if (responce == SOCKET_ERROR)
      {
         cout << "Unable to listen" << endl;
         WSACleanup();
         system("pause");
         return SOCKET_ERROR;
      }

      SOCKADDR_IN from;
      int fromlen = sizeof(from);
      auto clientSock = accept(servSock, (sockaddr*)&from, &fromlen);
      if (clientSock == INVALID_SOCKET)
      {
         cout << "Unable to accept" << endl;
         WSACleanup();
         system("pause");
         return SOCKET_ERROR;
      }
      cout << "New connection accepted from " << inet_ntoa(from.sin_addr) << ", port " << htons(from.sin_port) << endl;
      cout << "Current number of members: " << current_number + 1 << endl;

      char content[SCHAR_MAX];
      responce = recv(clientSock, content, SCHAR_MAX, 0);

      if (responce == SOCKET_ERROR)
      {
         cout << "Unable to recv" << endl;
         WSACleanup();
         system("pause");
         return SOCKET_ERROR;
      }

      sockets.push_back(clientSock);
      string new_member;
      new_member.append("New member: ");
      new_member.append(content);
      nicknames.emplace_back(content);
      new_member.append("; IP: ");
      new_member.append(inet_ntoa(from.sin_addr));
      cout << new_member << endl;
      current_number++;
      strncpy_s(content, new_member.c_str(), sizeof(content) - 1);
      for (auto socket : sockets)
      {
         responce = send(socket, content, SCHAR_MAX, 0);
         if (responce == SOCKET_ERROR)
         {
            WSACleanup();
            system("pause");
            return SOCKET_ERROR;
         }
      }

      _beginthread(server_chat, NULL, &clientSock);
   }
}
