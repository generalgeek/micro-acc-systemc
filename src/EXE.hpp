/*
 * @ASCK
 */
#pragma once
#include <systemc.h>

extern bool now_is_call;
extern sc_event micro_acc_ev;

// 定义一个名为EXE的SC_MODULE（SystemC模块）
SC_MODULE(EXE) {
  // 输入端口定义
  sc_in_clk clk;                 // 时钟信号输入端口
  sc_in<sc_int<8>> prev_result;  // 8位整数类型的结果输入端口
  sc_in<sc_int<13>> prev_Imm;    // 13位整数类型的立即数输入端口
  sc_in<bool> prev_r;            // 读取信号输入端口
  sc_in<bool> prev_w;            // 写入信号输入端口
  sc_in<bool> prev_WbMux;        // 写回多路复用器信号输入端口
  sc_in<bool> prev_call;         // 调用信号输入端口
  sc_in<bool> prev_regWrite;     // 寄存器写入信号输入端口
  sc_in<sc_uint<3>> prev_rd;     // 3位无符号整数类型的目标寄存器输入端口

  // 输出端口定义
  sc_out<sc_int<8>> next_result;  // 8位整数类型的结果输出端口
  sc_out<sc_int<13>> next_Imm;    // 13位整数类型的立即数输出端口
  sc_out<bool> next_r;            // 读取信号输出端口
  sc_out<bool> next_w;            // 写入信号输出端口
  sc_out<bool> next_WbMux;        // 写回多路复用器信号输出端口
  sc_out<bool> next_call;         // 调用信号输出端口
  sc_out<bool> next_regWrite;     // 寄存器写入信号输出端口
  sc_out<sc_uint<3>> next_rd;     // 3位无符号整数类型的目标寄存器输出端口

  // 模块全局变量
  bool now_is_call;       // 当前是否是调用信号
  sc_event micro_acc_ev;  // 微架构事件

  // 模块构造函数，定义一个名为process的线程，并在时钟的正边沿敏感
  SC_CTOR(EXE) {
    SC_THREAD(process);      // 定义进程线程
    sensitive << clk.pos();  // 对时钟的正边沿敏感
  }

  // 定义process线程函数
  void process() {
    while (true) {           // 无限循环，确保线程持续运行
      wait();                // 等待时钟正边沿信号
      if (now_is_call) {     // 如果当前是调用信号
        wait(micro_acc_ev);  // 等待微架构事件
      }
      // 将输入端口的值传递到输出端口
      next_result.write(prev_result.read());  // 将prev_result的值写入next_result
      next_Imm.write(prev_Imm.read());        // 将prev_Imm的值写入next_Imm
      next_r.write(prev_r.read());            // 将prev_r的值写入next_r
      next_w.write(prev_w.read());            // 将prev_w的值写入next_w
      next_WbMux.write(prev_WbMux.read());    // 将prev_WbMux的值写入next_WbMux
      next_call.write(prev_call.read());      // 将prev_call的值写入next_call
      next_regWrite.write(prev_regWrite);     // 将prev_regWrite的值写入next_regWrite
      next_rd.write(prev_rd.read());          // 将prev_rd的值写入next_rd
    }
  }
};
