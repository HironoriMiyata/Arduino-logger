#include <DHT.h>
#include "driver/adc.h" 
#include "esp_adc_cal.h"

esp_adc_cal_characteristics_t adcChar;

#define AD_VCC (double)4.85
#define EMF_RANGE (double)0.045
#define EMF_CHK_RANGE (double)0.01
#define EMF_0_PPM 400
#define EMF_1_PPM 4000
#define EMF_BASE (double)0.2
#define BOARD_GAIN 10

int mg812 = 13;
double initvcc;
double emfmax;
double emf_chk;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("start");
  Serial.println("sensor warming");
  delay(10000);
  initADC();
  initApp();
}

int getCo2Voltage(){
  int32_t  first_analog_v;
  uint32_t voltage;
  long d = 0;
    for (int i = 0; i < 50; i++) {  // 50回測定して
      esp_adc_cal_get_voltage(ADC_CHANNEL_6, &adcChar, &voltage);
      d += (int)voltage;
      delay(200);
    }
    first_analog_v = d / 50;

    return first_analog_v;
}

void initADC(){
    adc_power_on();
    // ADC1_CH6を初期化
    adc_gpio_init(ADC_UNIT_2, ADC_CHANNEL_6);
    // ADC1の解像度を12bit（0~4095）に設定
//    adc2_config_width(ADC_WIDTH_BIT_12);
    // ADC1の減衰を11dBに設定
    adc2_config_channel_atten(ADC2_CHANNEL_6, ADC_ATTEN_DB_11);
    // 電圧値に変換するための情報をaddCharに格納
    esp_adc_cal_characterize(ADC_UNIT_2, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adcChar);
}

void initApp(){
  static int32_t  first_analog_v;
  uint32_t voltage;
  bool chk = true;

  Serial.println("initvcc setting now");

  while(chk){
    delay(1000);
    first_analog_v = getCo2Voltage();
    initvcc = (float)first_analog_v / 1000 / BOARD_GAIN;
    Serial.print("value: ");
    Serial.print(first_analog_v);
    Serial.print("initvcc:");
    Serial.println(initvcc);
    if(initvcc > EMF_BASE ){
      emfmax = initvcc;
      chk = false;
    }
  } 
}

void loop() {
  static int32_t  analog_v;
  double e; //MG812の出力電圧
  double k; //対数の調整値（対数の傾き）
  double p; //ppm換算値


  analog_v = getCo2Voltage();
  emfmax = initvcc;
  e = (float)analog_v / 1000 / BOARD_GAIN;
  k = ((double)1/EMF_RANGE) * log10((double)EMF_1_PPM / EMF_0_PPM);
  p = (double)EMF_0_PPM * pow(10, (k * (emfmax - e)));

  Serial.print("value: ");
  Serial.print(analog_v);
  Serial.print("e: ");
  Serial.print(e);
  Serial.print("k: ");
  Serial.print(k);
  Serial.print("ppm: ");
  Serial.println(p);
  if( p < (double)EMF_0_PPM){
    emf_chk = emfmax + EMF_CHK_RANGE;
    if( e >= emf_chk ){
       emfmax = e;
       Serial.print(" update initvcc: ");
       Serial.println(emfmax,3);
     }
  }
  Serial.println("Done Wait for the patch");
  delay(1000);
}
