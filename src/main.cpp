#include "uart.h"
#include "i2c-ths.h"
#include "cconsole/console.h"

#include "drivers/i2c-lcd1602.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#define TAG "I2C_LIBTEST"

extern "C" void initialize_filesystem(void);

void lcd1602_task(void * pvParameter)
{
    // Set up I2C
    i2c_port_t i2c_num = I2C_NUM_0;
    uint8_t address = 0x27;

    // Set up the SMBus
    smbus_info_t * smbus_info = smbus_malloc();
    smbus_init(smbus_info, i2c_num, address);
    smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS);

    // Set up the LCD1602 device with backlight off
    i2c_lcd1602_info_t * lcd_info = i2c_lcd1602_malloc();
    i2c_lcd1602_init(lcd_info, smbus_info, true);

    // turn off backlight
    ESP_LOGI(TAG, "backlight off");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_set_backlight(lcd_info, false);

    // turn on backlight
    ESP_LOGI(TAG, "backlight on");
	vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_set_backlight(lcd_info, true);

    ESP_LOGI(TAG, "cursor on");
	vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_set_cursor(lcd_info, true);

    ESP_LOGI(TAG, "display A at 0,0");
	vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_move_cursor(lcd_info, 0, 0);
    i2c_lcd1602_write_char(lcd_info, 'A');

    ESP_LOGI(TAG, "display B at 8,0");
	vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_move_cursor(lcd_info, 8, 0);
    i2c_lcd1602_write_char(lcd_info, 'B');

    ESP_LOGI(TAG, "display C at 15,1");
	vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_move_cursor(lcd_info, 15, 1);
    i2c_lcd1602_write_char(lcd_info, 'C');

    ESP_LOGI(TAG, "move to 0,1 and blink");  // cursor should still be on
	vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_move_cursor(lcd_info, 0, 1);
    i2c_lcd1602_set_blink(lcd_info, true);

    ESP_LOGI(TAG, "display DE and move cursor back onto D")
	vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_write_char(lcd_info, 'D');
    i2c_lcd1602_set_right_to_left(lcd_info);
    i2c_lcd1602_write_char(lcd_info, 'E');
    i2c_lcd1602_set_left_to_right(lcd_info);

    ESP_LOGI(TAG, "disable display");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_set_display(lcd_info, false);

    ESP_LOGI(TAG, "display F at 7,1");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_move_cursor(lcd_info, 7, 1);
    i2c_lcd1602_write_char(lcd_info, 'F');

    ESP_LOGI(TAG, "enable display");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_set_display(lcd_info, true);

    ESP_LOGI(TAG, "disable blink");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_set_blink(lcd_info, false);  // cursor should still be on

    ESP_LOGI(TAG, "disable cursor");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_set_cursor(lcd_info, false);

    ESP_LOGI(TAG, "display alphabet at 0,0")  // should overflow to second line at "ABC..."
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_home(lcd_info);
    i2c_lcd1602_write_string(lcd_info, "abcdefghijklmnopqrstuvwxyz0123456789.,-+ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    ESP_LOGI(TAG, "scroll left 8 places slowly");
    vTaskDelay(pdMS_TO_TICKS(1000));
    for (int i = 0; i < 8; ++i)
    {
        i2c_lcd1602_scroll_display_left(lcd_info);
        vTaskDelay(200 / portTICK_RATE_MS);
    }

    ESP_LOGI(TAG, "scroll right 8 places quickly");
    vTaskDelay(pdMS_TO_TICKS(1000));
    for (int i = 0; i < 8; ++i)
    {
        i2c_lcd1602_scroll_display_right(lcd_info);
    }

    ESP_LOGI(TAG, "move to 8,0 and show cursor");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_move_cursor(lcd_info, 8, 0);
    i2c_lcd1602_set_cursor(lcd_info, true);

    ESP_LOGI(TAG, "move cursor 5 places to the right");
    vTaskDelay(pdMS_TO_TICKS(1000));
    for (int i = 0; i < 5; ++i)
    {
        i2c_lcd1602_move_cursor_right(lcd_info);
    }

    ESP_LOGI(TAG, "move cursor 3 places to the left");
    vTaskDelay(pdMS_TO_TICKS(1000));
    for (int i = 0; i < 3; ++i)
    {
        i2c_lcd1602_move_cursor_left(lcd_info);
    }

    ESP_LOGI(TAG, "enable auto-scroll and display >>>>>");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_set_auto_scroll(lcd_info, true);
    for (int i = 0; i < 5; ++i)
    {
        i2c_lcd1602_write_char(lcd_info, '>');
        vTaskDelay(200 / portTICK_RATE_MS);
    }

    ESP_LOGI(TAG, "change address counter to decrement (right to left) and display <<<<<");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_set_right_to_left(lcd_info);
    for (int i = 0; i < 5; ++i)
    {
        i2c_lcd1602_write_char(lcd_info, '<');
        vTaskDelay(200 / portTICK_RATE_MS);
    }

    ESP_LOGI(TAG, "disable auto-scroll and display +++++");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_set_auto_scroll(lcd_info, false);
    for (int i = 0; i < 5; ++i)
    {
        i2c_lcd1602_write_char(lcd_info, '+');
        vTaskDelay(200 / portTICK_RATE_MS);
    }

    ESP_LOGI(TAG, "set left_to_right and display >>>>>");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_set_left_to_right(lcd_info);
    for (int i = 0; i < 5; ++i)
    {
        i2c_lcd1602_write_char(lcd_info, '>');
        vTaskDelay(200 / portTICK_RATE_MS);
    }

    ESP_LOGI(TAG, "clear display and disable cursor");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_clear(lcd_info);
    i2c_lcd1602_set_cursor(lcd_info, false);

    ESP_LOGI(TAG, "create custom character and display");
    vTaskDelay(pdMS_TO_TICKS(1000));
    // https://github.com/agnunez/ESP8266-I2C-LCD1602/blob/master/examples/CustomChars/CustomChars.ino
    uint8_t bell[8]  = {0x4, 0xe, 0xe, 0xe, 0x1f, 0x0, 0x4};
    uint8_t note[8]  = {0x2, 0x3, 0x2, 0xe, 0x1e, 0xc, 0x0};
    uint8_t clock[8] = {0x0, 0xe, 0x15, 0x17, 0x11, 0xe, 0x0};
    uint8_t heart[8] = {0x0, 0xa, 0x1f, 0x1f, 0xe, 0x4, 0x0};
    uint8_t duck[8]  = {0x0, 0xc, 0x1d, 0xf, 0xf, 0x6, 0x0};
    uint8_t check[8] = {0x0, 0x1 ,0x3, 0x16, 0x1c, 0x8, 0x0};
    uint8_t cross[8] = {0x0, 0x1b, 0xe, 0x4, 0xe, 0x1b, 0x0};
    uint8_t retarrow[8] = { 0x1, 0x1, 0x5, 0x9, 0x1f, 0x8, 0x4};
    i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_0, bell);
    i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_1, note);
    i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_2, clock);
    i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_3, heart);
    i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_4, duck);
    i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_5, check);
    i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_6, cross);
    i2c_lcd1602_define_char(lcd_info, I2C_LCD1602_INDEX_CUSTOM_7, retarrow);

    // after defining custom characters, DDRAM address must be set by home() or moving the cursor

    ESP_LOGI(TAG, "display custom characters");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_move_cursor(lcd_info, 0, 0);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_0);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_1);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_2);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_3);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_4);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_5);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_6);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_CUSTOM_7);

    ESP_LOGI(TAG, "display special characters");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_move_cursor(lcd_info, 0, 1);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_ALPHA);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_BETA);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_THETA);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_PI);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_OMEGA);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_SIGMA);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_INFINITY);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_DEGREE);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_ARROW_LEFT);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_ARROW_RIGHT);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_SQUARE);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_DOT);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_DIVIDE);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_BLOCK);

    ESP_LOGI(TAG, "display all characters");
    vTaskDelay(pdMS_TO_TICKS(1000));
    i2c_lcd1602_clear(lcd_info);
    i2c_lcd1602_set_cursor(lcd_info, true);
    uint8_t c = 0;
    uint8_t col = 0;
    uint8_t row = 0;
    while (1)
    {
        i2c_lcd1602_write_char(lcd_info, c);
        vTaskDelay(100 / portTICK_RATE_MS);
        ESP_LOGI(TAG, "col %d, row %d, char 0x%02x", col, row, c);
        ++c;
        ++col;
        if (col >= I2C_LCD1602_NUM_VISIBLE_COLUMNS)
        {
            ++row;
            if (row >= I2C_LCD1602_NUM_ROWS)
            {
                row = 0;
            }
            col = 0;
            i2c_lcd1602_move_cursor(lcd_info, col, row);
        }
    }

    vTaskDelete(NULL);
}

extern "C" void app_main()
{
	initialize_filesystem();

	/* Setup thread-safe i2c */
	i2c_master_setup(I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22, 400000);

	console.HandleCmd(uart, "auth 123");
	console.HandleCmd(uart, "wifi init");
	console.HandleCmd(uart, "wifi connect Faramoz 12169931");
	console.HandleCmd(uart, "tcp 0");

	lcd1602_task(NULL);

	while(1)
		console.WaitForCmd(uart);

	/* Release thread-safe i2c and semaphore */
	i2c_release(I2C_NUM_0);
}
