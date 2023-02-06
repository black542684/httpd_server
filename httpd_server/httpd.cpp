#include "Http.h"

int main(void) {
	Http* app = getHttp();
	// 静态资源存放路径
	app->setStatic("htdocs");
	// 设置跨域请求
	app->all("*", [](Request& req, Response& res) {
		res.header("Access-Control-Allow-Origin", "*");
		res.header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, token");
		res.header("Access-Control-Max-Age", "86400");
	});
	// 处理GET请求
	app->get("/query", [](Request& req, Response& res) {
		res.header("Content-Type", "text/plain;charset=UTF-8");
		res.send(u8"hello world");
	});
	// 处理POST请求
	app->post("/query", [](Request& req, Response& res){
		res.header("Content-Type", "application/json");
		JSON query = { // JSON插件只支持utf8格式字符串
			{ u8"data", u8"Hello World!" },
			{ u8"status", 200 },
			{ u8"message", u8"操作成功" }
		};
		string jsonStr = query.dump(); // JSON序列化
		res.send(jsonStr);
	});
	app->listen(8080);
	return 0;
}

