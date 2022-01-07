#include <Arduino.h>
extern "C" void SystemClock_Config(void);
#include "lenval.h"
#include "micro.h"
#include "ports.h"
#include <SPI.h>
#include "SdFat.h"
#include "sdios.h"
#include "spi_control.hpp"

SdFat32 sd;
File32 file;

void bench();
void blink_error(int count){
  for (int i = 0; i < count; i++) {
    delay(200);
    digitalWrite(pin_led_yellow, 1);
    delay(200);
    digitalWrite(pin_led_yellow, 0);
  }
  delay(2000);
}

// File myFile;

int errorcode = 100;

ArduinoOutStream cout(Serial);

void setup() {
  Serial.begin(9600);
  pinMode(pin_led_yellow, OUTPUT);
  pinMode(pin_led_blue, OUTPUT);
  pinMode(pin_tdi, OUTPUT);
  pinMode(pin_tms, OUTPUT);
  pinMode(pin_tck, OUTPUT);
  pinMode(pin_tdo, INPUT);
  digitalWrite(pin_led_blue, 0);
  digitalWrite(pin_led_yellow, 0);
  Serial.println("=== start ===");
  while (!sd.begin(SdSpiConfig(pin_sd_cs, DEDICATED_SPI, SD_SCK_MHZ(120)))) {
    Serial.println("SD card problem.");
    blink_error(9);
  }

  // bench();
  while (!file.open("firmware.xsvf", FILE_READ)) {
    Serial.println("can't find/open firmware.xsvf");
    blink_error(8);
  }

  while (errorcode) {
    file.seek(0);
    errorcode = xsvfExecute();
    if (errorcode) {
      Serial.println("xsvf player error:");
      Serial.println(errorcode);
      blink_error(errorcode);
    } 
  }
  Serial.println("done");
  digitalWrite(pin_led_blue, 0);
  delay(500);
  digitalWrite(pin_led_blue, 1);
  delay(2000);
  digitalWrite(pin_led_blue, 0);
}

void loop() {
  // Serial.println(errorcode);
  // delay(500);
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}


/* readByte:  Implement to source the next byte from your XSVF file location */
/* read in a byte of data from the prom */
void readByte(unsigned char *data)
{
    /* pretend reading using a file */
    // *data   = (unsigned char)fgetc( in );
    /**data=*xsvf_data++;*/
    *data = (unsigned char) file.read();
}


/* setPort:  Implement to set the named JTAG signal (p) to the new value (v).*/
/* if in debugging mode, then just set the variables */
void setPort(short p,short val)
{
    /* Printing code for the xapp058_example.exe.  You must set the specified
       JTAG signal (p) to the new value (v).  See the above, old Win95 code
       as an implementation example. */
  if (val) {
    if (p==TMS)
      LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_11);
    if (p==TDI)
      LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_15);
    if (p==TCK) {
      LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_13);
    }
  } else {
    if (p==TMS)
      LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_11);
    if (p==TDI)
      LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_15);
    if (p==TCK) {
      LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_13);
    }    
  }

// const PinName pin_tdi = PB_15;
// const PinName pin_tms = PB_11;
// const PinName pin_tdo = PB_14;
// const PinName pin_tck = PB_13;    
}




/* toggle tck LH.  No need to modify this code.  It is output via setPort. */
void pulseClock()
{
    setPort(TCK,0);  /* set the TCK port to low  */
    setPort(TCK,1);  /* set the TCK port to high */
}




/* readTDOBit:  Implement to return the current value of the JTAG TDO signal.*/
/* read the TDO bit from port */
unsigned char readTDOBit()
{

    /* You must return the current value of the JTAG TDO signal. */
    return( (unsigned char) digitalRead(pin_tdo) );
}

/* waitTime:  Implement as follows: */
/* REQUIRED:  This function must consume/wait at least the specified number  */
/*            of microsec, interpreting microsec as a number of microseconds.*/
/* REQUIRED FOR SPARTAN/VIRTEX FPGAs and indirect flash programming:         */
/*            This function must pulse TCK for at least microsec times,      */
/*            interpreting microsec as an integer value.                     */
/* RECOMMENDED IMPLEMENTATION:  Pulse TCK at least microsec times AND        */
/*                              continue pulsing TCK until the microsec wait */
/*                              requirement is also satisfied.               */
void waitTime(long microsec)
{
   uint32_t t0 = micros();
   while( micros()-t0 < microsec )
   {
      pulseClock();
   }
}

#define error(s) sd.errorHalt(&Serial, F(s))
// Set PRE_ALLOCATE true to pre-allocate file clusters.
const bool PRE_ALLOCATE = true;

