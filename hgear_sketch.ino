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

#define pressures false
#define rumble false

#define SERVO_1_CHANNEL 7

#define NOTIFY_LED 13

#define MAX_SPEED    2048

const int journeyButton = 36; 
int value = 0; 

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

void setServo(uint8_t channel, uint16_t pulse) {
  pwm.setPWM(channel, 0, pulse);
}

PS2X ps2x; // khởi tạo class PS2x

void setup()
{
  pwm.begin();
  pwm.setOscillatorFrequency(27000000); 
  pwm.setPWMFreq(50);
  pinMode(NOTIFY_LED, OUTPUT); 
  pinMode(journeyButton, INPUT);
  
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
  value = digitalRead(journeyButton);
  ps2x.read_gamepad(0, 0);

  int leftspeed = map(ps2x.Analog(PSS_LY), 0, 255, MAX_SPEED, -MAX_SPEED);
  int rightspeed = map(ps2x.Analog(PSS_RY), 0, 255, MAX_SPEED, -MAX_SPEED);
  setMotorSpeed(leftspeed, -rightspeed);

  if(ps2x.Button(PSB_R1) && value == LOW){
    setSlideSpeed(-4095);
  } 
  else if(ps2x.Button(PSB_R2)){
    setSlideSpeed(4095);
  }
  else if(value == HIGH){
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

  setServo(7, 0);
  if(ps2x.Button(PSB_TRIANGLE)){
    setServo(7, 5000);
  }
  else if(ps2x.Button(PSB_CROSS)){
    setServo(7,0);
  }
}
