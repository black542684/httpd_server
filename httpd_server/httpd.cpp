#include "Http.h"


int main(void) {
	Http* app = getHttp();

	app->get("/query", [](Request& req, Response& res) {
		// TODO:

	});

	app->post("/list", [](Request& req, Response& res){
		// TODO:

	});

	app->listen(8080);

	return 0;
}

