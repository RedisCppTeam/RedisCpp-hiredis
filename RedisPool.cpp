/**
 * Copyright (c) 2015, 爱wifi（版权声明）
 *
 * @file	RedisPool.cpp		
 * @brief 此文件的简单描述。(必填字段)
 *
 * 此文件的详细功能描述。(可选字段)
 *
 * @author: 		yuhaiyang
 * @date: 		2015年4月27日
 *
 * 修订说明:初始版本
 */
#include "RedisPool.h"




namespace RedisCpp
{

const char* RedisPool::_errDes[2] = {"maxSize is smaller than minSize"};	///< describe error


RedisPool::RedisPool( )
{
	_host.clear( );	 		///< redis host
	_port = 0;         		///< redis sever port
	_password.clear( );         ///< redis server password
	_timeout = 0;      		///< connect timeout second

	_minSize = 5;			///< min pool size
	_maxSize = 10;		///< max pool size
	_conns.clear( );			///< all connection.
	_errStr.clear();				///<
}

RedisPool::~RedisPool( )
{
	// TODO Auto-generated destructor stub
}


bool RedisPool::init( const std::string& host, uint16_t port, std::string& pass,
						uint32_t timeout, uint16_t maxSize,uint16_t minSize  )
{
	_host = host;
	_port = port;
	_password = pass;
	_timeout = timeout;

	if ( minSize <= maxSize )
	{
		_maxSize = maxSize;
		_minSize = minSize;
		return true;
	}else
	{
		_errStr = _errDes[0];
		return false;
	}


}




} /* namespace RedisCpp */
