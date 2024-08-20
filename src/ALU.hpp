/*
 * @ASCK
 */
#pragma once
#include <systemc.h>

SC_MODULE(ALU) {
  // 输入端口
  sc_in<sc_int<8>> in1;     // 输入操作数 A
  sc_in<sc_int<8>> in2;     // 输入操作数 B
  sc_in<bool> c;            // 进位信号（在这个项目中始终为1）
  sc_in<sc_uint<5>> aluop;  // ALU操作码
  sc_in<sc_uint<3>> sa;     // 移位量

  // 输出端口
  sc_out<sc_int<8>> out;  // 输出结果

  // 构造函数
  SC_CTOR(ALU) {
    SC_METHOD(process);                // 定义处理函数为 process
    sensitive << in1 << in2 << aluop;  // 灵敏信号：in1、in2、aluop
  }

  // 处理函数
  void process() {
    // 读取输入信号
    sc_int<8> a = in1.read();      // 读取输入操作数 A
    sc_int<8> b = in2.read();      // 读取输入操作数 B
    bool cin = c.read();           // 读取进位信号（始终为1）
    sc_uint<5> op = aluop.read();  // 读取ALU操作码
    sc_uint<3> sh = sa.read();     // 读取移位量

    // 根据ALU操作码执行相应操作
    switch (op) {
      case 0b00000:
        out.write(a);  // 直接输出 A
        break;
      case 0b00001:
        out.write(++a);  // A + 1
        break;
      case 0b00010:
        out.write(a + b);  // A + B
        break;
      case 0b00011:
        out.write(a + b + cin);  // A + B + Cin
        break;
      case 0b00100:
        out.write(a - b);  // A - B
        break;
      case 0b00101:
        out.write(a - b - cin);  // A - B - Cin
        break;
      case 0b00110:
        out.write(--a);  // A - 1
        break;
      case 0b00111:
        out.write(b);  // 输出 B
        break;
      case 0b01000:
      case 0b01001:
        out.write(a & b);  // A & B（按位与）
        break;
      case 0b01010:
      case 0b01011:
        out.write(a | b);  // A | B（按位或）
        break;
      case 0b01100:
      case 0b01101:
        out.write(a ^ b);  // A ^ B（按位异或）
        break;
      case 0b01110:
      case 0b01111:
        out.write(~a);  // ~A（按位取反）
        break;
      case 0b10000:
      case 0b10001:
      case 0b10010:
      case 0b10011:
      case 0b10100:
      case 0b10101:
      case 0b10110:
      case 0b10111:
        out.write(a >> sh);  // A 右移 sh 位
        break;
      default:
        out.write(a << sh);  // A 左移 sh 位
        break;
    }
  }
};
