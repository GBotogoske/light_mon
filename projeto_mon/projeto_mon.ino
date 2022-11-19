//GABRIEL BOTOGOSKE
//SBND-FERMILAB
//PROJECT LIGHT MONITORING

#include <SD.h> 
#include <SPI.h>

//define library that handles time
#include <RTCZero.h>

//define a object rtczero to count time
RTCZero rtc;

//define the variable of sd card
const int SD_port = SDCARD_SS_PIN;
File data;
bool open_file = false;

#define  SiPM_n 4

//set the readout pins as ADC
const int SiPM[SiPM_n]={A0,A1,A2,A3};
int SiPM_ADC[SiPM_n]={};
double SiPM_mV[SiPM_n]={};

//precision of ADC in bits
const int precision = 12; 

//precision of ADC in voltage (mV)
const double resolution=3300/(pow(2,precision)-1);

void print_number(Stream &Serialx, int number)
{
   if (number < 10) 
   {
    Serialx.print("0"); // print a 0 before if the number is < than 10
   }
   Serialx.print(number);
}

//print in serial monitor//bluetooth
void print_Voltage(Stream &Serialx)
{
    if(Serialx.availableForWrite()>0)
    {
      print_number(Serialx,rtc.getDay());
      Serialx.print("/");
      print_number(Serialx,rtc.getMonth());
      Serialx.print("/");
      print_number(Serialx,rtc.getYear());
      Serialx.print(" ");
      print_number(Serialx,rtc.getHours());
      Serialx.print(":");
      print_number(Serialx,rtc.getMinutes());
      Serialx.print(":");
      print_number(Serialx,rtc.getSeconds());
      Serialx.println();
    
      for(int i=0;i<SiPM_n;i++)
      {
          Serialx.print("Channel ");
          Serialx.print(i);
          Serialx.print(": ");
          Serialx.print(SiPM_mV[i]);
          Serialx.print(" mV \n");
      }
      Serialx.print("\n");
    }
}

void setup() 
{
  //set the resolution of ADC as 12 bits
  //this means a resolution of 3300mV/(2¹²-1) = 805.96 uV
  analogReadResolution(12);

  //For debug in serial monitor
  Serial.begin(9600);

  //For Bluetooth
  Serial1.begin(9600);

    // see if the card is present and can be initialized:
  if (!SD.begin(SD_port)) 
  {
    Serial.print("Card failed, or not present");
    // don't do anything more:
    while (1);
  }

  //start counting
  rtc.begin();
   
  delay(5000);
  Serial.print("card initialized. \n");
}

void loop() 
{
  //check if the file is already open   
  if(!open_file)
  {
       //for test purposes
    if(SD.exists("teste.txt"))
    {
      SD.remove("teste.txt");
    }
     
    //if isnt, opens the file
    data = SD.open("teste.txt", FILE_WRITE);
    open_file=true;
  }

   //check if is there information being received by the bluetooth module
   if(Serial1.available()>0)
   {
      arduino:: String epoch=Serial1.readStringUntil('\n');
      //set the time
      rtc.setEpoch(epoch.toInt());
   }

   //read SIPM in adc_channels
   for(int i=0;i<SiPM_n;i++)
   {
     SiPM_ADC[i]=analogRead(SiPM[i]);
     SiPM_mV[i]=SiPM_ADC[i]*resolution;
   }
   //print in serial monitor
   if(Serial)
     print_Voltage(Serial);
   //send via bluetooth
   if(Serial1)
    print_Voltage(Serial1);

  //if file in sd card is open 
  if(data)
  {
    //save time in epoch in sd card
    data.print(rtc.getEpoch());
    data.print(" ");
    for(int i=0;i<SiPM_n;i++)
    { 
      //print in file in SD card
      data.print(SiPM_ADC[i]);
      data.print(" ");
    }
    data.print("\n");
    //certifies that the datas are saved
    data.flush(); 
  }

  //print_time(Serial);
  delay(2000);
}
