/*
 * @ASCK
 */
#pragma once
#include <systemc.h>

SC_MODULE(Bus) {

  // 输入端口
  sc_in_clk clk;     // 时钟信号
  sc_in<bool> req;   // 请求信号（未使用，因为没有被处理）
  sc_in<bool> read;  // 读取信号
  sc_in<bool> write; // 写入信号
  sc_in<bool> call;  // 调用信号
  sc_in<sc_uint<13>> addr; // 地址信号（用于内存和加速器）
  sc_in<sc_int<8>> data;   // 数据信号

  // 输出端口
  sc_out<bool> ack;             // 应答信号（未使用）
  sc_out<bool> read_out;        // 输出读取信号
  sc_out<bool> write_out;       // 输出写入信号
  sc_out<bool> call_out;        // 输出调用信号
  sc_out<sc_uint<13>> addr_out; // 输出地址信号
  sc_out<sc_int<8>> data_out;   // 输出数据信号

  // 构造函数
  SC_CTOR(Bus) {
    SC_METHOD(process);     // 定义一个方法 process
    sensitive << clk.pos(); // 在时钟上升沿敏感
  }

  // 方法 process 实现
  void process() {
    // 将输入信号直接输出到对应的输出端口
    // 这里简单地将输入端口的值复制到输出端口，实现了一个简单的总线逻辑
    read_out.write(read.read());
    write_out.write(write.read());
    call_out.write(call.read());
    addr_out.write(addr.read());
    data_out.write(data.read());
  }
};
