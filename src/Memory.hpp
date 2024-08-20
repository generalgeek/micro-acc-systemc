/*
 * @ASCK
 */
#pragma once
#include <systemc.h>

SC_MODULE(Memory) {

  sc_in<bool> r_nw;             // 读/写控制信号输入端口
  sc_in<sc_uint<13>> addr;      // 内存地址输入端口，13位无符号整数
  sc_in<sc_int<8>> data;        // 数据输入端口，8位有符号整数

  sc_out<sc_int<8>> out;        // 数据输出端口，8位有符号整数

  sc_int<8> mem[8192] = {0};    // 内存数组，共8192行，每行存储8位有符号整数，初始化为0
  bool done = false;            // 状态标志变量，表示内存是否已经完成操作，默认为false

  SC_CTOR(Memory) {
    SC_METHOD(process);         // 注册处理方法process
    sensitive << r_nw << addr << data;  // 当r_nw, addr或data信号发生变化时触发process方法
  }

  void process() {
    if (done) {                 // 如果done为true，直接返回，不执行后续操作
      return;
    }
    if (addr.read() < 8192) {   // 如果地址在有效范围内（0到8191）
      if (r_nw.read()) {        // 如果读写控制信号为读操作
        out.write(mem[addr.read()]);  // 将内存中地址为addr.read()的数据输出到out端口
      } else {                  // 如果读写控制信号为写操作
        mem[addr.read()] = data.read();  // 将data中的数据写入到内存中地址为addr.read()的位置
        out.write(mem[addr.read()]);  // 将写入后的内存数据输出到out端口
      }
    } else {                    // 如果地址超出了有效范围
      out.write(0);             // 输出0到out端口（表示无效地址）
    }
  }
};

