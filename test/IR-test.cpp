/*
 * @ASCK
 */

#include "IR.hpp"
#include <systemc.h>
#include <bitset>

using namespace std;

int sc_main(int argc, char* argv[]) {
  cout << "starting IR" << endl;

  sc_signal<sc_uint<14>> addr;
  sc_signal<sc_uint<20>> inst;

  IR ir("ir");
  ir(addr, inst);

  sc_trace_file* wf = sc_create_vcd_trace_file("tracer");
  sc_trace(wf, addr, "addr");
  sc_trace(wf, inst, "inst");

  for (sc_uint<14> i = 0; i < 10; i++) {
    addr.write(i);
    sc_start(1, SC_NS);
    cout << dec << "addr=" << addr.read() << hex << ", inst=0b" << bitset<20>(inst.read()) << endl;
  }

  sc_close_vcd_trace_file(wf);
  return 0;
}
