
/*
** client.c -- 一个 stream socket client 的 demo
*/
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <atomic>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "Socket.hh"
#include "threadpool.hh"
#include <csignal>
#define PORT "3490"      // Client 所要连接的 port
#define MAXDATASIZE 100  // 我们一次可以收到的最大字节数量（number of bytes）
//与服务器取得连接，返回描述符


void SigHandler(int i) {//NOLINT
 
	printf( "hello world! \n " );
}
 
/*主函数*/
int main() {
	signal( SIGALRM, SigHandler );
	alarm( 5 );
	for (int i = 0; i < 8; i++ )
	{
		printf( " sleep % d ... \n ", i );
		sleep( 1 );
	}
	return 0;
}
