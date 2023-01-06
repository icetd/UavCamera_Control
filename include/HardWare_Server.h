#ifndef HARDWARE_SERVER_H
#define HARDWARE_SERVER_H

struct cJSON;

struct uavParam {
    cJSON *uav_id;
    cJSON *auto_set;
    cJSON *light;
    cJSON *white_balance_temperature;
    cJSON *brightness;
    cJSON *contrast;
    cJSON *saturation;
    cJSON *hue;
    cJSON *sharpness;
    cJSON *backlight_compensation;
    cJSON *gamma;
    cJSON *exposure;
    cJSON *focus_absolute;
};


#endif
