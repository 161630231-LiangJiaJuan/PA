#include "cpu/exec.h"
#include "cpu/rtl.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */


    uint32_t low=vaddr_read(cpu.base+NO*sizeof(GateDesc),4);
    uint32_t high=vaddr_read(cpu.base+NO*sizeof(GateDesc)+4,4);
    assert((high<<16) >>31 == 1);
    rtl_push(&cpu.eflags);
    uint32_t tmpCS=cpu.CS;
    rtl_push(&tmpCS);
    rtl_push(&ret_addr);
    decoding.is_jmp=1;
    decoding.jmp_eip =((high>>16)<<16)+((low<<16)>>16);
    

}



void dev_raise_intr() {
}
