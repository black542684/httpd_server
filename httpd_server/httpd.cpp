#include "Http.h"


int main(void) {
	Http* app = getHttp();
	
	// ��̬��Դ���·��
	app->setStatic("htdocs");

	// ���ÿ�������
	app->all("*", [](Request& req, Response& res) {
		res.header("Access-Control-Allow-Origin", "*");
		res.header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
		res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, token");
		res.header("Access-Control-Max-Age", "86400");
	});


	// ����GET����
	app->get("/query", [](Request& req, Response& res) {
		res.header("Content-Type", "text/plain;charset=UTF-8");
		res.send(u8"hello world");
	});

	// ����POST����
	app->post("/query", [](Request& req, Response& res){
		res.header("Content-Type", "application/json");
		JSON query = { // JSON���ֻ֧��utf8��ʽ�ַ���
			{ u8"data", u8"Hello World!" },
			{ u8"status", 200 },
			{ u8"message", u8"�����ɹ�" }
		};
		string jsonStr = query.dump(); // JSON���л�
		res.send(jsonStr);
	});

	app->listen(8080);

	return 0;
}

