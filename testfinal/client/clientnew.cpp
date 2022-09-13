#include "RPCSocket.h"
#include "RPCSocket.cpp"
#include "TCPServerConnector.h"
#include <iostream>
#include <cstdlib>
#include <list>
#include <map>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include "ServerRegister.h"
#include <limits>
#include <fstream>

using namespace std;

ServerRegister::ServerRegister() {
	
}

ServerRegister::~ServerRegister() {
	
}


const unsigned short DIRECTORY_SERVICE_PORT = 2500;
const string DIRECTORY_SERVICE_ADDRESS = "127.0.0.1";// 10.0.0.43
const unsigned int RECEIVE_BUFFER_SIZE = 1024;

void retrieve(string docid, string daddress, unsigned short dport)
{
	ClientSocket sock(daddress, dport);
	string Msg = "Client:Retrieve*";
	const char *Info = Msg.c_str();
	int echoStringLen = strlen(Info);
	sock.sendData(Info, echoStringLen);
	cout<<"Retrieve Request sent";

	char echoBuffer[RECEIVE_BUFFER_SIZE];
	int recvMsgSize = 0;
	string receivedData = "";

	while (true) 
	{
		cout<<"Entered While: About to Receive"<<endl;
		recvMsgSize = sock.receiveData(echoBuffer,RECEIVE_BUFFER_SIZE);
		cout<<"Received message size:"<<recvMsgSize;
		string rd(echoBuffer);
		receivedData = receivedData+rd.substr(0,recvMsgSize);
		cout<<"Received Data:"<<receivedData;
		if(recvMsgSize < RECEIVE_BUFFER_SIZE)
		{
			sock.sendData(docid.c_str(), strlen(docid.c_str()));
				
				
			break;
		}			
	}

	recvMsgSize =0;
	receivedData = "";
	char echo[RECEIVE_BUFFER_SIZE];
	while ((recvMsgSize = sock.receiveData(echo,RECEIVE_BUFFER_SIZE))>0) 
	{
		cout<<"Entered While: About to Receive"<<endl;
		
		cout<<"received message size:"<<recvMsgSize;
		string rd(echo);
		receivedData = receivedData+rd.substr(0,recvMsgSize);
		cout<<"Received Data"<<receivedData;
		memset(echo, 0,RECEIVE_BUFFER_SIZE); 

		if(recvMsgSize < RECEIVE_BUFFER_SIZE)
		{
			string sto = "Thanks.";
			//sock.sendData(q.c_str(), strlen(q.c_str()));
				
			break;
		}
					
	}
	//delete(&sock);
	
	if(receivedData != "DNF")
	{

		int index = receivedData.find("address_")+8;
		int index2 = receivedData.find(":");
		string addr = receivedData.substr(index, (index2-index));
		cout<<"Address: "<<addr;
		int index3 = receivedData.find("port_")+5;
		int index4 = receivedData.find("*");
		string port = receivedData.substr(index3, (index4-index3));
		cout<<"\n..port: "<<port;

		unsigned short pt = (unsigned short) strtoul(port.c_str(), NULL, 0);
		cout<<"\n Short port: "<<pt<<"\n";

		ClientSocket sock(addr, pt);
		string Msg1 = "Retrieve:Docid_"+docid+"*"; 
		const char *Info1 = Msg1.c_str();
		
		sock.sendData(Info1, strlen(Info1));
		cout<<"Retrieve Request sent";
		//Receiving The file

		fstream myfile(docid.c_str(),ios::out);
		char frBuff[RECEIVE_BUFFER_SIZE];
		int bytesReceived = 0;
    		if(NULL == myfile)
    		{
        		printf("Error opening file");
        		//return 1;
    		}
		while((bytesReceived = sock.receiveData(frBuff,RECEIVE_BUFFER_SIZE)) > 0)
    		{
        		cout<<"Bytes received: "<<bytesReceived<<endl;    
        		myfile.write(frBuff, bytesReceived);
			memset (frBuff,0,RECEIVE_BUFFER_SIZE);
			cout<< "written";
			
			if(bytesReceived < RECEIVE_BUFFER_SIZE)
				break;
       		
  		}
		cout<<"hello there";
		myfile.close();
	}
	else
		cout<<"\n\nResult: Document not found.\n";
		

	
	
	
}