// Set SKIP_FIRST_LATENCY true if the first read/write to the SD can
// be avoid by writing a file header or reading the first record.
const bool SKIP_FIRST_LATENCY = true;

// Size of read/write.
const size_t BUF_SIZE = 512;

// File size in MB where MB = 1,000,000 bytes.
const uint32_t FILE_SIZE_MB = 5;

// Write pass count.
const uint8_t WRITE_COUNT = 2;

// Read pass count.
const uint8_t READ_COUNT = 2;
const uint32_t FILE_SIZE = 1000000UL*FILE_SIZE_MB;
uint32_t buf32[(BUF_SIZE + 3)/4];
uint8_t* buf = (uint8_t*)buf32;

void bench() {
  float s;
  uint32_t t;
  uint32_t maxLatency;
  uint32_t minLatency;
  uint32_t totalLatency;
  bool skipLatency;

  // F() stores strings in flash to save RAM
  cout << F("Type any character to start\n");
  while (!Serial.available()) {
    yield();
  }
#if HAS_UNUSED_STACK
  cout << F("FreeStack: ") << FreeStack() << endl;
#endif  // HAS_UNUSED_STACK

  // open or create file - truncate existing file.
  if (!file.open("bench.dat", O_RDWR | O_CREAT | O_TRUNC)) {
    error("open failed");
  }

  // fill buf with known data
  if (BUF_SIZE > 1) {
    for (size_t i = 0; i < (BUF_SIZE - 2); i++) {
      buf[i] = 'A' + (i % 26);
    }
    buf[BUF_SIZE-2] = '\r';
  }
  buf[BUF_SIZE-1] = '\n';

  cout << F("FILE_SIZE_MB = ") << FILE_SIZE_MB << endl;
  cout << F("BUF_SIZE = ") << BUF_SIZE << F(" bytes\n");
  cout << F("Starting write test, please wait.") << endl << endl;

  // do write test
  uint32_t n = FILE_SIZE/BUF_SIZE;
  cout <<F("write speed and latency") << endl;
  cout << F("speed,max,min,avg") << endl;
  cout << F("KB/Sec,usec,usec,usec") << endl;
  for (uint8_t nTest = 0; nTest < WRITE_COUNT; nTest++) {
    file.truncate(0);
    if (PRE_ALLOCATE) {
      if (!file.preAllocate(FILE_SIZE)) {
        error("preAllocate failed");
      }
    }
    maxLatency = 0;
    minLatency = 9999999;
    totalLatency = 0;
    skipLatency = SKIP_FIRST_LATENCY;
    t = millis();
    for (uint32_t i = 0; i < n; i++) {
      uint32_t m = micros();
      if (file.write(buf, BUF_SIZE) != BUF_SIZE) {
        error("write failed");
      }
      m = micros() - m;
      totalLatency += m;
      if (skipLatency) {
        // Wait until first write to SD, not just a copy to the cache.
        skipLatency = file.curPosition() < 512;
      } else {
        if (maxLatency < m) {
          maxLatency = m;
        }
        if (minLatency > m) {
          minLatency = m;
        }
      }
    }
    file.sync();
    t = millis() - t;
    s = file.fileSize();
    cout << s/t <<',' << maxLatency << ',' << minLatency;
    cout << ',' << totalLatency/n << endl;
  }
  cout << endl << F("Starting read test, please wait.") << endl;
  cout << endl <<F("read speed and latency") << endl;
  cout << F("speed,max,min,avg") << endl;
  cout << F("KB/Sec,usec,usec,usec") << endl;

  // do read test
  for (uint8_t nTest = 0; nTest < READ_COUNT; nTest++) {
    file.rewind();
    maxLatency = 0;
    minLatency = 9999999;
    totalLatency = 0;
    skipLatency = SKIP_FIRST_LATENCY;
    t = millis();
    for (uint32_t i = 0; i < n; i++) {
      buf[BUF_SIZE-1] = 0;
      uint32_t m = micros();
      int32_t nr = file.read(buf, BUF_SIZE);
      if (nr != BUF_SIZE) {
        error("read failed");
      }
      m = micros() - m;
      totalLatency += m;
      if (buf[BUF_SIZE-1] != '\n') {

        error("data check error");
      }
      if (skipLatency) {
        skipLatency = false;
      } else {
        if (maxLatency < m) {
          maxLatency = m;
        }
        if (minLatency > m) {
          minLatency = m;
        }
      }
    }
    s = file.fileSize();
    t = millis() - t;
    cout << s/t <<',' << maxLatency << ',' << minLatency;
    cout << ',' << totalLatency/n << endl;
  }
  cout << endl << F("Done") << endl;
  file.close();
}