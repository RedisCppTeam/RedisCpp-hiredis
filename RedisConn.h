/**
 * Copyright (c) 2015, 爱wifi（版权声明）
 *
 * @file	RedisConn.h		
 * @brief RedisConn 类一个封装 hiredis 与 redis-server 的链接类。
 *
 * 此类基于 hiredis 用于保持与 redis-server 的链接。
 *
 * @author: 		yuhaiyang
 * @date: 		2015年4月27日
 *
 * 修订说明:初始版本
 */
#ifndef REDISCONN_H_
#define REDISCONN_H_

#include <hiredis/hiredis.h>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <list>
#include <map>


#define DEBUGOUT( attr, value ) std::cout << attr << value << std::endl;

namespace RedisCpp
{

typedef std::list<std::string> ValueList;
typedef std::map<std::string,std::string> ValueMap;

/*
 * @brief 此类基于 hiredis 用于保持与 redis-server 的链接。
 * 
 * 此类用于链接 redis 并且封装操作 redis 的各种方法。
 *
 */
class RedisConn
{
public:

	RedisConn( );

	virtual ~RedisConn( );

	void init( const std::string &host="127.0.0.1" ,const uint16_t port=6379 ,
						const std::string& pass="" , const uint32_t timeout=0 );

	bool connect(  );

	void disConnect( );

	bool auth( const std::string& password );

	bool connect( const std::string &host ,const uint16_t port,
					const std::string& password , const uint32_t timeout );

	bool ping( ) ;
	/**
	 *
	 */
	bool reconnect( ) ;

	inline bool isConneced( )
	{
		return _connected;
	}
	redisReply*  redisCmd( const char *format, ... );

	const std::string getErrorStr( ) const;

	///////////////////////////////// list 的方法 /////////////////////////////////////
	bool lpush(const std::string& key, const std::string& value, uint64_t& retval );

	bool lpop(const std::string& key, std::string& value );


	bool lrange( const std::string &key, uint32_t start, uint32_t end, ValueList& valueList );

	/////////////////////////hash////////////////
	bool hget( const std::string& key , const std::string& filed , std::string& value );

	bool hset( const std::string& key , const std::string& filed , const std::string& value ,uint32_t& retval );

	bool hdel( const std::string& key , const std::string& filed , uint32_t& retval );

	bool hgetall( const std::string& key , ValueMap& valueMap);

protected:

	bool getArryToList( redisReply* reply , ValueList& valueList );

	bool getArryToMap( redisReply* reply , ValueMap& valueMap );

	bool _getError( const redisContext* redCtx );

	bool _getError( const redisReply* reply );

	redisContext* _getCtx( ) const
	{
		return _redCtx;
	}

private:
	redisContext *_redCtx;		///< redis connector context

	std::string _host;         		///< redis host
	uint16_t _port;         		///< redis sever port
	std::string _password;         	///< redis server password
	uint32_t _timeout;      		///< connect timeout second
	bool _connected;			///< if connected
	std::string _errStr;			///< Describe the reason for error..

	static const char* _errDes[2];	///< describe error
};

} /* namespace RedisCpp */

#endif /* REDISCONN_H_ */




