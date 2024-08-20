/*
 * @ASCK
 */
#pragma once
#include <systemc.h>
#include "Acc.hpp"
#include "Bus.hpp"
#include "Memory.hpp"
#include "Micro.hpp"

/*
**  GLOBAL EVENT FOR MICRO-ACC
*/
//////////////////////////
sc_event micro_acc_ev;  ///
////////////////////////

/*
**  GLOBAL EVENT FOR ACC-MEMORY -> READ MODE
*/
//////////////////////////
sc_event acc_mem_read_ev;  ///
////////////////////////

/*
**  GLOBAL EVENT FOR ACC-MEMORY -> WRITE MODE
*/
//////////////////////////
sc_event acc_mem_write_ev;  ///
////////////////////////

/*
**  variable for checking if IF/ID/EXE/WB should have been stopped after call=1
*on memory-access stage or not!
*/
bool now_is_call = false;

SC_MODULE(System) {
  sc_in_clk clk;      // 时钟输入信号
  sc_in_clk clk_bus;  // 总线时钟输入信号

  // 测试信号线
  sc_out<sc_uint<14>> pc;         // 输出程序计数器（PC）
  sc_out<sc_uint<5>> test_aluop;  // 输出 ALU 操作码
  sc_out<sc_int<8>> reg_dump[8];  // 输出寄存器的值

  /*
  ** 模块全局变量
  */

  //
  // 信号线
  //

  // 微处理器
  sc_signal<sc_int<8>> micro_data_in;                   // 输入数据
  sc_signal<bool> micro_read, micro_write, micro_call;  // 读、写和调用信号
  sc_signal<sc_uint<13>> micro_addr;                    // 地址信号
  sc_signal<sc_int<8>> micro_data_out;                  // 输出数据

  // 总线
  sc_signal<bool> req;             // 请求信号
  sc_signal<bool> read_in;         // 读信号输入
  sc_signal<bool> write_in;        // 写信号输入
  sc_signal<bool> call_in;         // 调用信号输入
  sc_signal<sc_uint<13>> addr_in;  // 地址输入信号（内存和加速器共用）
  sc_signal<sc_int<8>> data_in;    // 数据输入信号
  // 输入-向上/输出-向下
  sc_signal<bool> ack;              // 应答信号
  sc_signal<bool> read_out;         // 读信号输出
  sc_signal<bool> write_out;        // 写信号输出
  sc_signal<bool> call_out;         // 调用信号输出
  sc_signal<sc_uint<13>> addr_out;  // 地址输出信号（内存和加速器共用）
  sc_signal<sc_int<8>> data_out;    // 数据输出信号

  // 内存
  sc_signal<sc_int<8>> mem_data_in, mem_data_out;  // 内存数据输入和输出
  sc_signal<sc_uint<13>> mem_addr;                 // 内存地址
  sc_signal<bool> r_nw;                            // 读/写标志

  // 加速器1
  sc_signal<bool> acc_call_in, acc_read, acc_write;  // 加速器调用、读和写信号
  sc_signal<sc_uint<13>> acc_addr_out;               // 加速器地址输出
  sc_signal<sc_int<8>> acc_data_in, acc_data_out;    // 加速器数据输入和输出

  // 测试信号
  sc_signal<sc_uint<5>> test_aluOp;  // 测试 ALU 操作码
  sc_signal<sc_uint<14>> test_pc;    // 测试程序计数器（PC）
  sc_signal<sc_uint<20>> test_inst;  // 测试指令

  /*
  ** 创建组件指针
  */
  Micro* micro;    // 微处理器模块
  Bus* bus;        // 总线模块
  Memory* memory;  // 内存模块
  Acc* acc;        // 加速器模块

  SC_CTOR(System) {
    SC_METHOD(process);          // 将 `process` 方法注册为 SystemC 方法
    sensitive << clk_bus.pos();  // 在 `clk_bus` 时钟上升沿时触发 `process` 方法

    // 实例化子模块
    micro = new Micro("Micro");
    bus = new Bus("Bus");
    memory = new Memory("MEMORY");
    acc = new Acc("Acc1");

    // 连接微处理器模块的端口和信号
    micro->clk(clk);
    micro->mem_data(micro_data_in);
    micro->read(micro_read);
    micro->write(micro_write);
    micro->call(micro_call);
    micro->addr(micro_addr);
    micro->data(micro_data_out);

    micro->test_aluOp(test_aluOp);
    micro->test_pc(test_pc);
    micro->test_inst(test_inst);

    for (int i = 0; i < 8; i++) {
      micro->reg_dump[i](reg_dump[i]);
    }

    // 设置总线模块的信号
    req = 1;  // 初始化请求信号
    bus->clk(clk_bus);
    bus->req(req);
    bus->read(read_in);
    bus->write(write_in);
    bus->call(micro_call);
    bus->addr(addr_in);
    bus->data(data_in);

    bus->ack(ack);
    bus->read_out(read_out);
    bus->write_out(write_out);
    bus->call_out(call_out);
    bus->addr_out(addr_out);
    bus->data_out(data_out);

    // 设置内存模块的信号
    r_nw = 1;  // 初始化读/写标志
    memory->r_nw(r_nw);
    memory->addr(mem_addr);
    memory->data(mem_data_in);
    memory->out(mem_data_out);

    // 设置加速器模块的信号
    acc->mem_data(acc_data_in);
    acc->call(acc_call_in);
    acc->read(acc_read);
    acc->write(acc_write);
    acc->addr(acc_addr_out);
    acc->data(acc_data_out);
  }

  int c = 0;  // 时钟计数器，用于打印

  /*
  ** FLAG: 如果加速器的 `acc_read` 被启用，表示在 2 个时钟周期后
  **       总线上将有内存数据！
  **
  **       简要说明：这个标志用于通知是否需要触发 `acc_mem_read_ev`。
  */
  int notify_flag_read = 0;   // 读取通知标志
  int notify_flag_write = 0;  // 写入通知标志

  void process() {
    // 测试信号
    test_aluop.write(test_aluOp.read());  // 更新 ALU 操作码
    pc.write(test_pc.read());             // 更新程序计数器

    cout << "-----------------------------------------------" << endl;
    cout << "\t-___ " << "bus_clk: 0X" << c++ << " ___-" << endl
         << endl;

    /*
    ** 微处理器 - 内存 - 加速器
    */

    // 更新内存和加速器的数据和地址信号
    mem_addr = addr_out.read();
    mem_data_in = data_out.read();
    micro_data_in = data_out.read();
    acc_data_in = data_out.read();
    acc_call_in = call_out.read();

    if (read_out.read() || write_out.read() || call_out.read()) {
      if (read_out.read()) {
        r_nw = read_out.read();         // 设置读/写标志为读
        data_in = mem_data_out.read();  // 从内存读取数据
      } else if (write_out.read()) {
        r_nw = !(write_out.read());  // 设置读/写标志为写
      }
    }

    ////////////////////////处理加速器读取/写入////////////////////////
    if (notify_flag_write != 0 && notify_flag_write < 3) {
      // 增加标志以达到预定的时钟计数
      notify_flag_write++;
      return;
    } else if (notify_flag_write == 3) {
      // 写操作应该已经完成
      notify_flag_write = 0;
      acc_mem_write_ev.notify();  // 通知写事件
      return;
    }

    if (notify_flag_read != 0 && notify_flag_read < 4) {
      // 增加标志以达到预定的时钟计数
      notify_flag_read++;
      return;
    } else if (notify_flag_read == 4) {
      // 是否通知加速器事件？（经过了两个时钟周期）
      notify_flag_read = 0;
      acc_mem_read_ev.notify();  // 通知读事件
      return;
    }

    ///////////////////////////////////////////////////////////////////微处理器
    if (micro_read.read() || micro_write.read() || micro_call.read()) {
      // 更新总线的读、写和调用信号
      read_in = micro_read.read();
      write_in = micro_write.read();
      call_in = micro_call.read();

      if (micro_read.read()) {
        addr_in = micro_addr.read();  // 设置地址
      } else if (micro_write.read()) {
        data_in = micro_data_out.read();  // 设置数据
        addr_in = micro_addr.read();      // 设置地址
      }
    }

    ///////////////////////////////////////////////////////////////////加速器
    if (acc_read.read() || acc_write.read()) {
      // 更新总线的读和写信号
      read_in = acc_read.read();
      write_in = acc_write.read();

      if (acc_read.read()) {
        // 增加加速器读取标志
        notify_flag_read++;
        addr_in = acc_addr_out.read();  // 设置地址
      } else if (acc_write.read()) {
        // 增加加速器写入标志
        notify_flag_write++;
        data_in = acc_data_out.read();  // 设置数据
        addr_in = acc_addr_out.read();  // 设置地址
      }
    }
  }
};
