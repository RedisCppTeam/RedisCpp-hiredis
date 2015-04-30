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

/**
 *@brief 此类基于 hiredis 用于保持与 redis-server 的链接。
 * 
 * 例子代码:
 *int main( )
{
	RedisCpp::RedisConn con;
	if ( !con.connect( "127.0.0.1", 6379, "521110", 5 ) )
	{
		std::cout << "connect error " << con.getErrorStr( ) << std::endl;
		return 0;
	}

	std::string value;
	if ( !con.hget( "newHash", "yuhaiyang", value ) )
	{
		std::cout << "hget error " << con.getErrorStr( ) << std::endl;
	}
	return 0;
}
 *
 */
class RedisConn
{
public:
	RedisConn( );

	virtual ~RedisConn( );

	/**
	 *@brief 初始化链接信息
	 *
	 *用于初始化链接的详细信息。在　connect() 之前使用。也可以直接调用
	 *bool connect( const std::string &host ,const uint16_t port, const std::string& password , const uint32_t timeout );来初始化。
	 */
	void init( const std::string &host="127.0.0.1" ,const uint16_t port=6379 ,
						const std::string& pass="" , const uint32_t timeout=0 );

	/**
	 *@brief 链接　redis 数据库
	 *
	 * init() 之后调用此方法来链接数据库
	 *
	 *@return 成功返回　true,失败返回　false
	 */
	bool connect(  );

	/**
	 *@brief 断开与　redis 内存数据库链接
	 */
	void disConnect( );

	/**
	 *@brief 链接　redis 数据库之后，需要认证。
	 *
	 *@param [in] 登录的密码。
	 *@return 成功返回　true, 失败返回　false.
	 */
	bool auth( const std::string& password );

	/**
	 *@brief 初始化链接信息并且链接。
	 *
	 * 初始化链接需要的信息。如果需要　password 非空那么就自动　auth 认证。
	 *
	 *@param [in]  host .  redis-server 的 ip。
	 *@param [in] port .redis-server 的 port 。
	 *@param [in] password . redis-server 认证密码，此参数可以不填，不填旧不会发起认证。
	 *@param [in] timeout . 链接的超时时间。
	 *@return true 成功，　false 失败。
	 */
	bool connect( const std::string &host ,const uint16_t port,
					const std::string& password="" , const uint32_t timeout=0 );

	/**
	 * @brief 检查与服务器网络
	 *
	 * @return 网络链接正常返回　true, 不通返回　false.
	 */
	bool ping( ) ;

	/**
	 *@brief 重新链接　redis 服务器。
	 *
	 *@return 重连成功　true, 重连失败　false.
	 */
	bool reconnect( ) ;

	/**
	 * @brief 检查是否链接过　redis 服务器。
	 *
	 * @return 已经链接　true,没有链接过　false.
	 */
	inline bool isConneced( )
	{
		return _connected;
	}

	/**
	 * @brief 获取错误的原因
	 *
	 * ＠return 返回发生错误的原因，没有错误返回　""
	 */
	const std::string getErrorStr( ) const;

	/**
	 * @brief 直接发送指令参数给　redis 服务器。
	 *
	 * ＠param [in] format 格式化子复制。
	 * ＠param [in] ... 指令及数据字符串。
	 * @return NULL ,redis 执行失败，或者断开俩就。橙光返回　redisReply 指针。
	 */
	redisReply*  redisCmd( const char *format, ... );

	///////////////////////////////// list 的方法 /////////////////////////////////////
	bool lpush(const std::string& key, const std::string& value, uint64_t& retval );

	bool lpop(const std::string& key, std::string& value );



	bool lrange( const std::string &key, uint32_t start, int32_t end, ValueList& valueList );

	//////////////////////////////   hash 的方法 //////////////////////////////////////

	bool lrange( const std::string &key, uint32_t start, uint32_t end, ValueList& valueList );

	bool rpush(const std::string& key, const std::string& value, uint64_t& retval );

	bool rpop(const std::string& key, std::string& value );

	bool linsert(const std::string& key, const std::string& position, const std::string& pivot, const std::string value, int64_t& retval);

	bool lindex(const std::string& key, int32_t index, std::string& value);

	bool llen(const std::string& key, uint64_t& retval);

	bool hget( const std::string& key , const std::string& filed , std::string& value );

	bool hset( const std::string& key , const std::string& filed , const std::string& value ,uint32_t& retval );

	bool hdel( const std::string& key , const std::string& filed , uint32_t& retval );

	bool hgetall( const std::string& key , ValueMap& valueMap);

protected:
	bool _getArryToList( redisReply* reply , ValueList& valueList );

	bool _getArryToMap( redisReply* reply , ValueMap& valueMap );

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




