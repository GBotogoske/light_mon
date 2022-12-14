//GABRIEL BOTOGOSKE
//SBND-FERMILAB
//PROJECT LIGHT MONITORING

//To low power mode
#include "ArduinoLowPower.h"

//libraties that handles with Sd CARD
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
bool open_dir = false;
bool open_sd = true;

arduino::String file_name;
arduino::String dir_name;
int dir_year=0;
int dir_month=0;
int dir_day=0;
int file_hour=0;
int file_minute=0;
int file_second=0;

//directories per day
//files per hour


#define  SiPM_n 4

//set the readout pins as ADC
const int SiPM[SiPM_n]={A1,A2,A3,A4};
int SiPM_ADC[SiPM_n]={};
double SiPM_mV[SiPM_n]={};

//precision of ADC in bits
const int precision = 12; 

//precision of ADC in voltage (mV)
const double resolution=3300/(pow(2,precision)-1);


//function to print number in form of string
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
    int tempo = 10;
    if(Serialx.availableForWrite()>0)
    {
      print_number(Serialx,rtc.getDay());
      delay(tempo);
      Serialx.print("/");
      delay(tempo);
      print_number(Serialx,rtc.getMonth());
      delay(tempo);
      Serialx.print("/");
      delay(tempo);
      print_number(Serialx,rtc.getYear());
      delay(tempo);
      Serialx.print(" ");
      delay(tempo);
      print_number(Serialx,rtc.getHours());
      delay(tempo);
      Serialx.print(":");
      delay(tempo);
      print_number(Serialx,rtc.getMinutes());
      delay(tempo);
      Serialx.print(":");
      delay(tempo);
      print_number(Serialx,rtc.getSeconds());
      delay(tempo);
      Serialx.println();
      delay(tempo);
      for(int i=0;i<SiPM_n;i++)
      {
          Serialx.print("Channel ");
          delay(tempo);
          Serialx.print(i);
          delay(tempo);
          Serialx.print(": ");
          delay(tempo);
          Serialx.print(SiPM_mV[i]);
          delay(tempo);
          Serialx.print(" mV \n");
          delay(tempo);
      }
      Serialx.print("\n");
      delay(tempo);
    }
}

//create the name of the dir in sd card
void set_dir_name()
{
    dir_year=rtc.getYear();
    dir_month=rtc.getMonth();
    dir_day=rtc.getDay();

    dir_name=arduino::String(dir_year)+"_";
    
    if(dir_month<10)
    {
      dir_name+="0";
    }
    dir_name+=arduino::String(dir_month)+"_";   
    
    if(dir_day<10)
    {
      dir_name+="0";
    }
    dir_name+=arduino::String(dir_day);
}


//create the file name in sd card
void set_file_name()
{
   
    file_hour=rtc.getHours();
    file_minute=rtc.getMinutes();
    file_second=rtc.getSeconds();

    //file_name=dir_name+"_";

     if(file_hour<10)
    {
      file_name="0"+arduino::String(file_hour)+"_";
    }
    else
    {
      file_name=arduino::String(file_hour)+"_";   
    }
   

     if(file_minute<10)
    {
      file_name+="0";
    }
    file_name+=arduino::String(file_minute);
}

void setup() 
{
  //set the resolution of ADC as 12 bits
  //this means a resolution of 3300mV/(2????-1) = 805.96 uV
  analogReadResolution(12);
  rtc.begin();
  
  //For debug in serial monitor
  Serial.begin(9600);

  //For Bluetooth
  Serial1.begin(9600);

  delay(3000);
  
  if(!Serial1)
  {
    Serial.print("bluetooth failed");
  }

    Serial.print("Starting... \n\n");

  //For Bluetooth
   Serial1.print("Starting... \n\n"); 

    // see if the card is present and can be initialized:
  if (!SD.begin(SD_port)) 
  {
    Serial.print("Card failed, or not present\n");
    Serial1.print("Card failed, or not present\n");
    open_sd=false;
    // don't do anything more:
  }
 
    
  
  Serial.print("Time stamp: \n");
  Serial1.print("Time stamp: \n");

  //start counting
 
  while(!Serial1.available()>0)
  {
    
  }
  
      arduino:: String epoch=Serial1.readStringUntil('\n');
      //set the time
      unsigned int n= epoch.toInt();
      rtc.setEpoch(n);
      open_file=false;
      open_dir=false;


      Serial.print("time stamp received: ");
      Serial.print(n);
      Serial.print("\n");
      Serial.print("\n timestamp saved: ");
      Serial.print(rtc.getEpoch());
      Serial.print("\n\n");

      Serial1.print("time stamp received: ");
      Serial1.print(n);
      Serial1.print("\n");
      Serial1.print("\n timestamp saved: ");
      Serial1.print(rtc.getEpoch());
      Serial1.print("\n\n");
        

  
  delay(5000);
  //
  Serial1.print("ok\n");
  
}


//print for debug
void serial_print(arduino::String str)
{
  Serial1.print(str);
  Serial.print(str);
}

void loop() 
{
   //check if is there information being received by the bluetooth module

    Serial1.print("START\n");
    //create the directory
    if((!open_dir) && open_sd)
    {
      set_dir_name();
      if(!SD.exists(dir_name))
      {
        SD.mkdir(dir_name);
        delay(100);
      }
      open_dir=true;
    }
 
    //check if the file is already open     
    if((!open_file) && open_dir)
    {
      set_file_name();
      //open the file
      data = SD.open(dir_name+"/"+file_name+".txt", FILE_WRITE);
      
      delay(100);
      open_file=true;
    }
    

   //read SIPM in adc_channels
   for(int i=0;i<SiPM_n;i++)
   {
     SiPM_ADC[i]=analogRead(SiPM[i]);
     SiPM_mV[i]=SiPM_ADC[i]*resolution;
   }
   //print in serial monitor
   //if(Serial)
     print_Voltage(Serial);
   //send via bluetooth
   if(Serial1)
    print_Voltage(Serial1);

  
  
  //if file in sd card is open 
  if(data && open_sd)
  {
    //if tha day has changed, create a new dir and a new file
     if(rtc.getDay()!=dir_day)
     {
        data.close();
        open_dir=false;
        open_file=false;    
     }
     else if(rtc.getHours()!=file_hour)//if only the name have, changed create a new file in same dir
     {
        data.close();
        open_file=false;   
     }
     else
     {
          Serial.print("File in: "); 
          Serial.print(dir_name+"/"+file_name+".txt"+"\n");

          Serial1.print("File in: ");   open_sd=true;
          Serial1.print(dir_name+"/"+file_name+".txt"+"\n");
          
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
  }
  else
  {
     Serial.print("File not open\n");    
     Serial1.print("File not open\n");    
  }

    Serial1.print("END\n");    

     delay(200);

     //mode power save
     LowPower.sleep(1000);
    //delay(200);
    Serial.flush();
    //delay(1000);

    
}
