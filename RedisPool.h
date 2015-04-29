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
#include <list>

#include <Poco/Mutex.h>
#include <Poco/AutoPtr.h>

#include "RedisConn.h"

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
		RedisConn* pconn;	///< a connection
		bool idle;			///< if idel
	} RedisConnBuffer;


	RedisPool( );
	virtual ~RedisPool( );

	bool init( const std::string& host, uint16_t port, const std::string& pass,
						uint32_t timeout=0, uint16_t minSize=5,uint16_t  maxSize=10 );

	bool connect();

	void disconnect();

	bool auth( const std::string& password );

	RedisConn* getRedisConn( );

	bool putBack( const RedisConn* pconn );

	void onJanitorTimer( );

protected:

private:
	std::string _host;         		///< redis host
	uint16_t _port;         		///< redis sever port
	std::string _password;         	///< redis server password
	uint32_t _timeout;      		///< connect timeout second

	uint16_t _minSize;			///< min pool size
	uint16_t _maxSize;			///< max pool size
	std::list<RedisConnBuffer> _conns;	///< all connection.
	bool _connected;				///< Whether it is connected
	
	std::string _errStr;				///< Describe the reason for error..
	static const char* _errDes[2];	///< describe error
	Poco::Mutex _mutex;			///< lock
};

} /* namespace RedisCpp */

#endif /* REDISPOOL_H_ */
