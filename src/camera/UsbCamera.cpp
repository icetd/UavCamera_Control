#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "UsbCamera.h"

/** @param _id the video Index*/
UsbCamera::UsbCamera(uint8_t _id) 
    : uav_id(_id)
{
    initialize();
}

UsbCamera::~UsbCamera()
{
    close(uav_fd);
}

/** @brief initialize usb camera 
 *  @return true / flase
*/
bool UsbCamera::initialize()
{
    std::string devName = "/dev/video" + std::to_string(uav_id);
    uav_fd = open(devName.c_str(), O_RDWR);

    if (uav_fd < 0) {
        printf("open video device %d falied.\n", uav_id);
        return false;
    }

    checkUavAbility();
    getUavFormatInfo();
    getCurrentRate();
 
    setUavMode(WHITE_BALANCE_AUTO, V4L2_WHITE_BALANCE_AUTO);
    getUavMode(WHITE_BALANCE_AUTO);

    setUavMode(EXPOSURE_AUTO, V4L2_EXPOSURE_APERTURE_PRIORITY);
    getUavMode(EXPOSURE_AUTO);

    setUavMode(FOCUS_AUTO, V4L2_AUTO_FOCUS_RANGE_NORMAL);
    getUavMode(FOCUS_AUTO);

    return true;
}

/** @brief destroy uav camera 
 *  @return true / false
*/
bool UsbCamera::destroy()
{
    int ret;
    ret = close(uav_fd);
    if (ret < 0) {
        printf("clsoe video device %d failed.\n", uav_id);
        return false;
    }
    return true;
}

