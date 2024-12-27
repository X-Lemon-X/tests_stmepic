#include "stmepic.hpp"
#include "movement_controler.hpp"
#include "Timing.hpp"
#include "can_control.hpp"
#include "can_control.hpp"
#include "steper_motor.hpp"
#include "encoder_magnetic.hpp"
#include "memory_fram.hpp"
#include "filter.hpp"
#include "main.h"
#include "pin.hpp"
#include "movement_controler.hpp"
#include "controler_linear.hpp"
#include "encoder_magnetic.hpp"
#include "motor.hpp"
#include "Timing.hpp"
#include <memory>
#include <vector>
#include "fram_i2c.hpp"

extern CAN_HandleTypeDef hcan1;


extern CAN_HandleTypeDef hcan2;

extern I2C_HandleTypeDef hi2c1;

extern SPI_HandleTypeDef hspi1;

extern TIM_HandleTypeDef htim10;

extern UART_HandleTypeDef huart1;

extern HCD_HandleTypeDef hhcd_USB_OTG_FS;

using  namespace stmepic;


gpio::GpioPin direction_pin={GPIO_PIN_0, GPIOB, 0};
gpio::GpioPin enable_pin={GPIO_PIN_1, GPIOB, 0};
gpio::GpioPin step_pin={GPIO_PIN_2, GPIOB, 0};





movement::MovementControler movementControler;
encoders::EncoderAbsoluteMagnetic encoderAbsoluteMagnetic;
movement::BasicLinearPosControler basicLinearPosControler;
motor::SteperMotorStepDir motorStepDir(htim10,TIM_CHANNEL_1, direction_pin, enable_pin);
motor::MotorClosedLoop motorClosedLoop(motorStepDir,&encoderAbsoluteMagnetic,&encoderAbsoluteMagnetic,nullptr);
TimeScheduler timeScheduler(Ticker::get_instance());
filters::FilterSampleSkip fss;
CanControl<> canControl;
memory::FRAM *fram= new memory::FramI2C(hi2c1,0x60,0x0,0xffff);



void test_movement_controler(){
  movementControler.init(motorClosedLoop, movement::MovementControlMode::POSITION, basicLinearPosControler);
  movementControler.set_enable(false);
}

void task1(Timing& t){
  movementControler.handle();

}

void task2(Timing& t){
  encoderAbsoluteMagnetic.handle();
}

void can_callback(CanMsg& msg){
  __NOP();
}

void main_prog() {
  canControl.init(hcan1, 0, nullptr, nullptr);
  canControl.add_callback(CanControl<>::CAN_DEFAULT_FRAME_ID, can_callback);
  fram->device_start();
  encoderAbsoluteMagnetic.device_start();

  encoderAbsoluteMagnetic.init(hi2c1, encoders::translate_reg_to_angle_AS5600, &fss, &fss);
  auto s = stmepic::Timing::Make(
    100,
    true,
    task1,
    Ticker::get_instance()
  );
  timeScheduler.add_timer(s.valueOrDie());
  s = stmepic::Timing::Make(
    100,
    true,
    task2,
    Ticker::get_instance()
  );

  timeScheduler.add_timer(s.valueOrDie());


  timeScheduler.schedules_handle_blocking();
}