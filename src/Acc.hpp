/*
 * @ASCK
 */
#pragma once
#include <systemc.h>

extern bool now_is_call;
extern sc_event micro_acc_ev;
extern sc_event acc_mem_read_ev;
extern sc_event acc_mem_write_ev;

SC_MODULE(Acc) {

  // 输入端口
  sc_in<sc_int<8>> mem_data; // 内存数据
  sc_in<bool> call;          // 调用信号

  // 输出端口
  sc_out<bool> read, write; // 读写控制信号
  sc_out<sc_uint<13>> addr; // 地址信号
  sc_out<sc_int<8>> data;   // 数据信号

  // 构造函数
  SC_CTOR(Acc) {
    SC_THREAD(process); // 定义一个线程 process
    sensitive << call;  // 敏感信号：call
  }

  // 线程 process 实现
  void process() {
    while (true) {
      wait();                 // 等待调用信号
      if (call.read() == 1) { // 如果调用信号为1，开始加速器操作
        std::cout << "@@-_.|*^<!! STARTING ACCELERATOR !!>^*|._-@@" << endl;
        operation(); // 执行加速器操作
      }
    }
  }

  // 加速器操作函数
  void operation() {
    write_to_mem(11, 10); // 存储值 10 到地址 11
    sc_int<8> a = read_from_mem(5); // 从地址 5 读取内存数据，数据为 7
    sc_int<8> b = read_from_mem(11); // 从地址 11 读取内存数据，数据为 10
    sc_int<8> c = a + b; // 计算 a + b
    write_to_mem(20, c); // 将 c 存储到地址 20
    micro_notif(); // 结束加速器任务，通知微处理器任务
  }

  // 通知微处理器加速器任务完成
  void micro_notif() {
    micro_acc_ev.notify(); // 通知微处理器事件
    now_is_call = false;   // 标记当前非调用状态
  }

  // 从内存读取数据
  sc_int<8> read_from_mem(int ad) {
    read.write(1);  // 设置读取信号
    write.write(0); // 禁止写入信号
    addr.write(ad); // 设置地址
    wait(acc_mem_read_ev); // 等待内存读取事件
    no_read_write(); // 取消读写操作
    wait(10, SC_NS); // 等待一段时间
    return mem_data.read(); // 返回读取的数据
  }

  // 写入数据到内存
  void write_to_mem(int ad, int dt) {
    read.write(0);  // 禁止读取信号
    write.write(1); // 设置写入信号
    addr.write(ad); // 设置地址
    data.write(dt); // 设置数据
    wait(acc_mem_write_ev); // 等待内存写入事件
    no_read_write(); // 取消读写操作
    wait(1, SC_NS);  // 等待一段时间
  }

  // 取消读写信号
  void no_read_write() {
    read.write(0);
    write.write(0);
  }
};
