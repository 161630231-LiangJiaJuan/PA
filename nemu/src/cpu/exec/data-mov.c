#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);    

  print_asm_template1(push);
}

make_EHelper(pop) {
 rtl_pop(&id_dest->val); 
 operand_write(id_dest,&id_dest->val);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  rtl_mv(&t0,&cpu.esp);
  for(int i=0;i<8;i++){
      if(i==R_ESP){
          rtl_push(&t0);
          continue;
      }
      rtl_push(&reg_l(i));
  }

  print_asm("pusha");
}

make_EHelper(popa) {
  for(int i=7;i>=0;i--){
  //  if(i==R_ESP)
   //    ; //continue;
   // else
        rtl_pop(&reg_l(i));
  }

  print_asm("popa");
}

make_EHelper(leave) {
  rtl_mv(&cpu.esp,&cpu.ebp);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
   if (reg_w(R_EAX)<0){
       
   }
  // else cpu.dx=0;
  }
  else {
    if (cpu.eax<0){
        cpu.edx=0xffffffff;
    }
    else cpu.edx=0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    
  }
  else {
    
    rtl_sext(&t0,&reg_l(R_EAX),2);
    rtl_mv(&cpu.eax,&t0);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
