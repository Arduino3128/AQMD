/*
Author: Kanad H. Nemade
Version: V2.1
Desc: Firmware for Arduino Pro Mini
Code 1 of 2
*/

#include <SoftwareSerial.h>

SoftwareSerial nodemcu(2, 3);
//---------------------------------------------------------------------------------------------------------------
const int MQ_PIN = A0; // analog input channel
const int MQ_PIN135 = A1;
const int calibrationLed = 13; // when the calibration start , LED pin 13 will light up , off when finish calibrating
//---------------------------------------------------------------------------------------------------------------
int RL_VALUE = 5;                      // load resistance,kilo ohms
float RO_CLEAN_AIR_FACTOR = 9.83;      // RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
float RO_CLEAN_AIR_FACTOR2 = 3.67;     // RO_CLEAR_AIR_FACTOR2=(Sensor resistance in clean air)/RO,
int CALIBARAION_SAMPLE_TIMES = 100;    // no of samples taken in the calibration phase
int CALIBRATION_SAMPLE_INTERVAL = 500; // time interval(in ms) b/w each samples in the cablibration phase
int READ_SAMPLE_INTERVAL = 50;         // no of samples taken in normal operation
int READ_SAMPLE_TIMES = 5;             // time interval(in ms) b/w each samples in normal operation

//---------------------------------------------------------------------------------------------------------------
#define GAS_LPG 0
#define GAS_CO 1
#define GAS_SMOKE 2
#define GAS_BENZENE 3
#define GAS_AMMONIA 4

//----------------------------------------------------------------------------------------------------------------------------

float LPGCurve[3] = {2.3, 0.21, -0.47}; // two points,a line formed which is "approx equi" to the original curve.

float COCurve[3] = {2.3, 0.72, -0.34}; /// two points,a line formed which is "approx equi" to the original curve.

float SmokeCurve[3] = {2.3, 0.53, -0.44}; // two points,a line formed which is "approx equi" to the original curve.

float BenzeneCurve[3] = {1, 0.17, -0.30};

float AmmoniaCurve[3] = {1, 0.43, -0.40};

float Ro = 10; // Ro is initialized to 10 kilo ohms

//-----------------------------------------------------------------------------------------------------------------------------
String cdata;

int iPPM_LPG = 0;
int iPPM_CO = 0;
int iPPM_Smoke = 0;
int iPPM_Ammonia = 0;
int iPPM_Benzene = 0;

int sdata1 = 0; // LPG data
int sdata2 = 0; // CO data
int sdata3 = 0; // Smoke data
int sdata4 = 0; // Ammonia data
int sdata5 = 0; // Benzene data

//--------------------------------------------------------------------------------------------------------------------------------
void setup()
{
    Serial.begin(4800); // error with baud rate 4800 means 9600 in terminal                             //UART setup, baudrate = 9600bps
    nodemcu.begin(4800);
    pinMode(MQ_PIN, INPUT);
    pinMode(MQ_PIN135, INPUT);
    pinMode(calibrationLed, OUTPUT);

    Serial.print("Calibrating...\n");
    digitalWrite(calibrationLed, HIGH);
    Ro = MQCalibration(MQ_PIN); // Calibrating the sensor.
    Serial.print("Calibration is done...\n");
    Serial.print("Ro=");
    Serial.print(Ro);
    Serial.print("kohm");
    Serial.print("\n");

    Serial.print("Calibrating...\n");
    Ro = MQCalibration(MQ_PIN135); // Calibrating the sensor.
    Serial.print("Calibration is done...\n");
    Serial.print("Ro=");
    Serial.print(Ro);
    Serial.print("kohm");
    Serial.print("\n");
    digitalWrite(calibrationLed, LOW);
}

