# Centralized banking application

## Problem Statement
Developed a Centralized Banking Application to perform the deposit and withdrawal of money functionality. Maintained atomicity and evaluated the system scalability upto 100 clients and performance analysis for different client count and request interval.

## Overall Program Design
### Server module:
The server can connect to multiple client and process the request. Steps followed by the server are:
Server opens the connection for connecting to multiple clients.
Server reads the Records.txt file and loads the values to a struct array which contains the account details.
A thread has been used in order to accept the incoming messages and process the request.
When the client pings the server checks if the account is present in the record set and if present, will fetch the current account balance to perform the withdrawal or deposit operation.
In order, to ensure correctness of the data, withdrawal will not be performed if the requested amount is greater than the current account balance. This is notified to the client.
On successfully processing the request, a “Transaction successful” message is sent to the client; in case of error, the same is addressed.
Also, in order to ensure atomicity, locks have been used in order to ensure that the account currently performing an operation is not invoked by any other client. In case, there is a scenario, a message that “Account already in use. Please try again later” is sent to the client.

### Client module:
The client can ping the server and performs the following steps.
The client receives a few parameters as command line arguments such as.
Server IP
Server port
Request interval
Text file to read
The client connects to the given IP and port. The client also reads the text file and loads the values to the struct.
It then sends out messages to the server, and captures the time when it sends a request. Similarly, and end time for each transaction on completion of a request. This time is then used to compute the time taken for each transaction.
An average is computed for all the transactions that have been sent by that client. Using the average time taken, the scalability values have been computed.
Also, for the purpose of testing the scalability, the client logs these values to a log.tx

## Design Tradeoffs considered and made

#### How Multiple threads and synchronization has been handled
A thread is used at the server side in order to perform the operations. Thus, multiple clients can connect and multiple transactions can take place.
Locks have been put when the updated value is written back account information, in order to ensure correctness of the data and that no other request will be processing on that particular account.
Also, since locks have been put at the specific account level, another transaction of different account will be performed on request.
#### Possible Improvements and extensions to your program
Updating interests on timed manner.
Sending out only account balance information in an ‘i’ request from the client.
Logging the server and client details for the timestamps

## Scalability measurement - Average time calculation
Procedure for performance measurement:
The client instances were run multiple times (i.e. 1,5,10,20,50 and 100). For this purpose, a shell script has been written which will run as a background process and perform the activities. The calculated average time for each of the client has been logged to the log.txt file. Then an average of the count of these are taken.
The following table below gives the average time for multiple instances of clients with a fixed request interval.

Refer to the design and implementation document for more info.
