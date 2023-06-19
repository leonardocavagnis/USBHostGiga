#include "USBHost_H7.h"

USBHost_H7::USBHost_H7()
{}

USBHost_H7::~USBHost_H7()
{}

void USBHost_H7::begin() {
    pinMode(PA_15, OUTPUT);
    MX_USB_HOST_Init();
}

void USBHost_H7::end() 
{}

/** HID Handlers **/
RingBufferNGeneric<64, HID_KEYBD_Info_TypeDef> HIDKeyboard::rxBuffer;
RingBufferNGeneric<64, HID_MOUSE_Info_TypeDef> HIDMouse::rxBuffer;

extern "C" void USBH_HID_EventCallback(USBH_HandleTypeDef *phost) {
    /* if the HID is a Mouse */
	if(USBH_HID_GetDeviceType(phost) == HID_MOUSE) {
        HID_MOUSE_Info_TypeDef *mouse_info;
        mouse_info = USBH_HID_GetMouseInfo(phost);
        if (mouse_info != NULL) {
            HIDMouse::rxBuffer.store_elem(*mouse_info);
        }
    }
    /* if the HID is a Keyboard */
    if(USBH_HID_GetDeviceType(phost) == HID_KEYBOARD) {
        HID_KEYBD_Info_TypeDef *keybd_info;
        keybd_info = USBH_HID_GetKeybdInfo(phost);
        if (keybd_info != NULL) {
            HIDKeyboard::rxBuffer.store_elem(*keybd_info);
        }
    }
}

extern "C" void Error_Handler() {}

/** HID Keyboard **/
size_t HIDKeyboard::available() {
    return rxBuffer.available();
}

HID_KEYBD_Info_TypeDef HIDKeyboard::read() {
    return rxBuffer.read_elem();
}

char HIDKeyboard::getAscii(HID_KEYBD_Info_TypeDef evt) {
    return USBH_HID_GetASCIICode(&evt);
}

/** HID Mouse **/
size_t HIDMouse::available() {
    return rxBuffer.available();
}

HID_MOUSE_Info_TypeDef HIDMouse::read() {
    return rxBuffer.read_elem();
}

/** CDC Serial**/
extern "C" USBH_HandleTypeDef hUsbHostHS;
extern "C" ApplicationTypeDef Appli_state;

CDCSerial* _hostSerial = nullptr;
static uint8_t buf[64];
static CDC_LineCodingTypeDef linecoding;

extern "C" void USBH_CDC_ReceiveCallback(USBH_HandleTypeDef* phost) {   
    uint16_t data_len = USBH_CDC_GetLastReceivedDataSize(&hUsbHostHS); 
    data_len = (data_len > sizeof(buf)) ? sizeof(buf) : data_len;
    _hostSerial->rx_cb(buf, data_len);
}

void CDCSerial::begin(unsigned long baudrate, uint16_t config) {
    while (Appli_state != APPLICATION_READY) {
        delay(100);
    }
    _hostSerial = this;
 
    linecoding.b.dwDTERate      = baudrate;

    switch (config & SERIAL_DATA_MASK) {
        case SERIAL_DATA_7:
            linecoding.b.bDataBits = 7;
            break;
        case SERIAL_DATA_8:
            linecoding.b.bDataBits = 8;
            break;
        default: 
            linecoding.b.bDataBits = 8;
        break;
	}

    switch (config & SERIAL_STOP_BIT_MASK) {
        case SERIAL_STOP_BIT_1:
            linecoding.b.bCharFormat = 0;
            break;
        case SERIAL_STOP_BIT_2:
            linecoding.b.bCharFormat = 2;
            break;
        default: 
            linecoding.b.bCharFormat = 0;
            break;
    }

    switch (config & SERIAL_PARITY_MASK) {
        case SERIAL_PARITY_EVEN:
            linecoding.b.bParityType    = 2;
            break;
        case SERIAL_PARITY_ODD:
            linecoding.b.bParityType    = 1;
            break;
        case SERIAL_PARITY_NONE:
            linecoding.b.bParityType    = 0;
            break;
        default:
            linecoding.b.bParityType    = 0;
            break;
    }

    USBH_CDC_SetLineCoding(&hUsbHostHS, &linecoding);
    USBH_CDC_Receive(&hUsbHostHS, buf, sizeof(buf));
}

int CDCSerial::available() {
    _mut.lock();
    auto ret = rxBuffer.available();
    if (ret == 0 && Appli_state == APPLICATION_READY) {
        USBH_CDC_Receive(&hUsbHostHS, buf, sizeof(buf));
    }
    _mut.unlock();
    return ret;
}

int CDCSerial::read() {
    _mut.lock();
    auto ret = rxBuffer.read_char();
    _mut.unlock();
    return ret;
}

size_t CDCSerial::write(uint8_t) { };

void CDCSerial::rx_cb(uint8_t* data, size_t len) {
    _mut.lock();
    for (int i = 0; i < len; i++) {
        rxBuffer.store_char(data[i]);
    }
    _mut.unlock();
}