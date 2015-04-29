#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <hiredis/hiredis.h>

#include "RedisConn.h"
#include <iostream>
#include <unistd.h>
#include <pthread.h>

#include "RedisPool.h"

//redisReply，我们需要通过判断它的type字段来知道返回了具体什么样的内容：
//REDIS_REPLY_STATUS 表示状态，内容通过str字段查看，字符串长度是len字段
//REDIS_REPLY_ERROR 表示出错，查看出错信息，如上的str,len字段

//例如　dbsize(返回　key 的个数),strlen xxkey(获取某个key 的值的长度)。这些返回都是　integer
//REDIS_REPLY_INTEGER 返回整数，从integer字段获取值.

//REDIS_REPLY_NIL 没有数据返回
//REDIS_REPLY_STRING 返回字符串，查看str,len字段
//REDIS_REPLY_ARRAY 返回一个数组，查看elements的值（数组个数），通过
//element[index]的方式访问数组元素，每个数组元素是一个redisReply对象的指针.
//
//int main( )
//{
//	RedisCpp::RedisConn con;
////	con.connect();
////	con.connect();
////	con.connect();
//	con.init( );
//	if( !con.connect( ) )
//	{
//		std::cout << "connect error: " << con.getErrorStr( ) << std::endl;
//	}
//	if ( !con.connect( "192.168.5.102", 6379, "521110", 5 ) )
//	{
//		std::cout << "connect error: " << con.getErrorStr( ) << std::endl;
//	}
//
//	if ( !con.auth( "521110" ) )
//	{
//		std::cout << "error: " << con.getErrorStr( ) << std::endl;
//	}
//	else
//	{
//		std::cout << "auth ok" << std::endl;
//	}
//
//	for ( int i = 0 ; i < 5 ; i++ )
//	{
//		sleep( 1 );
//
//		if ( !con.ping( ) )
//		{
//			std::cout << "ping error: " << con.getErrorStr( ) << std::endl;
//			std::cout << "reconnect!" << std::endl;
//			if ( !con.reconnect( ) )
//			{
//				std::cout << "reconnect error: " << con.getErrorStr( ) << std::endl;
//			}
//		}
//		else
//		{
//			std::cout << "ping ok" << std::endl;
//		}
//		uint64_t ret = 0;
//		if( !con.lpush("newTest", "test value", ret ) )
//		{
//			std::cout << "lpush error:    !!!!!!!!!!!" << con.getErrorStr() << std::endl;
//		}else
//		{
//			std::cout << "lpush ok  " << "value number:  " << ret << std::endl;
//		}
//		std::string value;
//		if( !con.lpop( "newTest", value ) )
//		{
//			std::cout << "pop error:  " << con.getErrorStr( ) << std::endl;
//		}else
//		{
//			std::cout << "pop ok:  " << value << std::endl;
//		}
//
//	}
//}
//

//int main( )
//{
//	RedisCpp::RedisConn con;
//	if ( !con.connect( "192.168.5.102", 6379, "521110", 5 ) )
//	{
//		std::cout << "connect error: " << con.getErrorStr( ) << std::endl;
//	}
//	RedisCpp::ValueList valueList;
//	if ( !con.lrange( "newTest", 0 , -1, valueList ) )
//	{
//		std::cout << "lrange error: " << con.getErrorStr( ) << std::endl;
//	}else
//	{
//		std::cout << "lrange ok: " << std::endl;
//		RedisCpp::ValueList::const_iterator it = valueList.begin();
//
//		for( ; it != valueList.end(); it++ )
//		{
//			std::cout << "valueList:: "<< *it << std::endl;
//		}
//	}
//}



////////////////////////////////////pool test////////////////////////////////
void* CallBack( void* arg )
{
	RedisCpp::RedisPool* pool = ( RedisCpp::RedisPool* )arg;
	RedisCpp::RedisConn* conn = pool->getRedisConn();

	while( !conn )
	{
		std::cout << "sleep" << std::endl;
		usleep( 10 );
		 conn = pool->getRedisConn();
	}
//	std::cout << "ok" << std::endl;
	RedisCpp::ValueList valueList;
	conn->lrange( "newTest", 0, -1, valueList );

//	RedisCpp::ValueList::const_iterator it = valueList.begin( );
//	for ( ; it != valueList.end( ) ; it++ )
//	{
//		std::cout << "valueList:: " << *it << std::endl;
//	}

	pool->putBack( conn );
	return NULL;
}

int main( )
{
	RedisCpp::RedisPool pool;
	pool.init( "127.0.0.1", 6379, "521110" );
	if( !pool.connect( ) )
	{
		std::cout << "connect error" << std::endl;
		return -1;
	}
//	pool.disconnect( );

	pthread_t tid[100];
	int i = 1000000;
	while (  i )
	{
//		for ( int k = 0 ; k < 10 ; k++ )
//		{
//			pthread_create( &tid[k], NULL, CallBack, &pool );
//		}
//
//		for ( int k = 0 ; k < 10 ; k++ )
//		{
//			pthread_join( tid[k], NULL );
//		}

		i--;

		CallBack( &pool );
//		std::cout << "yes" << std::endl;
	}
	//sleep( 10 );
	return 0;
}

