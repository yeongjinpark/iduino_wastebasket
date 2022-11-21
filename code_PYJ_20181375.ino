#include <DHT.h>   //온습도 센서를 활용하기위한 라이브러리
#define DHT_TYPE DHT22
#define DHT_PIN 2
DHT dht(DHT_PIN, DHT_TYPE);
unsigned long Tms = 2000;
unsigned long prev_time = 0;

#include<Servo.h>           //서브모터를 활용하기위한 라이브러리

#include <Arduino.h>        //OLED를 활용하기위한 라이브러리
#include <U8x8lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

 

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLED 출력핀 설정  


int angle=0;                            //서브모터 초기각도
Servo myservo;                          //초음파센서와 연결되는 서브모터 설정
Servo myservo2;                         //온습도 센서와 조도 센서와 연결되는 서브모터 설정
const int trigPin = 4;                  //초음파 센서 Trig핀 설정     
const int echoPin = 5;                  //초음파 센서 Echo핀 설정
unsigned long roundtrip_time_us;
float dist_cm;                          //초음파 센서로 측정되는 거리값 변수


void setup()
{
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);                //초음파센서 trig출력설정
  pinMode(echoPin, INPUT_PULLUP);           //초음파센서 echo입력설정
  myservo.attach(9);                        //초음파센서와 연동되는 서브모터는 9번핀
  
  u8x8.begin();                             //OLED 출력 Setup
  u8x8.setPowerSave(0);             

      dht.begin();                         //온습도 센서 setup
    myservo2.attach(12);                   //온습도 센서와 조도 센서랑 연동되는 서브모터는 12번핀 

}
 
void loop()
{ 

  dist_servo();                           //초음파센서+ 서브모터 + OLED 출력함수
  tem_servo();                            //조도센서 + 온습도센서 + 서브모터 출력함수
  
}

void dist_servo(){                        //초음파센서+ 서브모터 + OLED 출력함수
  digitalWrite(trigPin, LOW);             // trig핀에서 초음파 발생
  delayMicroseconds(2);           

  digitalWrite(trigPin, HIGH);            
  delayMicroseconds(10);              
  digitalWrite(trigPin, LOW);         

  roundtrip_time_us = pulseIn(echoPin, HIGH);        // echopin이 high가 된 시간을 저장하는 변수 
  dist_cm = 0.017 * roundtrip_time_us;               //0.017 * trig핀에서 echo핀으로 초음바가 돌아오는 시간= 물체와의 거리
 
  Serial.println((String)"Distance: " + dist_cm + " cm");           //거리확인을 위해 초음파센서로 출력되는 거리 시리얼 모니터로 출력
 
  if(dist_cm < 5){                                                   //초음파 센서로 측정되는 거리가 5CM 이하인 경우 
    u8x8.clear();                                                    //OLED 화면 clear
    angle=90;                                                        //서브모터 각도 90도
    myservo.write(angle);                                            //서브모터 각도 90도 이동
    u8x8.setFont(u8x8_font_chroma48medium8_r);                       //OLED로 출력되는 폰트 설정
  u8x8.drawString(0,0,"full");                                       //OLED에 (0,0)위치에 "full" 문구 출력
  delay(500);    
  }
  else{
    u8x8.clear();                                                  //OLED 화면 clear
    angle=0;                                                       //서브모터 각도 0도
    myservo.write(angle);                                          //서브모터 각도 0도 이동
    u8x8.setFont(u8x8_font_chroma48medium8_r);                      //OLED로 출력되는 폰트 설정
  u8x8.drawString(0,0,"empty");                                    //OLED에 (0,0)위치에 "empty" 문구 출력
  delay(500);
  }

  Serial.println((String)"Ang = " + angle);                       //서브모터 현재각도 확인을 위한 서브모터 각도 현 상태를 시리얼 모니터로 출력
  delay(500);

}

void tem_servo(){                                                      //조도센서 + 온습도센서 + 서브모터 출력함수
    int value = analogRead(A0);                                        //아날로그핀 (A0)에서 현재 조도값 읽어옴
    if (millis() - prev_time > Tms) {                                  // 현재시간-이전시간이 TMS보다 큰 경우
    prev_time = millis();          
    float h = dht.readHumidity();                                     //현재 습도를 변수h에 저장                                     
    float t = dht.readTemperature();                                  //현재 온도를 변수t에 저장
    if (isnan(h) || isnan(t)) {                                        
      Serial.println("Failed to read from DHT sensor!"); 
      return;
    } else {
      Serial.println((String)"Temperature: " + t + "C");              //현재 온도 확인을 위해 시리얼 모니터로 출력
       Serial.println((String)"Sensor out: " + value + "work");       //현재 조도값 확인을 위해 시리얼 모니터로 출력
      if(t>25 && value>100){                                          //온도가 25도 이상이고 조도값이 100이상인 경우에 동작수행
        myservo2.write(180);                                         // 서브모터가 180도 회전
      }
      else{
        myservo2.write(0);                                            //온도가 25도 이하이고 조도값이 100 이하인 경우에 서브모터 0도로 회전
      }
    }
  }
}
