#include<iostream>
#include<stdio.h>
#include<vector>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<dirent.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>

#define PORT 10025

void runServer();
void showHelp();
std::vector<std::string> buildPlaylist();
void showList(std::vector<std::string> list);
void* bobok(void* args);
struct Packet {
    int sock;
    std::string command;
    int aux;
    std::vector<std::string> list;
};
void* sendPacket(void* args);

std::string basedir = "/home/avtors/Music/plist/";

int main() {
   struct sockaddr_in vlc_server;
   memset(&vlc_server, '0', sizeof(vlc_server));
   int sock = 0;

   // Run cvlc server
   runServer();
   sleep(1);
   std::cout<<"cvlc server is now running\n";

   // Creating connection with cvlc
   if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
       std::cout<<"Error creating socket. Exiting\n";
       return -1;
   }

   vlc_server.sin_family = AF_INET;
   vlc_server.sin_port = htons(PORT);

   if(inet_pton(AF_INET, "127.0.0.1", &vlc_server.sin_addr) <= 0) {
       std::cout<<"Invalid address. Exiting.\n";
       return -1;
   }

   if(connect(sock, (sockaddr *)&vlc_server, sizeof vlc_server) < 0) {
       std::cout<<"Connection failed. Exiting.";
       return -1;
   }

   std::vector<std::string> playlist = buildPlaylist();
   std::string command;
   Packet pack;
   int aux;
   pthread_t tid;

   pack.sock = sock;
   pack.list = playlist;

   while(1) {
       // Input command
       std::cout<<"[>] ";
       std::cin>>command;

       if(command=="help") {
            showHelp();
            continue;
       }
       else if(command=="exit")
           break;
       else if(command=="list") {
           showList(playlist);
           continue;
       }
       else if(command=="play" || command=="pause" ||
               command=="stop" || command=="resume") 
           std::cin>>aux;
       else {
           std::cout<<"Unknown command.\n";
       }

       // Packing packet
       pack.command = command;
       pack.aux = aux;

       // Send command to cvlc server
       pthread_create(&tid, NULL, &sendPacket, &pack);
   }

   std::cout<<"Shutting down cvlc server. Bye!\n";
   send(sock, "shutdown\n", 10, 0);

   return 0;
}

void runServer() {
    int status;

    char *run[] = {
        "cvlc",
        "-I", "rc",
        "--rc-host=localhost:10025",
        "-d",
        NULL
    };

    if(!fork())
        execv("/usr/bin/cvlc", run);
}

void showHelp() {
    std::cout<<"TTPlayer v. beta-0\n"
        <<"Command:\n"
        <<"   help\t\tShow this help text\n"
        <<"   list\t\tShow songs in the playlist\n"
        <<"   play <id>\tPlay song with 'id' in playlist\n"
        <<"   pause <t>\tPause the song after t seconds\n"
        <<"   resume <t>\tResume the song after t seconds\n"
        <<"   stop <t>\tStop the song after t seconds\n"
        <<"   exit\t\tExit this program.\n";
}

std::vector<std::string> buildPlaylist() {
    std::vector<std::string> temp;
    DIR *dir;
    dirent *ent;
    if((dir = opendir(basedir.c_str())) != NULL) {
        while((ent = readdir(dir)) != NULL) {
            if(strstr(ent->d_name, "mp3"))
                temp.push_back(ent->d_name);
        }
    }
    std::cout<<temp.size()<<" musics found in playlist\n";
    return temp;
}

void showList(std::vector<std::string> list) {
    int i;
    std::cout<<"\n[!] Playlist -----------------------\n";
    for(i = 0; i<list.size(); i++)
        std::cout<<i+1<<" "<<list[i]<<std::endl;
    std::cout<<"------------------------------------\n\n";
    return;
}

void* sendPacket(void* args) {
    std::string sendp = "";
    Packet *pack = (Packet *)args;
    if(pack->command == "play") {
        sendp = "clear\nadd file://" + basedir + pack->list[pack->aux-1];
    }
    else if(pack->command == "pause") {
        sleep(pack->aux);
        sendp = "pause";
    }
    else if(pack->command == "stop") {
        sleep(pack->aux);
        sendp = "stop";
    }
    else if(pack->command == "resume") {
        sleep(pack->aux);
        sendp = "pause";
    }

    sendp+="\n";

    send(pack->sock, sendp.c_str(), sendp.length(), 0);
    
}
