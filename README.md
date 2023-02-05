# httpd_server
# 编译工具 Visual Studio 2019

### 项目结构
```
|-- httpd_server
    |-- httpd.cpp 项目入口
    |-- Http.cpp Http类
    |-- Request.cpp Request类
    |-- Response.cpp Response类
|-- /zlib  gzip压缩插件
|-- /nlohmann  JSON格式解析插件
|-- /tinyxml2 XML格式解析插件
```
### 项目设计
项目采用`socket`进行网络直接的进程通信，通过TCP协议进行解析HTTP请求，解析后的请求放在Request类中。

目前支持GET、POST、OPTIONS请求.    
支持GZIP压缩。  

POST请求支持:  
    -- `application/x-www-form-urlencoded`  
    -- `application/json`  
    -- `text/xml`  
    -- `multipart/form-data`  
