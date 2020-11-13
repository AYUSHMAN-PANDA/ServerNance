// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8000


#define CLR_NRM  "\x1B[1;0m"
#define CLR_RED  "\x1B[1;31m"
#define CLR_GRN  "\x1B[1;32m"
#define CLR_YEL  "\x1B[1;33m"
#define CLR_BLU  "\x1B[1;34m"
#define CLR_MAG  "\x1B[1;35m"
#define CLR_CYN  "\x1B[1;36m"
#define CLR_WHT  "\x1B[1;37m"
#define CLR_GRN "\x1B[1;32m"

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    size_t sock = 0, valread;
    struct sockaddr_in serv_addr;
   // char *command;
    char buffer[1000000] = {0};
    float temp=0.0;
    int count=0,c1=0,i=0,tt;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    else
    {
        printf("\n\t\tXXX---%sConnected to Server Nance%s---XXX \n\n\n",CLR_YEL,CLR_NRM);
        printf("%sSupported Commands:\n%s1.get - request a file from server\n\n\tUsage:\n\t\t>get <filename>\n\t\t>get <filename1> <filename2> ...\n\n2.exit - Disconnects from server and terminates client program%s\n",CLR_MAG,CLR_CYN,CLR_NRM);
    }
    

    char command[200];
    while(1)
    {
        memset(buffer, 0, 1024);
        printf("\n> ");
        scanf(" %[^\n]s", command);
        //printf("%s\n",command);
        if(strcmp(command, "exit")==0)
        {
             printf("\n%sDisconnected from server!%s\n",CLR_RED,CLR_NRM);
            send(sock , command , strlen(command) , 0 );
            break;
        }
        if(!strncmp(command,"get",3))
        {
            char *token=strtok(command," ");
            
            if(strcmp(token, "get"))
            {
                printf("\n%sCommand not Recognised,try again !%s\n",CLR_RED,CLR_NRM);
                continue;
            }

            token=strtok(NULL," ");
            
            if(token==NULL)
            {
                printf("\n%sFile name not found,try again !%s\n",CLR_RED,CLR_NRM);
                continue;
            }
            while(token!=NULL)
            {
                //use pid and open file in next line incase u want to share fd with server
                //but that works only on a local machine
                //int fd_write =open(token, O_CREAT | O_WRONLY | O_TRUNC, 0600);
                //values like pid,fd_write are ignored in client and server ,consider those if u want to share fd
                int fd_write=99;
                int pid=getpid();
                //printf("pid : %d\n",pid);
                char str[1024];
                sprintf(str,"%s/%d/%d",token,fd_write,pid);
                //printf("%s\n",str);
                send(sock , str , strlen(str) , 0 );  // send the message to server.
                //printf("message sent: %s\n",token);
                //printf("MESSAGE RECIEVED FROM SERVER : \n");
                size_t f=0,buff,pos=0,file_size=0;
                char strr[200];
                while(1)
                {
                    if(f==0)
                    {
                        memset(&buffer,0,sizeof(buffer));
                        valread = read( sock , buffer, 15);
                        if(!strcmp(buffer,"FileDoesntExist"))
                        {   
                            printf("\n%sFile ( %s ) could not be fetched, check file name and try again%s\n\n",CLR_RED,token,CLR_NRM); 
                            break;
                        }
                        memset(&buffer,0,sizeof(buffer));
                        valread = read( sock , buffer, 1024);
                        buff=atoi(buffer);

                        memset(&buffer,0,sizeof(buffer));
                        valread = read( sock , buffer, 1024);
                        file_size=atoll(buffer);

                        
                        fd_write =open(token, O_CREAT | O_WRONLY | O_TRUNC, 0600);

                        printf("\n");
                        f=1;

                        write(1, "                      [.............................]", 53); // Visualised progress bar
                        temp=0.0;
                        count=0;
                        c1=0;
                        i=0;
                    }
                    char *read_buff = (char *)malloc(buff);

                    valread = read( sock , read_buff, buff);// receive message back from server, into the buffer
                    
                    if(valread < 0) 
                        break;
                        
                    write(fd_write , read_buff , buff);
                    //printf("%s",read_buff);
                    
                    pos=pos+valread;
                    lseek(fd_write,pos,SEEK_SET);
                    
                    

                    //progress bar:
                    temp = (float)pos / file_size * 100;
                    sprintf(strr, "\rDownloading File: %6.2f%% [", temp);
                    write(1, strr, strlen(strr));
                    if ((int)temp - count == 4)
                    {
                        count = count + 4;
                        ++c1;
                    }
                    for (i = 0; i < c1; i++)
                        write(1, ">>", 1);

                    fflush(stdout);

                    //terminating condition
                    if(lseek(fd_write,0,SEEK_CUR)==file_size)
                    {
                        ftruncate(fd_write,pos);
                        printf("\n%sFile Downloaded(%s) %s\n\n",CLR_GRN,token,CLR_NRM);
                        fflush(stdout);
                        break;
                    }
                    
                }
                close(fd_write);
                token=strtok(NULL," ");
            }
        }
        else
        {
            printf("\n%sCommand not Recognised,try again !%s\n",CLR_MAG,CLR_NRM);
        }
        
    }

    return 0;
}