#include "aliyunRequest.h"
#include "curlrequest.h"
#include "plugins/CJsonObject.h"


aliyunRequest *aliyunRequest::instance=nullptr;
aliyunRequest *aliyunRequest::getInstance()
{
    if(instance==nullptr){
        instance=new aliyunRequest();
    }
    return instance;
}

void aliyunRequest::releaseInstance()
{
    if(instance!=nullptr){
        delete instance;
        instance=nullptr;
    }
}

void aliyunRequest::setAccessKeySecret(std::string AccessKey, std::string AccessSecret)
{
    this->AccessKey=AccessKey;
    this->AccessSecret=AccessSecret;
}

aliyunRequest::aliyunRequest()
{
//    this->AccessKey="xx";
//    this->AccessSecret="xx";
}

int aliyunRequest::sendsms(std::string PhoneNumber, std::string code, std::string SignName, std::string TemplateCode)
{
    if(this->checkAccessSecret()==0){
        return 0;
    }

    std::string host="dysmsapi.aliyuncs.com";
    std::string action="SendSms";

    //==== 接口参数
    std::map<std::string, std::string> params;
    params["PhoneNumbers"]=PhoneNumber;                 //---------接收短信的手机号码
    params["SignName"]=SignName;                        //---------短信签名名称
    params["TemplateCode"]=TemplateCode;                //---------短信模板CODE
    std::string TemplateParam="{\"code\":\""+code+"\"}";
    params["TemplateParam"]=TemplateParam;              //---------短信模板变量对应的实际值

    int response_status;        //http响应状态码
    std::string response_body;      //http响应数据
    this->post(host, action, params, response_status, response_body);

    std::cout<<"response_status: "<<response_status<<std::endl;
    std::cout<<"response_body: "<<response_body<<std::endl;

    if(response_status==200){
        JsonObj resJson;
        if(resJson.Parse(response_body)){
            if(resJson("Code")=="OK"){
                std::cout << "Sending Success." << std::endl;
                return 1;       //---------发送成功
            }
            else if(resJson("Code")=="isv.BUSINESS_LIMIT_CONTROL"){
                return -5;      //---------发送频率过高，请等会再试.
            }
            else{
                return -4;      //---------发送失败
            }
        }else{
            return -3;      //---------接口调用失败
        }
    }else{
        std::cout<<"request failed."<<std::endl;
        return -1;
    }
}


int aliyunRequest::post(std::string host, std::string action, std::map<std::string, std::string> &fields, int &response_status, std::string &response_body)
{
    if(host.empty()) return -1;
    if(action.empty()) return -1;

    std::string url="https://"+host;        //请求地址
    std::string RequestBody="";  //请求正文
    std::string QueryString;
    std::vector<std::string> headers;   //header参数列表

    //==== 组装接口参数
    std::string CanonicalizedQueryString;
    for(auto item : fields){
        std::string key1=this->UrlEncode(item.first);
        std::string value1=this->UrlEncode(item.second);
        CanonicalizedQueryString+=key1+"="+value1+"&";
    }
    CanonicalizedQueryString=CanonicalizedQueryString.substr(0,CanonicalizedQueryString.size()-1);

    //==== 获取UTC时间
    uint64_t sec;
    std::string timestr;
    this->getutc(sec, timestr);
    std::string x_acs_date=timestr;

    //==== 生成随机字符
    int randnumber=rand();
    std::string nonce=std::to_string(sec)+std::to_string(randnumber);
    std::string x_acs_signature_nonce=nonce;

    std::string x_acs_content_sha256=this->sha256_base16(RequestBody);
    std::string HashedRequestPayload=x_acs_content_sha256;
    std::string x_acs_version="2017-05-25";
    std::string CanonicalURI="/";
    std::string CanonicalQueryString=CanonicalizedQueryString;
    std::string SignedHeaders="host;x-acs-action;x-acs-content-sha256;x-acs-date;x-acs-signature-nonce;x-acs-version";
    std::string CanonicalRequest =
        "POST\n" +
        CanonicalURI+"\n" +
        CanonicalQueryString+"\n"+
        "host:"+host+"\n"+
        "x-acs-action:"+action+"\n"+
        "x-acs-content-sha256:"+x_acs_content_sha256+"\n"+
        "x-acs-date:"+x_acs_date+"\n"+
        "x-acs-signature-nonce:"+x_acs_signature_nonce+"\n"+
        "x-acs-version:" +x_acs_version+"\n\n"+
        SignedHeaders + "\n" +
        HashedRequestPayload;

//    std::cout<<"CanonicalRequest: "<<CanonicalRequest<<std::endl;

    //==== 使用hmac_sha256_base16签名
    std::string HashedCanonicalRequest=this->sha256_base16(CanonicalRequest);
    std::string StringToSign="ACS3-HMAC-SHA256\n"+HashedCanonicalRequest;
    std::string Signature=this->hmac_sha256_base16(AccessSecret, StringToSign);

//    std::cout<<"StringToSign: "<<StringToSign<<std::endl;

    //==== 组装Authorization
    char AuthorizationChar[1024]={'\0'};
    sprintf(AuthorizationChar, "ACS3-HMAC-SHA256 Credential=%s,SignedHeaders=%s,Signature=%s", AccessKey.c_str(), SignedHeaders.c_str(), Signature.c_str());
    std::string Authorization=AuthorizationChar;

    headers.push_back("Authorization: "+Authorization);
    headers.push_back("host: "+host);
    headers.push_back("x-acs-action: "+action);
    headers.push_back("x-acs-date: "+x_acs_date);
    headers.push_back("x-acs-version: "+x_acs_version);
    headers.push_back("x-acs-content-sha256: "+x_acs_content_sha256);
    headers.push_back("x-acs-signature-nonce: "+x_acs_signature_nonce);

    curlrequest cli;
    cli.post(url+"/?"+CanonicalQueryString, "", headers, response_status, response_body);

    return 1;
}

