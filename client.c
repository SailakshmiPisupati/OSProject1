#include<stdio.h> 
#include<string.h>   
#include<sys/socket.h>    
#include<arpa/inet.h> 

struct transactions{
   int timestamp;
   int account_number;
   char transaction_type;
   int amount;
};

struct messages{
  char transaction_queries[20];
};


int get_file_count(){
   FILE *get_records;
   int transaction_count=1;
   get_records =fopen("Transactions.txt","r");
   if(!get_records){
      printf("Unable to open file\n");
   }

   //Get the total number of lines i.e. accounts in the file.
   int ch=0;
   while(!feof(get_records)){
      ch = fgetc(get_records);
      if(ch=='\n'){
         transaction_count++;
      }
   }
   fclose(get_records);
   return transaction_count;
}
 
int main(int argc , char *argv[])
{
  int sock;
  struct sockaddr_in server;
  char message[20] , server_reply[2000];
   
   int transaction_count = get_file_count();

   FILE *get_records;
   int a,bal_amt,no;
   char type;
  
   int count=0;

   get_records =fopen("Transactions.txt","r");
   if(!get_records){
      printf("Unable to open file\n");
   }
   printf("Number of accounts in file are : %d\n", transaction_count);

   //struct transactions transaction_each[transaction_count];    //for reacding each space sepearted value
   struct messages transac[transaction_count];

   // while(fscanf(get_records,"%d %d %c %d",&a,&no,&type,&bal_amt)!= EOF){
   //    printf("inside loop\n");
   //    printf("%d %d %c %d \n", a ,no, type,bal_amt);
   //    //message = (char*)a + (char*)no + type + (char*)bal_amt;
   //    //printf("Message is %s\n",message );
   //    transaction_each[count].timestamp = a;
   //    transaction_each[count].account_number = no;
   //    transaction_each[count].transaction_type = type;
   //    transaction_each[count].amount = bal_amt;
   //    count++;
   // }

   // fclose(get_records);

   // int i;
   // for(i=0;i<transaction_count;i++){
   //    printf("The Transation details are\n");
   //    printf("Transation time: %d\n", transaction_each[i].timestamp);
   //    printf("Transation account number: %d\n", transaction_each[i].account_number);
   //    printf("Transation type : %c\n", transaction_each[i].transaction_type);
   //    printf("Transation balance amount : %d\n", transaction_each[i].amount);
   // }

   printf("Online Banking Application\n");
   printf("Number of clients connecting to the server are: %d\n",transaction_count);

   //int send_count = htonl(transaction_count);

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

   while(fgets(message,20,get_records)){
      printf("Message %s\n",message );
      // transac[count].transaction_queries = &message;
      // count++;
      if(send(sock,&message,sizeof(message),0)<0){
         printf("Unable to connect to server. Please try again later");
      return 1;
       }

      if( recv(sock , server_reply , 2000 , 0) < 0)
      {
         printf("recv failed");
      }
      printf("Server reply %s:",server_reply);
      
      
   }
  
   fclose(get_records);

   // while(count ! = 0){
   //    message = transac[count].transaction_queries;
   //    if(send(sock,&message,sizeof(message),0)<0){
   //       printf("Unable to connect to server. Please try again later");
   //    return 1;
   //     }

   //    if( recv(sock , server_reply , 2000 , 0) < 0)
   //    {
   //       printf("recv failed");
   //    }
   //    count--;
   // }
   // while(count!=0){
   //    if(send(sock,&message,sizeof(message),0)<0){
   //       printf("Unable to connect to server. Please try again later");
   //    return 1;
   //     }

   //    if( recv(sock , server_reply , 2000 , 0) < 0)
   //    {
   //       printf("recv failed");
   //    }
   // }
   

 
   //strcmp(exit_parameter,'y')==1
  
 

    //keep communicating with server
    // while(1)
    // {
    //     // printf("Enter message : ");
    //     // scanf("%s" , message);
         
    //     //Send some data
    //     if( send(sock , message , strlen(message) , 0) < 0)
    //     {
    //         printf("Send failed");
    //         return 1;
    //     }
         
    //     //Receive a reply from the server
    //     if( recv(sock , server_reply , 2000 , 0) < 0)
    //     {
    //         printf("recv failed");
    //         break;
    //     }
         
    // printf("Server reply :");
    // printf(server_reply);
    // }
     
    close(sock);
    return 0;
}