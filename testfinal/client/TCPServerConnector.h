/*
 * TCPServerConnector.h
 *
 *  Created on: 2015年9月26日
 *      Author: fanlingling
 */

#ifndef SRC_TCPSERVERCONNECTOR_H_
#define SRC_TCPSERVERCONNECTOR_H_

class TCPServerConnector {
public:
	TCPServerConnector(unsigned short SERVER_PORT);
	void setupServer(unsigned short SERVER_PORT);
	virtual ~TCPServerConnector();
};

#endif /* SRC_TCPSERVERCONNECTOR_H_ */
