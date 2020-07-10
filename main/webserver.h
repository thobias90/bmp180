#include "esp_http_server.h"

void stop_webserver(httpd_handle_t server);
httpd_handle_t start_webserver(void);
uint32_t filter(uint32_t sample);