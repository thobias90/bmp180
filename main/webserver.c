#include "esp_log.h"				//ESP32 logging library
#include "freertos/FreeRTOS.h"		//FreeRTOS base library
#include "freertos/task.h"			//FreeRTOS tasks library
#include "webserver.h"
#include <cJSON.h>
#include "bmp180.h"

#define TAG "WS"


uint32_t lastPressure = 0;

uint32_t filter(uint32_t sample) {
    if(sample-7 > lastPressure || lastPressure-7 > sample) {
        lastPressure = sample;
        return sample;
    }

    else {
        return lastPressure;
    }
}

/**
 * @fn esp_err_t get_menu(httpd_req_t *req)
 * @brief Handler para URI de menu inicial
 * @param httpd_req_t *req - Estrutura para dados da requisição HTTP
 * @return esp_err_r - Retorno de erro
 */
esp_err_t get_menu(httpd_req_t *req) {
    //Dado inserido na memória através do component.mk
    extern const uint8_t menu_html_start[] asm("_binary_menu_html_start");
    extern const uint8_t menu_html_end[] asm("_binary_menu_html_end");
    //ESP_LOGI(TAG, "URI root");
    
    
	/* Send response with custom headers and body set as the
	 * string passed in user context*/

	httpd_resp_send(req, (const char *)menu_html_start, strlen((char *)menu_html_start));

	return ESP_OK;
}

/**
 * @fn esp_err_t get_bootstrap(httpd_req_t *req)
 * @brief Handler para URI do bootstrap (Elementos de CSS para embelezar códigos HTML)
 * @param httpd_req_t *req - Estrutura para dados da requisição HTTP
 * @return esp_err_r - Retorno de erro
 */
esp_err_t get_bootstrap(httpd_req_t *req) {
    extern const uint8_t bootstrap_min_css_start[] asm("_binary_bootstrap_min_css_start");
    extern const uint8_t bootstrap_min_css_end[] asm("_binary_bootstrap_min_css_end");
    //ESP_LOGI(TAG, "get_bootstrap");     

	/* Send response with custom headers and body set as the
	 * string passed in user context*/	 
    httpd_resp_set_type(req,"text/css");

	httpd_resp_send(req, (const char *)bootstrap_min_css_start, strlen((char *)bootstrap_min_css_start));

	return ESP_OK;
}

esp_err_t get_bmpdata(httpd_req_t *req) {
    cJSON *root = cJSON_CreateObject();

    char buff[8];
    sprintf(buff,"%.2f",get_bmp180_data().temperature);
    cJSON_AddItemToObject(root,"temperature",cJSON_CreateString(buff));
    cJSON_AddItemToObject(root,"pressure",cJSON_CreateNumber(get_bmp180_data().pressure));
    sprintf(buff,"%.2f",get_bmp180_data().altitude);
    cJSON_AddItemToObject(root,"altitude",cJSON_CreateString(buff));
    cJSON_AddItemToObject(root,"filterValue",cJSON_CreateNumber(filter(get_bmp180_data().pressure)));

    uint8_t *JSON = NULL;
    JSON = (uint8_t *)cJSON_PrintUnformatted(root);

    httpd_resp_send(req, (const char *)JSON, strlen((char *)JSON));

    free(JSON);
    return ESP_OK;
}


httpd_uri_t menu = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = get_menu,
    .user_ctx = NULL
};

httpd_uri_t bootstrap = {
	.uri = "/bootstrap.min.css",
	.method = HTTP_GET,
	.handler = get_bootstrap,
	.user_ctx = NULL
};

httpd_uri_t getBMPData = {
    .uri = "/getBMPData",
    .method = HTTP_GET,
    .handler = get_bmpdata,
    .user_ctx = NULL
};

httpd_handle_t start_webserver(void) {
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 16;
    // Start the httpd server
	ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
	if (httpd_start(&server, &config) == ESP_OK) {
		// Set URI handlers
		ESP_LOGI(TAG, "Registering URI handlers");
        if (httpd_register_uri_handler(server, &menu) == ESP_OK) {
		    ESP_LOGI(TAG,"URI menu get registered..");
		}
        if (httpd_register_uri_handler(server, &bootstrap) == ESP_OK) {
            ESP_LOGI(TAG,"URI bootstrap get registered..");
        }
        if (httpd_register_uri_handler(server, &getBMPData) == ESP_OK) {
            ESP_LOGI(TAG,"URI getBMPData get registered..");
        }
        return server;
    }
    return NULL;
}

void stop_webserver(httpd_handle_t server) {
	// Stop the httpd server
    if(server) {
	    ESP_LOGI(TAG,"httpd_stop");
        if(httpd_stop(server) == ESP_OK)
            ESP_LOGI(TAG,"HTTP STOP SUCCESS");
        else
            ESP_LOGE(TAG,"HTTP STOP FAIL");
    }
}