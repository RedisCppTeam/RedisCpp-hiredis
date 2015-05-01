/**
 * Copyright (c) 2015, 爱wifi（版权声明）
 *
 * @file	RedisConn_test.cpp		
 * @brief 此文件用于对　RedisConn 类进行测试。
 *
 * 此文件用于对　RedisConn 类进行测试。
 *
 * @author: 		yuhaiyang
 * @date: 		2015年4月30日
 *
 * 修订说明:初始版本
 */
#include "RedisConn.h"

using RedisCpp::BEFORE;
using RedisCpp::AFTER;

void TestList( )
{
	int64_t ret = 0;
	uint64_t ret2 = 0;
	RedisCpp::RedisConn con;
	if ( !con.connect( "127.0.0.1", 6379 ) )
	{
		std::cout << "connect error " << con.getErrorStr( ) << std::endl;
		return ;
	}

	std::string value;

//	if ( !con.lpush( "testList", "yuhaiyang", ret2 ) )
//	{
//		std::cout << "error: " << con.getErrorStr( ) << std::endl;
//	}else
//	{
//		std::cout << "len = " << ret2 << std::endl;
//	}

//	if( !con.linsert("testList",AFTER, "huang", "chenjun", ret ) )
//	{
//		std::cout << "error: " << con.getErrorStr( ) << std::endl;
//	}

//	if( !con.lpop( "testList", value ) )
//	{
//		std::cout << "error: " << con.getErrorStr( ) << std::endl;
//	}else
//	{
//		std::cout << "value: " << value << std::endl;
//	}
	RedisCpp::ValueList valueList;
	if( !con.lrange( "testList", 0 , -1, valueList ) )
	{
		std::cout << "error " << con.getErrorStr() << std::endl;
	}else
	{
		RedisCpp::ValueList::const_iterator it = valueList.begin();

		for( ; it != valueList.end(); it++ )
		{
			std::cout << "value: " << *it << std::endl;
		}
	}
	if( !con.rpop( "testList", value) )
	{
		std::cout << "error " << con.getErrorStr() << std::endl;
	}
	else
	{
		std::cout << "pop value: " << value << std::endl;
	}

}

void TestHash( void )
{
	int64_t ret = 0;
	uint64_t ret2 = 0;
	RedisCpp::RedisConn con;
	if ( !con.connect( "127.0.0.1", 6379 ) )
	{
		std::cout << "connect error " << con.getErrorStr( ) << std::endl;
		return ;
	}

	std::string value;
	if ( !con.hget( "newHash", "yuhaiyang", value ) )
	{
		std::cout << "hget error " << con.getErrorStr( ) << std::endl;
	}

}


int main( )
{
	TestList();
	return 0;
}


