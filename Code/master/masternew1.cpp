#include "RPCSocket.h"
#include "RPCSocket.cpp"
#include <iostream>
#include <cstdlib>
#include <list>
#include <map>
#include "DirectoryService.h"
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

const unsigned int RECEIVE_BUFFER_SIZE = 1024;
const unsigned short DIRECTORY_SERVICE_PORT = 2500;//Ports below 1024 can only be used by administrator.

DirectoryService::DirectoryService() {
	
}

DirectoryService::~DirectoryService() {
	
}

int worker_id = 0;
int worker_sel = 0;
unsigned long doc_id = 0;
struct workerDet2
{
	string address;
	unsigned short port;
};


struct workerDet
{
	string address;
	string port;
};

struct DocCount
{
	unsigned long count;
	string doc_id;
};

struct genResult
{
	unsigned long count;
	string result;

};

struct actualResult
{
	unsigned long count;
	string result;
	string docid;
};

typedef map<string, genResult> resultMap;

map<int, workerDet> workerList;

map <string, workerDet> DocList; 

//map <string, map<string, int> > InvertedIndex;

typedef map<string, map<string, unsigned long> > InvertedIndex;

typedef vector<actualResult> aRes;
InvertedIndex India;

bool Sort_by(const actualResult& a ,const actualResult& b)
{
	return a.count >= b.count;
}


void receiveFile()
{
	
}

