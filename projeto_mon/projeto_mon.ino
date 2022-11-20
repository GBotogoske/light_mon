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
bool open_dir = false;

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


void set_file_name()
{
   
    file_hour=rtc.getHours();
    file_minute=rtc.getMinutes();
    file_second=rtc.getSeconds();

    //file_name=dir_name+"_";

     if(file_hour<10)
    {
      file_name+="0";
    }
    file_name+=arduino::String(file_hour)+"_";

     if(file_minute<10)
    {
      file_name+="0";
    }
    file_name+=arduino::String(file_minute);
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
  if(!Serial1)
  {
    Serial.print("bluetooth failed");
  }

    // see if the card is present and can be initialized:
  if (!SD.begin(SD_port)) 
  {
    Serial.print("Card failed, or not present");
    Serial1.print("Card failed, or not present");
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
   //check if is there information being received by the bluetooth module
   if(Serial1.available()>0)
   {
      arduino:: String epoch=Serial1.readStringUntil('\n');
      //set the time
      int n= epoch.toInt();
      if(n>1000)
      {
        Serial.print(n);
        rtc.setEpoch(n);
      }  
   }

    //create the directory
    if(!open_dir)
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
    if(!open_file)
    {
      set_file_name();
      if(SD.exists(dir_name+"/"+file_name+".txt"))
      {
          SD.remove(dir_name+"/"+file_name+".txt");
      }
      //if isnt, opens the file
      data = SD.open(dir_name+"/"+file_name+".txt", FILE_WRITE);
      Serial.print(dir_name+"/"+file_name+".txt"+"\n");
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
