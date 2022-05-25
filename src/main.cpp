
//  |ADS1220 pin label| Pin Function         |Arduino Connection|
//  |-----------------|:--------------------:|-----------------:|
//  | DRDY            | Data ready Output pin|  D2              |
//  | MISO            | Slave Out            |  D12             |
//  | MOSI            | Slave In             |  D11             |
//  | SCLK            | Serial Clock         |  D13             |
//  | CS              | Chip Select          |  D7              |
//  | DVDD            | Digital VDD          |  +5V             |
//  | DGND            | Digital Gnd          |  Gnd             |
//  | AN0-AN3         | Analog Input         |  Analog Input    |
//  | AVDD            | Analog VDD           |  -               |
//  | AGND            | Analog Gnd           |  -               |
//

#include "Protocentral_ADS1220.h"
#include <SPI.h>
#include "Seeed_SHT35.h"
/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SDAPIN  20
    #define SCLPIN  21
    #define RSTPIN  7
    #define SERIAL SerialUSB
#else
    #define SDAPIN  A4
    #define SCLPIN  A5
    //#define RSTPIN  4
    #define SERIAL Serial
#endif
#define TEMP1 2
#define TEMP2 4
#define Rset 33750
#define Kgain 10
bool flag = 1;
float temp1, hum1;
float temp2, hum2;


SHT35 sensor(SCLPIN);




//#define PGA          1                 // Programmable Gain = 1
#define VREF         3.301            // External reference of 3.301
#define VFSR         VREF
#define R1            402000
#define FULL_SCALE   (((long int)1<<23)-1)

#define ADS1220_CS_PIN    7
#define ADS1220_DRDY_PIN  6

Protocentral_ADS1220 pc_ads1220;
int32_t adc_data;
volatile bool drdyIntrFlag = false;
float convertToMilliV(int32_t i32data);
int32_t convertToRes(float i32data, float temp);
float Vres(float temp);

const uint8_t adc_ch[2] = {MUX_SE_CH0,MUX_SE_CH3}; // AIN1,AIN2
const uint8_t iter = 3;
int32_t buf[2][iter] = {0};
int32_t max_buf[2] = {0};
int32_t min_buf[2] = {FULL_SCALE};
int32_t sum_buf[2] = {0};
int32_t pseumed[2] = {0};
float filter_val[2] = {0.0f};


void drdyInterruptHndlr(){
  drdyIntrFlag = true;
}


void enableInterruptPin(){

  attachInterrupt(digitalPinToInterrupt(ADS1220_DRDY_PIN), drdyInterruptHndlr, FALLING);
}

void setup()
{
    Serial.begin(9600);

    if (sensor.init()) {
        SERIAL.println("sensor init failed!!!");
    }


    pc_ads1220.begin(ADS1220_CS_PIN,ADS1220_DRDY_PIN);

    pc_ads1220.set_data_rate(DR_20SPS);
    pc_ads1220.PGA_OFF();
    //pc_ads1220.set_pga_gain(PGA_GAIN_1);
    pc_ads1220.writeRegister(CONFIG_REG2_ADDRESS,0xE0); //set external voltage reference as AVCC , 50 Hz filtering

    pc_ads1220.set_conv_mode_continuous(); //Set continious  mode

    pinMode(TEMP1,OUTPUT);
    pinMode(TEMP2,OUTPUT);
    digitalWrite(TEMP1,flag);
    delay(1000);
}

void loop()
{

    if (!(millis() % 5000))
    {
        
        if (flag)
        {
            
            if (NO_ERROR != sensor.read_meas_data_single_shot(HIGH_REP_WITH_STRCH, &temp1, &hum1))
            {
                SERIAL.println("read temp1 failed!!");
                SERIAL.println("   ");
                SERIAL.println("   ");
                SERIAL.println("   ");
            }
            else
            {
             
            }
        }
        
        SERIAL.print(temp1);
        SERIAL.print(',');
        SERIAL.print(hum1);
        SERIAL.print(',');

            for (int8_t p = 0; p < 2; p++)
            {
                for (int8_t j = 0; j < 3; j++)
                {

                    buf[p][j] = pc_ads1220.Read_SingleShot_SingleEnded_WaitForData(adc_ch[p]);
                    if (max_buf[p] <= buf[p][j])
                        max_buf[p] = buf[p][j];
                    if (min_buf[p] >= buf[p][j])
                        min_buf[p] = buf[p][j];
                    sum_buf[p] += buf[p][j];
                    //delayMicroseconds(10);
                }
                
                pseumed[p] = sum_buf[p] - min_buf[p] - max_buf[p];
                sum_buf[p] = 0;
                min_buf[p] = FULL_SCALE;
                max_buf[p] = 0;
            }

        Serial.print(convertToMilliV(pseumed[0]));
        Serial.print(',');
        Serial.print(convertToMilliV(pseumed[1]));
        Serial.println(',');

      
        
    }

}

float convertToMilliV(int32_t i32data)
{
    return (float)((i32data * VFSR * 1000) / FULL_SCALE);
}

float Icur(float temp)
{
    float Vres(float temp);
    return (Vres(temp) * 1.059) / Rset;
}
float Vres(float temp)
{    return 0.23 * temp + 58;    }

int32_t convertToRes(float i32data, float temp)
{
    float Icur(float temp);

    return (int32_t)((((i32data*VFSR*1000)/FULL_SCALE))/Icur(temp));
}