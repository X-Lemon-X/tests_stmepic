#include "stmepic.hpp"
#include "movement_controler.hpp"
#include "Timing.hpp"
#include "steper_motor.hpp"
#include "encoder_magnetic.hpp"
#include "memory_fram.hpp"
#include "filter.hpp"
#include "main.h"
#include "gpio.hpp"
#include "movement_controler.hpp"
#include "controler_linear.hpp"
#include "encoder_magnetic.hpp"
#include "motor.hpp"
#include "Timing.hpp"
#include <memory>
#include <vector>
#include "fram_i2c.hpp"
#include "logger.hpp"
#include "can.hpp"
#include "i2c.hpp"

extern CAN_HandleTypeDef hcan1;


extern CAN_HandleTypeDef hcan2;

extern I2C_HandleTypeDef hi2c1;

extern SPI_HandleTypeDef hspi1;

extern TIM_HandleTypeDef htim10;

extern UART_HandleTypeDef huart1;

extern HCD_HandleTypeDef hhcd_USB_OTG_FS;

using  namespace stmepic;


GpioPin direction_pin(*GPIOB,GPIO_PIN_0);
GpioPin enable_pin(*GPIOB,GPIO_PIN_1);
GpioPin step_pin(*GPIOB,GPIO_PIN_2);
GpioPin pin_i2c1_sda(*GPIOB, GPIO_PIN_7);
GpioPin pin_i2c1_scl(*GPIOB, GPIO_PIN_6);



movement::MovementControler movementControler;
encoders::EncoderAbsoluteMagnetic encoderAbsoluteMagnetic;
movement::BasicLinearPosControler basicLinearPosControler;
motor::SteperMotorStepDir motorStepDir(htim10,TIM_CHANNEL_1, direction_pin, enable_pin);
motor::MotorClosedLoop motorClosedLoop(motorStepDir,&encoderAbsoluteMagnetic,&encoderAbsoluteMagnetic,nullptr);
TimeScheduler timeScheduler(Ticker::get_instance());
filters::FilterSampleSkip fss;
memory::FRAM *fram;

std::shared_ptr<stmepic::I2C> i2c1;
std::shared_ptr<stmepic::CAN> can1;


void can_callback(stmepic::CAN&can, CanDataFrame& frame, void *arg) {
  __NOP();
}

void main_prog() {
   CAN_FilterTypeDef can_filter;
  can_filter.FilterBank           = 1;
  can_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  can_filter.FilterActivation     = CAN_FILTER_ENABLE;
  can_filter.FilterMode           = CAN_FILTERMODE_IDMASK;
  can_filter.FilterScale          = CAN_FILTERSCALE_16BIT;
  can_filter.FilterIdHigh         = 0;
  can_filter.FilterIdLow          = 0;
  can_filter.FilterMaskIdHigh     = 0;
  can_filter.FilterMaskIdLow      = 0;
  can_filter.SlaveStartFilterBank = 0;
  auto mayby_can1 = stmepic::CAN::Make(hcan1,can_filter);
  if(!mayby_can1.ok()) {
    log_error("CAN1 error: " + mayby_can1.status().to_string());
    HAL_NVIC_SystemReset();
  }
  can1 = mayby_can1.valueOrDie();

  auto mayby_i2c1 = stmepic::I2C::Make(hi2c1,pin_i2c1_sda,pin_i2c1_scl,HardwareType::BLOCKING);
  if(!mayby_i2c1.ok()) {
    log_error("I2C1 error: " + mayby_i2c1.status().to_string());
    HAL_NVIC_SystemReset();
  }
  i2c1 = mayby_i2c1.valueOrDie();


  can1->add_callback(0x123,can_callback);
  can1->hardware_start();
  i2c1->hardware_start();
  
  fram = new memory::FramI2C(i2c1,0x60,0x0,0xffff)
  fram->device_enable();
  encoderAbsoluteMagnetic.init(i2c1, encoders::translate_reg_to_angle_AS5600, &fss, &fss);
  encoderAbsoluteMagnetic.device_enable();
  encoderAbsoluteMagnetic.device_task_start();
}