/*
 * @ASCK
 */
#pragma once
#include <systemc.h>

SC_MODULE(ID) {
  // 定义输入和输出端口类型
  sc_in_clk clk;                 // 输入时钟信号
  sc_in<sc_int<8>> prev_A;       // 上一个周期的A值
  sc_in<sc_int<8>> prev_B;       // 上一个周期的B值
  sc_in<sc_int<13>> prev_Imm;    // 上一个周期的立即数（Imm）
  sc_in<sc_uint<3>> prev_Sa;     // 上一个周期的移位量（Sa）
  sc_in<sc_uint<5>> prev_AluOp;  // 上一个周期的ALU操作码（AluOp）
  sc_in<bool> prev_r;            // 上一个周期的读信号（r）
  sc_in<bool> prev_w;            // 上一个周期的写信号（w）
  sc_in<bool> prev_AluMux;       // 上一个周期的ALU多路选择器信号（AluMux）
  sc_in<bool> prev_WbMux;        // 上一个周期的写回多路选择器信号（WbMux）
  sc_in<bool> prev_call;         // 上一个周期的调用信号（call）
  sc_in<bool> prev_regWrite;     // 上一个周期的寄存器写信号（regWrite）
  sc_in<sc_uint<3>> prev_rd;     // 上一个周期的目的寄存器编号（rd）

  // 定义输出端口类型
  sc_out<sc_int<8>> next_A;       // 下一个周期的A值
  sc_out<sc_int<8>> next_B;       // 下一个周期的B值
  sc_out<sc_int<13>> next_Imm;    // 下一个周期的立即数（Imm）
  sc_out<sc_uint<3>> next_Sa;     // 下一个周期的移位量（Sa）
  sc_out<sc_uint<5>> next_AluOp;  // 下一个周期的ALU操作码（AluOp）
  sc_out<bool> next_r;            // 下一个周期的读信号（r）
  sc_out<bool> next_w;            // 下一个周期的写信号（w）
  sc_out<bool> next_AluMux;       // 下一个周期的ALU多路选择器信号（AluMux）
  sc_out<bool> next_WbMux;        // 下一个周期的写回多路选择器信号（WbMux）
  sc_out<bool> next_call;         // 下一个周期的调用信号（call）
  sc_out<bool> next_regWrite;     // 下一个周期的寄存器写信号（regWrite）
  sc_out<sc_uint<3>> next_rd;     // 下一个周期的目的寄存器编号（rd）

  /*
  ** module global variables
  */

  // 模块的构造函数
  SC_CTOR(ID) {
    // 注册进程函数process，并对clk的上升沿敏感
    SC_THREAD(process);
    sensitive << clk.pos();
  }

  // 进程函数定义
  void process() {
    while (true) {
      wait();  // 等待时钟的上升沿
      if (now_is_call) {
        wait(micro_acc_ev);  // 如果当前是调用指令，则等待micro_acc_ev事件
      }
      // 将上一个周期的输入值传递到下一个周期的输出端口
      next_A.write(prev_A.read());
      next_B.write(prev_B.read());
      next_Imm.write(prev_Imm.read());
      next_Sa.write(prev_Sa.read());
      next_AluOp.write(prev_AluOp.read());
      next_AluMux.write(prev_AluMux.read());
      next_r.write(prev_r.read());
      next_w.write(prev_w.read());
      next_WbMux.write(prev_WbMux.read());
      next_call.write(prev_call.read());
      next_regWrite.write(prev_regWrite);  // 注意，这里缺少了.read()，应该是next_regWrite.write(prev_regWrite.read());
      next_rd.write(prev_rd.read());
    }
  }
};
