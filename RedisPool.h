/**
 * Copyright (c) 2015, 爱wifi（版权声明）
 *
 * @file	RedisPool.h		
 * @brief 此文件的简单描述。(必填字段)
 *
 * 此文件的详细功能描述。(可选字段)
 *
 * @author: 		yuhaiyang
 * @date: 		2015年4月27日
 *
 * 修订说明:初始版本
 */
#ifndef REDISPOOL_H_
#define REDISPOOL_H_

#include <stdint.h>
#include <iostream>
#include "RedisConn.h"
#include <list>

namespace RedisCpp
{

/*
 * @brief 此类的简要说明(必填字段)
 * 
 * 此类的详细说明(选填字段)
 */
class RedisPool
{
public:
	typedef struct
	{
		RedisConn* conn;	///< a connection
		bool idel;		///< if idel
	} RedisConnBuffer;


	RedisPool( );
	virtual ~RedisPool( );

	void init( const std::string& host, uint16_t port, std::string& pass,
						uint32_t timeout, uint16_t maxSize,uint16_t minSize  );


private:
	std::string _host;         		///< redis host
	uint16_t _port;         		///< redis sever port
	std::string _password;         	///< redis server password
	uint32_t _timeout;      		///< connect timeout second

	uint16_t _minSize;			///< min pool size
	uint16_t _maxSize;			///< max pool size
	std::list<RedisConnBuffer> _conns;	///< all connection.
};

} /* namespace RedisCpp */

#endif /* REDISPOOL_H_ */
