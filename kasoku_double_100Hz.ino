// MPU-6050 Accelerometer + Gyro

// I2CにアクセスするためにWireライブラリを使用
#include <Wire.h>

// レジスタアドレス
#define MPU6050_ACCEL_XOUT_H 0x3B  // R  
#define MPU6050_WHO_AM_I     0x75  // R
#define MPU6050_PWR_MGMT_1   0x6B  // R/W
#define MPU6050_I2C_ADDRESS  0x68
#define MPU6050_I2C_ADDRESS_2  0x69

// 構造体定義
typedef union accel_t_gyro_union {
  struct {
    uint8_t x_accel_h;
    uint8_t x_accel_l;
    uint8_t y_accel_h;
    uint8_t y_accel_l;
    uint8_t z_accel_h;
    uint8_t z_accel_l;
    uint8_t t_h;
    uint8_t t_l;
    uint8_t x_gyro_h;
    uint8_t x_gyro_l;
    uint8_t y_gyro_h;
    uint8_t y_gyro_l;
    uint8_t z_gyro_h;
    uint8_t z_gyro_l;
  }
  reg;
  struct {
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
    int16_t temperature;
    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
  }
  value;
};

unsigned long sequence = 0;
unsigned long times = 0;
unsigned long times_zero = 0;

int error;
int error2;
float dT;

accel_t_gyro_union accel_t_gyro;
accel_t_gyro_union accel_t_gyro_2;

uint8_t swap;

double acc_x;
double acc_y;
double acc_z;
double acc_x_2;
double acc_y_2;
double acc_z_2;

double acc_x_f=0;
double acc_y_f=0;
double acc_z_f=0;
double acc_x_f_2=0;
double acc_y_f_2=0;
double acc_z_f_2=0;

double gyro_x;
double gyro_y;
double gyro_z;
double gyro_x_2;
double gyro_y_2;
double gyro_z_2;

double gyro_x_f=0;
double gyro_y_f=0;
double gyro_z_f=0;
double gyro_x_f_2=0;
double gyro_y_f_2=0;
double gyro_z_f_2=0;

// デバイス初期化時に実行される
void setup() {
  int error;
  uint8_t c;

  Wire.begin();

  // ボーレートを115200bpsにセット
  Serial.begin(115200);
  Serial.print("InvenSense MPU-6050");
  Serial.print("June 2012");

  // 初回の読み出し
  error = MPU6050_read(MPU6050_WHO_AM_I, &c, 1);
  Serial.print("WHO_AM_I : ");
  Serial.print(c, HEX);
  Serial.print(", error = ");
  Serial.println(error, DEC);

  error = MPU6050_read_2(MPU6050_WHO_AM_I, &c, 1);
  Serial.print("WHO_AM_I : ");
  Serial.print(c, HEX);
  Serial.print(", error = ");
  Serial.println(error, DEC);

  // 動作モードの読み出し
  error = MPU6050_read(MPU6050_PWR_MGMT_1, &c, 1);
  Serial.print("PWR_MGMT_1 : ");
  Serial.print(c, HEX);
  Serial.print(", error = ");
  Serial.println(error, DEC);

  error = MPU6050_read_2(MPU6050_PWR_MGMT_1, &c, 1);
  Serial.print("PWR_MGMT_1 : ");
  Serial.print(c, HEX);
  Serial.print(", error = ");
  Serial.println(error, DEC);

  // MPU6050動作開始
  MPU6050_write_reg(MPU6050_PWR_MGMT_1, 0);
  
  times_zero = micros();
}

