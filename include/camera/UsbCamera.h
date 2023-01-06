#ifndef USBCAMERA_H
#define USBCAMERA_H

#include <linux/videodev2.h>
#include <stdint.h>
#include <string>

enum UavMode {
    WHITE_BALANCE_AUTO,
    EXPOSURE_AUTO,
    FOCUS_AUTO,
};

enum ControlType {
    WHITE_BALANCE_TEMPERATURE,
    BRIGHTNESS,
    CONTRAST,
    SATURATION,
    HUE,
    SHARPNESS,
    BACKLIGHT_COMPENSATION,
    GAMMA,
    EXPOSURE,
    FOCUS_ABSOLUTE
};

class UsbCamera
{
public:
    v4l2_capability capability;
    v4l2_fmtdesc fmtdesc;
    v4l2_frmsizeenum frmsizeenum;
    v4l2_streamparm streamparm;
    v4l2_control ctrl;

public:
    UsbCamera(uint8_t _id); 
    ~UsbCamera();

    bool initialize();
    bool destroy();

    void checkUavAbility();
    void getUavFormatInfo();
    int getCurrentRate();
    bool setUavMode(UavMode uavMode, int autoMode);
    bool getUavMode(UavMode uavmode);
    bool setUavParam(ControlType type, int value);
    //bool getUavParam();
private:
    uint8_t uav_id;
    std::string devName;
    int uav_fd;
};

#endif