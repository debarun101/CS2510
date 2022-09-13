Compiling and running the master node:

1) cd testfinal/master
2) g++ masternew.cpp -o m
3) ./m <port no>
For example, ./m 2000

Compiling and running the worker node:

1) cd testfinal/worker1
2) g++ workernew.cpp -o w1
3) ./w1 <worker_port> <master node's IP address> <Master Node's port>

eg: ./w1 3000 136.142.227.10 2000


Compiling and running the client:

1) cd testfinal/client
2) g++ clientnew.cpp -o c
3) ./c <master node's IP address> <Master Node's port>

For example: ./c 136.142.227.10 2000
