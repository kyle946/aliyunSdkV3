#include <QCoreApplication>

#include "aliyunRequest.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    /// 阿里云SDK V3版签名请求C++版
    /// 阿里云V3版签名文档地址：https://help.aliyun.com/zh/sdk/product-overview/v3-request-structure-and-signature

    aliyunRequest::getInstance()->setAccessKeySecret("xx", "xx");
    aliyunRequest::getInstance()->sendsms("8618600000000", "123456", "xx", "xx");
    aliyunRequest::releaseInstance();

    return a.exec();
}