void loop() {
  times = micros();

  if(times - times_zero > 10000){
    times_zero += 10000;
  

  // 加速度、角速度の読み出し
  // accel_t_gyroは読み出した値を保存する構造体、その後ろの引数は取り出すバイト数
  error = MPU6050_read(MPU6050_ACCEL_XOUT_H, (uint8_t *)&accel_t_gyro, sizeof(accel_t_gyro));
  //Serial.print(error, DEC);
  //Serial.print(",");

  error2 = MPU6050_read_2(MPU6050_ACCEL_XOUT_H, (uint8_t *)&accel_t_gyro_2, sizeof(accel_t_gyro_2));
  //Serial.print(error2, DEC);
  //Serial.print(",");

  // 取得できるデータはビッグエンディアンなので上位バイトと下位バイトの入れ替え（AVRはリトルエンディアン）
#define SWAP(x,y) swap = x; x = y; y = swap
  SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
  SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
  SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
  SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
  SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
  SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
  SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);

  //uint8_t swap;
//#define SWAP(x,y) swap = x; x = y; y = swap
  SWAP (accel_t_gyro_2.reg.x_accel_h, accel_t_gyro_2.reg.x_accel_l);
  SWAP (accel_t_gyro_2.reg.y_accel_h, accel_t_gyro_2.reg.y_accel_l);
  SWAP (accel_t_gyro_2.reg.z_accel_h, accel_t_gyro_2.reg.z_accel_l);
  SWAP (accel_t_gyro_2.reg.t_h, accel_t_gyro_2.reg.t_l);
  SWAP (accel_t_gyro_2.reg.x_gyro_h, accel_t_gyro_2.reg.x_gyro_l);
  SWAP (accel_t_gyro_2.reg.y_gyro_h, accel_t_gyro_2.reg.y_gyro_l);
  SWAP (accel_t_gyro_2.reg.z_gyro_h, accel_t_gyro_2.reg.z_gyro_l);

  // 取得した加速度値を分解能で割って加速度(G)に変換する
  acc_x = accel_t_gyro.value.x_accel / 1638.4; //FS_SEL_0 16,384 LSB / g
  acc_y = accel_t_gyro.value.y_accel / 1638.4;
  acc_z = accel_t_gyro.value.z_accel / 1638.4;

  acc_x_2 = accel_t_gyro_2.value.x_accel / 1638.4; //FS_SEL_0 16,384 LSB / g
  acc_y_2 = accel_t_gyro_2.value.y_accel / 1638.4;
  acc_z_2 = accel_t_gyro_2.value.z_accel / 1638.4;

  //acc_x_f = acc_x_f * 0.9 + acc_x * 0.1;
  //acc_y_f = acc_y_f * 0.9 + acc_y * 0.1;
  //acc_z_f = acc_z_f * 0.9 + acc_z * 0.1;

  //acc_x_f_2 = acc_x_f_2 * 0.9 + acc_x_2 * 0.1;
  //acc_y_f_2 = acc_y_f_2 * 0.9 + acc_y_2 * 0.1;
  //acc_z_f_2 = acc_z_f_2 * 0.9 + acc_z_2 * 0.1;


  Serial.print(-1 * acc_z,4);
  Serial.print(",");
  Serial.print(acc_y,4);
  Serial.print(",");
  Serial.print(acc_x,4);

  Serial.print(",");


  Serial.print(-1 * acc_z_2,4);
  Serial.print(",");
  Serial.print(acc_y_2,4);
  Serial.print(",");
  Serial.print(acc_x_2,4);

  Serial.print(",");

  Serial.print(sequence);
  Serial.print(",");

 
  

  sequence++;

  
  // 取得した角速度値を分解能で割って角速度(degrees per sec)に変換する
  gyro_x = accel_t_gyro.value.x_gyro * 0.00013323;//FS_SEL_0 131 LSB / (°/s) 131.0 * 0.0174533 2.2863823
  gyro_y = accel_t_gyro.value.y_gyro * 0.00013323;
  gyro_z = accel_t_gyro.value.z_gyro * 0.00013323;

  gyro_x_2 = accel_t_gyro_2.value.x_gyro * 0.00013323;//FS_SEL_0 131 LSB / (°/s)
  gyro_y_2 = accel_t_gyro_2.value.y_gyro * 0.00013323;
  gyro_z_2 = accel_t_gyro_2.value.z_gyro * 0.00013323;

  //gyro_x_f = 0.9 * gyro_x_f + 0.1 * gyro_x;
  //gyro_y_f = 0.9 * gyro_y_f + 0.1 * gyro_y;
  //gyro_z_f = 0.9 * gyro_z_f + 0.1 * gyro_z;

  //gyro_x_f_2 = 0.9 * gyro_x_f_2 + 0.1 * gyro_x_2;
  //gyro_y_f_2 = 0.9 * gyro_y_f_2 + 0.1 * gyro_y_2;
  //gyro_z_f_2 = 0.9 * gyro_z_f_2 + 0.1 * gyro_z_2;
  
  Serial.print(",");
  Serial.print(-1 * gyro_z,4);
  Serial.print(",");
  Serial.print(gyro_y,4);
  Serial.print(",");
  Serial.print(gyro_x,4);

  Serial.print(",");

  Serial.print(-1 * gyro_z_2,4);
  Serial.print(",");
  Serial.print(gyro_y_2,4);
  Serial.print(",");
  Serial.print(gyro_x_2,4);

  Serial.print(",");
  
  Serial.print(sequence);
  
  Serial.print(",");
  
  Serial.print(times);
  Serial.print("\n");
  //Serial.println();

  sequence++;
  }
}

