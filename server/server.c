#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sendfile.h>

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

//Custom Log_base10 function:
#define LN10 2.3025850929940456840179914546844
double ln(double x)
{
    double sum_cache = 0.0,
    temp_var = (x - 1) / (x + 1),
    temp_var2 = temp_var * temp_var,
    denominator = 1.0,
    fraction = temp_var,
    term = fraction,
    sum = term;

    while (sum != sum_cache)
    {
        sum_cache = sum;
        denominator += 2.0;
        fraction *= temp_var2;
        sum += fraction / denominator;
    }
    return 2.0 * sum;
}
double log_new(double x)
{
    return ln(x) / LN10;
}
//Custom Power Function:
int pow_new(int base, int exp)
{
    return (exp != 0) ? (base * pow_new(base, exp - 1)) : 1;
}


int send_client(int,char *);
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char buffer1[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("\n\n\t\t\tXXX========%sSERVER NANCE%s========XXX\n",CLR_YEL,CLR_NRM);
    printf("\n%sWaiting for Client %s\n",CLR_RED,CLR_NRM);
    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                    (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("\n%sClient connected with SERVER NANCE , waiting for file request.. %s\n\n",CLR_GRN,CLR_NRM);
        

    // while(strcmp(buffer,"exit"))
    // {
    //     valread = read(new_socket , buffer, 1024); // read infromation received into the buffer
    //     printf("Message recived : %s\n",buffer);
    //     //send_client(new_socket,buffer);
    // }


    while(strcmp(buffer, "exit")!=0)
    {   
        char fname[1024],fd[1000],pid[1000];             
        memset(buffer, 0, 1024);
        valread = read(new_socket , buffer, 1024);  // read infromation received into the buffer
        //printf("Client sent: %s, length :%ld\n",buffer, strlen(buffer));
        
        if(!strcmp(buffer,"exit"))
        {
            break;
        }

        //Use the below section of code to use shared fd
            //======================================
            char *token=strtok(buffer,"/");
            strcpy(fname,token);
            //printf("fname :%s\n",fname);
            token=strtok(NULL,"/");
            
            strcpy(fd,token);
            int write_fd=atoi(fd);
            //printf("fd:%d\n",write_fd);

            token=strtok(NULL,"/");
            strcpy(pid,token);
            int pidt=atoi(pid);
            //printf("pid:%d\n",pidt);
            //send_client(new_socket,fname,write_fd,pidt);
            
            //=========================================
        
        
        send_client(new_socket,fname);
        printf("\n%sWaiting for command...%s\n",CLR_YEL,CLR_NRM);
    }

    printf("%s\nClient Disconnected%s\n",CLR_RED,CLR_NRM);

    // send(new_socket , hello , strlen(hello) , 0 );  // use sendto() and recvfrom() for DGRAM
    // printf("message sent to client\n");
    return 0;
}


int send_client(int new_socket,char *fname)
{
    //char fd_path[64];  // actual maximal length: 37 for 64bit systems
    //snprintf(fd_path, sizeof(fd_path), "/proc/%d/fd/%d", pid, fd);
    //printf("%s\n",fd_path);
    //int file_write = open(fd_path, O_RDWR);

    
    float temp=0.0;
    int count=0,c1=0,i=0;
    
    int file_read= open(fname,O_RDONLY);
    struct stat stat_buf;
    off_t offset = 0;
    fstat (file_read, &stat_buf);
    int file_size = lseek(file_read, 0, SEEK_END);
    if(file_read<0 || file_size<=0)
    {
        write(new_socket , "FileDoesntExist" , 15 );
        if(file_read>=0 && file_size<=0)
        {
            printf("%sFile ( %s ) is empty,request declined!%s\n",CLR_RED,fname,CLR_NRM);
            return 1;
        }
        printf("%sRequested File ( %s ) not found%s\n",CLR_RED,fname,CLR_NRM);
        perror("fopen");
        return 1;
    }
    else 
        write(new_socket,"Good2godudeeee",15);
    
    
    int chunk = file_size/500;
    size_t buffer_size = (chunk == 0) ? 1 : pow_new(2, (int)(log_new((double)(chunk)) / log_new(2.0)));
    
    char *read_buff, *write_buff;
    read_buff = (char *)malloc(buffer_size);
    write_buff = (char *)malloc(buffer_size);

    int index = ((file_size - 1) / buffer_size) * buffer_size;
    size_t new_chunksize = buffer_size;
    lseek(file_read, 0, SEEK_SET);

    char str[200];
    int tt=new_chunksize;

    //send buffer size to client
    sprintf(read_buff,"%ld",buffer_size);
    write(new_socket, read_buff, 1024);

    //send file size to client
    sprintf(read_buff,"%d",file_size);
    write(new_socket, read_buff, 1024);

    write(1, "                  [.............................]", 49); // Visualised progress bar
    while (1)
    {

        read(file_read, read_buff, new_chunksize);

        write(new_socket, read_buff, new_chunksize);
        
        //calculating percentage of file copied
        temp = (float)tt / file_size * 100;
        sprintf(str, "\rSending File: %6.2f%% [", temp);
        write(1, str, strlen(str));
        //implementing progress bar with 25 progress character(>>) ; each ">>" represents 4%
        if ((int)temp - count == 4)
        {
            count = count + 4;
            ++c1;
        }
        for (i = 0; i < c1; i++)
            write(1, ">>", 1);

        fflush(stdout);
        // terminating condition : are there any more chunks left to read ?
        if (lseek(file_read, 0, SEEK_CUR) >= file_size)
            break;
        //to handle last chunk:update decrease buffer size to remaining bytes
        if((file_size-lseek(file_read,0,SEEK_CUR)) < new_chunksize)
             new_chunksize=file_size-lseek(file_read,0,SEEK_CUR);
        tt = tt+new_chunksize;
    }
    fflush(stdout);
    printf("\n%sFile ( %s ) Sent to client !! %s\n",CLR_GRN,fname,CLR_NRM);
    //write(new_socket , "FileTransferred" , 15 );

    close(file_read);
    //close(file_write);
}