# httpd_server
# 编译工具 Visual Studio 2019

### 项目结构
```
|-- httpd_server
    |-- httpd           项目入口文件
    |-- Http            Http类
    |-- Request         处理请求类
    |-- Response        处理响应类
    |-- FormDataField   post请求from-data用户提交的字段类
    |-- FormDataFile    post请求from-data用户上传的文件类
    |-- GZipAssistant   gzip压缩类
    |-- utils           工具函数
    |-- public_method   网络通信初始化、处理用户请求的线程 等函数
|-- /zlib               gzip压缩插件
|-- /nlohmann           JSON格式解析插件
|-- /tinyxml2           XML格式解析插件
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
