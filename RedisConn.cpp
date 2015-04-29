/**
 * Copyright (c) 2015, 爱wifi（版权声明）
 *
 * @file	RedisConn.cpp		
 * @brief 此文件的简单描述。(必填字段)
 *
 * 此文件的详细功能描述。(可选字段)
 *
 * @author: 		yuhaiyang
 * @date: 		2015年4月27日
 *
 * 修订说明:初始版本
 */

#include "RedisConn.h"
#include <string.h>
using namespace std;
namespace RedisCpp
{
///< 错误描述。
const char* RedisConn::_errDes[2] =
{ "NULL return , fatal error!", "Has no connection to the redis server." };

RedisConn::RedisConn( )
{
	_redCtx = NULL;
	_host.clear();
	_password.clear();
	_port = 0;
	_timeout = 0;
	_connected = false;
	_errStr.clear();
}

void RedisConn::init( const std::string &host , const uint16_t port , const std::string& password ,
		const uint32_t timeout )
{
	_host = host;
	_port = port;
	_password = password;
	_timeout = timeout;
}

bool RedisConn::_getError( const redisReply* reply )
{
	_errStr = "";
	if ( reply == NULL )
	{
		_errStr = _errDes[0];
		return true;
	}
	///< have error
	if ( reply->type == REDIS_REPLY_ERROR )
	{
		_errStr = reply->str;
		return true;
	}
	else
	{
		return false;
	}
}

bool RedisConn::_getError( const redisContext* redCtx )
{
	_errStr = "";
	if ( redCtx == NULL )
	{
		_errStr = _errDes[0];
		return true;
	}
	if ( redCtx->err != 0 )
	{
		_errStr = redCtx->errstr;
		return true;
	}
	else
	{
		return false;
	}
}

bool RedisConn::auth( const std::string& password )
{
	if ( !_connected )
	{
		_errStr = _errDes[1];
		return false;
	}

	bool ret = false;
	_password = password;
	redisReply *reply = static_cast<redisReply *>(redisCommand(_redCtx, "AUTH %s",
			_password.c_str()));

	if ( _getError(reply) )
	{
		ret = false;
	}
	else
	{
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject(reply);
	}
	return ret;
}

bool RedisConn::connect( void )
{
	if ( _connected )
	{
		disConnect();
	}

	struct timeval timeoutVal;
	timeoutVal.tv_sec = _timeout;
	timeoutVal.tv_usec = 0;

	_redCtx = redisConnectWithTimeout(_host.c_str(), _port, timeoutVal);
	if ( _getError(_redCtx) )
	{
		if ( NULL != _redCtx )
		{
			redisFree(_redCtx);
			_redCtx = NULL;
		}
		return false;
	}

	_connected = true;

	///< if connection  need password
	if ( _password == "" )
	{
		return true;
	}
	else
	{
		return ( auth(_password) );
	}
}

void RedisConn::disConnect( )
{
	if ( _connected && NULL != _redCtx )
	{
		redisFree(_redCtx);
		_redCtx = NULL;
	}
	_connected = false;
}

bool RedisConn::connect( const std::string &host , const uint16_t port ,
		const std::string& password , const uint32_t timeout )
{
	///< Init attribute.
	init(host, port, password, timeout);

	return ( connect() );
}

bool RedisConn::ping( )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[1];
		return false;
	}

	redisReply *reply = static_cast<redisReply *>(redisCommand(_redCtx, "PING"));
	bool ret = false;

	if ( _getError(reply) )
	{
		ret = false;
	}
	else
	{
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject(reply);
	}

	return ret;
}

bool RedisConn::reconnect( )
{
	return ( connect() );
}

const std::string RedisConn::getErrorStr( ) const
{
	return _errStr;
}

redisReply* RedisConn::redisCmd( const char *format , ... )
{
	va_list ap;
	va_start(ap, format);
	redisReply* reply = static_cast<redisReply *>(redisvCommand(_redCtx, format, ap));
	va_end(ap);
	return reply;
}

RedisConn::~RedisConn( )
{
	disConnect();
}

////////////////////////////////// list 类的方法 ////////////////////////////////////////

bool RedisConn::lpush( const std::string& key , const std::string& value , uint64_t& retval )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[1];
		return false;
	}

	retval = 0;
	bool ret = false;

	redisReply *reply = redisCmd("LPUSH %s %s", key.c_str(), value.c_str());

	if ( _getError(reply) )
	{
		ret = false;
	}
	else
	{
		retval = reply->integer;
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject(reply);
	}

	return ret;
}

bool RedisConn::lpop( const std::string& key , std::string& value )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[1];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd("LPOP %s", key.c_str());

	if ( _getError(reply) )
	{
		ret = false;
	}
	else
	{
		value = reply->str;
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject(reply);
	}
	else
	{

	}

	return ret;
}

bool RedisConn::getArryToList( redisReply* reply , ValueList& valueList )
{
	if ( NULL == reply )
	{
		return false;
	}

	std::size_t num = reply->elements;

	for ( std::size_t i = 0 ; i < num ; i++ )
	{
		valueList.push_back(reply->element[i]->str);
	}

//	ValueList::iterator it = valueList.begin();
//
//	for( ; it != valueList.end(); it++ )
//	{
//		std::cout << "valueList: "<< *it << std::endl;
//	}
	return true;
}

