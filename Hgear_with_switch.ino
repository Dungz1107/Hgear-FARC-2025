#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include <PS2X_lib.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


#define MOTOR_1_CHANNEL_A 8 //leftwheel
#define MOTOR_1_CHANNEL_B 9
#define MOTOR_2_CHANNEL_A 10 //rightwheel
#define MOTOR_2_CHANNEL_B 11
#define MOTOR_3_CHANNEL_A 12 //slide
#define MOTOR_3_CHANNEL_B 13
#define MOTOR_4_CHANNEL_A 14 //hook
#define MOTOR_4_CHANNEL_B 15

#define PS2_DAT 12 // MISO
#define PS2_CMD 13 // MOSI
#define PS2_SEL 15 // SS
#define PS2_CLK 14 // SLK

#define SERVOMIN  800 // the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  800 // the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

#define pressures false
#define rumble false

#define SERVO_1_CHANNEL 7

#define NOTIFY_LED 13

#define MAX_SPEED    2048

uint8_t servonum = 0;

const int journeyButton1 = 36; 
int value1 = 0; 
const int journeyButton2 = 39; 
int value2 = 0; 

void setMotorSpeed(int speedR1, int speedR2) {
  if (speedR1 >= 0) {
    pwm.setPin(MOTOR_1_CHANNEL_A, speedR1);
    pwm.setPin(MOTOR_1_CHANNEL_B, 0);
  } else {
    pwm.setPin(MOTOR_1_CHANNEL_A, 0);
    pwm.setPin(MOTOR_1_CHANNEL_B, abs(speedR1));
  }

  if (speedR2 >= 0) {
    pwm.setPin(MOTOR_2_CHANNEL_A, speedR2);
    pwm.setPin(MOTOR_2_CHANNEL_B, 0);
  } else {
    pwm.setPin(MOTOR_2_CHANNEL_A, 0);
    pwm.setPin(MOTOR_2_CHANNEL_B, abs(speedR2));
  }
}

void setSlideSpeed(int speedR3){
  if (speedR3 >= 0) {
    pwm.setPin(MOTOR_3_CHANNEL_A, speedR3);
    pwm.setPin(MOTOR_3_CHANNEL_B, 0);
  } else {
    pwm.setPin(MOTOR_3_CHANNEL_A, 0);
    pwm.setPin(MOTOR_3_CHANNEL_B, abs(speedR3));
  }
}

void setHookSpeed(int speedR4){
  if (speedR4 >= 0) {
    pwm.setPin(MOTOR_4_CHANNEL_A, speedR4);
    pwm.setPin(MOTOR_4_CHANNEL_B, 0);
  } else {
    pwm.setPin(MOTOR_4_CHANNEL_A, 0);
    pwm.setPin(MOTOR_4_CHANNEL_B, abs(speedR4));
  }
}

void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= SERVO_FREQ;   // Analog servos run at ~60 Hz updates
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000000;  // convert input seconds to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}

PS2X ps2x; // khởi tạo class PS2x

void setup()
{
  pwm.begin();
  pwm.setOscillatorFrequency(27000000); 
  pwm.setPWMFreq(50);
  pinMode(NOTIFY_LED, OUTPUT); 
  pinMode(journeyButton1, INPUT);
  pinMode(journeyButton2, INPUT);
  
  Serial.begin(115200);
  Serial.print("Ket noi voi tay cam PS2:");

  int error = -1;
  for (int i = 0; i < 10 && error != 0; i++) // thử kết nối với tay cầm ps2 trong 10 lần
  {
    delay(1000); // đợi 1 giây
    // cài đặt chân và các chế độ: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
    error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
    Serial.print(".");
  }

  switch (error) // kiểm tra lỗi nếu sau 10 lần không kết nối được
  {
  case 0:
    Serial.println(" Ket noi tay cam PS2 thanh cong");
    break;
  case 1:
    Serial.println(" LOI: Khong tim thay tay cam, hay kiem tra day ket noi vơi tay cam ");
    break;
  case 2:
    Serial.println(" LOI: khong gui duoc lenh");
    break;
  case 3:
    Serial.println(" LOI: Khong vao duoc Pressures mode ");
    break;
  }
}

void loop()
{
  value1 = digitalRead(journeyButton1);
  value2 = digitalRead(journeyButton2);
  ps2x.read_gamepad(0, 0);

  int leftspeed = map(ps2x.Analog(PSS_LY), 0, 255, MAX_SPEED, -MAX_SPEED);
  int rightspeed = map(ps2x.Analog(PSS_RY), 0, 255, MAX_SPEED, -MAX_SPEED);
  setMotorSpeed(leftspeed, -rightspeed);

  if(ps2x.Button(PSB_R1) && value1 == LOW){
    setSlideSpeed(-4095);
  } 
  else if(ps2x.Button(PSB_R2) && value2 == LOW){
    setSlideSpeed(4095);
  }
  else if(value1 == HIGH){
    setSlideSpeed(0);
  }
  else if(value2 == HIGH){
    setSlideSpeed(0);
  }
  else{
    setSlideSpeed(0);
  }

  if(ps2x.Button(PSB_L1)){
    setHookSpeed(1024);
  }
  else if(ps2x.Button(PSB_L2)){
    setHookSpeed(-1024);
  }
  else{
    setHookSpeed(0);
  }

  pwm.writeMicroseconds(7, 0);
  if(ps2x.Button(PSB_TRIANGLE)){
    pwm.writeMicroseconds(7, USMIN);
  }
  else if(ps2x.Button(PSB_CROSS)){
    pwm.writeMicroseconds(7, USMAX);
  }
  else{
    pwm.writeMicroseconds(7, 0);
  }

  pwm.writeMicroseconds(6, 0);
  if(ps2x.Button(PSB_SQUARE)){
    pwm.writeMicroseconds(6, USMIN);
  }
  else if(ps2x.Button(PSB_CIRCLE)){
    pwm.writeMicroseconds(6, USMAX);
  }
  else{
    pwm.writeMicroseconds(6, 0);
  }
}
