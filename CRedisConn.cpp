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

#include "CRedisConn.h"

namespace RedisCpp
{
// 错误描述。
const char* CRedisConn::_errDes[ERR_BOTTOM] =
{
		"No error." ,
		"NULL pointer , fatal error!",
		"Has no connection to the redis server.",
		"Insert into list error ,position must BEFORE or AFTER.",
		"Insert Error,pivot is not found.",
		"List is empty",
		"Key not find",
		"Index out of range"
};

CRedisConn::CRedisConn( )
{
	_redCtx = NULL;
	_host.clear( );
	_password.clear( );
	_port = 0;
	_timeout = 0;
	_connected = false;
	_errStr = _errDes[ ERR_NO_ERROR ] ;
}

void CRedisConn::init( const std::string &host , const uint16_t port , const std::string& password ,
                const uint32_t timeout )
{
	_host = host;
	_port = port;
	_password = password;
	_timeout = timeout;
}

bool CRedisConn::_getError( const redisReply* reply )
{
	_errStr = _errDes[ ERR_NO_ERROR ];
	if ( reply == NULL )
	{
		_errStr = _errDes[ ERR_NULL ];
		throw NullReplyException();
	}
	// have error
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

bool CRedisConn::_getError( const redisContext* redCtx )
{
	_errStr = _errDes[ ERR_NO_ERROR ];
	if ( redCtx == NULL )
	{
		_errStr = _errDes[ ERR_NULL ];
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



bool CRedisConn::auth( const std::string& password )
{
	if ( !_connected )
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	_password = password;
	redisReply *reply = static_cast<redisReply *>( redisCommand( _redCtx, "AUTH %s",
	                _password.c_str( ) ) );

	if ( _getError( reply ) )
	{
		ret = false;
	}
	else
	{
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject( reply );
	}
	return ret;
}

bool CRedisConn::connect( void )
{
	if ( _connected )
	{
		disConnect( );
	}

	struct timeval timeoutVal;
	timeoutVal.tv_sec = _timeout;
	timeoutVal.tv_usec = 0;

	_redCtx = redisConnectWithTimeout( _host.c_str( ), _port, timeoutVal );
	if ( _getError( _redCtx ) )
	{
		if ( NULL != _redCtx )
		{
			redisFree( _redCtx );
			_redCtx = NULL;
		}
		return false;
	}

	_connected = true;

	// if connection  need password
	if ( _password == "" )
	{
		return true;
	}
	else
	{
		return ( auth( _password ) );
	}
}

void CRedisConn::disConnect( )
{
	if ( _connected && NULL != _redCtx )
	{
		redisFree( _redCtx );
		_redCtx = NULL;
	}
	_connected = false;
}

bool CRedisConn::connect( const std::string &host , const uint16_t port ,
                const std::string& password , const uint32_t timeout )
{
	// Init attribute.
	init( host, port, password, timeout );

	return ( connect( ) );
}

bool CRedisConn::ping( )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	redisReply *reply = static_cast<redisReply *>( redisCommand( _redCtx, "PING" ) );
	bool ret = false;

	if ( _getError( reply ) )
	{
		ret = false;
	}
	else
	{
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject( reply );
	}

	return ret;
}

bool CRedisConn::reconnect( )
{
	return ( connect( ) );
}

const std::string CRedisConn::getErrorStr( ) const
{
	return _errStr;
}

redisReply* CRedisConn::redisCmd( const char *format , ... )
{
	va_list ap;
	va_start( ap, format );
	redisReply* reply = static_cast<redisReply *>( redisvCommand( _redCtx, format, ap ) );
	va_end( ap );
	return reply;
}

CRedisConn::~CRedisConn( )
{
	disConnect( );
}

////////////////////////////////// list 类的方法 ////////////////////////////////////////

bool CRedisConn::lpush( const std::string& key , const std::string& value , uint64_t& retval )
                throw ( NullReplyException )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	retval = 0;
	bool ret = false;

	redisReply *reply = redisCmd( "LPUSH %s %s", key.c_str( ), value.c_str( ) );

	if ( _getError( reply ) )
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
		freeReplyObject( reply );
	}
	return ret;

}

bool CRedisConn::lpop( const std::string& key , std::string& value )  throw ( NullReplyException )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd( "LPOP %s", key.c_str( ) );

