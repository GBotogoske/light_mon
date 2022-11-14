//GABRIEL BOTOGOSKE
//SBND-FERMILAB
//PROJECT LIGHT MONITORING

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
  
}

void loop() 
{
  //read SIPM in adc_channels

  for(int i=0;i<SiPM_n;i++)
  {
    Serial.print("Channel ");
    Serial.print(i);
    Serial.print(": ");
    SiPM_ADC[i]=analogRead(SiPM[i]);
    SiPM_mV[i]=SiPM_ADC[i]*resolution;
    Serial.print(SiPM_mV[i]);
    Serial.print(" mV \n");
  }
  
  
  Serial.print("\n");   
  delay(1000);
  //Serial.print(resolution);
}
