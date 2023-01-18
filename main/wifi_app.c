#include "freertos/freeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"

#include "tasks_common.h"
#include "wifi_app.h"

static const char TAG [] = "wifi_app";

// Queue handle used to manipulate the main queue of events
static QueueHandle_t wifi_app_queue_handle;

esp_netif_t* esp_netif_sta = NULL;
esp_netif_t* esp_netif_ap = NULL;

/**
 * Main Task the Wifi application
 * @param pvParameters parameter witch can be passed to the task
*/

static void wifi_app_task(void *pvParameters)
{
    wifi_app_queue_message_t msg;

    // Initialize the event handler
    wifi_app_event_handler_init();

    // iinitialize TCP/IP stack and wifi config
    wifi_app_default_wifi_init();

    // softAP config
    wifi_app_soft_ap_config();

    // Start wifi
    ESP_ERROR_CHECK(esp_wifi_start());

    // Send first event message

    wifi_app_send_message(WIFI_APP_MSG_START_HTTP_SERVER);

    for(;;)
    {
        if (xQueueReceiv(wifi_app_queue_handle, &msg, portMAX_DELAY))
        {
            switch(msg.msgID)
            {
                case WIFI_APP_MSG_START_HTTP_SERVER:
                    ESP_LOGI(TAG, "WIFI_APP_MSG_START_HTTP_SERVER");
                    break;
                
                case WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER:
                    ESP_LOGI(TAG, "WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER");

                    break;
                
                case WIFI_APP_MSG_STA_CONNECTED_GOT_IP;
                    ESP_LOGI(TAG,"WIFI_APP_MSG_STA_CONNECTED_GOT_IP");
                    break;
                
                default:
                    break;
            }
        }
    }
}


BaseType_t wifi_app_send_message(wifi_app_message_e msgID)
{
    wifi_app_queue_message_t msg;
    msg.msgID = msgID;
    return xQueueSend(wifi_app_queue_handle, &msg, portMAX_DELAY);
}

void wifi_app_start(void)
{
    ESP_LOGI(TAG, "Starting wifi aplication");

    // Start wii started LED

    // Disable default wifi logging messages
    esp_log_level_set("wifi",ESP_LOG_NONE);

    // Create message queue
    wifi_app_queue_handle = xQueueCreate(3, sizeof(wifi_app_queue_message_t));

    // Start wifi application task
    xTaskCreatePinnedToCore(&wifi_app_task, "wifi_app_task",WIFI_APP_TASK_STACK_SIZE, NULL, WIFI_APP_TASK_PRIORITY, NULL, WIFI_APP_TASK_CORE_ID);
}