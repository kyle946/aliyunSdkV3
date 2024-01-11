#ifndef CURLREQUEST_H
#define CURLREQUEST_H


#include <stdio.h>
#include <curl/curl.h>
#include <string>
#include <iostream>
#include <map>
#include <vector>


struct curlrequestmemory {
    char *response;
    size_t size;
};

class curlrequest
{
public:
    curlrequest();

    void post(std::string urlstr, std::string inParam, std::vector<std::string> &headers, int &response_status, std::string &response_body);
    struct curlrequestmemory https_post_xwww_write_data = {};
    static size_t https_post_xwww_write_function(void *data, size_t size, size_t nmemb, void *clientp);

public:
    long connect_timeout=3;
    long response_code;
};

#endif // CURLREQUEST_H
