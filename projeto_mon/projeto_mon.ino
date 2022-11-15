//GABRIEL BOTOGOSKE
//SBND-FERMILAB
//PROJECT LIGHT MONITORING

#include <SD.h> 
#include <SPI.h>

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

void setup() 
{

  //set the resolution of ADC as 12 bits
  //this means a resolution of 3300mV/(2¹²-1) = 805.96 uV
  analogReadResolution(12);

  //For debug in serial monitor
  Serial.begin(9600);

    // see if the card is present and can be initialized:
  if (!SD.begin(SD_port)) 
  {
    Serial.print("Card failed, or not present");
    // don't do anything more:
    while (1);
  }

 
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
  
  if(data)
  {
    //read SIPM in adc_channels
    for(int i=0;i<SiPM_n;i++)
    {
      SiPM_ADC[i]=analogRead(SiPM[i]);
      SiPM_mV[i]=SiPM_ADC[i]*resolution;

      //print in serial monitor
      Serial.print("Channel ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(SiPM_mV[i]);
      Serial.print(" mV \n");

      //print in file in SD card
      data.print(SiPM_ADC[i]);
      data.print(" ");
    }
    data.print("\n");
    //certifies that the datas are saved
    data.flush();
    Serial.print("\n");   
  }

  delay(1000);
}
