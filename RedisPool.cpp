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
	_connected = false;

	_conns.clear( );			///< all connection.
	_errStr.clear();				///< describe the errStr.
}

RedisPool::~RedisPool( )
{
	disconnect();
}


bool RedisPool::init( const std::string& host, uint16_t port, const std::string& pass,
						uint32_t timeout, uint16_t minSize,uint16_t  maxSize  )
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

bool RedisPool::connect( )
{
	if( _connected )
	{
		return false;
	}
	///< 会自动析构自动解锁
	Poco::Mutex::ScopedLock lock( _mutex );

	for ( int i = 0 ; i < _minSize ; i++ )
	{
		RedisConnBuffer conBuf;
		conBuf.pconn = new RedisConn;
		if( !conBuf.pconn->connect( _host, _port, _password, _timeout ) )
		{
			///<  获取错误描述。
			_errStr = conBuf.pconn->getErrorStr();
			return false;
		}
		conBuf.idle = true;
		_conns.push_back( conBuf );
	}

	_connected = true;
	return true;
}

void RedisPool::disconnect()
{
	if( !_connected )
	{
		return ;
	}
	///< 会自动析构自动解锁
	Poco::Mutex::ScopedLock lock( _mutex );
	std::list<RedisConnBuffer>::iterator it = _conns.begin();

	for ( ; it != _conns.end(); it++ )
	{
		if( it->pconn )
		{
			delete it->pconn;
			it->idle = false;
		}
	}
	_connected = false;
}

RedisConn* RedisPool::getRedisConn( )
{
	if( !_connected )
	{
		return NULL;
	}
	Poco::Mutex::ScopedLock lock( _mutex );
	std::list<RedisConnBuffer>::iterator it = _conns.begin();

	///< find a idel 
	for( ; it != _conns.end(); it++ )
	{
		if( it->idle )
		{
			it->idle = false;
			return it->pconn;
		}else
		{
			
		}
	}
	
	///< create a new conn
	if(  _conns.size() > _maxSize )
	{
		return NULL;
	}else
	{
		RedisConnBuffer conBuf;
		conBuf.pconn = new RedisConn;
		if ( !conBuf.pconn->connect( _host, _port, _password, _timeout ) )
		{
			///<  获取错误描述。
			_errStr = conBuf.pconn->getErrorStr( );
			return NULL;
		}
		conBuf.idle = false;
		_conns.push_back( conBuf );

		return conBuf.pconn;
	}
}


bool RedisPool::putBack( const RedisConn* pconn )
{
	if( !_connected )
	{
		return false;
	}
	Poco::Mutex::ScopedLock lock( _mutex );
	std::list<RedisConnBuffer>::iterator it = _conns.begin();

	for( ; it != _conns.end(); it++ )
	{
		if ( it->pconn == pconn )
		{
			it->idle = true;
			break;
		}
	}
	return true;
}




} /* namespace RedisCpp */








