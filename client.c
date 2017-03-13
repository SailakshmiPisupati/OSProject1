#include <stdio.h> 
#include <string.h>   
#include <sys/socket.h>    
#include <arpa/inet.h> 

struct transactions{
   int timestamp;
   int account_number;
   char transaction_type;
   int amount;
};

struct messages{
  char transaction_queries[20];
};

char* file_to_read;

int get_file_count(){
   FILE *get_records;
   int transaction_count=1;
   get_records =fopen(file_to_read,"r");
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
  //float wait_sec = atof(argv[3]);
  int wait_sec = 10;
  file_to_read = argv[4];
  short port_number = 8043;
  // short port_number = argv[2];
  // if(file_to_read == NULL){
  //   strcpy(file_to_read,"Transactions.txt");
  // }

  // if(port_number == NULL){
  //   port_number = 8043;
  // }

  printf("file_to_read %s\n", file_to_read);
  
  int sock;
  struct sockaddr_in server;
  char message[20] , server_reply[2000];
   
   int transaction_count = get_file_count();

   FILE *get_records;
   int a,bal_amt,no;
   char type;
  
   int count=0;

   get_records =fopen(file_to_read,"r");
   if(!get_records){
      printf("Unable to open file\n");
   }
   printf("Number of accounts in file are : %d\n", transaction_count);

   struct transactions transaction_each[transaction_count];    //for reacding each space sepearted value
   //struct messages transac[transaction_count];

   while(fscanf(get_records,"%d %d %c %d",&a,&no,&type,&bal_amt)!= EOF){
      transaction_each[count].timestamp = a;
      transaction_each[count].account_number = no;
      transaction_each[count].transaction_type = type;
      transaction_each[count].amount = bal_amt;
      count++;
   }

   fclose(get_records);

   int i;
   for(i=0;i<transaction_count;i++){
      printf("The Transation details are\n");
      printf("Transation time: %d\n", transaction_each[i].timestamp);
      printf("Transation account number: %d\n", transaction_each[i].account_number);
      printf("Transation type : %c\n", transaction_each[i].transaction_type);
      printf("Transation balance amount : %d\n", transaction_each[i].amount);
   }

   printf("Online Banking Application\n");
   printf("Number of clients connecting to the server are: %d\n",transaction_count);
    count = 0;
    
     //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(port_number);
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
   get_records =fopen(file_to_read,"r");
   while(fgets(message,20,get_records)){
    int next_time, current_time, wait_time;
    printf("Transaction count and count value:%d %d \n",(transaction_count-1),count );
   if(count < transaction_count-1){
       current_time = transaction_each[count].timestamp;
       next_time = transaction_each[count+1].timestamp;
       printf("Transaction count and count value:%d %d \n",current_time,next_time );
       wait_time = (next_time - current_time)* wait_sec;
       printf("Wait time is : %d\n", wait_time);
    }
    count++;
   

      printf("Message %s\n",message );
      if(send(sock,&message,sizeof(message),0)<0){
         printf("Unable to connect to server. Please try again later");
      return 1;
       }

       
      if( recv(sock , server_reply , 2000 , 0) < 0)
      {
         printf("recv failed");
      }
      printf("Server reply %s:",server_reply);
      if(count<=transaction_count){
         printf("Wait will start now\n");
         sleep(wait_time);
         printf("Wait ends now\n");
      }
     
   }

   fclose(get_records);     
    close(sock);
    return 0;
}