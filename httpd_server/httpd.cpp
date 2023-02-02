#include "Http.h"


int main(void) {
	Http* app = getHttp();

	app->listen(8080);
	return 0;
}

