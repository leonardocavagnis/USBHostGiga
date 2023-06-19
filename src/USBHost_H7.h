#include "Arduino.h"
#include "mbed.h"

#include "usb_host.h"
#include "usbh_def.h"

#include "usbh_hid_keybd.h"
#include "usbh_hid_mouse.h"
#include "usbh_cdc.h"

#include "ring_buffer_generic.h"

#define HIDMouse_evt    HID_MOUSE_Info_TypeDef   
#define HIDKeyboard_evt HID_KEYBD_Info_TypeDef

class USBHost_H7 {
    public:
        USBHost_H7();
        ~USBHost_H7();
        void begin();
        void end();
};

class HIDKeyboard {
    public:
        size_t available();
        HID_KEYBD_Info_TypeDef read();
        char getAscii(HID_KEYBD_Info_TypeDef evt);
        static RingBufferNGeneric<64, HID_KEYBD_Info_TypeDef> rxBuffer;
};

class HIDMouse {
    public:
        size_t available();
        HID_MOUSE_Info_TypeDef read();
        static RingBufferNGeneric<64, HID_MOUSE_Info_TypeDef> rxBuffer;
};

class CDCSerial : public arduino::HardwareSerial {
    public:
        void begin(unsigned long baudrate, uint16_t config);
        void begin(unsigned long baudrate) {
            uint16_t config = 0;
            config |= SERIAL_DATA_8;
            config |= SERIAL_STOP_BIT_1;
            config |= SERIAL_PARITY_NONE;
            begin(baudrate, config);     
        }
        int available();
        int read();
        void end() {}
        void flush() {}
        int peek(void) {
            return rxBuffer.peek();
        }
        size_t write(uint8_t c);
        operator bool() {
            return true;
        }
        void rx_cb(uint8_t* data, size_t len);
    private:
        RingBufferN<128> rxBuffer;
        rtos::Mutex _mut;
};