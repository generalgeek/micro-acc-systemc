/*
 * @ASCK
 */
#pragma once
#include <systemc.h>

SC_MODULE(RegFile) {

  sc_in_clk clk;              // 时钟输入端口，用于接收时钟信号
  sc_in<bool> regWrite;       // 寄存器写使能信号输入端口
  sc_in<sc_uint<3>> r1;       // 寄存器读端口1的地址输入端口
  sc_in<sc_uint<3>> r2;       // 寄存器读端口2的地址输入端口
  sc_in<sc_uint<3>> r3;       // 寄存器写端口的地址输入端口
  sc_in<sc_int<8>> data;      // 写入寄存器的数据输入端口

  sc_out<sc_int<8>> Rr1;      // 寄存器读端口1的数据输出端口
  sc_out<sc_int<8>> Rr2;      // 寄存器读端口2的数据输出端口

  // testing wires
  sc_out<sc_int<8>> r[8];     // 用于测试的8个寄存器输出端口数组

  sc_int<8> reg[8];           // 寄存器文件中的8个寄存器数组

  int c = 0;                  // 计数器变量

  SC_CTOR(RegFile) {
    SC_METHOD(process);       // 注册处理方法process
    sensitive << regWrite << r1 << r2 << r3 << data;  // 当regWrite、r1、r2、r3、data任一信号变化时触发process方法
  }

  void process() {
    // 无论regWrite是0还是1，都会根据r1和r2的地址从reg中读取数据写入到Rr1和Rr2的输出端口
    Rr1.write(reg[r1.read()]);
    Rr2.write(reg[r2.read()]);

    if (regWrite.read() == 1) {  // 如果regWrite为1，则执行写操作
      reg[r3.read()] = data.read();  // 将data中的数据写入到r3指定的寄存器中
    }

    for (int i = 0; i < 8; i++) {
      r[i].write(reg[i]);       // 将reg数组中的数据写入到测试寄存器输出端口数组r中
    }

    if (c++ == 32) {             // 每经过32个时钟周期
      reg[0] = 3;                // 将寄存器文件中第0号寄存器的值设置为3
    }
  }
};
