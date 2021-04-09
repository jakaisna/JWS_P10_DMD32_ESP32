/*************************************************************************************
  Waktu Sholat P10   - Program jam petunjuk waktu Sholat otomatis yang dilekapi dengan
  Copyright (C) Des 2017 MFH Robotic. (info https://www.facebook.com/MFH.Robotic/)

  Fitur :
  1. Perhitungan waktu sholat otomatis berdasarkan data geografis lokasi
    (Latitude, Longitude, Altitude, Time Zone dan Prameter pengaman/
    Ihtiyati)
  2. Fasilitas remote menggunakan aplikasi yang berjalan pada HP Android.
  3. Fasilitas running text informasi yang berjumlah 3 x 150 karakter.
    Running text ini cukup besar untuk di isi dengan hadis atau informasi lainnya
    melalui fasilitas remote Aplikasi Android.
  4. Perhitungan Hari dan tanggal Hijriah secara otomatis dan perubahan tanggalnya
    sesuai sesuai standar Hijriah yaitu setelah azzan magrib
  5. Fasilitas nama Masjid dan jenis masjid nya:
        a. Masjid
        b. Musholla
        c. Surau
        d. Langgar
  6. Pengingat waktu sholat mulai dari Azzan, menunggu iqomah dan mulai Sholat
    termasuk pengingat sholat Jumat jika parameter jenis nya di set sebagai Masjid.
  7. Program ini sangat baik dipakai sebagai sarana belajar programing Arduino karena
    menggunakan banyak librari dan teknik pemrogramn yang asik seperti:
       - Library yang digunakan adalah:
              #include <SPI.h>            --> komunikasi ke modul P10
              #include <DMD32.h>           --> https://github.com/Qudor-Engineer/DMD32
              #include <Wire.h>           --> komunikasi ke modul RTC DS3231
              #include <DS3231.h>         --> library modul RTC DS 3231
              #include <EEPROM.h>         --> library untuk penggunaan EEPROM sebagai sarana penyimpan parameter
              #include <avr/pgmspace.h>   --> library untuk penggunaan PROGMEM agar utilisasi SRAM bisa optimal
  8. Program dipecah menjadi 4 File sehingga maintenance bisa lebih mudah dan aman dan tidak mengganggu bagian
    lain.

  Created by Wardi Utari
  30 Des 2017

  ---

  Progarm ini TIDAK ROYALTY FREE, program ini FREE jika dipergunakan untuk
    1. Jika dipergunakan di sekolah/lembaga latihan/perorangan untuk pendidikan atau proses belajar
    2. Dipergunakan dalam proyek pembuatan Jam pengingat waktu sholat yang disumbangkan ke masjid, musholla dll.

  Jika program ini dipergunakan secara komersial baik keseluruhan atau sebagian, anda diwajibkan
  membayar ROYALTY FEE sebesar 2.5%. Royalty tersebut agar dibayarkan ke Masjid terdekat dengan lokasi
  anda, dalam bentuk Sedekah yang diniatkan untuk semua orang yang telah turut urun rembug dalam membuat dan
  menyempurnakan program ini.

  Semoga Bermanfaat
  Salam Mujahid 212.

  edit : jakaisna

**************************************************************************************/
#include <DMD32.h>

#include <Wire.h>
#define  F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr
#include <DS3231.h>
#include <EEPROM.h>
//#include <avr/pgmspace.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

#include "fonts/SystemFont5x7.h"
#include "fonts/Arial_Black_16_ISO_8859_1.h"

#define Font0 Arial_Black_16_ISO_8859_1
#define Font1 SystemFont5x7
#define Font2 SystemFont5x7
#define Font3 SystemFont5x7
#define Font4 SystemFont5x7
#define Font5 SystemFont5x7

#define dmdLAYS_ACROSS 1 //menyamping
#define dmdLAYS_DOWN 1
DMD dmd(dmdLAYS_ACROSS, dmdLAYS_DOWN);

//Timer setup
//create a hardware timer  of ESP32
hw_timer_t * timer = NULL;

#define BUZZ 32


// Object Declarations
RTClib          RTC;
DS3231          Clock;