/** @brief check the usb camera ability*/
void UsbCamera::checkUavAbility()
{
    printf("=====================================================\n");
    
    if (ioctl(uav_fd, VIDIOC_QUERYCAP, &capability) < 0) 
        perror("get uav ability failed.\n");

    printf("Driver Name:%s\nCard Name:%s\nBus info:%s\nversion:%d\ncapabilities:%x\n", 
                capability.driver, capability.card, capability.bus_info, capability.version, capability.capabilities);

    if ((capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE) {
        printf("Device %s: supports capture.\n", devName.c_str());
    }
    if ((capability.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING) {
        printf("Device %s: supports streaming.\n", devName.c_str());
    }

    printf("=====================================================\n");
}

/** @brief get usb camera format support
 *  print to the screen
*/
void UsbCamera::getUavFormatInfo()
{
    for (int i = 0; ; ++i) {
        fmtdesc.index = i;
        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        int ret = ioctl(uav_fd, VIDIOC_ENUM_FMT, &fmtdesc);
        if (ret < 0)
            break;
        printf("Index[%d] Picture Format:%s\n", fmtdesc.index, fmtdesc.description);

        frmsizeenum.pixel_format = fmtdesc.pixelformat;
        for(int j = 0; ;++j) {
            frmsizeenum.index = j;
            ret = ioctl(uav_fd, VIDIOC_ENUM_FRAMESIZES, &frmsizeenum);
            if (ret < 0)
                break;
            printf("Size: %dx%d\n", 
                    frmsizeenum.discrete.width, frmsizeenum.discrete.height);
        }
    }
}

/** @brief get usb camera current fps rate*/
int UsbCamera::getCurrentRate()
{
    /*get current frame rate*/
    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(uav_fd, VIDIOC_G_PARM, &streamparm) == -1) {
        perror("ioctl");
        return -1;
    }
    printf("\nCreate Frame rate: [%u/%u]\n", 
            streamparm.parm.capture.timeperframe.numerator, streamparm.parm.capture.timeperframe.denominator);
    return 0;
}

/** @brief set uav mode
 *  @param uavMode:
 *         enum | WHITE_BALANCE_AUTO
 *              | EXPOSURE_AUTO 
 *              | FOCUS_AUTO |
 *  
 *  @param autoMode: value of set
*/
bool UsbCamera::setUavMode(UavMode uavMode, int autoMode)
{
    int ret;
    switch (uavMode) {
    case WHITE_BALANCE_AUTO: {
            ctrl.id = V4L2_CID_AUTO_WHITE_BALANCE;
            ctrl.value = autoMode;
            ret = ioctl(uav_fd, VIDIOC_S_CTRL, &ctrl);
            if (ret < 0) {
                printf("Set uav white balance mode failed,\n \
                        This UAV Don't support auto white balance\n");
                return false;
            }
            break;
        }

    case EXPOSURE_AUTO: {
            ctrl.id = V4L2_CID_EXPOSURE_AUTO;
            ctrl.value = autoMode;
            ret = ioctl(uav_fd, VIDIOC_S_CTRL, &ctrl);
            if (ret < 0) {
                printf("Set uav exposure mode failed.\n \
                        This UAV Don't support auto exposure\n");
                return false;
            }
            break;
        }

    case FOCUS_AUTO: {
            ctrl.id = V4L2_CID_FOCUS_AUTO;
            ctrl.value = autoMode;
            ret = ioctl(uav_fd, VIDIOC_S_CTRL, &ctrl);
            if (ret < 0) {
                printf("Set uav focus mode failed.\n \
                        This UAV Don't support auto focus\n");
                return false;
            }
            break;
        }

    default: 
        break;
    }

    return true;
}

/** @brief get the UAV Auto mode */
bool UsbCamera::getUavMode(UavMode uavMode)
{
    int ret;
    switch (uavMode) {
    case WHITE_BALANCE_AUTO: {
            ctrl.id = V4L2_CID_AUTO_WHITE_BALANCE;
            ret = ioctl(uav_fd, VIDIOC_G_CTRL, &ctrl);
            if (ret < 0) {
                printf("get uav white balance mode falied.\n");
                return false;
            }

            printf((ctrl.value) ? ("uav white balance mode is AUTO.\n")
                               : ("uav white balance mode is MANUAL.\n"));
            break;
        }
 
    case EXPOSURE_AUTO: {
            ctrl.id = V4L2_CID_EXPOSURE_AUTO;
            ret = ioctl(uav_fd, VIDIOC_G_CTRL, &ctrl);
            if (ret < 0) {
                printf("get uav exposure mode falied\n");
                return false;
            }
            if (ctrl.value == 3) {
                printf("uav exposure mode is SHUTTER_PRIORITY\n");
            } else if (ctrl.value == 2) {
                printf("uav exposure mode is APERTURE_PRIORITY\n");
            } else {
                printf( (ctrl.value) ? ("uav exposure mode is MANUAL.\n") 
                             : ("uav exposure mode is AUTO.\n"));
            }
            break;
        }

    case FOCUS_AUTO: {
            ctrl.id = V4L2_CID_FOCUS_AUTO;
            ret = ioctl(uav_fd, VIDIOC_G_CTRL, &ctrl);
            if (ret < 0) {
                printf("get uav focus mode failed\n");
                return false;
            }
            printf((ctrl.value) ? ("uav focus mode is MANUAL.\n")
                            : ("uav focuse mode is AUTO.\n"));
            break;
        }
    }

    return true;
}

/** @brief set usb camera param 
 *  @param type enum:
 *      WHITE_BALANCE_TEMPERATURE: 4600[2800, 6500],
 *      BRIGHTNESS 0[-64, 64],
 *      CONTRAST 40[0, 100],
 *      SATURATION 64[0, 100],
 *      HUE 0[-180, 180],
 *      SHARPNESS 50[0, 100],
 *      BACKLIGHT_COMPENSATION 0[0, 2],
 *      GAMMA 300[100 500],
 *      EXPOSURE 166[5, 10000],
 *      FOCUS_ABSOLUTE 68[0 1023]
*/
bool UsbCamera::setUavParam(ControlType type, int value)
{
    int ret;
    switch (type) {
    case WHITE_BALANCE_TEMPERATURE: {
            setUavMode(WHITE_BALANCE_AUTO, V4L2_WHITE_BALANCE_MANUAL);
            ctrl.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE;
            ctrl.value = value;
            ret = ioctl(uav_fd, VIDIOC_S_CTRL, &ctrl);
            if (ret < 0) {
                printf("Set uav white balance temperature failed.\n\
                    Setting this parameter is not supported.\n");
                return false;
             }
            break;
        }
    case BRIGHTNESS: {
            setUavMode(WHITE_BALANCE_AUTO, V4L2_WHITE_BALANCE_MANUAL);
            ctrl.id = V4L2_CID_BRIGHTNESS;
            ctrl.value = value;
            ret = ioctl(uav_fd, VIDIOC_S_CTRL, &ctrl);
            if (ret < 0) {
                printf("Set uav brightness falied.\n\
                    Setting this parameter is not supported.\n");
                return false;
            }
            break;
        }
    case CONTRAST: {
            setUavMode(WHITE_BALANCE_AUTO, V4L2_WHITE_BALANCE_MANUAL);
            ctrl.id = V4L2_CID_CONTRAST;
            ctrl.value = value;
            ret = ioctl(uav_fd, VIDIOC_S_CTRL, &ctrl);
            if (ret < 0) {
                printf("Set uav contrast falised.\n\
                    Setting this paramter is not suppoted.\n");
                return false;
            }
            break;
        }
    case SATURATION: {
            setUavMode(WHITE_BALANCE_AUTO, V4L2_WHITE_BALANCE_MANUAL);
            ctrl.id = V4L2_CID_SATURATION;
            ctrl.value = value;
            ret = ioctl(uav_fd, VIDIOC_S_CTRL, &ctrl);
            if (ret < 0) {
                printf("Set uav saturation falied\n\
                    Setting this paramter is not supported\n");
                return false;
            }
            break;
        }
    case HUE: {
            setUavMode(WHITE_BALANCE_AUTO, V4L2_WHITE_BALANCE_MANUAL);
            ctrl.id = V4L2_CID_HUE;
            ctrl.value = value;
            ret = ioctl(uav_fd, VIDIOC_S_CTRL, &ctrl);
            if (ret < 0) {
                printf("Set uav hue failed\n\
                    Setting this paramter is not supported\n");
                return false;
            }
            break;
        }
    case SHARPNESS: {
            setUavMode(WHITE_BALANCE_AUTO, V4L2_WHITE_BALANCE_MANUAL);
            ctrl.id = V4L2_CID_SHARPNESS;
            ctrl.value = value;
            ret = ioctl(uav_fd, VIDIOC_S_CTRL, &ctrl);
            if(ret < 0) {
                printf("Set uav shapbess failed.\n\
                    Setting this paramter is not supported\n");
                return false;
            }
            break;
        }
    case BACKLIGHT_COMPENSATION: {
            setUavMode(WHITE_BALANCE_AUTO, V4L2_WHITE_BALANCE_MANUAL);
            ctrl.id = V4L2_CID_BACKLIGHT_COMPENSATION;
            ctrl.value = value;
            ret = ioctl(uav_fd, VIDIOC_S_CTRL, &ctrl);
            if (ret < 0) {
                printf("Set uav backlight compensation failed\n\
                    Setting this paramter is not supported\n");
                return false;
            }
            break;
        }
 
    case GAMMA: {
            setUavMode(WHITE_BALANCE_AUTO, V4L2_WHITE_BALANCE_MANUAL);
            ctrl.id = V4L2_CID_GAMMA;
            ctrl.value = value;
            ret = ioctl(uav_fd, VIDIOC_S_CTRL, &ctrl);
            if (ret < 0) {
                printf("Set uav gamma failed\n\
                    Setting this paramter is not supported\n");
                return false;
            }
            break;
        }
    case EXPOSURE: {
            setUavMode(EXPOSURE_AUTO, V4L2_EXPOSURE_MANUAL);
            ctrl.id = V4L2_CID_EXPOSURE_ABSOLUTE;
            ctrl.value = value;
            ret = ioctl(uav_fd, VIDIOC_S_CTRL, &ctrl);
            if(ret < 0) {
                printf("Set uav exposure failed\n\
                    Setting this paramter is not supported\n");
                return false;
            }
            break;
        }
     case FOCUS_ABSOLUTE: {
            setUavMode(FOCUS_AUTO, V4L2_AUTO_FOCUS_RANGE_AUTO);
            ctrl.id = V4L2_CID_FOCUS_ABSOLUTE;
            ctrl.value = value;
            ret = ioctl(uav_fd, VIDIOC_S_CTRL, &ctrl);
            if(ret < 0) {
                printf("Set uav focus avsolute failed\n\
                    Setting this paramter is not supported\n");
                return false;
            }
            break;
        }
 
 
    default:
        break;
    }

    return true;   
}

#if 0
bool UsbCamera::getUavParam()
{

}
#endif