void workerSetup(unsigned short dport)
{

	try
	{
		ServerSocket servSock(dport);
		ClientSocket *sock;
		while(1)
		{
			sock = servSock.accept();
			string callerAddress;
			try {
				callerAddress = sock->getPeerAddress() ;
				cout << "\n\nCaller's address:"<< callerAddress;
			} catch (SRPCSocketException e) {
				cerr << "Unable to get peer address" << endl;
			}
			try {
				unsigned short callerPort = sock->getPeerPort();
				//cout << "\t   Caller's port:" << callerPort;
			} catch (SRPCSocketException e) {
				cerr << "Unable to get peer port" << endl;
			}
			
			char echoBuffer[RECEIVE_BUFFER_SIZE];
			int recvMsgSize;
			string receivedData = "";
			while ((recvMsgSize = sock->receiveData(echoBuffer,RECEIVE_BUFFER_SIZE)) > 0) 
			{
				string rd(echoBuffer);
				//cout<<rd<<endl;
				receivedData = receivedData + rd.substr(0,recvMsgSize);
				int index = receivedData.find(":");
				string requestType = receivedData.substr(0,index) ;
				//cout << "Request Type:" << requestType << endl;
				
				if(requestType == "workerRegister")
				{
					//cout << "Received Data:" << receivedData << endl;
					int index2 = receivedData.find("Port_") + 5; //length of "Port_" is 5
					int length = receivedData.find("*") - index2;
					string port = receivedData.substr(index2,length);
					cout <<  "  Port:" << port << endl;				
					
					workerDet info;
					info.address = callerAddress;
					info.port = port;
					++worker_id;
					workerList[worker_id] = info;
					//cout<<"index: "<<worker_id<<" address: "<<workerList[worker_id].address<<"port: "<<workerList[worker_id].port<<endl;
					
					char * response = "success.";
					int len = strlen(response);
					sock->sendData(response, len);
				}
				else if(requestType == "Client")
				{
					//cout<< "Received Data"<<receivedData<<endl;
					int index = receivedData.find(":");
					int index2 = receivedData.find("*");
					int length = index2 - index;
					string request = receivedData.substr(index+1, length-1);
					//cout<<"\n Request: "<<request<<endl;
					
					if(request == "Index")
					{
						//cout<<"\nAgain  Request: "<<request<<endl;
						if(workerList.size() ==0)
						{
							char *response = "\nNo Worker Available At This Moment\n";
							int len = strlen(response);
							sock->sendData(response, len);
							break;
						}


						char frBuff[RECEIVE_BUFFER_SIZE];
						char *response = "sendFile";
						int len = strlen(response);
						sock->sendData(response, len);
						//Receiving File from client user
						int bytesReceived =0;
						
						char buffer1[64];
						++doc_id;
						snprintf(buffer1, 64, "%lu",doc_id);
						string temp1(buffer1);
						fstream myfile(temp1.c_str(),ios::out);
    						if(NULL == myfile)
    						{
        						printf("Error opening file");
        						//return 1;
    						}

						int count =0;
						while((bytesReceived = sock->receiveData(frBuff,RECEIVE_BUFFER_SIZE)) > 0)
    						{
							count++;
        						cout<<"Bytes received: "<<bytesReceived<<endl;    
        						myfile.write(frBuff, bytesReceived);
							memset (frBuff,0,RECEIVE_BUFFER_SIZE);
							if(bytesReceived < RECEIVE_BUFFER_SIZE)
								break;
							//cout<< "written";
       		
    						}
						//cout<<"hello there";
						myfile.close();
						if(count == 0)
							break;
						
						//try
						//{
						string add = "";
						string pt = "";
						++worker_sel;
						//cout<<"size:"<<workerList.size();
						if(worker_sel <= workerList.size())
						{
							//cout<<"index:"<<worker_sel;
							add = workerList[worker_sel].address;
							pt = workerList[worker_sel].port;
							//cout<< "Address:"<<add<<":Port: "<<pt;
						}
						else if(worker_sel > workerList.size())
						{
							worker_sel =1;
							//cout<<"index:"<<worker_sel;
							add = workerList[worker_sel].address;
							pt = workerList[worker_sel].port;
							//cout<< "Address:"<<add<<":Port: "<<pt;
						}
						//cout<< "string Port"<<pt;
						unsigned short port = (unsigned short) strtoul(pt.c_str(), NULL, 0);
						//cout<<"Unsigned short port"<<port;
						

						try
						{
							ClientSocket sock1(add,port);
							
							char buffer[64];
							
							snprintf(buffer, 64, "%lu",doc_id);

							string temp(buffer);
							workerDet info1;
							info1.address = add;
							info1.port = pt;
							DocList[temp]=info1;
							//cout<<"\n\nEntered address:"<<info1.address<<" Entered Port: "<<info1.port<<"\n";
							

							string str = "Index:Docid_"+temp+"*"; 
							const char *registrationInfo = str.c_str();
							int echoStringLen = strlen(registrationInfo);

							sock1.sendData(registrationInfo, echoStringLen);
							

							char echoBuffer[RECEIVE_BUFFER_SIZE];
							int recvMsgSize = 0;
							string receivedData = "";
							while (true) 
							{
								//cout<<"Entered While: About to Receive"<<endl;
								recvMsgSize = sock1.receiveData(echoBuffer,RECEIVE_BUFFER_SIZE);
								cout<<"received message size: "<<recvMsgSize<<"\n";
								string rd(echoBuffer);
								receivedData = receivedData+rd.substr(0,recvMsgSize);
								cout<<"Received Data: "<<receivedData<<"\n";
								break;
							}
							
							//sending file to the worker for indexing
			
							//cout<<"\n sending file"<<endl;
							FILE *mfile;
							mfile = fopen(temp.c_str(),"rb");
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
		     							//printf("Sending \n");
									sock1.sendDataFile(buff, nread);
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
							char echo[RECEIVE_BUFFER_SIZE];
							recvMsgSize = 0;
							receivedData = "";
							
							while ((recvMsgSize =sock1.receiveData(echo,RECEIVE_BUFFER_SIZE))>0) 
							{
								//cout<<"Entered While: About to Receive"<<endl;
								//recvMsgSize = sock1.receiveData(echo,RECEIVE_BUFFER_SIZE);
								//cout<<
								cout<<"received message size:"<<recvMsgSize;
								string rd(echo);
								receivedData = receivedData+rd.substr(0,recvMsgSize);
								cout<<"Received Data:"<<receivedData;
								memset(echo, 0, RECEIVE_BUFFER_SIZE);
								if(recvMsgSize < RECEIVE_BUFFER_SIZE)
									break;
								//break;
							}
							
							cout<< "size of received Data:"<<receivedData.size()<<"\n";
							
							string send = receivedData;
							string words = "";
							string docd = "";
							for(int ij = 0; ij < send.size(); ij++)
							{
								if(send[ij] == ':')
								{
									//cout<<"\n word:"<<words;
									if(words == "Docid_")
									{
										words ="";
										int j;
										for( j = ij+1;send[j]!=':'; j++)
										{
											docd += send[j];
										}
										ij = j;
										//cout<<"\ndoc id:"<<docd;
									}
									else
									{
										string w= words;
										words = "";
										string count = "";
										int j;
										for( j = ij+1; send[j]!= ':'; j++)
										{
											count+= send[j];
										}
										ij = j;
										cout<<"\n Word:"<<w<<" Count:"<<count;
										unsigned long ct = (unsigned long) strtoul(count.c_str(), NULL, 0);
										/*DocCount dc;
										dc.count = ct;
										dc.doc_id = docd;*/
										//cout<<"\nIndia word:"<<w<<"\n";
										//cout<<"\nIndia doc id:"<<docd<<"\n";
										//cout<<"\nIndia count:"<<ct<<"\n";

										India[w][docd]+= ct;
				
				
									}
								}
								else
									words += send[ij];
							}
							cout<<"\nContents of India";

							for (InvertedIndex::iterator p = India.begin();p != India.end(); ++p) 
							{
								cout<<"\n word:"<< p-> first<<":";
								for(map<string, unsigned long>:: iterator it= p->second.begin(); it != p->second.end(); it++)
								{
									cout<<"doc id:"<<it->first<<":"<<"count:"<<it->second<<":";
								}

							}
							cout<<"\n";
							
							/*
							string str1 = "Successful"; 
							const char *registrationInfo1 = str1.c_str();
							int echoLen = strlen(registrationInfo1);

							sock1.sendData(registrationInfo1, echoLen);

							*/
							//delete sock1;
							
						}
						catch(SRPCSocketException &e)
						{
							cerr << e.info() << endl;
							exit(1);	
						}						
						
						
					}
					
					else if(request == "Query")
					{
						//cout<<"\nAgain  Request: "<<request<<endl;
						char frBuff[RECEIVE_BUFFER_SIZE];
						char *response = "sendQuery";
						int len = strlen(response);
						sock->sendData(response, len);

						char buffer[RECEIVE_BUFFER_SIZE];
						char echoBuffer[RECEIVE_BUFFER_SIZE];
						int recvMsgSize = 0;
						string receivedData = "";
						while ((recvMsgSize = sock->receiveData(echoBuffer,RECEIVE_BUFFER_SIZE)) > 0) 
						{
							//
							//cout<<"Entered While: About to Receive"<<endl;
							//recvMsgSize = sock->receiveData(echoBuffer,RECEIVE_BUFFER_SIZE);
							cout<<"received message size:"<<recvMsgSize<<"\n";
							string rd(echoBuffer);
							receivedData = receivedData+rd.substr(0,recvMsgSize);
							cout<<"Received Data:"<<receivedData<<"\n";
							if(recvMsgSize < RECEIVE_BUFFER_SIZE)
							{
								
								//changed portion
								receivedData += " ";
								resultMap rmap;
								string s ="";
								cout<<"\nsize:"<<receivedData.size()<<"\n";

								cout<<"\nContents of India\n";
							/*

								for (InvertedIndex::iterator p = India.begin();p != India.end(); ++p) 
								{
									cout<<"\n word:"<< p-> first<<":";
									for(map<string, unsigned long>:: iterator it= p->second.begin(); it != p->second.end(); it++)
									{
										cout<<"doc id:"<<it->first<<":"<<"count:"<<it->second<<":";
									}

								}*/
								cout<<"\n";
								//char* a[receivedData.size()];
								typedef vector<string> vstr;
								vstr v;
								vector<string>::iterator st;
									
								string pseudo="";
								for(int r = 0; r<receivedData.size(); r++)
								{
									if(isalpha(receivedData[r]) || isdigit(receivedData[r]))
									{
										s+=toupper(receivedData[r]);
									}
									else
									{
										//cout<<"char:"<<c<<"::";
										//cout << s<<"|";
			
										if(s != "")
										{
											int flag = 0;
											for(st=v.begin();st!=v.end(); st++)
											{
												if(s == *st)
													flag =1;	
											}
											if(flag ==0)
											{
												v.push_back(s);
												pseudo+= s+" ";
											}
										}
										s="";
			
			
									}
		
								}
								cout<<"pseudo: "<<pseudo;

								receivedData = pseudo;
								for(int r = 0; r< receivedData.size(); r++)
								{
									if(isalpha(receivedData[r]) || isdigit(receivedData[r]) )
									{
										//cout<<"entered alpha";
										s+=toupper(receivedData[r]);
										
									}

									else
									{
										//cout<<"char:"<<c<<"::";
										//cout << s<<"|";
										//cout<<"now time forward"; 
										if(s != "")
										{
											if ( India.find(s) == India.end() ) 
											{
												cout<<s;
												cout<<"word:"<<s<<"not found:";
												s="";
  												// not found
												continue;
											} 
											else 
											{
												cout<<"word "<<s<<" found\n";
												
												for(map<string, unsigned long>:: iterator it= India[s].begin(); it != India[s].end(); it++)
												{
													cout<<" matched doc id:"<<it->first<<":"<<"count:"<<it->second<<":";
													genResult g;
													cout<<"rmap initial count for"<<it->first<<rmap[it->first].count;
													rmap[it->first].count+= it->second;
													char buffer2[64];
								
													snprintf(buffer2, 64, "%lu",it->second);
													string ct1(buffer2);
													rmap[it->first].result += s+", Count: "+ct1+" : ";
													//rmap[it->first]=g;
													
												}
  												// found
												
											}
											
											
											//++words[s];
										}
										s="";
			
			
									}
									//cout<<"\ns:\n"<<s;
								}

								for(map<string, genResult>:: iterator it= rmap.begin(); it != rmap.end(); it++)
								{
									cout<<"\n\nDoc:"<<it->first<<":Total count:"<<it->second.count<<"Result:"<<it->second.result;

								}
								
								aRes vect;
								actualResult ar;
								for(map<string, genResult>:: iterator it= rmap.begin(); it != rmap.end(); it++)
								{
									ar.docid = it->first;
									ar.count = rmap[it->first].count;
									ar.result = rmap[it->first].result;
									vect.push_back(ar);

								}
								sort(vect.begin(),vect.end(),Sort_by);

								
								cout<<"\nsorted vector:\n";
								string result = "Result:\n";
								for(int i = 0; i< vect.size();i++)
								{
									cout<< "Doc id: "<<vect[i].docid<<", Total Count: "<<vect[i].count<<", Result: "<<vect[i].result;
									result = result +"Document name/id:"+ vect[i].docid +"\t"+ vect[i].result+"\n";

								}
								
								
								//changed portion
								sock->sendData(result.c_str(), strlen(result.c_str()));
								
								break;
							}
						}
						//string result = "result:";
						//sock->sendData(result.c_str(), strlen(result.c_str()));


					}
					else if(request == "Retrieve")
					{
						cout<<"\nAgain  Request: "<<request<<endl;
						char frBuff[RECEIVE_BUFFER_SIZE];
						char *response = "sendDocid";
						int len = strlen(response);
						sock->sendData(response, len);

						char buffer[RECEIVE_BUFFER_SIZE];
						char echoBuffer[RECEIVE_BUFFER_SIZE];
						int recvMsgSize = 0;
						string receivedData = "";
						while ((recvMsgSize = sock->receiveData(echoBuffer,RECEIVE_BUFFER_SIZE)) > 0) 
						{
							//
							//cout<<"Entered While: About to Receive"<<endl;
							//recvMsgSize = sock->receiveData(echoBuffer,RECEIVE_BUFFER_SIZE);
							cout<<"received message size:"<<recvMsgSize<<"\n";
							string rd(echoBuffer);
							receivedData = receivedData+rd.substr(0,recvMsgSize);
							cout<<"Received Data:"<<receivedData<<"\n";
							if(recvMsgSize < RECEIVE_BUFFER_SIZE)
							{
								//string result = "result:";
								//sock->sendData(result.c_str(), strlen(result.c_str()));
								
								break;
							}
						}
						
						cout<<"address: "<<DocList[receivedData].address<<" port: "<<DocList[receivedData].port;
						
						if(DocList[receivedData].address !="" && DocList[receivedData].port != "")
						{
							string result = "address_"+DocList[receivedData].address+":port_"+DocList[receivedData].port+"*";
							sock->sendData(result.c_str(), strlen(result.c_str()));
						}
						else
						{
							string result = "DNF";
							sock->sendData(result.c_str(), strlen(result.c_str()));
						}
											
					}

				}				
				
			}

			//delete sock;
			//break;
		}
	}
	catch(SRPCSocketException &e)
	{
		cerr << e.info() << endl;
		exit(1);	
	}

}