//Structure of Variable
typedef struct  // loaded to EEPROM
{
  uint8_t state;   // 1 byte  add 0
  float   L_LA;    // 4 byte  add 1
  float   L_LO;    // 4 byte  add 5
  float   L_AL;    // 4 byte  add 9
  float   L_TZ;    // 4 byte  add 13
  uint8_t MT;      // 1 byte  add 17  // value 1-masjid  2-mushollah 3-surau 4-langgar
  uint8_t BL;      // 1 byte  add 18
  uint8_t IH;      // 1 byte  add 19
  uint8_t SO;      // 1 byte  add 20
  uint8_t JM;      // 1 byte  add 21
  uint8_t I1;      // 1 byte  add 22
  uint8_t I4;      // 1 byte  add 23
  uint8_t I5;      // 1 byte  add 24
  uint8_t I6;      // 1 byte  add 25
  uint8_t I7;      // 1 byte  add 26
  uint8_t BZ;      // 1 byte  add 27
  uint8_t SI;      // 1 byte  add 28
  uint8_t ST;      // 1 byte  add 29
  uint8_t SU;      // 1 byte  add 30
  int8_t  CH;      // 1 byte  add 31

  int8_t K1;      // 1 byte  add 22
  int8_t K4;      // 1 byte  add 23
  int8_t K5;      // 1 byte  add 24
  int8_t K6;      // 1 byte  add 25
  int8_t K7;      // 1 byte  add 26
} struct_param;

typedef struct
{
  uint8_t   hD;
  uint8_t   hM;
  uint16_t  hY;
} hijir_date;


// Variable by Structure
struct_param    Prm;
hijir_date      nowH;

// Time Variable
DateTime        now;
float           floatnow   = 0;
uint8_t         daynow     = 0;
uint8_t         ty_puasa   = 0;
uint8_t         hd_puasa   = 0;
int8_t          SholatNow  = -1;
boolean         jumat      = false;
boolean         azzan      = false;
uint8_t         reset_x    = 0;
uint8_t         rtc_akses  = 0;

//Other Variable
float sholatT[8]  = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t Iqomah[8] = {0, 0, 0, 0, 0, 0, 0, 0};

//Blue tooth Pram Receive
char        CH_Prm[155];
int         DWidth  = 32;
int         DHeight = 16;
boolean     DoSwap;
int         RunSel    = 1; //
int         RunFinish = 0 ;

uint8_t     bzzz   = 0;


const char* msg;

int x;
int hour, minute, second, year, month, day, dayofweek;
boolean ret = false;


void IRAM_ATTR triggerScan()
{
  if (rtc_akses == 0)
  dmd.scanDisplayBySPI();
}

void display_update_enable(bool is_enable)
{
  if (is_enable)
  {
    // return the clock speed of the CPU
    uint8_t cpuClock = ESP.getCpuFreqMHz();

    // Use 1st timer of 4
    // devide cpu clock speed on its speed value by MHz to get 1us for each signal  of the timer
    timer = timerBegin(0, cpuClock, true);
    // Attach triggerScan function to our timer
    timerAttachInterrupt(timer, &triggerScan, true);
    // Set alarm to call triggerScan function
    // Repeat the alarm (third parameter)
    timerAlarmWrite(timer, 800, true);

    // Start an alarm
    timerAlarmEnable(timer);

  } else
  {
    timerDetachInterrupt(timer);
    timerAlarmDisable(timer);
  }

}
//=======================================
//===SETUP===============================
//=======================================
void setup()
{ //init comunications
  Wire.begin();
  EEPROM.begin(4096);
  //    set_default_prm();
  Serial.begin(9600);
  SerialBT.begin("JWS_ESP32"); //Bluetooth device name
  // Get Saved Parameter from EEPROM
  rtcUpdate();
  updateTime();
  GetPrm();
  SendPrm();

  display_update_enable(true);


  //clear/init the DMD pixels held in RAM
  dmd.clearScreen( true );
  dmd.setBrightness(EEPROM.read(18));

  pinMode(BUZZ, OUTPUT);
  digitalWrite(BUZZ, HIGH);
  delay(100);
  digitalWrite(BUZZ, LOW);
}

