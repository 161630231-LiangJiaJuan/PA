#include "cpu/exec.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  decoding.is_jmp = 1;

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  decoding.is_jmp = t2;
  //printf("%d %x %x %d\n",t2,decoding.jmp_eip,*eip,decoding.is_jmp);
  print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage

rtl_push(&decoding.seq_eip);
decoding.is_jmp=1;
decoding.jmp_eip=id_dest->val+*eip; //id_dest->val;
  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
    decoding.is_jmp=1;
 if(decoding.is_operand_size_16){
     rtl_pop(eip);
     rtl_andi(eip,eip,0x0000ffff);
     decoding.jmp_eip=*eip;
 }
 else{
 rtl_pop(&t0);
 decoding.jmp_eip = t0;

 }
  print_asm("ret");
}

make_EHelper(call_rm) {
 rtl_push(&decoding.seq_eip); 
decoding.is_jmp=1;
//printf("%x  %x",id_dest->val,*eip);
decoding.jmp_eip=id_dest->val;
  print_asm("call *%s", id_dest->str);
}