	if ( _getError( reply ) )
	{
		ret = false;
	}
	else
	{
		//key 不存在
		if ( NULL == reply->str )
		{
			_errStr = std::string( _errDes[ERR_NO_KEY] ) ;//+ " or " + _errDes[ERR_LIST_EMPTY];
			value = "";
			ret = false;
		}
		else
		{
			value = reply->str;
			ret = true;
		}
	}

	if ( NULL != reply )
	{
		freeReplyObject( reply );
	}
	else
	{

	}

	return ret;
}

bool CRedisConn::_getArryToList( redisReply* reply , ValueList& valueList )
{
	if ( NULL == reply )
	{
		return false;
	}

	std::size_t num = reply->elements;

	for ( std::size_t i = 0 ; i < num ; i++ )
	{
		valueList.push_back( reply->element[i]->str );
	}

//	ValueList::iterator it = valueList.begin();
//
//	for( ; it != valueList.end(); it++ )
//	{
//		std::cout << "valueList: "<< *it << std::endl;
//	}
	return true;
}

bool CRedisConn::_getArryToMap( redisReply* reply , ValueMap& valueMap )
{
	if ( NULL == reply )
	{
		return false;
	}

	std::size_t num = reply->elements;

	for ( std::size_t i = 0 ; i < num ; i += 2 )
	{
		valueMap.insert(
		                std::pair<std::string , std::string>( reply->element[i]->str,
		                                reply->element[i + 1]->str ) );
	}

	return true;
}

bool CRedisConn::lrange( const std::string &key , uint32_t start , int32_t end ,
                ValueList& valueList )  throw ( NullReplyException )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd( "LRANGE %s %d %d", key.c_str( ), start, end );

	if ( _getError( reply ) )	//< key 存在但不是list类型
	{
		ret = false;
	}
	else
	{
		if ( REDIS_REPLY_ARRAY == reply->type && 0 == reply->elements) 		//<  key是list类型但 start > end
		{
			_errStr = std::string( _errDes[ERR_INDEX] ) + " or " + _errDes[ERR_LIST_EMPTY];
			ret = false;

		}
		else
		{
			_getArryToList( reply, valueList );
			ret = true;
		}
	}

	if ( NULL != reply )
	{
		freeReplyObject( reply );
	}

	return ret;
}

bool CRedisConn::rpush( const std::string& key , const std::string& value , uint64_t& retval )
		throw ( NullReplyException )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	retval = 0;
	bool ret = false;

	redisReply *reply = redisCmd( "RPUSH %s %s", key.c_str( ), value.c_str( ) );

	if ( _getError( reply ) )
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
		freeReplyObject( reply );
	}

	return ret;
}

bool CRedisConn::rpop( const std::string& key , std::string& value )   throw ( NullReplyException )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd( "RPOP %s", key.c_str( ) );

	if ( _getError( reply ) )
	{
		ret = false;
	}
	else
	{
		// 失败
		if ( NULL == reply->str )
		{
			_errStr = std::string( _errDes[ERR_NO_KEY] ) + " or "
			                + _errDes[ERR_LIST_EMPTY];
			value = "";
			ret = false;
		}
		else
		{
			value = reply->str;
			ret = true;
		}
	}

	if ( NULL != reply )
	{
		freeReplyObject( reply );
	}


	return ret;
}

