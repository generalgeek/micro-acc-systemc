/*
 * @ASCK
 */
#pragma once
#include <systemc.h>

SC_MODULE(Controller) {
  // 输入端口：操作码(opcode)和操作选择(opselect)
  sc_in<sc_uint<4>> opcode, opselect;

  // 输出端口：ALU操作码(aluOp)和多个控制信号
  sc_out<sc_uint<5>> aluOp;
  sc_out<bool> regWrite, r, w, aluMux, regMux, wbMux, call;

  // 模块内部变量
  sc_uint<4> opcd, opsel;  // 存储操作码和操作选择
  sc_uint<5> op;           // 存储ALU操作码

  // 临时变量
  int c = 0;

  // 构造函数
  SC_CTOR(Controller) {
    SC_METHOD(process);               // 指定处理函数为 process
    sensitive << opcode << opselect;  // 响应 opcode 和 opselect 的变化
  }

  // 处理函数，根据操作码和操作选择生成控制信号
  void process() {
    opcd = opcode.read();     // 读取操作码
    opsel = opselect.read();  // 读取操作选择

    // 根据操作码进行不同的操作
    switch (opcd) {
      case 0b0000:
      case 0b0001:
        op = opsel;         // 将操作选择直接赋给 ALU 操作码
        op = opsel << 1;    // 左移操作选择一位
        op[0] = opcd[0];    // 将操作码的最低位赋给 ALU 操作码的最低位
        aluOp.write(op);    // 输出 ALU 操作码
        regWrite.write(1);  // 写使能信号，允许写入寄存器
        r.write(0);         // 读使能信号，不从寄存器读取
        w.write(0);         // 写使能信号，不向寄存器写入
        regMux.write(0);    // 寄存器选择信号，选择 r1 = rs
        aluMux.write(0);    // ALU 输入选择信号，B = Rr2
        wbMux.write(0);     // 写回数据选择信号，使用 ALU 结果
        call.write(0);      // 调用信号，不调用
        break;

      case 0b0010:
        aluOp.write(0b00111);  // 设置 ALU 操作码，传输 B (rd = Imm)
        regWrite.write(1);     // 允许写入寄存器
        r.write(0);            // 不从寄存器读取
        w.write(0);            // 不向寄存器写入
        regMux.write(0);       // 寄存器选择信号，r1 = rs
        aluMux.write(1);       // ALU 输入选择信号，B = Imm
        wbMux.write(0);        // 写回数据选择信号，使用 ALU 结果
        call.write(0);         // 调用信号，不调用
        break;

      case 0b0011:
        aluOp.write(0b00100);  // 设置 ALU 操作码，加法 A + B (rd += Imm)
        regWrite.write(1);     // 允许写入寄存器
        r.write(0);            // 不从寄存器读取
        w.write(0);            // 不向寄存器写入
        regMux.write(1);       // 寄存器选择信号，r1 = reg (rd)
        aluMux.write(1);       // ALU 输入选择信号，B = Imm
        wbMux.write(0);        // 写回数据选择信号，使用 ALU 结果
        call.write(0);         // 调用信号，不调用
        break;

      case 0b0100:
        aluOp.write(0);     // 设置 ALU 操作码，传输 A
        regWrite.write(1);  // 允许写入寄存器
        r.write(1);         // 从寄存器读取
        w.write(0);         // 不向寄存器写入
        regMux.write(0);    // 寄存器选择信号，r1 = rs
        aluMux.write(0);    // ALU 输入选择信号，B = Rr2
        wbMux.write(1);     // 写回数据选择信号，使用内存结果
        call.write(0);      // 调用信号，不调用
        break;

      case 0b0101:
        aluOp.write(0);     // 设置 ALU 操作码，传输 A
        regWrite.write(0);  // 禁止写入寄存器
        r.write(0);         // 不从寄存器读取
        w.write(1);         // 向寄存器写入
        regMux.write(1);    // 寄存器选择信号，r1 = reg (rd)
        aluMux.write(0);    // ALU 输入选择信号，忽略
        wbMux.write(0);     // 写回数据选择信号，忽略
        call.write(0);      // 调用信号，不调用
        break;

      case 0b1111:
        aluOp.write(0);     // 设置 ALU 操作码，传输 A
        regWrite.write(0);  // 禁止写入寄存器
        r.write(0);         // 不从寄存器读取
        w.write(0);         // 不向寄存器写入
        regMux.write(0);    // 寄存器选择信号，r1 = rs
        aluMux.write(0);    // ALU 输入选择信号，B = Rr2
        wbMux.write(0);     // 写回数据选择信号，忽略
        call.write(0);      // 调用信号，不调用
        break;

      default:
        regWrite.write(0);  // 禁止写入寄存器
        r.write(0);         // 不从寄存器读取
        w.write(0);         // 不向寄存器写入
        call.write(1);      // 调用信号，调用
        break;
    }
  }
};
