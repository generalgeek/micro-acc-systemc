/*
 * @ASCK
 */
#pragma once
#include <systemc.h>

// 指令获取阶段中间寄存器 - 用于流水线
SC_MODULE(IF) {
  sc_in_clk clk;                  // 声明输入时钟信号
  sc_in<sc_uint<20>> prev_data;   // 声明一个 20 位无符号整数的输入端口
  sc_out<sc_uint<20>> next_data;  // 声明一个 20 位无符号整数的输出端口
  /*
  ** 模块全局变量
  */

  // 模块的构造函数，初始化时声明一个名为 "process" 的线程
  SC_CTOR(IF) {
    SC_THREAD(process);
    sensitive << clk.pos();  // 使该线程对时钟上升沿敏感
  }

  // 定义线程函数 "process"
  void process() {
    while (true) {                                      // 无限循环
      wait();                                           // 等待时钟事件
      if (now_is_call) {                                // 如果 "now_is_call" 为真
        cout << "\t\t\t\t*******************" << endl;  // 输出一行分隔符
        wait(micro_acc_ev);                             // 等待 "micro_acc_ev" 事件
      }
      next_data.write(prev_data.read());  // 将 "prev_data" 的值写入到 "next_data"
    }
  }
};
