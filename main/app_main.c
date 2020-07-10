#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "bmp180.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "freertos/event_groups.h"
#include "esp_http_server.h"
#include "esp_event_loop.h"
#include "webserver.h"

/* Compensate altitude measurement
   using current reference pressure, preferably at the sea level,
   obtained from weather station on internet
   Assume normal air pressure at sea level of 101325 Pa
   in case weather station is not available.
 */
#define REFERENCE_PRESSURE 101325l

static const char *BMP180_I2C_LOG_TAG = "BMP180 I2C Read";

#define TAG "WiFi"

#define I2C_PIN_SDA 22
#define I2C_PIN_SCL 23

#define AP_SSID     "Estacao-Meteorologica"



bmp180_data_t bmp180_data;

bmp180_data_t get_bmp180_data(void) {
    return bmp180_data;
}



void bmp180_i2c_task(void *pvParameter)
{
    while(1) {
        esp_err_t err;


        err = bmp180_read_pressure(&bmp180_data.pressure);
        if (err != ESP_OK) {
            ESP_LOGE(BMP180_I2C_LOG_TAG, "Reading of pressure from BMP180 failed, err = %d", err);
        }
        err = bmp180_read_altitude(REFERENCE_PRESSURE, &bmp180_data.altitude);
        if (err != ESP_OK) {
            ESP_LOGE(BMP180_I2C_LOG_TAG, "Reading of altitude from BMP180 failed, err = %d", err);
        }
        err = bmp180_read_temperature(&bmp180_data.temperature);
        if (err != ESP_OK) {
            ESP_LOGE(BMP180_I2C_LOG_TAG, "Reading of temperature from BMP180 failed, err = %d", err);
        }
        //ESP_LOGI(BMP180_I2C_LOG_TAG, "Pressure %d Pa, Altitude %.1f m, Temperature : %.1f oC", pressure, altitude, temperature);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/**
 * @fn static esp_err_t event_handler(void *ctx, system_event_t *event)
 * @brief Handler para o driver do WiFi, toda vez que a FSM que controla o driver muda de estado esse handler é chamado
 * @param void *ctx - variavel passada pela configuração, nesse caso é um ponteiro para uma estrutura de WebServer
 * @param system_event_t *event - Evento recebido pelo driver da WiFi
 * @return esp_err_t - Retorno do handler
 */
static esp_err_t event_handler(void *ctx, system_event_t *event) {

    httpd_handle_t *server = (httpd_handle_t *) ctx;
    
    switch(event->event_id) {
    case SYSTEM_EVENT_AP_START:
	    ESP_LOGI(TAG,"EVENT AP START");
	    /* Start the web server */
		if (*server == NULL) { //Caso o ponteiro do servidor seja nulo, chama a função para iniciar o serviço web
		 	*server = start_webserver(); 
		}
		break;

    case SYSTEM_EVENT_AP_STOP:
        ESP_LOGI(TAG,"EVENT AP STOP");
        /* Stop the web server */
        stop_webserver(server); //Caso o AP pare, o serviço web também irá parar.

        break;

    case SYSTEM_EVENT_AP_STACONNECTED: //Quando um cliente conecta-se ao Access Point local
        ESP_LOGI(TAG,"SYSTEM_EVENT_AP_STACONNECTED");

        break;

    case SYSTEM_EVENT_AP_STADISCONNECTED: //Quando um cliente desconecta-se ao Access Point local
        ESP_LOGI(TAG,"SYSTEM_EVENT_AP_STADISCONNECTED");

        break;

    case SYSTEM_EVENT_AP_STAIPASSIGNED:
        ESP_LOGI(TAG,"SYSTEM_EVENT_AP_STAIPASSIGNED");
        break;

    default:
        ESP_LOGI(TAG,"default");
        break;
    }
    return ESP_OK;
}

void config_wifi(void *arg) {
    esp_err_t ret = nvs_flash_init(); //Inicializa Non-volatile storage
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) { //Se nao existem páginas livres
      ESP_ERROR_CHECK(nvs_flash_erase()); //Apaga a flash
      ret = nvs_flash_init(); //Reinicializa o nvs
    }
    ESP_ERROR_CHECK(ret);

    tcpip_adapter_init(); //Inicializa a pilha TCP/IP


    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg)); //Registra o handler do periférico

    wifi_init_config_t initcfg = WIFI_INIT_CONFIG_DEFAULT(); //Iniciliza as configurações wifi
	ESP_ERROR_CHECK(esp_wifi_init(&initcfg)); //Cria a tarefa e inicializa o driver da WiFi

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA)); //Seta o modo para AP+STA

    wifi_config_t wifi_configAP = {  
        .ap = { 
            .ssid = AP_SSID, 
            .max_connection = 3, 
            .authmode = WIFI_AUTH_OPEN 
        },
    }; //Configurações do AP

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_configAP)); //Seta as configurações do AP

    ESP_ERROR_CHECK(esp_wifi_start()); //Começa o driver do WiFi.
}
static httpd_handle_t server = NULL; //Handler do WebServer
void app_main()
{
    esp_err_t err = bmp180_init(I2C_PIN_SDA, I2C_PIN_SCL);
    if(err == ESP_OK){
        xTaskCreate(&bmp180_i2c_task, "bmp180_i2c_task", 1024*4, NULL, 5, NULL);
    } else {
        ESP_LOGE(BMP180_I2C_LOG_TAG, "BMP180 init failed with error = %d", err);
    }  

    config_wifi(&server);
}