void loop()
{
    int iPPM_LPG = MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_LPG);
    int iPPM_CO = MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_CO);
    int iPPM_Smoke = MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_SMOKE);
    int iPPM_Ammonia = MQ135GetGasPercentage(MQRead135(MQ_PIN135) / Ro, GAS_AMMONIA);
    int iPPM_Benzene = MQ135GetGasPercentage(MQRead135(MQ_PIN135) / Ro, GAS_BENZENE);

    sdata1 = iPPM_LPG;
    sdata2 = iPPM_CO;
    sdata3 = iPPM_Smoke;
    sdata4 = iPPM_Ammonia;
    sdata5 = iPPM_Benzene;
    cdata = cdata + iPPM_LPG + "," + iPPM_CO + "," + iPPM_Smoke + "," + iPPM_Ammonia + "," + iPPM_Benzene; // comma will be used a delimeter

    Serial.println(cdata);
    nodemcu.println(cdata);
    delay(2000);
    cdata = "";

    Serial.print("LPG:");
    Serial.print(MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_LPG));
    Serial.print("ppm");
    Serial.print("    ");
    Serial.print("CO:");
    Serial.print(MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_CO));
    Serial.print("ppm");
    Serial.print("    ");
    Serial.print("SMOKE:");
    Serial.print(MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_SMOKE));
    Serial.print("ppm");
    Serial.print("    ");
    Serial.print("NH4:");
    Serial.print(MQ135GetGasPercentage(MQRead135(MQ_PIN135) / Ro, GAS_AMMONIA));
    Serial.print("ppm");
    Serial.print("    ");
    Serial.print("C6H6:");
    Serial.print(MQ135GetGasPercentage(MQRead135(MQ_PIN135) / Ro, GAS_BENZENE));
    Serial.print("ppm");
    Serial.print("    ");

    delay(200);

    //---------------------------------------------------------------------------------------------
}

float MQResistanceCalculation(int raw_adc)
{
    return (((float)RL_VALUE * (1023 - raw_adc) / raw_adc));
}

float MQ135ResistanceCalculation(int raw_adc135)
{
    return (((float)RL_VALUE * (1023 - raw_adc135) / raw_adc135));
}

//==================================================================================================

float MQCalibration(int mq_pin)
{
    int i;
    float val = 0;

    for (i = 0; i < CALIBARAION_SAMPLE_TIMES; i++)
    { // take multiple samples
        val += MQResistanceCalculation(analogRead(mq_pin));
        delay(CALIBRATION_SAMPLE_INTERVAL);
    }
    val = val / CALIBARAION_SAMPLE_TIMES; // calculate the average value

    val = val / RO_CLEAN_AIR_FACTOR; // divided by RO_CLEAN_AIR_FACTOR yields the Ro
                                     // according to the chart in the datasheet

    return val;
}

float MQCalibration135(int mq_pin135)
{
    int i135;
    float val135 = 0;

    for (i135 = 0; i135 < CALIBARAION_SAMPLE_TIMES; i135++)
    { // take multiple samples
        val135 += MQ135ResistanceCalculation(analogRead(mq_pin135));
        delay(CALIBRATION_SAMPLE_INTERVAL);
    }
    val135 = val135 / CALIBARAION_SAMPLE_TIMES; // calculate the average value

    val135 = val135 / RO_CLEAN_AIR_FACTOR2; // divided by RO_CLEAN_AIR_FACTOR2 yields the Ro
                                            // according to the chart in the datasheet

    return val135;
}

//------------------------------------------------------------------------------------------------------------------------------------

float MQRead(int mq_pin)
{
    int i;
    float rs = 0;

    for (i = 0; i < READ_SAMPLE_TIMES; i++)
    {
        rs += MQResistanceCalculation(analogRead(mq_pin));
        delay(READ_SAMPLE_INTERVAL);
    }

    rs = rs / READ_SAMPLE_TIMES;

    return rs;
}
float MQRead135(int mq_pin135)
{
    int i135;
    float rs135 = 0;

    for (i135 = 0; i135 < READ_SAMPLE_TIMES; i135++)
    {
        rs135 += MQ135ResistanceCalculation(analogRead(mq_pin135));
        delay(READ_SAMPLE_INTERVAL);
    }

    rs135 = rs135 / READ_SAMPLE_TIMES;

    return rs135;
}
//---------------------------------------------------------------------------------------------
int MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
    if (gas_id == GAS_LPG)
    {
        return MQGetPercentage(rs_ro_ratio, LPGCurve);
    }
    else if (gas_id == GAS_SMOKE)
    {
        return MQGetPercentage(rs_ro_ratio, SmokeCurve);
    }
    else if (gas_id == GAS_CO)
    {
        return MQGetPercentage(rs_ro_ratio, COCurve);
    }

    return 0;
}

int MQ135GetGasPercentage(float rs_ro_ratio135, int gas_id135)
{
    if (gas_id135 == GAS_BENZENE)
    {
        return MQGetPercentage(rs_ro_ratio135, BenzeneCurve);
    }
    else if (gas_id135 == GAS_AMMONIA)
    {
        return MQGetPercentage(rs_ro_ratio135, AmmoniaCurve);
    }

    return 0;
}

//----------------------------------------------------------------------------------------------
int MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
    return (pow(10, (((log10(rs_ro_ratio) - pcurve[1]) / pcurve[2]) + pcurve[0])));
}