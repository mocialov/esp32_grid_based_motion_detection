#define CAMERA_MODEL_AI_THINKER

#include "esp_camera.h"
#include "camera_pins.h"
#include <numeric>

#define FRAME_SIZE FRAMESIZE_QVGA
#define WIDTH 320
#define HEIGHT 240

#define DEBUG 1


uint8_t arrays[16*12][20*20] = {0};

uint8_t pixel_averages[16*12] = {0};

bool setup_camera(framesize_t);



void setup() {
    Serial.begin(115200);
    Serial.println(setup_camera(FRAME_SIZE) ? "OK" : "ERR INIT");
}


void loop() {

    camera_fb_t *frame_buffer = esp_camera_fb_get();

    if (!frame_buffer)
        Serial.println("No image");

    size_t windowSize = 20;


    size_t hCount = (WIDTH-1) / windowSize + 1;
    size_t vCount = (HEIGHT-1) / windowSize + 1;


    int sizes [12*16] = {0};

    for (uint32_t j = 0; j < (HEIGHT * WIDTH); j++){
        uint8_t b  = j / WIDTH / windowSize * hCount + j % WIDTH / windowSize;

        arrays[b][sizes[b]] = frame_buffer->buf[j];

        sizes[b] = sizes[b] + 1;
    }


    for (int j = 0; j < hCount*vCount; j++){
        int sum = 0;
        for(int i = 0; i < sizes[j]; i++){
            sum += arrays[j][i];
        }
        int avg = sum / sizes[j];

        if (std::abs(pixel_averages[j] - avg) > 50){
            Serial.printf("change in %d %d \n", j / hCount, j % vCount);
        }

        pixel_averages[j] = avg;
    }

    Serial.println("");

    delay(5000);

}


/**
 *
 */
bool setup_camera(framesize_t frameSize) {
    camera_config_t config;

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_GRAYSCALE;
    config.frame_size = frameSize;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    bool ok = esp_camera_init(&config) == ESP_OK;

    sensor_t *sensor = esp_camera_sensor_get();
    sensor->set_framesize(sensor, frameSize);

    return ok;
}