void query(string q, string daddress, unsigned short dport)
{
	try
	{
		ClientSocket sock(daddress, dport);
		string Msg = "Client:Query*";
		const char *Info = Msg.c_str();
		int echoStringLen = strlen(Info);
		sock.sendData(Info, echoStringLen);
		cout<<"Query Request sent";

		char echoBuffer[RECEIVE_BUFFER_SIZE];
		int recvMsgSize = 0;
		string receivedData = "";
		while (true) 
		{
			cout<<"Entered While: About to Receive"<<endl;
			recvMsgSize = sock.receiveData(echoBuffer,RECEIVE_BUFFER_SIZE);
			cout<<"received message size:"<<recvMsgSize;
			string rd(echoBuffer);
			receivedData = receivedData+rd.substr(0,recvMsgSize);
			cout<<"Received Data:"<<receivedData;

			if(recvMsgSize < RECEIVE_BUFFER_SIZE)
			{
				sock.sendData(q.c_str(), strlen(q.c_str()));
				
				
				break;
			}			
		}

		recvMsgSize =0;
		receivedData = "";
		char echo[RECEIVE_BUFFER_SIZE];
		while ((recvMsgSize = sock.receiveData(echo,RECEIVE_BUFFER_SIZE))>0) 
		{
			cout<<"Entered While: About to Receive"<<endl;
			
			cout<<"received message size:"<<recvMsgSize;
			string rd(echo);
			receivedData = receivedData+rd.substr(0,recvMsgSize);
			cout<<"Received Data\n"<<receivedData;
			memset(echo, 0,RECEIVE_BUFFER_SIZE); 

			if(recvMsgSize < RECEIVE_BUFFER_SIZE)
			{
				string sto = "Thanks.";
				//sock.sendData(q.c_str(), strlen(q.c_str()));
				
				
				break;
			}
						
		}

		
		
	}
	catch (SRPCSocketException &e) 
	{
			cerr << e.info() << endl;
			exit(1);
	}
	
}



void index(string filename, string daddress, unsigned short dport)
{
	try
	{

		ClientSocket sock(daddress, dport);
		string Msg = "Client:Index*";
		const char *Info = Msg.c_str();
		int echoStringLen = strlen(Info);
		sock.sendData(Info, echoStringLen);
		cout<<"Index Request sent";
		
		char echoBuffer[RECEIVE_BUFFER_SIZE];
		int recvMsgSize = 0;
		string receivedData = "";
		while (true) 
		{
			//cout<<"Entered While: About to Receive"<<endl;
			recvMsgSize = sock.receiveData(echoBuffer,RECEIVE_BUFFER_SIZE);
			cout<<"received message size:"<<recvMsgSize<<"\n";
			string rd(echoBuffer);
			receivedData = receivedData+rd.substr(0,recvMsgSize);
			cout<<"\n Result :"<<receivedData<<"\n";
			break;
		}
			if(receivedData == "sendFile")
			{
				cout<<"\n sending file"<<endl;
				FILE *myfile;
				myfile = fopen(filename.c_str(),"rb");
				if(!myfile)
				{
					cout<<"No such file exists\n";
					return;
					
				}
				while(1)
				{
					cout<<" stuck inside";
			
					char buff[1024]={0};
					int nread = fread(buff,1,1024,myfile);
					cout<<"Bytes read: "<<nread<<"\n";  
		 			// If read was success, send data. 
					if(nread > 0)
					{
		     				printf("Sending \n");
						sock.sendDataFile(buff, nread);
		     				//send(connfd, buff, nread, 0);
					}

					if (nread < 1024)
					{
			    			if (feof(myfile))
			        			printf("End of file.. File successfully uploaded\n");
			     			if (ferror(myfile))
			        			printf("Error reading\n");
			     			break;
					}
			
		
				}

				fclose(myfile);
				
			}
			
			
		
		
			
	}
	catch (SRPCSocketException &e) 
	{
			cerr << e.info() << endl;
			exit(1);
	}

}
int main(int argc, char *argv[])
{

	string daddress = argv[1];

	unsigned short dport = (unsigned short) strtoul(argv[2], NULL, 0);

	while(1)
	{
		cout<<"\n\n ===================||tinyGOOGLE||==================\n\n";

		cout<<"	\n******What's In Your Mind?******\n";
		cout<<"\n Choose one of the three options below...\n\n";
		cout<<"1.. Upload a file (Press U) \n";
		cout<<"2.. Enter a Query (Press Q)\n";
		cout<<"3.. Retreive a file (Press R) \n\n";

		cout<<"\n....Press E to Exit....\n";
		cout<<"===========================================================\n";
		string input;

		getline(cin, input);
	
		if(input == "U" || input == "u")
		{
			cout<<"\n** Enter the no. of file you want to upload: ";
			int no;
			cin>>no;
			if (cin.fail()) {
   				cout<<"\nEnter a number.. Enter again"; //Not an int.
				cin.clear();
       				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        			std::cin >> no;
				//continue;
			}
			
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			int i =0;
			while(i<no)
			{
				cout<<"\n** Enter the filename you want to upload:";
				//char file[100];
				//gets(file);
				string file;
				//cin.clear();
	
				getline(cin,file);
				cout<<"filename:"<<file;
				if(file !="")			
					index(file, daddress, dport);
				i++;		
			}
		}
		else if(input == "Q" || input == "q")
		{
			cout<<"\n--Enter Query--\n";
			string q;
			getline(cin,q);
			query(q, daddress, dport);
		}
		else if(input == "R" || input == "r")
		{
			cout<<"\n-- Enter the Document Id you want to retrieve: ";
			string docid;
			getline(cin, docid);
			retrieve(docid, daddress, dport);
		}		

		else if(input == "E" || input == "e")
		{
				break;
		}
	}
	return 0;
}