int aliyunRequest::checkAccessSecret()
{
    if(this->AccessKey.empty()||this->AccessSecret.empty()){
        return 0;
    }else{
        return 1;
    }
}


///==========================================================================================


std::string aliyunRequest::base64_encode(uint8_t* buf, uint32_t bufLen){

    std::string ret;
    int i = 0;
    int j = 0;
    uint8_t char_array_3[3];
    uint8_t char_array_4[4];

    while (bufLen--) {
        char_array_3[i++] = *(buf++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            ret += '=';
    }
    return ret;
}


std::string aliyunRequest::base16_encode(const std::string& sUrl)
{
    static char HEX_TABLE[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    std::string result;
    for (size_t i = 0; i < sUrl.length(); i++)
    {
        char c = sUrl[i];
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
            result.append(1, c);
        else
        {
            result.append(1, '%');
            result.append(1, HEX_TABLE[(c >> 4) & 0x0f]);
            result.append(1, HEX_TABLE[c & 0x0f]);
        }
    }
    return result;
}


std::string aliyunRequest::hmac_sha1_base64(std::string key1, std::string &input)
{
    uint8_t md[20];
    uint32_t len=0;
    HMAC(EVP_sha1(), key1.data(), key1.length(), (unsigned char*)input.data(), input.length(), md, &len);
    return base64_encode(md, len);
}


std::string aliyunRequest::hmac_sha256_base16(std::string key1, std::string &input)
{
    unsigned char *result;
    int result_len = 32;
    int i;
    static char res_hexstring[64];
    result = HMAC(EVP_sha256(), key1.data(), key1.length(), (unsigned char*)input.data(), input.length(), NULL, NULL);
    for (i = 0; i < result_len; i++) {
        sprintf(&(res_hexstring[i * 2]), "%02x", result[i]);
    }
    return base16_encode(res_hexstring);
}


std::string aliyunRequest::sha256_base16(std::string &input)
{
    char outputBuffer[65];
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.data(), input.length());
    SHA256_Final(hash, &sha256);
    int i = 0;
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
    std::string sha1string;
    sha1string.append(outputBuffer);
    return base16_encode(sha1string);
}


uint8_t aliyunRequest::ToHex(unsigned char x)
{
    return  x > 9 ? x + 55 : x + 48;
}


uint8_t aliyunRequest::FromHex(unsigned char x)
{
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else{
        return NULL;
    }
    return y;
}


std::string aliyunRequest::UrlEncode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ')
            strTemp += "+";
        else
        {
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] % 16);
        }
    }
    return strTemp;
}


std::string aliyunRequest::UrlDecode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (str[i] == '+') strTemp += ' ';
        else if (str[i] == '%')
        {
            if((i + 2 < length)){
                //
            }else{
                return "";
            }
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high*16 + low;
        }
        else strTemp += str[i];
    }
    return strTemp;
}


void aliyunRequest::getutc(uint64_t &sec1, std::string &timestr)
{
    time_t  now =time(0);
    char  * data = ctime(&now);
    sec1=now;
    struct tm *local=gmtime(&now);

    int year=local->tm_year + 1900;  // 就是现在的2020年
    int mon=local->tm_mon + 1;  // 就是现在的11月
    int mday=local->tm_mday;  // 日
    int hour=local->tm_hour;  // 时
    int min=local->tm_min;  // 分
    int sec=local->tm_sec;  // 秒

    char str2[32]={'\0'};
    sprintf(str2, "%04d-%02d-%02dT%02d:%02d:%02dZ", year, mon, mday, hour, min, sec);
    timestr=str2;
}
