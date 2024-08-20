/*
 * @ASCK
 */
#pragma once
#include <systemc.h>

SC_MODULE(WB) {

  sc_in_clk clk;                        // 时钟输入端口
  sc_in<sc_int<8>> prev_alu_result;     // 上一阶段ALU计算结果输入端口
  sc_in<sc_int<8>> prev_mem_result;     // 上一阶段内存操作结果输入端口
  sc_in<bool> prev_WbMux;               // 上一阶段写回选择信号输入端口
  sc_in<bool> prev_regWrite;            // 上一阶段寄存器写使能信号输入端口
  sc_in<sc_uint<3>> prev_rd;            // 上一阶段目标寄存器地址输入端口

  sc_out<sc_int<8>> next_alu_result;    // 下一阶段ALU计算结果输出端口
  sc_out<sc_int<8>> next_mem_result;    // 下一阶段内存操作结果输出端口
  sc_out<bool> next_WbMux;              // 下一阶段写回选择信号输出端口
  sc_out<bool> next_regWrite;           // 下一阶段寄存器写使能信号输出端口
  sc_out<sc_uint<3>> next_rd;           // 下一阶段目标寄存器地址输出端口

  /*
  ** module global variables
  */

  SC_CTOR(WB) {
    SC_THREAD(process);                 // 定义SC_THREAD类型的进程process
    sensitive << clk.pos();             // 当时钟上升沿检测到时触发process方法
  }

  void process() {
    while (true) {                      // 无限循环，模拟持续运行的行为
      wait();                           // 等待时钟上升沿事件发生
      if (now_is_call) {                // 如果当前正在调用
        wait(micro_acc_ev);             // 等待微执行事件
      }
      next_alu_result.write(prev_alu_result.read());   // 将上一阶段ALU计算结果写入到下一阶段输出端口
      next_mem_result.write(prev_mem_result.read());   // 将上一阶段内存操作结果写入到下一阶段输出端口
      next_WbMux.write(prev_WbMux.read());             // 将上一阶段写回选择信号写入到下一阶段输出端口
      next_regWrite.write(prev_regWrite);              // 将上一阶段寄存器写使能信号写入到下一阶段输出端口
      next_rd.write(prev_rd.read());                   // 将上一阶段目标寄存器地址写入到下一阶段输出端口
    }
  }
};
