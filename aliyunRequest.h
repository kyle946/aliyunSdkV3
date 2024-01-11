#ifndef ALIYUNREQUEST_H
#define ALIYUNREQUEST_H



#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <algorithm>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <map>
#include <ctime>
#include <time.h>



class aliyunRequest
{
public:
    static aliyunRequest *getInstance();
    static void releaseInstance();
    void setAccessKeySecret(std::string,std::string);
    int sendsms(std::string PhoneNumber, std::string code, std::string SignName, std::string TemplateCode);

private:
    static aliyunRequest *instance;
    aliyunRequest();

private:

    /**
     * @brief post
     * @param host      阿里云SDK主机域名
     * @param action    接口名称
     * @param fields        接口字段
     * @param response_status       响应状态码
     * @param response_body         响应数据
     * @return
     */
    int post(std::string host, std::string action, std::map<std::string, std::string> &fields, int &response_status, std::string &response_body);

    /**
     * 检查AccessKey和AccessSecret有没有设置
     * @brief checkAccessSecret
     * @return
     */
    int checkAccessSecret();

    std::string AccessKey;
    std::string AccessSecret;

    const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    /**
     * @brief base64_encode
     * @param input
     * @return
     */
    std::string base64_encode(uint8_t* buf, uint32_t bufLen);
    std::string base16_encode(const std::string& sUrl);

    /**
     * @brief hmac_sha1_base64
     * @param key
     * @param input
     * @return
     */
    std::string hmac_sha1_base64(std::string key1, std::string &input);
    std::string hmac_sha256_base16(std::string key1, std::string &input);
    std::string sha256_base16(std::string &input);

    uint8_t ToHex(unsigned char x);
    uint8_t FromHex(unsigned char x);
    std::string UrlEncode(const std::string& str);
    std::string UrlDecode(const std::string& str);
    void getutc(uint64_t &sec1, std::string &timestr);

};

#endif // ALIYUNREQUEST_H
