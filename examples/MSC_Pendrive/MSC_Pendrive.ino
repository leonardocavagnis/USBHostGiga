#include "USBHost_H7.h"

//REDIRECT_STDOUT_TO(Serial)

USBHost_H7 usbhost;
MSCDrive   pendrive;

void MSC_FileOperations(void);

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("MSC Pendrive");

  usbhost.begin();
  pendrive.begin();

  MSC_FileOperations();
}

void loop() { }

void MSC_FileOperations(void) {
  FIL MyFile;
  FRESULT res;
  uint32_t bytesWritten;
  uint8_t rtext[200];
  uint8_t wtext[] = "Mass Storage Device Test application";
  uint16_t bytesread;

  Serial.println("MSC File operations");

  if(fatfs_open(&MyFile, "0:USBHost.txt", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
  {
    Serial.println("Cannot Open 'USBHost.txt' file");
  }
  else
  {
    Serial.println("INFO: 'USBHost.txt' opened for write");
    res = fatfs_write (&MyFile, wtext, sizeof(wtext)*20, (UINT *)&bytesWritten);

    fatfs_close(&MyFile);
    if((bytesWritten == 0) || (res != FR_OK)) /*EOF or Error*/
    {
      Serial.println("Cannot Write on the 'USBHost.txt' file");
    }
    else
    {
      if(fatfs_open(&MyFile, "0:USBHOST.TXT", FA_READ) != FR_OK)
      {
        Serial.println("Cannot Open 'USBHost.txt' file for read.");
      }
      else
      {
        Serial.println("INFO: Text written on the 'USBHost.txt' file");

        res = fatfs_read(&MyFile, rtext, sizeof(rtext), (UINT *)&bytesread);

        if((bytesread == 0) || (res != FR_OK)) /*EOF or Error*/
        {
          Serial.println("Cannot Read from the 'USBHost.txt' file");
        }
        else
        {
          Serial.println("Read Text:");
          Serial.println((char *)rtext);
        }
        fatfs_close(&MyFile);
      }
      /* Compare read data with the expected data */
      if((bytesread == bytesWritten) && bytesread != 0)
      {
        Serial.println("INFO: FatFs data compare SUCCES");
      }
      else
      {
        Serial.println("FatFs data compare ERROR");
      }
    }
  }
  return;
}