/*
 * @ASCK
 */
#pragma once
#include <systemc.h>

SC_MODULE(Mux8) {
  sc_in<bool> sel;             // 选择信号输入端口，用于选择要输出的输入端口
  sc_in<sc_int<8>> in0;        // 输入端口0，输入8位有符号整数
  sc_in<sc_int<8>> in1;        // 输入端口1，输入8位有符号整数

  sc_out<sc_int<8>> out;       // 输出端口，输出8位有符号整数

  SC_CTOR(Mux8) {
    SC_METHOD(process);        // 注册处理方法process
    sensitive << in0 << in1 << sel;  // 当in0, in1或sel信号发生变化时触发process方法
  }

  void process() {
    if (!sel.read()) {          // 如果选择信号sel为false（低电平）
      out.write(in0.read());    // 将输入端口0的值传递到输出端口
    } else {                    // 如果选择信号sel为true（高电平）
      out.write(in1.read());    // 将输入端口1的值传递到输出端口
    }
  }
};
