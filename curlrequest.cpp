#include "curlrequest.h"

curlrequest::curlrequest()
{

}



void curlrequest::post(std::string urlstr, std::string inParam, std::vector<std::string> &headers, int &response_status, std::string &response_body)
{
//    std::string urlstr="https://vildoc.com/vildoc/";

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, urlstr.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, inParam.c_str());

#ifdef SKIP_PEER_VERIFICATION
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

        /* set custom headers */
        struct curl_slist *slist1 = NULL;
        slist1 = curl_slist_append(slist1, "Accept: application/json");
        if(headers.size()>0){
            for(std::string s : headers){
                slist1 = curl_slist_append(slist1, s.c_str());
            }
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);

        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connect_timeout);
        curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, https_post_xwww_write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&https_post_xwww_write_data);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else{
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
//            std::cout << "Response code: " << response_code << std::endl;
//            std::cout<<https_post_xwww_write_data.response<<std::endl;
        }

        //
        response_status=response_code;
        response_body=https_post_xwww_write_data.response;

        free(https_post_xwww_write_data.response);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

size_t curlrequest::https_post_xwww_write_function(void *data, size_t size, size_t nmemb, void *clientp)
{
    size_t realsize = size * nmemb;
    struct curlrequestmemory *mem = (struct curlrequestmemory *)clientp;
    char *ptr = (char *)realloc(mem->response, mem->size + realsize + 1);
    if(!ptr)
        return 0;  /* out of memory! */
    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;
    return realsize;
}
