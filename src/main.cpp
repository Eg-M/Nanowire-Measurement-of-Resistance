//////////////////////////////////////////////////////////////////////////////////////////
//
//    Demo code for the ADS1220 24-bit ADC breakout board
//
//    Author: Ashwin Whitchurch
//    Copyright (c) 2018 ProtoCentral
//
//    This example sequentially reads all 4 channels in continuous conversion mode
//
//    Arduino connections:
//
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
//    This software is licensed under the MIT License(http://opensource.org/licenses/MIT).
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
//   NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//   For information on how to use, visit https://github.com/Protocentral/Protocentral_ADS1220
//
/////////////////////////////////////////////////////////////////////////////////////////

#include "Protocentral_ADS1220.h"
#include <SPI.h>

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
int32_t convertToRes(float i32data);

const uint8_t adc_ch[2] = {MUX_SE_CH1,MUX_SE_CH2};
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

    pc_ads1220.begin(ADS1220_CS_PIN,ADS1220_DRDY_PIN);

    pc_ads1220.set_data_rate(DR_20SPS);
    pc_ads1220.PGA_OFF();
    //pc_ads1220.set_pga_gain(PGA_GAIN_1);
    pc_ads1220.writeRegister(CONFIG_REG2_ADDRESS,0xE0); //set external voltage reference as AVCC , 50 Hz filtering

    pc_ads1220.set_conv_mode_continuous(); //Set continious  mode
}

void loop()
{
    // adc_data=pc_ads1220.Read_SingleShot_SingleEnded_WaitForData(MUX_SE_CH0);
    // //Serial.print("\nCh1 (mV): ");
    // Serial.print(convertToMilliV(adc_data));
    // Serial.print(',');
    // delay(100);
    if (!(millis() % 5000))
    {
        
        // adc_data=pc_ads1220.Read_SingleShot_SingleEnded_WaitForData(MUX_SE_CH1);
        // Serial.print("\nCh2 (mV): ");
        

        // for (int8_t i = 0; i < 20; i++)
        // {

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

        Serial.print(convertToRes(pseumed[0]));
        Serial.print(',');
        Serial.print(convertToRes(pseumed[1]));
        Serial.println(',');

        //     filter_val[0] += (pseumed[0] - filter_val[0]) * 0.05f;
        //     filter_val[1] += (pseumed[1] - filter_val[1]) * 0.05f;
        // }

        //delay(1000);
        
    }
    //  adc_data=pc_ads1220.Read_SingleShot_SingleEnded_WaitForData(MUX_SE_CH2);
    // // Serial.print("\nCh3 (mV): ");
    //  Serial.print(convertToRes(adc_data));
    //  Serial.print(',');
    //  delay(100);

    //  adc_data=pc_ads1220.Read_SingleShot_SingleEnded_WaitForData(MUX_SE_CH3);
    // // Serial.print("\nCh4 (mV): ");
    //  Serial.print(convertToMilliV(adc_data));
    //  Serial.println(',');
    //  delay(100);
}

float convertToMilliV(int32_t i32data)
{
    return (float)((i32data*VFSR*1000)/FULL_SCALE);
}

int32_t convertToRes(float i32data)
{
    return (int32_t)((((i32data*VFSR)/FULL_SCALE)*R1)/(VFSR-((i32data*VFSR)/FULL_SCALE)));
}