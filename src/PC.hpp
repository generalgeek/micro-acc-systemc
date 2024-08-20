/*
 * @ASCK
 */
#pragma once
#include <systemc.h>

// 程序计数器
SC_MODULE(PC) {
  
  sc_in<bool> clk;                // 时钟输入端口，用于接收时钟信号
  sc_in<sc_uint<14>> prev_addr;   // 上一个周期输入端口，接收14位无符号整数
  sc_out<sc_uint<14>> next_addr;  // 下一个周期输出端口，输出14位无符号整数

  SC_CTOR(PC) {
    SC_METHOD(process);          // 注册处理方法process
    sensitive << clk.pos();      // 当时钟上升沿检测到时触发process方法
    // dont_initialize();
  }

  void process() {
    next_addr.write(prev_addr.read());  // 将上一个地址的值写入到下一个地址输出端口
    std::cout << sc_time_stamp() << " next_addr=" << next_addr.read() << std::endl;
  }
};
