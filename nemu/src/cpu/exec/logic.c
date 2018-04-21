#include "cpu/exec.h"

make_EHelper(test) {
  rtl_and(&t0,&id_dest->val,&id_src->val);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  rtl_update_ZFSF(&t0,id_dest->width);
   

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t1,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t1);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  rtl_update_ZFSF(&t1,id_dest->width);
//  printf("CF:%d OF:%d ZF:%d SF:%d\n",cpu.CF,cpu.OF,cpu.ZF,cpu.SF);
  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(or);
}

make_EHelper(sar) {
/*  rtl_mv(&t0,&id_src->val);
  rtl_mv(&t1,&id_dest->val);
  rtl_li(&t2,2);
  for(int i=0;i<t0;i++){
    //  rtl_idiv(&t1,&t1,&t2);
    t1=t1/t2;
  }*/
  // unnecessary to update CF and OF in NEMU
  rtl_sar(&t1,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t1);
  rtl_update_ZFSF(&t1,id_dest->width);
  print_asm_template2(sar);
}

make_EHelper(shl) {
  
  rtl_shl(&t1,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t1);
  rtl_update_ZFSF(&t1,id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  TODO();

  print_asm_template1(not);
}
