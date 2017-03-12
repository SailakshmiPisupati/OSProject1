#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>   
#include <pthread.h>
#include <stdbool.h>

void *transaction_processor(void *);

struct account_details{
   int account_number;
   char name[20];
   int balance_amount;
   int connection_status;
};
struct account_details bank_accounts[2];
int account_count=1;

int get_file_count(){
   FILE *get_records;
   
   get_records =fopen("Records.txt","r");
   if(!get_records){
      printf("Unable to open file\n");
   }

   //Get the total number of lines i.e. accounts in the file.
   int ch=0;
   while(!feof(get_records)){
      ch = fgetc(get_records);
      if(ch=='\n'){
         account_count++;
      }
   }
   fclose(get_records);
   return account_count;
}

int main(int argc , char *argv[])
{
   int socket_desc , c , *new_sock;
   int client_sock;
   struct sockaddr_in server , client;
   
   int account_count= get_file_count();

   FILE *get_records;
   int a,bal_amt;
   char b[20];
   int count=0;

   get_records =fopen("Records.txt","r");
   printf("Number of accounts in file are :%d\n", account_count);
   
   while(fscanf(get_records,"%d %s %d",&a,b,&bal_amt)!= EOF){
      bank_accounts[count].account_number = a;
      strcpy(bank_accounts[count].name, b);
      bank_accounts[count].balance_amount = bal_amt;
      count++;
   }

   fclose(get_records);
   int i;
   for(i=0;i<account_count;i++){
      printf("The account details are\n");
      printf("Account number: %d\n", bank_accounts[i].account_number);
      printf("Account name: %s\n", bank_accounts[i].name);
      printf("Account balance amount: %d\n", bank_accounts[i].balance_amount);
   }
   
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket\n");
    }
    printf("Server up and running....\n");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    printf("bind done\n");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    printf("Waiting for clients...\n");
    c = sizeof(struct sockaddr_in);
     
     
    //Accept and incoming connection
   
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        printf("New client connected.\n");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  transaction_processor , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        //printf("Assigned new thread connection");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;

}

int get_account_info(char *account_no) {
  int i;
  // char x = &account_no;
  int x = atoi(account_no);
  printf("%d", x);
  for(i=0;i<account_count;i++){
    printf("Account details: %d\n",bank_accounts[i].account_number);
    // printf("Account details from client : %d \n",*(*int)account_no);
    if(bank_accounts[i].account_number==x){
       if(bank_accounts[i].connection_status == 0){
          bank_accounts[i].connection_status=1;
          return bank_accounts[i].balance_amount;
       }
       else{
          printf("Account in use. try again later.\n");
        }  
    }
    else{
      printf("Unable to find account details\n");
   }
  }
}

bool perform_transaction(char *account_no,char *transaction_type,char *transation_amount){
  printf("Records recieved at perform_transaction are:\n");
  printf("Account no: %s\n", account_no);
  printf("Transaction type: %s\n", transaction_type);
  printf("transaction number: %s\n", transation_amount);

  int account_balance = 0;
  account_balance = get_account_info(account_no);
  int itransaction_amt = atoi(transation_amount);

  switch(*(char*)transaction_type){
    case 'w':
      printf("Performing Withdrawal\n");
      if((account_balance - itransaction_amt)> 0){
        account_balance = account_balance - itransaction_amt;
        printf("Account balance after withdrawal is: %d\n",account_balance);
        //TODO write back to file
        return true;  
      }
      else{
        return false;
      }
      
      printf("Amount after withdrawing :%d\n",account_balance);
      //withdraw(account_no,balance_amount
      break;
    case 'd':
      printf("Performing Deposit\n");
      //withdraw(account_no,balance_amount);
       account_balance = account_balance + itransaction_amt;
        printf("Account balance after deposit is: %d\n",account_balance);
        return true;
        //TODO write back to file
      break;
  }

}

// bool check_if_account_exist(char client_message){
//    for(int i=0;i<account_count;i++){
//       if()
//    }
// }

/*
 * This will handle connection for each client
 * */
void *transaction_processor(void *socket_desc)
{
    //Get the socket descriptor
   int sock = *(int*)socket_desc;
   int read_size;
   char *message , client_message[2000];
  
   while((read_size = recv(sock,client_message,2000,0))>0){
    printf("Message from client %s\n",client_message );

    char *timestamp,*account_no,*transaction_type,*transation_amount;

    printf("Splitting string %s\n", client_message);

    timestamp = strtok(client_message," ");
    account_no = strtok(NULL," ");
    transaction_type = strtok(NULL," ");
    transation_amount = strtok(NULL," ");

    bool status = perform_transaction(account_no,transaction_type,transation_amount);
    if(status == true){
      message ="Transaction successful.";
      send(sock,&message,sizeof(message),0);
    }
    else{
      message ="Transaction unsuccessful. Please try again later.";
      send(sock,&message,sizeof(message),0);
    }

    if(read_size == 0)
    {
        printf("Client disconnected\n");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
       }  
    //Free the socket pointer
    free(socket_desc);
    
    return 0;
}