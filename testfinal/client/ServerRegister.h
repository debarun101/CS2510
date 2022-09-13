/*
 * ClientTCPConnector.h
 *
 *  Created on: 2015年9月26日
 *      Author: fan
 */

#ifndef SRC_CLIENTTCPCONNECTOR_H_
#include <string>
#define SRC_CLIENTTCPCONNECTOR_H_
using namespace std;
class ServerRegister {
public:
	ServerRegister();
	int registerServer(string program ,string version,string procedure, string functions);
	virtual ~ServerRegister();
};

#endif /* SRC_CLIENTTCPCONNECTOR_H_ */
