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

int main( )
{
	RedisCpp::RedisConn con;
	if ( !con.connect( "127.0.0.1", 6379 ) )
	{
		std::cout << "connect error " << con.getErrorStr( ) << std::endl;
		return 0;
	}

	std::string value;
//	if ( !con.hget( "newHash", "yuhaiyang", value ) )
//	{
//		std::cout << "hget error " << con.getErrorStr( ) << std::endl;
//	}

	int64_t ret = 0;
	uint64_t ret2 = 0;
	if ( !con.lpush( "testSet", "yuhaiyang", ret2 ) )
	{
		std::cout << "error: " << con.getErrorStr( ) << std::endl;
	}
	std::cout << ret2 << std::endl;

	if( !con.linsert("testList",BEFORE, "yuhaiyang", "huangshaoliang", ret ) )
	{
		std::cout << "error: " << con.getErrorStr( ) << std::endl;
	}

	if( !con.lpop( "testList12", value ) )
	{
		std::cout << "error: " << con.getErrorStr( ) << std::endl;
	}else
	{
		std::cout << "value: " << value << std::endl;
	}
	return 0;
}


