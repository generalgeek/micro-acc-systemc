/*
 * @ASCK
 */

#include <systemc.h>
#include "System.hpp"

using namespace std;

int sc_main(int argc, char* argv[]) {
  cout << "starting the complete project" << endl;  // 输出启动项目的消息

  sc_trace_file* wf =
      sc_create_vcd_trace_file("project");  // 创建一个 VCD（Value Change
                                            // Dump）跟踪文件，文件名为 "project"

  // 定义信号
  sc_signal<bool> clk;               // 时钟信号
  sc_signal<bool> clk_bus;           // 总线时钟信号
  sc_signal<sc_int<8>> reg_dump[8];  // 寄存器转储信号
  sc_signal<sc_uint<5>> aluop;       // ALU 操作码信号
  sc_signal<sc_uint<14>> pc;         // 程序计数器（PC）信号

  // 实例化系统模块
  System sys("System");          // 创建名为 "System" 的 System 模块实例
  sys(clk, clk_bus, pc, aluop);  // 连接系统模块的端口和信号

  // 将信号与系统模块端口连接
  for (int i = 0; i < 8; i++) {
    sys.reg_dump[i](reg_dump[i]);  // 连接寄存器转储信号到系统模块
  }

  // 为 VCD 文件添加信号跟踪
  sc_trace(wf, clk, "clk");          // 跟踪时钟信号
  sc_trace(wf, clk_bus, "bus_clk");  // 跟踪总线时钟信号
  sc_trace(wf, pc, "pc");            // 跟踪程序计数器（PC）信号
  sc_trace(wf, aluop, "aluop");      // 跟踪 ALU 操作码信号
  for (int i = 0; i < 8; i++) {
    char str[3];
    sprintf(str, "%d", i);                         // 将寄存器索引转换为字符串
    sc_trace(wf, reg_dump[i], "R" + string(str));  // 跟踪每个寄存器的信号
  }

  // 仿真时钟信号
  for (int i = 0; i < 40; i++) {  // 运行 40 个时钟周期
    clk_bus = 0;                  // 设置总线时钟为低电平
    clk = 1;                      // 设置时钟信号为高电平
    sc_start(1, SC_NS);           // 启动仿真，持续时间为 1 纳秒

    clk_bus = 1;         // 设置总线时钟为高电平
    sc_start(1, SC_NS);  // 启动仿真，持续时间为 1 纳秒

    clk_bus = 0;         // 设置总线时钟为低电平
    sc_start(1, SC_NS);  // 启动仿真，持续时间为 1 纳秒

    clk_bus = 1;         // 设置总线时钟为高电平
    sc_start(1, SC_NS);  // 启动仿真，持续时间为 1 纳秒

    clk_bus = 0;         // 设置总线时钟为低电平
    clk = 0;             // 设置时钟信号为低电平
    sc_start(1, SC_NS);  // 启动仿真，持续时间为 1 纳秒

    clk_bus = 1;         // 设置总线时钟为高电平
    sc_start(1, SC_NS);  // 启动仿真，持续时间为 1 纳秒

    clk_bus = 0;         // 设置总线时钟为低电平
    sc_start(1, SC_NS);  // 启动仿真，持续时间为 1 纳秒

    clk_bus = 1;         // 设置总线时钟为高电平
    sc_start(1, SC_NS);  // 启动仿真，持续时间为 1 纳秒
  }

  sc_close_vcd_trace_file(wf);  // 关闭 VCD 跟踪文件

  cout << "vcd file completed" << endl;  // 输出完成 VCD 文件的消息

  return 0;  // 返回 0，表示程序成功结束
}