bool CRedisConn::lindex( const std::string& key , int32_t index , std::string& value )
		throw ( NullReplyException )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd( "LINDEX %s %d", key.c_str( ), index );

	if ( _getError( reply ) )
	{
		ret = false;
	}
	else
	{

		if ( NULL == reply->str )
		{
			_errStr = std::string( _errDes[ERR_NO_KEY] ) + " or "
			                + _errDes[ERR_INDEX];
			value = "";
			ret = false;
		}
		else
		{
			value = reply->str;
			ret = true;
		}
	}

	if ( NULL != reply )
	{
		freeReplyObject( reply );
	}

	return ret;
}

bool CRedisConn::linsert( const std::string& key , INSERT_POS position ,
                const std::string& pivot , const std::string value , int64_t& retval )  throw ( NullReplyException )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	std::string pos;

	if( BEFORE == position )
	{
		pos = "BEFORE";
	}else if( AFTER == position )
	{
		pos = "AFTER";
	}else
	{
		_errStr = _errDes[ ERR_POSITION ];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd( "LINSERT %s %s %s %s", key.c_str( ), pos.c_str( ),
	                pivot.c_str( ), value.c_str( ) );

	if ( _getError( reply ) )	//< 不是list 类型
	{
		ret = false;
	}
	else
	{
		if ( REDIS_REPLY_INTEGER == reply->type )
		{
			if( reply->integer == -1 )
			{
				_errStr = _errDes[ ERR_PIVOT_NO_EXIST ];
				ret = false;
			}else if( reply->integer == 0 )
			{
				_errStr = _errDes[ ERR_LIST_EMPTY ];
				ret = false;
			}else
			{
				retval = reply->integer;
				ret = true;
			}
		}else
		{
			ret = false;
		}
	}

	if ( NULL != reply )
	{
		freeReplyObject( reply );
	}

	return ret;
}


bool CRedisConn::llen( const std::string& key , uint64_t& retval )   throw ( NullReplyException )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd( "LLEN %s", key.c_str( ) );

	if ( _getError( reply ) )
	{
		ret = false;
	}
	else
	{
		if ( REDIS_REPLY_INTEGER == reply->type && ( 0 == reply->integer))
		{
			_errStr = _errDes[ ERR_LIST_EMPTY ];
			return false;
		}
		else
		{
			retval = reply->integer;
			ret = true;
		}
	}

	if ( NULL != reply )
	{
		freeReplyObject( reply );
	}

	return ret;
}

//////////////////hash方法/////////////////////////////
bool CRedisConn::hget( const std::string& key , const std::string& filed , std::string& value )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd( "HGET %s %s", key.c_str( ), filed.c_str( ) );

	if ( _getError( reply ) )
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
		freeReplyObject( reply );
	}


	return ret;
}

bool CRedisConn::hset( const std::string& key , const std::string& filed , const std::string& value ,
                uint32_t& retval )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd( "HSET %s %s %s", key.c_str( ), filed.c_str( ),
	                value.c_str( ) );

	if ( _getError( reply ) )
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
		freeReplyObject( reply );
	}

	return ret;

}

bool CRedisConn::hdel( const std::string& key , const std::string& filed , uint32_t& retval )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd( "HDEL %s %s", key.c_str( ), filed.c_str( ), retval );

	if ( _getError( reply ) )
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
		freeReplyObject( reply );
	}

	return ret;
}

bool CRedisConn::hgetall( const std::string& key , ValueMap& valueMap )
{
	if ( !_connected || !_redCtx )
	{
		_errStr = _errDes[ERR_NO_CONNECT];
		return false;
	}

	bool ret = false;
	redisReply *reply = redisCmd( "HGETALL %s", key.c_str( ) );

	if ( _getError( reply ) )
	{
		ret = false;
	}
	else
	{
		if ( REDIS_REPLY_ARRAY == reply->type )
		{
			_getArryToMap( reply, valueMap );
		}
		ret = true;
	}

	if ( NULL != reply )
	{
		freeReplyObject( reply );
	}

	return ret;
}

} /* namespace RedisCpp */

