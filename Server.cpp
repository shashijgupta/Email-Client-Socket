#include <unistd.h>  
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/time.h>
#include <stdio.h>
using namespace std;
/* Print usage of program */
void usage(char *progname) {
  fprintf(stderr, "%s <portNum> <passwdfile> <user-database>\n",
      progname);
} // End usage()

int main(int argc, char *argv[]) 
{
    int server_fd, activity, new_socket, valread, max_clients = 30, client_socket[30], sd; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
    int max_sd;
    char mes[1024] = {0};
    string username[30];       
    if(argc !=4) {
        usage(argv[0]); exit(1);
    }
    fd_set readfds;
    for (int i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }    
    int PORT = atoi(argv[1]);
    ifstream myfile(string(argv[2]).c_str());    
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        fprintf(stderr,"Bind on the given port fails"); 
        exit(2); 
    } 
    else {
        cout<<"BindDone: "<<PORT<<"\n";
    }
    if (myfile.is_open())
    {
    }

    else{
        fprintf (stderr, "Can't open the passwdfile");
        exit(3);

    }
    DIR *pdir = NULL;
    struct dirent  *pent = NULL;
    string direc = "./" + string(argv[3]);
    pdir = opendir(direc.c_str());
    if (pdir == NULL){
        fprintf (stderr, "Can't access the user database");
        exit(4);
    }
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    else {
        cout<<"ListenDone: "<<PORT<<"\n";
    }
    while(true){
    FD_ZERO(&readfds);
    FD_SET(server_fd, &readfds);
    max_sd = server_fd;
     for ( int i = 0 ; i < max_clients ; i++)   
        {   
            //socket descriptor  
            sd = client_socket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        } 
     activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);      
    if (FD_ISSET(server_fd, &readfds)){

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 
    else{
        struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&address;
        struct in_addr ipAddr = pV4Addr->sin_addr;
        char str[INET_ADDRSTRLEN];
        inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );
        cout<<"Client: "<<str<<":"<<address.sin_port<<endl;   
    }

    valread = recv( new_socket , buffer, 1024, 0); 
    string inf[4];
    try{
        int i = 0;
        stringstream ssin(string(buffer).c_str());
        while (ssin.good() && i < 4){
            ssin >> inf[i];
            ++i;
        }
    }
    catch(...){
        cout<<"Unknown Command\n";
        close(new_socket);
    }
    string user = inf[1];
    string pass = inf[3];
    string s;
    bool b = false;
    ifstream myfile1(string(argv[2]).c_str());
    if (myfile1.is_open())
    {   
        while ( getline (myfile1,s) )
        {
            string arr[2];    
            int i = 0;
            std::string delimiter = " ";

            size_t pos = 0;
            std::string token;
            while ((pos = s.find(delimiter)) != std::string::npos) {
                token = s.substr(0, pos);
                arr[0] = token;
                s.erase(0, pos + delimiter.length());
            }
            arr[1] = s;
            if (arr[0]==user){
                b = true;
                if(arr[1]==pass){
                    string a = "Welcome " + user;
                    cout<<a<<"\n";
                    int t = send(new_socket , a.c_str() , a.size() , 0 );
            
                    }                   
                
                else{
                    cout<<"Wrong Passwd\n";
                    close(new_socket);
                }
            }
        }
        myfile1.close();
    }
    if(!b){
        cout<<"Invalid User\n";
        close(new_socket);
    }

    for (int i = 0; i < max_clients; i++)   
            {   
                //if position is empty  
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;
                    username[i] = user;   
                    //printf("Adding to list of sockets as %d\n" , i);   
                         
                    break;   
                }   
            }

}
    for (int i = 0; i < max_clients; i++)   
        {   
            new_socket = client_socket[i];   
                 
            if (FD_ISSET( new_socket , &readfds))   
            {   
                //Check if it was for closing , and also read the  
                //incoming message  
                if ((valread = read( new_socket , mes, 1024)) != 0)   
                {   
                    
                    //cout<<"hi"<<endl;
                    string l = string(mes);
                    string user = username[i];
                    string a;
                    //cout<<l<<endl;
                    try{
                        if(l.substr(0,4)=="quit"){
                            cout<<"Bye "<<user<<endl;
                            close(new_socket);
                            client_socket[i] = 0;
                            break;
                        }
                        else if (l=="LIST"){
                            DIR *dp;
                            int i = 0;
                            struct dirent *ep;  
                            string user_direc = direc + "/" + user;   
                            dp = opendir (user_direc.c_str());

                            if (dp != NULL)
                            {
                                while (ep = readdir (dp))
                                    i++;

                                (void) closedir (dp);
                            }
                        else{
                            cout<<user<<": Folder Read Fail\n";
                            close(new_socket);
                            client_socket[i] = 0;
                            break;
                        }   
                            i = i-2;
                            a = user + ": No of messages " + to_string(i);
                            cout<<a<<endl;
                            send(new_socket , a.c_str() , a.size() , 0 );
                        }
                        else if (l.substr(0,5)=="RETRV"){
                            l.erase(0,6);
                            DIR *dp;
                            struct dirent *ep;  
                            string user_direc = direc + "/" + user;   
                            dp = opendir (user_direc.c_str());
                            bool b1 = false;
                            string abc;
                            string bcd;
                            char mfcc[1500] = {0};
                            if (dp != NULL)
                            {
                                while (ep = readdir (dp)){

                                    abc = string(ep->d_name);
                                    
                                    //cout<<ep->d_type<<endl;
                                    //cout<<abc<<endl;
                                    size_t pos1 = abc.find(".");
                                    string token1 = abc.substr(0,pos1);
                                    bcd = abc;
                                    bcd.erase(0,pos1 + 1);
                                    //cout<<bcd<<endl;
                                    if(token1==l){
                                        b1 = true;

                                        string file_add = user_direc + "/" + abc;
                                        
                                        FILE *p_file = NULL;

                                        p_file = fopen(file_add.c_str(),"rb");

                                        fseek(p_file,0,SEEK_END);
                                        int filesize = ftell(p_file);
                                        rewind(p_file);
                                        int sizecheck = 0;
                                        cout<<user<<": Transferring Message "<<l<<endl;
                                
                                        send(new_socket, to_string(filesize).c_str(), 1024, 0);
                                        send(new_socket, bcd.c_str(),1024,0);
                                        while(sizecheck < filesize){
                                            int Read = fread(mfcc, sizeof(char), 1500, p_file);
                                            send(new_socket, mfcc, Read, 0);
                                            sizecheck += Read;
                                        }
                                    

                                        fclose(p_file);
                                

                                    }}

                                (void) closedir (dp);
                            
                            }
                            else{
                            cout<<"Message Read Fail\n";
                            close(new_socket);
                            client_socket[i] = 0;
                            break;
                        }
                            if (!b1){
                                cout<<"Message Read Fail\n";
                                close(new_socket);
                                client_socket[i] = 0;
                                break;
                            }
                           

                        }
                        else{
                            cout<<"unknown Command"<<endl;
                            close(new_socket);
                            client_socket[i] = 0;
                            break;
                        }
                    }
                    catch(...){
                        cout<<"Unknown Command1\n";
                        close(new_socket);
                        client_socket[i] = 0;
                        break;
                    }  
                       
                       
             
                     
                //Echo back the message that came in  
                   
            }   
        }   

}}
myfile.close();
} 