bool RedisConn::getArryToMap( redisReply* reply , ValueMap& valueMap )
{
	if ( NULL == reply )
	{
		return false;
	}

	std::size_t num = reply->elements;

	for ( std::size_t i = 0 ; i < num ; i+=2 )
	{
		valueMap.insert(pair<string, string>(reply->element[i]->str,reply->element[i+1]->str));
	}

	return true;
}

bool RedisConn::lrange( const std::string &key , uint32_t start , uint32_t end ,
		ValueList& valueList )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[1];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd("LRANGE %s %d %d", key.c_str(), start, end);

	if ( _getError(reply) )
	{
		ret = false;
	}
	else
	{
		if ( REDIS_REPLY_ARRAY == reply->type )
		{
			getArryToList(reply, valueList);
		}
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject(reply);
	}
	else
	{

	}

	return ret;
}


bool RedisConn::rpush(const std::string& key, const std::string& value, uint64_t& retval )
{
	if( !_connected || !_redCtx )
	{
		_errStr = _errDes[1];
		return false;
	}

	retval = 0;
	bool ret = false;

	redisReply *reply = redisCmd( "RPUSH %s %s", key.c_str(), value.c_str() );

	if ( _getError( reply ) )
	{
		ret = false;
	}else
	{
		retval = reply->integer;
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject( reply );
	}

	return ret;
}


bool RedisConn::rpop(const std::string& key, std::string& value )
{
	if( !_connected || !_redCtx )
	{
		_errStr = _errDes[1];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd( "RPOP %s", key.c_str() );


	if ( _getError( reply ) )
	{
		ret = false;
	}else
	{
		value = reply->str;
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject( reply );
	}else
	{

	}

	return ret;
}

bool RedisConn::lindex(const std::string& key, int32_t index, std::string& value)
{
	if( !_connected || !_redCtx )
	{
		_errStr = _errDes[1];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd( "LINDEX %s %d", key.c_str(),index );

	if ( _getError( reply ) )
	{
		ret = false;
	}else
	{
		value = reply->str;
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject( reply );
	}else
	{

	}

	return ret;
}

bool RedisConn::linsert(const std::string& key, const std::string& position, const std::string& pivot, const std::string value, int64_t& retval)
{
	if( !_connected || !_redCtx )
	{
		_errStr = _errDes[1];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd( "LINSERT %s %s %s %s", key.c_str(), position.c_str(), pivot.c_str(), value.c_str() );

	if ( _getError( reply ) )
	{
		ret = false;
	}else
	{
		if( REDIS_REPLY_INTEGER == reply->type )
		{
			retval = reply->integer;
		}
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject( reply );
	}else
	{

	}

	return ret;
}

bool RedisConn::llen(const std::string& key, uint64_t& retval)
{
	if( !_connected || !_redCtx )
	{
		_errStr = _errDes[1];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd( "LLEN %s", key.c_str());

	if ( _getError( reply ) )
	{
		ret = false;
	}else
	{
		if( REDIS_REPLY_INTEGER == reply->type )
		{
			retval = reply->integer;
		}
		ret = true;
	}


	if ( NULL != reply )
	{
		freeReplyObject( reply );
	}else
	{

	}

	return ret;
}

//////////////////hash方法/////////////////////////////
bool RedisConn::hget( const std::string& key , const std::string& filed , std::string& value )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[1];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd("HGET %s %s", key.c_str(), filed.c_str());

	if ( _getError(reply) )
	{
		ret = false;
	}
	else
	{

		value = reply->str;
		ret = true;

	}

	if ( NULL != reply )
	{
		freeReplyObject(reply);
	}
	else
	{

	}

	return ret;
}

bool RedisConn::hset( const std::string& key , const std::string& filed , const std::string& value ,
		uint32_t& retval )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[1];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd("HSET %s %s %s", key.c_str(), filed.c_str(), value.c_str());

	if ( _getError(reply) )
	{
		ret = false;
	}
	else
	{
		retval = reply->integer;
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject(reply);
	}
	else
	{

	}

	return ret;

}

bool RedisConn::hdel( const std::string& key , const std::string& filed , uint32_t& retval )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[1];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd("HDEL %s %s", key.c_str(), filed.c_str(), retval);

	if ( _getError(reply) )
	{
		ret = false;
	}
	else
	{
		retval = reply->integer;
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject(reply);
	}
	else
	{

	}

	return ret;
}

bool RedisConn::hgetall( const std::string& key , ValueMap& valueMap )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[1];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd("HGETALL %s", key.c_str());

	if ( _getError(reply) )
	{
		ret = false;
	}
	else
	{
		if ( REDIS_REPLY_ARRAY == reply->type )
		{
			getArryToMap(reply, valueMap);
		}
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject(reply);
	}
	else
	{

	}

	return ret;
}

} /* namespace RedisCpp */

