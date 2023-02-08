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
    |-- Algorithm       工具栏，包含一些算法
|-- /zlib               gzip压缩插件
|-- /nlohmann           JSON格式解析插件
|-- /tinyxml2           XML格式解析插件
```
### 项目设计
项目采用`socket`进行网络之间的进程通信，传输层采用TCP协议。收到客户端的HTTP请求之后，进行解析后放在Request类中，Response类用来返回数据给客户端。

目前支持GET、POST、OPTIONS请求.    
支持GZIP压缩、XML数据提交、JSON数据提交。  

POST请求支持:  
    -- `application/x-www-form-urlencoded`  
    -- `application/json`  
    -- `text/xml`  
    -- `multipart/form-data`  
