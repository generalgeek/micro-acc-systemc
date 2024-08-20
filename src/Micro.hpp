/*
 * @ASCK
 */
#pragma once
#include <systemc.h>
#include "ALU.hpp"
#include "Controller.hpp"
#include "EXE.hpp"
#include "ID.hpp"
#include "IF.hpp"
#include "IR.hpp"
#include "Mux3.hpp"
#include "Mux8.hpp"
#include "PC.hpp"
#include "RegFile.hpp"
#include "WB.hpp"

#include <bitset>

// 定义SystemC模块Micro
SC_MODULE(Micro) {
  // 输入时钟信号
  sc_in_clk clk;

  // 输入内存数据，8位有符号整数
  sc_in<sc_int<8>> mem_data;

  // 输出信号: 读、写、调用控制信号
  sc_out<bool> read, write, call;
  // 输出地址，13位无符号整数
  sc_out<sc_uint<13>> addr;
  // 输出数据，8位有符号整数
  sc_out<sc_int<8>> data;

  // 测试信号
  sc_out<sc_uint<5>> test_aluOp;
  sc_out<sc_uint<14>> test_pc;
  sc_out<sc_uint<20>> test_inst;
  sc_out<sc_int<8>> reg_dump[8];
  // sc_out <bool> test_regWrite, test_r_nw, test_aluMux, test_regMux,
  // test_wbMux, test_call;

  /*
  ** 模块全局变量
  */

  /*
  ** 信号
  */
  // 临时变量: 上一个PC地址，14位无符号整数
  sc_uint<14> tmp_pc_prev_addr;
  // 信号: 上一个PC地址
  sc_signal<sc_uint<14>> pc_prev_addr;
  // 信号: 下一个PC地址
  sc_signal<sc_uint<14>> pc_next_addr;

  // 信号: 指令寄存器内容，20位无符号整数
  sc_signal<sc_uint<20>> ir_inst;

  // 信号: 下一条指令的数据，20位无符号整数
  sc_signal<sc_uint<20>> if_next_data;
  // 信号: 操作码，4位无符号整数
  sc_signal<sc_uint<4>> opcode;
  // 信号: 源寄存器、目标寄存器、寄存器选择等
  sc_signal<sc_uint<3>> rd;
  sc_signal<sc_uint<3>> rs;
  sc_signal<sc_uint<3>> rt;
  sc_signal<sc_uint<3>> sa;
  sc_signal<sc_uint<4>> opselect;
  // 临时变量: 偏移量，13位有符号整数
  sc_uint<20> tmp;
  sc_signal<sc_int<13>> offset;

  // 控制器输出信号
  sc_signal<sc_uint<5>> aluOp;
  sc_signal<bool> regWrite, r, w, aluMux, regMux, wbMux, acc_call;

  // 多路选择器输出信号
  sc_signal<sc_uint<3>> mux_reg_res;

  /*
  ** 寄存器文件信号
  */
  // 信号: 寄存器读数
  sc_signal<sc_int<8>> Rr1;
  sc_signal<sc_int<8>> Rr2;
  sc_signal<sc_int<8>> regs[8];

  // 信号: ID阶段的下一步数据
  sc_signal<sc_int<8>> id_next_A;
  sc_signal<sc_int<8>> id_next_B;
  sc_signal<sc_int<13>> id_next_Imm;
  sc_signal<sc_uint<3>> id_next_Sa;
  sc_signal<sc_uint<5>> id_next_AluOp;
  sc_signal<bool> id_next_r;
  sc_signal<bool> id_next_w;
  sc_signal<bool> id_next_AluMux;
  sc_signal<bool> id_next_WbMux;
  sc_signal<bool> id_next_call;
  sc_signal<bool> id_next_regWrite;
  sc_signal<sc_uint<3>> id_next_rd;

  // 信号: ALU输入和输出
  sc_signal<sc_int<8>> alu_in_B;
  sc_signal<sc_int<8>> id_nex_imm_8bits;
  sc_signal<sc_int<8>> alu_out;
  sc_signal<bool> carry;

  // EXE阶段信号
  sc_signal<sc_int<8>> exe_next_result;
  sc_signal<sc_int<13>> exe_next_Imm;
  sc_signal<bool> exe_next_r;
  sc_signal<bool> exe_next_w;
  sc_signal<bool> exe_next_WbMux;
  sc_signal<bool> exe_next_call;
  sc_signal<bool> exe_next_regWrite;
  sc_signal<sc_uint<3>> exe_next_rd;

  // WB阶段信号
  sc_signal<sc_int<8>> wb_next_result;
  sc_signal<sc_int<8>> wb_prev_mem_data;
  sc_signal<sc_int<8>> wb_next_mem_data;
  sc_signal<bool> wb_next_WbMux;
  sc_signal<bool> wb_next_regWrite;
  sc_signal<sc_uint<3>> wb_next_rd;

  // 信号: 内存地址
  sc_signal<sc_uint<13>> mem_addr;

  // 信号: 寄存器文件数据
  sc_signal<sc_int<8>> regFileData;

  // 临时变量: ID阶段下一步立即数
  sc_int<13> tmp_id_next_Imm;

  // 信号: 内存数据
  sc_signal<sc_int<8>> sig_mem_data;

  // -----

  // 模块实例
  PC* pc;
  IR* ir;
  IF* iff;
  Controller* ctl;
  Mux3* mux_reg;
  ID* id;
  Mux8* alu_in_mux;
  ALU* alu;
  EXE* exe;
  WB* wb;
  Mux8* wb_out_mux;
  RegFile* reg_file;

  // 模块构造函数
  SC_CTOR(Micro) {
    // 定义一个线程过程，敏感于时钟上升沿
    SC_THREAD(process);
    sensitive << clk.pos();

    // 实例化子模块
    pc = new PC("PC");
    ir = new IR("IR");
    iff = new IF("IF");
    ctl = new Controller("Controller");
    mux_reg = new Mux3("RegMux3bits");
    id = new ID("ID");
    alu_in_mux = new Mux8("AluInputMux");
    alu = new ALU("ALU");
    exe = new EXE("EXE");
    wb = new WB("WB");
    wb_out_mux = new Mux8("WBOutputMux");
    reg_file = new RegFile("RegisterFile");

    // 连接子模块和信号
    // PC模块
    pc->clk(clk);
    pc->prev_addr(pc_prev_addr);
    pc->next_addr(pc_next_addr);

    // IR模块
    ir->addr(pc_next_addr);
    ir->inst(ir_inst);

    // IF模块
    iff->clk(clk);
    iff->prev_data(ir_inst);
    iff->next_data(if_next_data);

    // Controller模块
    ctl->opcode(opcode);
    ctl->opselect(opselect);
    ctl->aluOp(aluOp);
    ctl->regWrite(regWrite);
    ctl->r(r);
    ctl->w(w);
    ctl->aluMux(aluMux);
    ctl->regMux(regMux);
    ctl->wbMux(wbMux);
    ctl->call(acc_call);

    // Mux3模块
    mux_reg->sel(regMux);
    mux_reg->in0(rs);
    mux_reg->in1(rd);
    mux_reg->out(mux_reg_res);

    // ID模块
    id->clk(clk);
    id->prev_A(Rr1);
    id->next_A(id_next_A);
    id->prev_B(Rr2);
    id->next_B(id_next_B);
    id->prev_Imm(offset);
    id->next_Imm(id_next_Imm);
    id->prev_Sa(sa);
    id->next_Sa(id_next_Sa);
    id->prev_AluOp(aluOp);
    id->next_AluOp(id_next_AluOp);
    id->prev_r(r);
    id->next_r(id_next_r);
    id->prev_w(w);
    id->next_w(id_next_w);
    id->prev_AluMux(aluMux);
    id->next_AluMux(id_next_AluMux);
    id->prev_WbMux(wbMux);
    id->next_WbMux(id_next_WbMux);
    id->prev_call(acc_call);
    id->next_call(id_next_call);
    id->prev_regWrite(regWrite);
    id->next_regWrite(id_next_regWrite);
    id->prev_rd(rd);
    id->next_rd(id_next_rd);

    // Mux8模块用于ALU输入
    alu_in_mux->sel(id_next_AluMux);
    alu_in_mux->in0(id_next_B);
    alu_in_mux->in1(id_nex_imm_8bits);
    alu_in_mux->out(alu_in_B);

    // ALU模块
    carry = 1;  // 设定进位初始值
    alu->in1(id_next_A);
    alu->in2(alu_in_B);
    alu->c(carry);
    alu->aluop(id_next_AluOp);
    alu->sa(id_next_Sa);
    alu->out(alu_out);

    // EXE模块
    exe->clk(clk);
    exe->prev_result(alu_out);
    exe->next_result(exe_next_result);
    exe->prev_Imm(id_next_Imm);
    exe->next_Imm(exe_next_Imm);
    exe->prev_r(id_next_r);
    exe->next_r(exe_next_r);
    exe->prev_w(id_next_w);
    exe->next_w(exe_next_w);
    exe->prev_WbMux(id_next_WbMux);
    exe->next_WbMux(exe_next_WbMux);
    exe->prev_call(id_next_call);
    exe->next_call(exe_next_call);
    exe->prev_regWrite(id_next_regWrite);
    exe->next_regWrite(exe_next_regWrite);
    exe->prev_rd(id_next_rd);
    exe->next_rd(exe_next_rd);

    // WB模块
    wb->clk(clk);
    wb->prev_alu_result(exe_next_result);
    wb->next_alu_result(wb_next_result);
    wb->prev_mem_result(mem_data);
    wb->next_mem_result(wb_next_mem_data);
    wb->prev_WbMux(exe_next_WbMux);
    wb->next_WbMux(wb_next_WbMux);
    wb->prev_regWrite(exe_next_regWrite);
    wb->next_regWrite(wb_next_regWrite);
    wb->prev_rd(exe_next_rd);
    wb->next_rd(wb_next_rd);

    // Mux8模块用于WB输出
    wb_out_mux->sel(wb_next_WbMux);
    wb_out_mux->in0(wb_next_result);
    wb_out_mux->in1(wb_next_mem_data);
    wb_out_mux->out(regFileData);

    // RegisterFile模块
    reg_file->clk(clk);
    reg_file->regWrite(wb_next_regWrite);
    reg_file->r1(mux_reg_res);
    reg_file->r2(rt);
    reg_file->r3(wb_next_rd);
    reg_file->data(regFileData);
    reg_file->Rr1(Rr1);
    reg_file->Rr2(Rr2);

    // 连接寄存器文件的寄存器信号
    for (int i = 0; i < 8; i++) {
      reg_file->r[i](regs[i]);
    }
  }

  /*
   ** CLOCK THREAD FOR DOING PROCESSES
   */
  void process() {
    while (true) {
      // 如果 id_next_call 为真，表示需要处理加速器调用
      if (id_next_call) {
        now_is_call = true;
      }
      wait();  // 等待时钟的下一个上升沿

      // 执行微处理器的各个阶段
      pcInc();       // 增加程序计数器 PC
      decode();      // 解码指令
      ImmTo8bits();  // 将立即数转换为 8 位数据
      busAccess();   // 访问内存
      tester();      // 测试并输出信号值

      /*
      ** HANDLE ACCELERATOR CALLS
      */
      // 如果 EXE 阶段的调用信号为真，处理加速器调用
      if (exe_next_call.read()) {
        call.write(1);                             // 发出调用信号
        pc_prev_addr = (pc_prev_addr.read());      // 读取当前的 PC 地址
        wait(micro_acc_ev);                        // 等待加速器事件的发生
        pc_prev_addr = (pc_prev_addr.read()) + 1;  // 增加 PC 地址
      }
    }
  }

  void pcInc() {
    // 增加程序计数器 PC
    tmp_pc_prev_addr = pc_prev_addr.read();  // 读取当前 PC 地址
    pc_prev_addr = ++tmp_pc_prev_addr;       // PC 地址加 1
  }

  void decode() {
    /*
    **  分解下一条指令到对应的信号 (指令解码)
    */
    tmp = ir_inst.read();                   // 读取指令寄存器内容
    opcode = tmp.range(19, 16);             // 提取操作码（高 4 位）
    opselect = tmp.range(3, 0);             // 提取操作选择信号（低 4 位）
    rd = tmp.range(15, 13);                 // 提取目标寄存器（3 位）
    rs = tmp.range(12, 10);                 // 提取源寄存器 1（3 位）
    rt = tmp.range(9, 7);                   // 提取源寄存器 2（3 位）
    sa = tmp.range(6, 4);                   // 提取移位量（3 位）
    offset = (sc_int<13>)tmp.range(12, 0);  // 提取立即数（13 位，带符号）
  }

  void ImmTo8bits() {
    /*
    ** Mux 8 选择立即数或 B 数据
    */
    tmp_id_next_Imm = offset.read();                   // 读取立即数
    id_nex_imm_8bits = (tmp_id_next_Imm.range(7, 0));  // 提取 8 位立即数
  }

  void busAccess() {
    /*
    ** 通过总线访问内存
    */
    wb_prev_mem_data = mem_data.read();          // 读取内存数据
    mem_addr = (sc_uint<13>)id_next_Imm.read();  // 设定内存地址
    read.write(exe_next_r.read());               // 设置读信号
    write.write(exe_next_w.read());              // 设置写信号
    addr.write(mem_addr.read());                 // 输出内存地址
    data.write(exe_next_result.read());          // 输出数据
    call.write(0);                               // 重置调用信号
  }

  void tester() {
    // 测试信号
    for (int i = 0; i < 8; i++) {
      reg_dump[i].write(regs[i].read());  // 输出寄存器内容
    }
    test_aluOp.write(aluOp.read());                     // 输出 ALU 操作码
    test_pc.write(pc_next_addr.read());                 // 输出下一个 PC 地址
    test_inst.write((sc_uint<20>)if_next_data.read());  // 输出下一条指令

    print();  // 打印信号值
  }

  void print() {
    // 打印程序计数器 PC 地址
    cout << "pc addr ((FETCH)) :\t" << pc_next_addr << endl
         << endl;

    // 打印 IF 阶段的指令数据
    cout << "IF inst ((DECODE)):\t" << std::hex << if_next_data << endl;
    cout << "controller| opcode:\t" << opcode << endl;
    cout << "regFile| regMux   :\t" << regMux << endl;
    cout << "regFile| r1       :\t" << mux_reg_res << endl;
    cout << "regFile| r2       :\t" << rt << endl;
    cout << "regFile| r3       :\t" << wb_next_rd << endl;
    cout << "regFile| imm      :\t" << offset << endl;
    cout << "regFile| data     :\t" << wb_next_rd << endl;
    cout << "regFile| regWrite :\t" << wb_next_regWrite << endl
         << endl;

    // 打印 EXE 阶段的 ALU 操作信息
    cout << "A ((EXE))         :\t" << id_next_A << endl;
    cout << "B                 :\t" << alu_in_B << endl;
    cout << "aluOp             :\t" << id_next_AluOp << endl;
    cout << "aluMux            :\t" << id_next_AluMux << endl;
    cout << "imm               :\t" << id_next_Imm << endl;
    cout << "aluOut            :\t" << alu_out << endl
         << endl;

    // 打印 MEM 阶段的内存访问信息
    cout << "imm               :\t" << exe_next_Imm << endl;
    cout << "data_in ((MEM))   :\t" << exe_next_result << endl;
    cout << "addr              :\t" << exe_next_Imm << endl;
    cout << "read              :\t" << exe_next_r << endl;
    cout << "write             :\t" << exe_next_w << endl;
    cout << "data_out          :\t" << mem_data << endl
         << endl;

    // 打印 WB 阶段的信息
    cout << "data ((WB))       :\t" << regFileData << endl;
    cout << "rd                :\t" << exe_next_rd << endl;
    cout << "wbMux             :\t" << wb_next_WbMux << endl
         << endl;
  }
};