/*std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}
*/


int main(int argc, char *argv[])
{

	unsigned short dport = (unsigned short) strtoul(argv[1], NULL, 0);
	//cout<<"created socket6";
	//printf("%hu",dport);

	

	workerSetup(dport);
	return 0;
}


//void acceptUser()
//{
//	try
//	{
//		ServerSocket servSock(WORKER_SERVICE_PORT);
//		while(1)
//		{
//			ClientSocket *sock = servSock.accept();
			
//			char echoBuffer[RECEIVE_BUFFER_SIZE];
//			int recvMsgSize;
//			string receivedData = "";
//			while ((recvMsgSize = sock->receiveData(echoBuffer,RECEIVE_BUFFER_SIZE)) > 0) 
//			{			
//				
//			}
//		
//			delete(sock);
//			break;
//		}
		
		
//	}
//	catch (SRPCSocketException &e) 
//	{
//			cerr << e.info() << endl;
//			exit(1);
//	}
//}


/*if(DocList[receivedData].address !="" || DocList[receivedData].port != 0)
						{
							try
							{
								ClientSocket sock2(DocList[receivedData].address,DocList[receivedData].port);
								string str = "Retrieve:Docid_"+receivedData+"*"; 
								const char *registrationInfo = str.c_str();
								int echoStringLen = strlen(registrationInfo);

								sock2.sendData(registrationInfo, echoStringLen);
								
							}
							catch(SRPCSocketException &e)
							{
								cerr << e.info() << endl;
								exit(1);	
							}					

							sock->sendData(result.c_str(), strlen(result.c_str()));							


						}
						else
						{
							result="Document not found!";							
							sock->sendData(result.c_str(), strlen(result.c_str()));							
						}*/
