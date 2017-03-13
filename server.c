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
   pthread_mutex_t lock_account;
};
struct account_details bank_accounts[2];
int account_count=1;
 pthread_t calculate_interest;

char message[2000];

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

void *update_interest(){
  int rate_of_interest = 8.2F;
  int i,interest,number_of_years =1,account_balance;
  printf("\n");
  printf("********************Server is now calculating the interest**************\n");
  for(i=0;i<account_count;i++){

    account_balance = bank_accounts[i].balance_amount;
    interest = (int) (account_balance * number_of_years * rate_of_interest) /100;
    account_balance = account_balance + interest;
    bank_accounts[i].balance_amount = account_balance;
    printf("Account number %d has been credited with interest of %d\n",bank_accounts[i].account_number,interest);
    printf("Balance after deposit for account number %d is %d\n", bank_accounts[i].account_number,bank_accounts[i].balance_amount);
  }
  pthread_join( calculate_interest , NULL);
}

int main(int argc , char *argv[])
{
   int socket_desc , c , *new_sock;
   int client_sock;
   struct sockaddr_in server , client;
   
   account_count= get_file_count();

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
    
   

    if(pthread_create(&calculate_interest,NULL, update_interest,(void*) new_sock)< 0){
      perror("could not create thread.");
    }
     
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
       // pthread_join( sniffer_thread , NULL);
        //printf("Assigned new thread connection");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;

}

// int get_account_info(char *account_no) {
//   int i;
//   // char x = &account_no;
//   int x = atoi(account_no);
//   printf("%d", x);
//   for(i=0;i<account_count;i++){
//     printf("Account details: %d\n",bank_accounts[i].account_number);
//     // printf("Account details from client : %d \n",*(*int)account_no);
//     if(bank_accounts[i].account_number==x){
//        if(bank_accounts[i].connection_status == 0){
//           bank_accounts[i].connection_status=1;
//           //pthread_mutex_lock(&lock_account);
//           return bank_accounts[i].balance_amount;
//        }
//        else{
//           printf("Account in use. try again later.\n");
//         }  
//     }
//     else{
//       printf("Unable to find account details\n");
//    }
//   }
// }

bool perform_transaction(char *account_no,char *transaction_type,char *transation_amount){
  // printf("Records recieved at perform_transaction are:\n");
  // printf("Account no: %s\n", account_no);
  // printf("Transaction type: %s\n", transaction_type);
  // printf("transaction number: %s\n", transation_amount);

  int account_balance = 0;
  int itransaction_amt = atoi(transation_amount);
  //account_balance = get_account_info(account_no);

  int i;
  // char x = &account_no;
  int x = atoi(account_no);
  
  for(i=0;i<account_count;i++){
    //printf("Account details: %d\n",bank_accounts[i].account_number);
    // printf("Account details from client : %d \n",*(*int)account_no);
    if(bank_accounts[i].account_number==x){
       if(bank_accounts[i].connection_status == 0){
          // bank_accounts[i].connection_status=1;
          
          account_balance = bank_accounts[i].balance_amount;
          printf("Account Number: %d Account balance: %d \n", x ,account_balance);
          switch(*(char*)transaction_type){
          case 'w':
            printf("*************************************************\n");
            printf("Performing Withdrawal for account number %d\n",x);
            
            if((account_balance - itransaction_amt)> 0){
              account_balance = account_balance - itransaction_amt;
              
              
              printf("i value is %d\n", i);

              pthread_mutex_lock(&bank_accounts[i].lock_account);
             // pthread_mutex_lock(&lock_account);
              wait(100);
              bank_accounts[i].balance_amount = account_balance;
              pthread_mutex_unlock(&bank_accounts[i].lock_account);
              printf("Account balance after withdrawal is: %d\n",bank_accounts[i].balance_amount);
             // bank_accounts[i].connection_status = 0;
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
            printf("*************************************************\n");
            printf("Performing Deposit\n");
            //withdraw(account_no,balance_amount);
             account_balance = account_balance + itransaction_amt;
              
               printf("i value is %d\n", i);

              pthread_mutex_lock(&bank_accounts[i].lock_account);
             // pthread_mutex_lock(&lock_account);
             // sleep(100);
              bank_accounts[i].balance_amount = account_balance;
              pthread_mutex_unlock(&bank_accounts[i].lock_account);
              //bank_accounts[i].connection_status = 0;
              printf("Account balance after withdrawal is: %d\n",bank_accounts[i].balance_amount);
              strcpy(message,"Transaction successful. Account balance after withdrawal is: %d\n"+bank_accounts[i].balance_amount);


              return true;
              //TODO write back to file
            break;
        }
       }
       else{
          printf("Account in use. try again later.\n");
          strcpy(message,"Account in use. try again later.\n");
        }  
        break;
    }
    else{
      printf("Unable to find account details\n");
   }
  }
}

/*
 * This will handle connection for each client
 * */
void *transaction_processor(void *socket_desc)
{
    //Get the socket descriptor
   int sock = *(int*)socket_desc;
   int read_size;
   char client_message[2000];
  
   while((read_size = recv(sock,client_message,2000,0))>0){
    printf("Message from client %s\n",client_message );

    char *timestamp,*account_no,*transaction_type,*transation_amount;

    //printf("Splitting string %s\n", client_message);

    timestamp = strtok(client_message," ");
    account_no = strtok(NULL," ");
    transaction_type = strtok(NULL," ");
    transation_amount = strtok(NULL," ");

    bool status = perform_transaction(account_no,transaction_type,transation_amount);
    if(status == true){
      //strcpy(message,"Transaction successful");
     //&message ="Transaction successful.";
      send(sock,&message,sizeof(message),0);
    }
    else{
      //&message ="Transaction unsuccessful. Please try again later.";
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