// MPU6050_read
int MPU6050_read(int start, uint8_t *buffer, int size) {
  int i, n, error;
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);
  if (n != 1) {
    return (-10);
  }
  n = Wire.endTransmission(false);// hold the I2C-bus
  if (n != 0) {
    return (n);
  }
  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
  i = 0;
  while (Wire.available() && i < size) {
    buffer[i++] = Wire.read();
  }
  if ( i != size) {
    return (-11);
  }
  return (0); // return : no error
}

int MPU6050_read_2(int start, uint8_t *buffer, int size) {
  int i, n, error;
  Wire.beginTransmission(MPU6050_I2C_ADDRESS_2);
  n = Wire.write(start);
  if (n != 1) {
    return (-10);
  }
  n = Wire.endTransmission(false);// hold the I2C-bus
  if (n != 0) {
    return (n);
  }
  // Third parameter is true: relase I2C-bus after data is read.
  Wire.requestFrom(MPU6050_I2C_ADDRESS_2, size, true);
  i = 0;
  while (Wire.available() && i < size) {
    buffer[i++] = Wire.read();
  }
  if ( i != size) {
    return (-11);
  }
  return (0); // return : no error
}

// MPU6050_write
int MPU6050_write(int start, const uint8_t *pData, int size) {
  int n, error;

  //Wire.beginTransmission(0x68);
  //Wire.write(0x1A);
  //Wire.write(0x03);
  //Wire.endTransmission();
  
  Wire.beginTransmission(MPU6050_I2C_ADDRESS);
  n = Wire.write(start);// write the start address
  if (n != 1) {
    return (-20);
  }
  n = Wire.write(pData, size);// write data bytes
  if (n != size) {
    return (-21);
  }
  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0) {
    return (error);
  }

  return (0);// return : no error
}

// MPU6050_write
int MPU6050_write_2(int start, const uint8_t *pData, int size) {
  int n, error;

  //Wire.beginTransmission(0x68);
  //Wire.write(0x1A);
  //Wire.write(0x03);
  //Wire.endTransmission();
  
  Wire.beginTransmission(MPU6050_I2C_ADDRESS_2);
  n = Wire.write(start);// write the start address
  if (n != 1) {
    return (-20);
  }
  n = Wire.write(pData, size);// write data bytes
  if (n != size) {
    return (-21);
  }
  error = Wire.endTransmission(true); // release the I2C-bus
  if (error != 0) {
    return (error);
  }

  return (0);// return : no error
}

// MPU6050_write_reg
int MPU6050_write_reg(int reg, uint8_t data) {
  int error;
  error = MPU6050_write(reg, &data, 1);
  error = MPU6050_write_2(reg, &data, 1);
  Serial.print("error = ");
  Serial.println(error);
  return (error);
};
