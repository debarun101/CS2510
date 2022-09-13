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
#include <fstream>
using namespace std;

const unsigned int RECEIVE_BUFFER_SIZE = 1024;
const unsigned short DIRECTORY_SERVICE_PORT = 2500;
const string DIRECTORY_SERVICE_ADDRESS = "127.0.0.1";// 10.0.0.43
const unsigned short WORKER_SERVICE_PORT = 1030;

ServerRegister::ServerRegister() {
	
}

ServerRegister::~ServerRegister() {
	
}

typedef map<string, int> wordCount;


void countWords(fstream& in, wordCount& words)
{
	char c;
	string s="";
  	while (in.get(c))
	{
		if(isalpha(c) || isdigit(c))
		{
			s+=toupper(c);
		}
		else
		{
			//cout<<"char:"<<c<<"::";
			//cout << s<<"|";
			
			if(s != "")
			{
				++words[s];
			}
			s="";
			
			
		}
		
    		
	}
}


void registerMaster(unsigned short inport, string dirAdd, unsigned short dirPort)
{
	try{
		//cout<<"about to connect";
		ClientSocket sock(dirAdd, dirPort);
		//cout<<"created socket";
		//string strPort = std::to_string(WORKER_SERVICE_PORT);
		char buffer[32];
		snprintf(buffer, 32, "%hu",inport);
		//cout<<"created socket2";
		string temp(buffer);
		string str = "workerRegister:Port_"+ temp+"*"; 
		const char *registrationInfo = str.c_str();
		int echoStringLen = strlen(registrationInfo);

		sock.sendData(registrationInfo, echoStringLen);
		
		char echoBuffer[RECEIVE_BUFFER_SIZE];
		int recvMsgSize = 0;

		string receivedData;
		//cout<<"created socket3";
		//receiving message from master node if it was successfully registered or not
		while (true) 
		{
			//cout<<"created socket4";
			recvMsgSize = sock.receiveData(echoBuffer,RECEIVE_BUFFER_SIZE);
			string rd(echoBuffer);
			receivedData = receivedData+rd.substr(0,recvMsgSize);
			if(receivedData=="success."){
				cout << "Successfully registered in the directory service. Return message1:" << receivedData <<"\n";
				//setupServer(SERVER_SERVICE_PORT);
				//return 1;
			}else{
				cout << "Failed to register in the directory service. Return message0:" << receivedData <<"\n";
				//return 0;
			}
			//Close the session regardless of the returned result.
			break;
		}
		//cout<<"created socket5";
		
	}
	catch (SRPCSocketException &e) 
	{
			cerr << e.info() << endl;
			exit(1);
	}
}

