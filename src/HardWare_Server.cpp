#include "UsbCamera.h"
#include "udpServer.h"
#include "light.h"
#include "HardWare_Server.h"
#include <cJSON.h>
#include <stdio.h>
#include <string.h>


void uavGetParam(uavParam *param, cJSON* jsonrecv)
{
    param->uav_id = cJSON_GetObjectItem(jsonrecv, "UAV_ID");
    param->light = cJSON_GetObjectItem(jsonrecv, "LIGHT");
    param->auto_set = cJSON_GetObjectItem(jsonrecv, "AUTO_SET");
    param->white_balance_temperature = cJSON_GetObjectItem(jsonrecv, "WHITE_BALANCE_TEMPERATURE");
    param->brightness = cJSON_GetObjectItem(jsonrecv, "BRIGHTNESS");
    param->contrast = cJSON_GetObjectItem(jsonrecv, "CONTRAST");
    param->saturation = cJSON_GetObjectItem(jsonrecv, "SATURATION");
    param->hue = cJSON_GetObjectItem(jsonrecv, "HUE");
    param->sharpness = cJSON_GetObjectItem(jsonrecv, "SHARPNESS");
    param->backlight_compensation = cJSON_GetObjectItem(jsonrecv, "BACKLIGHT_COMPENSATION");
    param->gamma = cJSON_GetObjectItem(jsonrecv, "GAMMA");
    param->exposure = cJSON_GetObjectItem(jsonrecv, "EXPOSURE");
    param->focus_absolute = cJSON_GetObjectItem(jsonrecv, "FOCUS_ABSOLUTE");
}

void configUav(UsbCamera *uav, uavParam *param)
{            
	if (param->light)
		lightSet(0, param->light->valueint);

    if (param->auto_set) {
        if(param->auto_set->valueint) {
            uav->initialize();
        } else {
		if (param->white_balance_temperature)
                uav->setUavParam(WHITE_BALANCE_TEMPERATURE, param->white_balance_temperature->valueint);
            if (param->brightness)
                uav->setUavParam(BRIGHTNESS, param->brightness->valueint);
            if (param->contrast)
                uav->setUavParam(CONTRAST, param->contrast->valueint);
            if (param->saturation)
                uav->setUavParam(SATURATION, param->saturation->valueint);
            if (param->hue)
                uav->setUavParam(HUE, param->hue->valueint);
            if (param->sharpness)
                uav->setUavParam(SHARPNESS, param->sharpness->valueint);
            if (param->backlight_compensation)
                uav->setUavParam(BACKLIGHT_COMPENSATION, param->backlight_compensation->valueint);
            if (param->gamma)
                uav->setUavParam(GAMMA, param->gamma->valueint);
            if (param->exposure)
                uav->setUavParam(EXPOSURE, param->exposure->valueint);
            if (param->focus_absolute)
                uav->setUavParam(FOCUS_ABSOLUTE, param->focus_absolute->valueint);
        }
    }
}

int main(int argc, char **argv)
{
    UsbCamera *uav0 = new UsbCamera(0);
    UsbCamera *uav1 = new UsbCamera(2);
    UsbCamera *uav2 = new UsbCamera(4);
    UsbCamera *uav3 = new UsbCamera(6);
    lightInit(0);
	
    UdpServer *server = new UdpServer(6666);
    cJSON *jsonrecv = new cJSON;
    uavParam *uavparam = new uavParam;
	

    for(;;) {
        server->receiveData();
        if(server->recv_buf) {
            printf("%s\nlen:%d\n", server->recv_buf, server->recv_len);
            
            jsonrecv = cJSON_Parse(server->recv_buf);
            uavGetParam(uavparam, jsonrecv);

            if(uavparam->uav_id)
            {
                switch (uavparam->uav_id->valueint){
                case 0:
                    configUav(uav0, uavparam);
                    break;
                case 1:
                    configUav(uav1, uavparam);
                    break;
                case 2:
                    configUav(uav2, uavparam);
                    break;
                case 3:
                    configUav(uav3, uavparam);
                    break;
                default:
                    break;
                }
            }
            memset(server->recv_buf, 0, sizeof(server->recv_buf));
        }       
    }

    delete uav0;
    delete uav1;
    delete uav2;
    delete uav3
    delete server;
    delete jsonrecv;
    delete uavparam;
    return 0;
}
