#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "tasks_common.h"
#include "wifi_app.h"
#include "wifi_reset_button.h"

static const char TAG[] = "wifi_reset_button";

// Sempahore handle
SemaphoreHandle_t wifi_reset_semaphore = NULL;

/**
 * ISR handler for the wifi reset boot button
 * @param arg parameter wich can be passed to the isr handler
*/
void IRAM_ATTR wifi_reset_button_isr_handler(void *arg)
{
    // Notify the button task
    xSemaphoreGiveFromISR(wifi_reset_semaphore, NULL);
}


/**
 * Wifi reset button task reactos to a boot button event by sending a message to the wifi application to disconnect from wifiand clear the saved credentials
 * @param pvPaam parameter wich can be passed to the task
*/
void wifi_reset_button_task(void *pvParam)
{
    while(1)
    {
        if (xSemaphoreTake(wifi_reset_semaphore, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(TAG, "WIFI RESET BUTTON INTERRPUT OCCURED");

            //Send a message to disconnect wifi and clear credentials
            wifi_app_send_message(WIFI_APP_MSG_USER_REQUESTED_STA_DISCONNECT);

            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
}

void wifi_reset_button_config(void)
{
    // Create the binary semaphore
    wifi_reset_semaphore = xSemaphoreCreateBinary();

    // Configure the button
    esp_rom_gpio_pad_select_gpio(WIFI_RESET_BUTTON);
    gpio_set_direction(WIFI_RESET_BUTTON, GPIO_MODE_INPUT);

    // Enable interruption
    gpio_set_intr_type(WIFI_RESET_BUTTON, GPIO_INTR_NEGEDGE);

    // Create the wifi reset button task
    xTaskCreatePinnedToCore(&wifi_reset_button_task, "wifi_reset_button", WIFI_RESET_BUTTON_TASK_STACK_SIZE, NULL, WIFI_RESET_BUTTON_TASK_PRIORITY, NULL, WIFI_RESET_BUTTON_TASK_CORE_ID);

    // Install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    // Attach the interrupt service routine
    gpio_isr_handler_add(WIFI_RESET_BUTTON, wifi_reset_button_isr_handler, NULL);
}