void respondMaster(unsigned short port)
{
	try
	{
		ServerSocket servSock(port);
		ClientSocket *sock;
		while(1)
		{
			//ServerSocket servSock(port);
			sock = servSock.accept();
			char echoBuffer[RECEIVE_BUFFER_SIZE];
			int recvMsgSize;
			string receivedData = "";
			//string docId = "";
			cout<<"I came here\n";
			while ((recvMsgSize = sock->receiveData(echoBuffer,RECEIVE_BUFFER_SIZE)) > 0) 
			{
				string rd(echoBuffer);
				cout<<rd<<"\n";
				receivedData = receivedData + rd.substr(0,recvMsgSize);
				int index = receivedData.find(":");
				string requestType = receivedData.substr(0,index) ;
				cout <<"Request Type:" << requestType << "\n"; 
				if(requestType == "Index")
				{
					cout << "Received Data:" << receivedData << endl;
					int index2 = receivedData.find("Docid_") + 6; //length of "Docid_" is 6
					int length = receivedData.find("*") - index2;
					string docId = receivedData.substr(index2,length);
					cout <<"doc id: "<< docId << endl;
					
					char * response = "sendFile";
					int len = strlen(response);
					sock->sendData(response, len);

					char frBuff[RECEIVE_BUFFER_SIZE];
					fstream myfile(docId.c_str(),ios::out);
    					if(NULL == myfile)
    					{
        					printf("Error opening file");
        					//return 1;
    					}
					int bytesReceived = 0;
					while((bytesReceived = sock->receiveData(frBuff,RECEIVE_BUFFER_SIZE)) > 0)
    					{
						
        					//cout<<"Bytes received: "<<bytesReceived<<endl;    
        					myfile.write(frBuff, bytesReceived);
						
						//cout<< "written";
						//cout<<"frbuff: "<<frBuff;
						memset (frBuff,0,RECEIVE_BUFFER_SIZE);
						if(bytesReceived < RECEIVE_BUFFER_SIZE)
						{
							char * response1 = "Indexed";
							int len1 = strlen(response1);
							//cout<<"yes";
							myfile.close();
							//sock->sendDataFile(response1, len1);

							fstream m2file(docId.c_str(),ios::in |ios:: out);
							if(NULL == m2file)
		    					{
								printf("Error opening file");
								//return 1;
		    					}
							//cout<<"hi there2";
							wordCount w;
							countWords(m2file, w);	
					
							string send = "Docid_:"+docId+":";
							//cout<<"\nsend"<<send;
							for (wordCount::iterator p = w.begin();p != w.end(); ++p) 
							{
						       		//cout << p->first << " occurred "<< p->second << " times.\n";
								send = send+ (p->first)+":";
								char buffer[32];
								snprintf(buffer, 32, "%d",p->second);
								string temp12(buffer);
								send = send + temp12+":";
		
							}
							
							cout<<"\n sent data"<<send<<"\n";
							m2file.close();
					
					
							//char url[send.length()+1];
						
							//strcpy(url, send.c_str());

							const char* url = send.c_str();
							int len = strlen(url);

							cout<<"\n\nlength of data to be sent: "<<len<<"\n";
							int offset =0;
							while(true)
							{
								cout<<"entered while true";
								if(len < 1024)
								{
									cout<<"len less than 1024\n send size actual: "<<len;
									sock->sendData((url+offset), len);
									break;
								}
								else
								{
									cout<<"length greater than 1024:"<<len;	
									sock->sendData((url+offset), 1024);
									offset = offset+1024;
									len = len - 1024;
								}
								
							}
							cout<<"\nbreaking\n";
							break;
						}
						      		
    					}
					cout<<"bytes received:"<<bytesReceived;
					//cout<<"hello there";
					
				}

				else if(requestType == "Retrieve")
				{
					cout << "Received Data:" << receivedData << endl;
					int index2 = receivedData.find("Docid_") + 6; //length of "Docid_" is 6
					int length = receivedData.find("*") - index2;
					string docId = receivedData.substr(index2,length);
					cout <<"doc id: "<< docId << endl;

					
					FILE *mfile;
					mfile = fopen(docId.c_str(),"rb");
					if(!mfile)
					{
						cout<<"could not open file:\n";
					
					}
					while(1)
					{
						//cout<<" stuck inside";
						char buff[1024]={0};
						int nread = fread(buff,1,1024,mfile);
						cout<<"Bytes read: "<<nread<<"\n";  
		 				// If read was success, send data. 
						if(nread > 0)
						{
		     					printf("Sending \n");
							sock->sendDataFile(buff, nread);
		     					//send(connfd, buff, nread, 0);
						}

						if (nread < 1024)
						{
			    				if (feof(mfile))
			        				printf("End of file\n");
			     				if (ferror(mfile))
			        				printf("Error reading\n");
			     				break;	
						}
					
					}

					fclose(mfile);

					
				}

				
			}
		
			//delete(sock);
			
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
	//cout<<"created socket5";
	unsigned short port = (unsigned short) strtoul(argv[1], NULL, 0);
//	cout<<"created socket6";
	printf("%hu",port);

	string daddress = argv[2];

	unsigned short dport = (unsigned short) strtoul(argv[3], NULL, 0);	
	registerMaster(port, daddress, dport);

	respondMaster(port);
	return 0;
}

/*int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 onm failure, 0 on success
} */
