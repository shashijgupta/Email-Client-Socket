#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h>
#include <unistd.h>  
#include <string.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>  
#include <experimental/filesystem>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

using namespace std; 
void usage(char *progname) {
  fprintf(stderr, "%s <ServerIPAddr:port> <user-name> <passwd> <list-of-messages> <local-folder> <interval>\n",
      progname);
} // End usage()
void deldir(const char* szDir)
{
    DIR *d;
    struct dirent *de;
    
    if (!(d = opendir(szDir)))
    {
        //cout << "Can't clean files from directory " << szDir << " errno=" << errno << endl;
        return;
    }
    char szCurDir[256];
    getcwd(szCurDir, 256);
    chdir(szDir);
    while ((de = readdir(d)))
    {
        struct stat fs;
        if (stat(de->d_name, &fs) != 0)
        {
            //cout << "Couldn't get stat of " << de->d_name << " errno=" << errno << endl;
            return;
        }

        if ((fs.st_mode & S_IFDIR) != 0)
        {
            if (de->d_name[0] != '.')
               deldir(de->d_name);
        }
        else
            remove(de->d_name);

   }

    closedir(d);
    chdir(szCurDir);
}

int main(int argc, char *argv[]) 
{ 
    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in serv_addr;  
    char buffer[1024] = {0}; 

    if(argc !=7) {
        usage(argv[0]); exit(1);
    }
    string k = string(argv[1]);
    int pos = k.find(":");
    const char * ip = k.substr(0,pos).c_str();
    k.erase(0,pos+1);
    vector<string> messages;
    string list_mes = string(argv[4]);
    int port = atoi(k.c_str());
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(port); 
    serv_addr.sin_addr.s_addr = inet_addr(ip);   
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        fprintf(stderr,"Connection to the server fails"); 
        exit(2); 
    }
    else{
        cout<<"ConnectDone: "<<string(argv[1])<<endl;
    }
    try{
        std::string delimiter = ",";

        size_t pos = 0;
        std::string token;
        while ((pos = list_mes.find(delimiter)) != std::string::npos) {
            token = list_mes.substr(0, pos);
            int z = atoi(token.c_str());
            if (to_string(z)!=token){
                throw 3;
            }
            messages.push_back(token);
            list_mes.erase(0, pos + delimiter.length());
        }
        messages.push_back(list_mes);
        int t1 = stoi(list_mes.c_str());
        if (to_string(t1)!=list_mes){
            throw 3;
        }
        
    }
    catch(...){
       usage(argv[0]); exit(3); 
    }
    try{
        string myDirectory = argv[5];
        deldir(myDirectory.c_str());
        int check = mkdir(myDirectory.c_str(), S_IRWXU);
    } 
    catch(...){
        exit(4);
    }

    string x = "User: " + string(argv[2]) + " Pass: " + string(argv[3]);
    const char * hello = x.c_str();
    send(sock , hello , strlen(hello) , 0 );  
    valread = recv( sock , buffer, 1024, 0); 
    printf("%s\n",buffer );
    x = "LIST";
    // const char * q = x.c_str();
    // send(sock , q , strlen(q) , 0 );
    // valread = recv( sock , buffer, 1024, 0); 
    // printf("%s\n",buffer );
    sleep(atoi(argv[6]));
    for(int i=0;i<messages.size();i++){
        string m1 = "RETRV " + messages[i];
        const char * q3 = m1.c_str();
        send(sock , q3 , strlen(q3) , 0 );
        char gotfilesize[1024];
        recv(sock,gotfilesize,1024,0);
        int filesize = atoi(gotfilesize);
        char filetype[1024];
        recv(sock,filetype,1024,0);

        string file_n = string(argv[5]) + "/" + messages[i] + "." +  string(filetype);
        int sizecheck = 0;
        //cout<<file_n<<endl;
        FILE *fp = fopen(file_n.c_str(), "wb");
        char mfcc[1500] = {0};
        while(sizecheck < filesize){
           int Received = recv(sock, mfcc, 1500, 0);
          // cout<<Received<<endl;
            fwrite(mfcc, 1, Received, fp);
            fflush(fp);
            sizecheck += Received;
    }
    fclose(fp);
    }
    x = "quit";
    const char * q1 = x.c_str();
    send(sock , q1 , strlen(q1) , 0 );

    close(sock); 
} 