//=======================================
//===MAIN LOOP Function =================
//=======================================
void loop()
{
  serialEvent();
  bz();
  // Reset & Init matrixlay State
  updateTime();   //every time
  check_azzan();  //check Sholah Time for Azzan




  // Timer Function every 10 Minutes
  // Up All function with Timer in this fuction
  Timer_Minute(1);
  Timer_Minute1(1);


  // =========================================
  // List of matrixlay Component Block =======
  // =========================================


  dwMrq(drawMasjidName(), 50, 3, 1);                        // addr: 1 show Masjid Name
  dwMrq(drawDayDate(), 50, 1, 2);                          // addr: 2 show Hijriah date
  dwMrq(msgPuasa(hd_puasa, ty_puasa), 50, 2, 3);              // addr: 3 show Remander Puasa
  dwMrq(drawSholat().c_str()    , 50, 3, 4);
  dwMrq(drawInfo(130)   , 50, 1, 5);                          //show Info 1
  dwMrq(drawInfo(280)   , 50, 2, 6);                          //show Info 2
  dwMrq(drawSholat().c_str()    , 50, 3, 7);
  dwMrq(drawInfo(430)   , 50, 1, 8);                         // show Info 3


  drawAzzan(100);                                             // addr: 100 show Azzan
  drawIqomah(101);                                            // addr: 101 show Iqomah
  dwMrq(drawInfo(580), 50, 2, 102); //Message Sholat biasa       // addr: 202 show Message Sholah
  dwMrq(drawInfo(730), 50, 2, 103); //Message Sholat jumat       // addr: 203 show Message Jum'at
  blinkBlock(104);                                            // addr: 104 show Blink  Sholat

  // =========================================
  // matrixlay Control Block =================
  // =========================================
  if (RunFinish == 1) {
    RunSel = 2;  //after anim 1 set anim 2
    RunFinish = 0;
  }
  if (RunFinish == 2)                                               //after anim 3 set anim 5 or anim 4 if puasa
  {
    if (ty_puasa != 0)  {
      RunSel = 3;
      RunFinish = 0;
    }
    else {
      RunSel = 4;
      RunFinish = 0;
    }
  }
  if (RunFinish == 3) {
    RunSel = 4;  //after anim 3 set anim 4
    RunFinish = 0;
  }
  if (RunFinish == 4)  {
    RunSel = 5;   //after anim 4 set anim 5
    RunFinish = 0;
  }
  if (RunFinish == 5)  {
    RunSel = 6;   //after anim 5 set anim 6
    RunFinish = 0;
  }
  if (RunFinish == 6)  {
    RunSel = 7;   //after anim 6 set anim 7
    RunFinish = 0;
  }
  if (RunFinish == 7)  {
    RunSel = 8;   //after anim 6 set anim 7
    RunFinish = 0;
  }
  if (RunFinish == 8)  {
    RunSel = 1;   //after anim 6 set anim 7
    RunFinish = 0;
  }

  if (RunFinish == 100 and jumat )     {
    RunSel = 103;  //after Azzan Jumat (anim 100)
    RunFinish = 0;
    reset_x = 1;
  }
  else if (RunFinish == 100)           {
    RunSel = 101;  //after Azzan Sholah (Iqomah)
    RunFinish = 0;
  }

  if (RunFinish == 101) {
    RunSel = 102;  //after Iqomah(anim 101) set Message Sholah (anim 102)
    RunFinish = 0;
    reset_x = 1;
  }
  if (RunFinish == 102) {
    RunSel = 104;  //after Message Sholah (anim 102) set Blink Sholah(anim 104)
    RunFinish = 0;
  }
  if (RunFinish == 103) {
    RunSel = 104;  //after Messagw Jum'at (anim 103) set Blink Sholah(anim 104)
    RunFinish = 0;
  }
  if (RunFinish == 104) {
    RunSel = 1;  //after Blink Sholah back to anim 1
    RunFinish = 0;
  }


}

void rtcUpdate() {
  now = RTC.now();
  hour = now.hour();
  minute = now.minute();
  second = now.second();
  day = now.day();
  month = now.month();
  year = now.year();
  dayofweek = Clock.getDoW();
}

// =========================================
// Time Calculation Block===================
// =========================================
void updateTime()
{
  floatnow = (float)hour + (float)minute / 60 + (float)second / 3600;
  daynow   = dayofweek ;    // load day Number
}

void Timer_Minute(int repeat_time) //load every  1 minute
{
  static uint16_t   lsRn;
  uint16_t          Tmr = millis();
  if ((Tmr - lsRn) > (repeat_time * 60000))
  {
    lsRn = Tmr;
    update_All_data();
  }
}

void Timer_Minute1(int repeat_time) //load every  1 minute
{
  static long   lsRn;
  long          Tmr = millis();
  if ((Tmr - lsRn) > (repeat_time * 1000))
  {
    rtc_akses = 1;
    lsRn = Tmr;
    rtcUpdate();
    rtc_akses = 0;
  }
}

void update_All_data()
{
  uint8_t   date_cor = 0;
  updateTime();
  sholatCal();                                                // load Sholah Time
  check_puasa();                                              // check jadwal Puasa Besok
  if (floatnow > sholatT[6]) {
    date_cor = 1; // load Hijr Date + corection next day after Mhagrib
  }
  nowH = toHijri(year, month, day, date_cor); // load Hijir Date

  // if ((floatnow > (float)21) or (floatnow < (float)3.5) )    {setBrightness(15);}
  //   else                                                   {setBrightness(Prm.BL);}
}


void check_azzan()
{ //Check Waktu Sholat
  SholatNow  = -1;
  for (int i = 0; i <= 7; i++)
  {
    if (i != 0 and i != 2 and i != 3) // bukan terbit dan bukan dhuha
    {
      if (floatnow >= sholatT[i])
      {
        SholatNow = i;
        if (!azzan and (floatnow > sholatT[i]) and (floatnow < (sholatT[i] + 0.03)))
        {
          if (daynow == 6 and SholatNow == 4 and Prm.MT == 1) {
            jumat = true;
          }
          azzan = true;
          RunSel = 100;
        }
      }
    }
  }
}
