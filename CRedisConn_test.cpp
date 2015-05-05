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
#include "CRedisConn.h"
#include <unistd.h>

using RedisCpp::BEFORE;
using RedisCpp::AFTER;

void TestList( )
{
	int64_t ret = 0;
	uint64_t ret2 = 0;
	RedisCpp::CRedisConn con;
	if ( !con.connect( "127.0.0.1", 6379 ) )
	{
		std::cout << "connect error " << con.getErrorStr( ) << std::endl;
		return ;
	}

	std::string value;
/*
	//< key 不是list类型
	if ( !con.lpush( "test", "yuhaiyang", ret2 ) )
	{
		std::cout << "lpush error: " << con.getErrorStr( ) << std::endl;
	}else
	{
		std::cout << "len = " << ret2 << std::endl;
	}


	//< key 不存在 ；key 不是list类型
	if( !con.lpop( "test", value ) )
	{
		std::cout << "lpop error: " << con.getErrorStr( ) << std::endl;
	}else
	{
		std::cout << "value: " << value << std::endl;
	}


	//< key 不是list类型
	if ( !con.rpush( "testM", "yuhaiyang", ret2 ) )
	{
		std::cout << "rpush error: " << con.getErrorStr( ) << std::endl;
	}else
	{
		std::cout << "len = " << ret2 << std::endl;
	}


	//< key 不存在 ；key 不是list类型
	if( !con.rpop( "Atest", value ) )
	{
		std::cout << "rpop error: " << con.getErrorStr( ) << std::endl;
	}else
	{
		std::cout << "value: " << value << std::endl;
	}

	//< key 不存在; key 不是list类型
	if( !con.llen("test",ret2) )
	{
		std::cout << "llen error: " << con.getErrorStr( ) << std::endl;

	}
	else
	{
		std::cout << "llen = " << ret2 << std::endl;
	}

	//< key 不存在；key 不是list类型; position 错误; pivot 不存在
	if( !con.linsert("Ltest",AFTER, "value0", "chenjun", ret ) )
	{
		std::cout << "linsert error: " << con.getErrorStr( ) << std::endl;
	}
	else
	{
		std::cout << "revalue = " << ret << std::endl;
	}



	//< key不存在； key 不是list类型； start stop超出范围；
	RedisCpp::ValueList valueList;
	if( !con.lrange( "test",  -9, -8, valueList ) )
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

*/
	//key 不存在;  key 不是list类型; index不在范围内
	if( !con.lindex("test",1,value) )
	{
		std::cout << "lindex error: " << con.getErrorStr( ) << std::endl;
	}
	else
	{
		std::cout<<value.c_str()<<std::endl;
	}

}

void TestHash( void )
{
	int64_t ret = 0;
	uint64_t ret2 = 0;
	RedisCpp::CRedisConn con;
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


