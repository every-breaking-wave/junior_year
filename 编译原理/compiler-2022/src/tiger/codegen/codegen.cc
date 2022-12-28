#include "tiger/codegen/codegen.h"

#include <iostream>
#include <sstream>

extern frame::RegManager *reg_manager;
extern std::vector<std::string> functions_ret_ptr;
std::vector<int> pointer_in_arg;
std::vector<int> pointer_in_frame_offset;
namespace {

// constexpr int maxlen = 1024;

} // namespace

namespace cg {

std::map<std::string, temp::Temp *> str_tempReg_map;

/***************** For GC *****************/

/* 指针传递规则：
    (1) movq的dst的指针属性和src相同
    (2) addq/subq若src中的一个为pointer则dst为pointer,
    (3) subq若src中的两个均为pointer则dst不是pointer(tiger中不涉及)
*/
/*
  指针的根(指针最初的位置, 指针传递的起始)
    (1) 帧中存储指针的位置
    (2) call返回指针的函数的%rax
    (3) 函数参数中传入的指针
  除此之外寄存器中的指针均为传递而来
*/

void pointerTransfer(assem::Instr *instr) {
  // movq中指针的传递
  if (typeid(*instr) == typeid(assem::MoveInstr)) {
    assem::MoveInstr *mov_ins = static_cast<assem::MoveInstr *>(instr);
    if (mov_ins->dst_ && mov_ins->src_)
      mov_ins->dst_->GetList().front()->store_pointer_ =
          mov_ins->src_->GetList().front()->store_pointer_;
  }
  // addq中指针的传递
  if (typeid(*instr) == typeid(assem::OperInstr)) {
    assem::OperInstr *add_ins = static_cast<assem::OperInstr *>(instr);
    if ((add_ins->assem_.find("add") != add_ins->assem_.npos ||
         add_ins->assem_.find("sub") != add_ins->assem_.npos) &&
        add_ins->dst_ && add_ins->src_ &&
        add_ins->src_->GetList().front()->store_pointer_)
      add_ins->dst_->GetList().front()->store_pointer_ = true;
  }
}

void generatePointRoot(frame::Frame *frame_) {
  /* 提取函数中的
    (1) 帧中存储指针的位置
    (2) 参数中存储指针的位置
    */
  std::list<frame::Access *> *InframeFormals = &frame_->formals_;
  for (const auto &access : *InframeFormals) {
    if (typeid(*access) == typeid(frame::InFrameAccess) &&
        static_cast<frame::InFrameAccess *>(access)->store_pointer_)
      pointer_in_frame_offset.push_back(
          static_cast<frame::InFrameAccess *>(access)->offset);
  }
  auto iter = (*InframeFormals).begin();
  int pos = 1;
  for (; iter != (*InframeFormals).end(); iter++, pos++) {
    frame::Access *access = *iter;
    if (typeid(*access) == typeid(frame::InRegAccess) &&
        static_cast<frame::InRegAccess *>(access)->reg->store_pointer_)
      pointer_in_arg.push_back(pos);
  }
}

bool argIsPointer(int pos) {
  return std::find(pointer_in_arg.begin(), pointer_in_arg.end(), pos) !=
         pointer_in_arg.end();
}

bool returnValueIsPointer(std::string func_name) {
  return func_name == "init_array" || func_name == "alloc_record" ||
         std::find(functions_ret_ptr.begin(), functions_ret_ptr.end(),
                   func_name) != functions_ret_ptr.end();
}

bool inFrameValueIsPointer(int offset) {
  return std::find(pointer_in_frame_offset.begin(),
                   pointer_in_frame_offset.end(),
                   offset) != pointer_in_frame_offset.end();
}

/***************** End For GC *****************/

// void emit(assem::Instr *instr, assem::InstrList &instr_list_) {
//   pointerTransfer(instr);
//   instr_list_.Append(instr);
// }

void CodeGen::Codegen() {
  fs_ = frame_->name->Name() + "_framesize";
  auto *instr_list = new assem::InstrList();
  std::list<tree::Stm *> stm_list = traces_->GetStmList()->GetList();
  std::vector<temp::Temp *> tmp_vec;
  int callee_save_reg_size = reg_manager->CalleeSaves()->GetList().size();

  // For GC
  generatePointRoot(frame_);
  // save callee-save regs
  for (const auto& reg : reg_manager->CalleeSaves()->GetList()) {
    auto tmp = temp::TempFactory::NewTemp();
    tmp_vec.emplace_back(tmp);
    assem::Instr *instr = new assem::MoveInstr(
        "movq `s0, `d0", new temp::TempList(tmp), new temp::TempList(reg));
    instr_list->Append(instr);
    pointerTransfer(instr);
  }

  for (auto stm : stm_list) {
    stm->Munch(*instr_list, fs_);
  }

  // restore callee-save regs
  for (size_t i = 0; i < callee_save_reg_size; i++) {
    auto instr = new assem::MoveInstr(
        "movq `s0, `d0",
        new temp::TempList(reg_manager->CalleeSaves()->NthTemp(i)),
        new temp::TempList(tmp_vec[i]));
    instr_list->Append(instr);
    pointerTransfer(instr);
  }

  assem_instr_ = std::make_unique<AssemInstr>(instr_list);

  assem_instr_->GetInstrList()->Append(
      new assem::OperInstr("", nullptr, reg_manager->ReturnSink(), nullptr));
}

void AssemInstr::Print(FILE *out, temp::Map *map) const {
  for (auto instr : instr_list_->GetList())
    instr->Print(out, map);
  fprintf(out, "\n");
}
} // namespace cg

namespace tree {

void SeqStm::Munch(assem::InstrList &instr_list, std::string_view fs) {
  left_->Munch(instr_list, fs);
  right_->Munch(instr_list, fs);
}

void LabelStm::Munch(assem::InstrList &instr_list, std::string_view fs) {
  instr_list.Append(new assem::LabelInstr(label_->Name(), label_));
}

void JumpStm::Munch(assem::InstrList &instr_list, std::string_view fs) {
  auto instr = new assem::OperInstr(
      std::string("jmp " + temp::LabelFactory::LabelString(exp_->name_)),
      nullptr, nullptr, new assem::Targets(jumps_));
  instr_list.Append(instr);
  cg::pointerTransfer(instr);
}

void CjumpStm::Munch(assem::InstrList &instr_list, std::string_view fs) {
  temp::Temp *left = left_->Munch(instr_list, fs);
  temp::Temp *right = right_->Munch(instr_list, fs);
  std::string jxx;
  std::string jmp_instr_str;
  switch (op_) {
  case tree::EQ_OP:
    jxx = "je";
    break;
  case tree::NE_OP:
    jxx = "jne";
    break;
  case tree::LT_OP:
    jxx = "jl";
    break;
  case tree::LE_OP:
    jxx = "jle";
    break;
  case tree::GT_OP:
    jxx = "jg";
    break;
  case tree::GE_OP:
    jxx = "jge";
    break;
  default:
    break;
  }
  jmp_instr_str = jxx + std::string(" `j0");
  std::vector<temp::Label *> *jmp_targets = new std::vector<temp::Label *>();
  jmp_targets->emplace_back(this->true_label_);
  jmp_targets->emplace_back(this->false_label_);
  auto instr_cmp =
      new assem::OperInstr(std::string("cmpq `s0, `s1"), nullptr,
                           new temp::TempList({right, left}), nullptr);
  auto instr_cjmp = new assem::OperInstr(jmp_instr_str, nullptr, nullptr,
                                         new assem::Targets(jmp_targets));
  instr_list.Append(instr_cmp);
  instr_list.Append(instr_cjmp);
  cg::pointerTransfer(instr_cmp);
  cg::pointerTransfer(instr_cjmp);
}

void MoveStm::Munch(assem::InstrList &instr_list, std::string_view fs) {
  if (typeid(*dst_) == typeid(tree::MemExp)) {
    auto mem_dst_exp = static_cast<tree::MemExp *>(dst_)->exp_;
    // auto mem_dst_exp = mem_dst->exp_;
    bool success = false;
    auto binop_exp = static_cast<tree::BinopExp *>(mem_dst_exp);
    if (typeid(*mem_dst_exp) == typeid(tree::BinopExp) &&
        binop_exp->op_ == tree::BinOp::PLUS_OP) {
      auto left_exp = binop_exp->left_;
      auto right_exp = binop_exp->right_;
      tree::Exp *e1 = nullptr;
      int imm;
      if (typeid(*left_exp) == typeid(tree::ConstExp)) {
        auto const_exp = static_cast<tree::ConstExp *>(left_exp);
        imm = const_exp->consti_;
        e1 = binop_exp->right_;
      } else {
        auto const_exp = static_cast<tree::ConstExp *>(right_exp);
        imm = const_exp->consti_;
        e1 = binop_exp->left_;
      }
      if (e1) {
        auto e1_reg = e1->Munch(instr_list, fs);
        auto e2_reg = src_->Munch(instr_list, fs);

        // movq e2, imm(e1)
        auto instr = new assem::OperInstr(
            "movq `s0, " + std::to_string(imm) + "(`s1)", nullptr,
            new temp::TempList({e2_reg, e1_reg}), nullptr);
        instr_list.Append(instr);
        cg::pointerTransfer(instr);

        success = true;
      }
    }
    if (!success && typeid(*src_) == typeid(tree::MemExp)) {
      auto mem_src_exp = static_cast<tree::MemExp *>(src_)->exp_;
      temp::Temp *t = temp::TempFactory::NewTemp();
      auto instr1 = new assem::OperInstr(
          "movq (`s0), `d0", new temp::TempList({t}),
          new temp::TempList({mem_src_exp->Munch(instr_list, fs)}), nullptr);
      auto instr2 = new assem::OperInstr(
          "movq `s0, (`s1)", nullptr,
          new temp::TempList({t, mem_dst_exp->Munch(instr_list, fs)}), nullptr);
      instr_list.Append(instr1);
      instr_list.Append(instr2);
      cg::pointerTransfer(instr1);
      cg::pointerTransfer(instr2);
      success = true;
    } else {
      auto e1_reg = mem_dst_exp->Munch(instr_list, fs);
      auto e2_reg = src_->Munch(instr_list, fs);
      auto instr = new assem::OperInstr("movq `s0, (`s1)", nullptr,
                                        new temp::TempList({e2_reg, e1_reg}),
                                        nullptr);
      instr_list.Append(instr);
    }
  } else {
    auto instr = new assem::MoveInstr(
        "movq `s0, `d0",
        new temp::TempList({static_cast<tree::TempExp *>(dst_)->temp_}),
        new temp::TempList({src_->Munch(instr_list, fs)}));
    instr_list.Append(instr);
    cg::pointerTransfer(instr);
  }
}

void ExpStm::Munch(assem::InstrList &instr_list, std::string_view fs) {
  exp_->Munch(instr_list, fs);
}

temp::Temp *BinopExp::Munch(assem::InstrList &instr_list, std::string_view fs) {
  std::string assem_instr;
  std::string instr_ss;
  temp::Temp *left_reg = left_->Munch(instr_list, fs);
  temp::Temp *right_reg = right_->Munch(instr_list, fs);
  temp::Temp *res_reg = temp::TempFactory::NewTemp();
  temp::TempList *dst = new temp::TempList(res_reg);
  if (op_ == PLUS_OP || op_ == MINUS_OP) {
    if (op_ == PLUS_OP) {
      assem_instr = "addq";
    } else {
      assem_instr = "subq";
    }
    auto move_instr = new assem::MoveInstr(std::string("movq `s0, `d0"), dst,
                                           new temp::TempList(left_reg));
    temp::TempList *src = new temp::TempList(right_reg);
    src->Append(res_reg);
    auto op_instr = new assem::OperInstr(
        std::string(assem_instr + " `s0, `d0"), dst, src, nullptr);
    instr_list.Append(move_instr);
    instr_list.Append(op_instr);
    cg::pointerTransfer(move_instr);
    cg::pointerTransfer(op_instr);

  } else if (op_ == MUL_OP || op_ == DIV_OP) {
    if (op_ == MUL_OP) {
      assem_instr = "imulq";
    } else {
      assem_instr = "idivq";
    }
    temp::Temp *rax = reg_manager->ReturnValue();
    temp::Temp *rdx = reg_manager->Registers()->NthTemp(3);
    temp::Temp *rax_saver = temp::TempFactory::NewTemp();
    temp::Temp *rdx_saver = temp::TempFactory::NewTemp();
    // Save %rax and %rdx
    auto save_rax_instr = new assem::MoveInstr("movq `s0, `d0",
                                           new temp::TempList(rax_saver),
                                           new temp::TempList(rax));
    auto save_rdx_instr = new assem::MoveInstr("movq `s0, `d0",
                                           new temp::TempList(rdx_saver),
                                           new temp::TempList(rdx));
    auto move_instr = new assem::MoveInstr(std::string("movq `s0, `d0"),
                                           new temp::TempList(rax),
                                           new temp::TempList(left_reg));
    instr_list.Append(save_rax_instr);
    instr_list.Append(save_rdx_instr);

    // move left to rax
    instr_list.Append(move_instr);
    dst->Append(rdx);
    instr_list.Append(new assem::OperInstr(std::string("cqto"), dst,
                                           new temp::TempList(rax), nullptr));
    temp::TempList *src = new temp::TempList(right_reg);
    src->Append(rax);
    src->Append(rdx);
    // do imulq or idivq
    instr_list.Append(new assem::OperInstr(std::string(assem_instr + " `s0"),
                                           nullptr, src, nullptr));
    instr_list.Append(new assem::MoveInstr(std::string("movq `s0, `d0"),
                                           new temp::TempList(res_reg),
                                           new temp::TempList(rax)));
    // restore %rax and %rdx
    auto restore_rax_instr = new assem::MoveInstr("movq `s0, `d0",
                                           new temp::TempList(rax),
                                           new temp::TempList(rax_saver));
    auto restore_rdx_instr = new assem::MoveInstr("movq `s0, `d0",
                                           new temp::TempList(rdx),
                                           new temp::TempList(rdx_saver));                                           
    instr_list.Append(restore_rax_instr);
    instr_list.Append(restore_rdx_instr);
    // transfer pointer releated
    cg::pointerTransfer(save_rax_instr);
    cg::pointerTransfer(save_rdx_instr);
    cg::pointerTransfer(move_instr);
    cg::pointerTransfer(restore_rax_instr);
    cg::pointerTransfer(restore_rdx_instr);
  }
  return res_reg;
}

temp::Temp *MemExp::Munch(assem::InstrList &instr_list, std::string_view fs) {
  temp::Temp *reg = temp::TempFactory::NewTemp();
  auto instr = new assem::OperInstr(
      "movq (`s0), `d0", new temp::TempList(reg),
      new temp::TempList(exp_->Munch(instr_list, fs)), nullptr);
  instr_list.Append(instr);
  cg::pointerTransfer(instr);
  return reg;
}

temp::Temp *TempExp::Munch(assem::InstrList &instr_list, std::string_view fs) {
  temp::Temp *res = nullptr;
  if (temp_ != reg_manager->FramePointer()) {
    res = temp_;
  } else {
    temp::Temp *reg = temp::TempFactory::NewTemp();
    std::string instr_str =
        std::string("leaq ") + std::string(fs) + std::string("(`s0), `d0");
    instr_list.Append(new assem::OperInstr(
        instr_str, new temp::TempList(reg),
        new temp::TempList(reg_manager->StackPointer()), nullptr));
    res = reg;
  }
  return res;
}

temp::Temp *EseqExp::Munch(assem::InstrList &instr_list, std::string_view fs) {
  stm_->Munch(instr_list, fs);
  return exp_->Munch(instr_list, fs);
}

temp::Temp *NameExp::Munch(assem::InstrList &instr_list, std::string_view fs) {
  temp::Temp *address = temp::TempFactory::NewTemp();
  std::string instr_str =
      std::string("leaq ") + name_->Name() + std::string("(%rip), `d0");
  instr_list.Append(new assem::OperInstr(instr_str, new temp::TempList(address),
                                         nullptr, nullptr));

  return address;
}

temp::Temp *ConstExp::Munch(assem::InstrList &instr_list, std::string_view fs) {
  temp::Temp *reg = temp::TempFactory::NewTemp();
  std::string instr_str =
      std::string("movq $") + std::to_string(consti_) + std::string(", `d0");
  instr_list.Append(new assem::OperInstr(instr_str, new temp::TempList(reg),
                                         nullptr, nullptr));

  return reg;
}

temp::Temp *CallExp::Munch(assem::InstrList &instr_list, std::string_view fs) {
  temp::Temp *ret = temp::TempFactory::NewTemp();
  // temp::TempList *args_list = args_->MunchArgs(instr_list, fs);
  // std::list<temp::Temp *> arg_regs = reg_manager->ArgRegs()->GetList();
  temp::TempList *alloced_args = new temp::TempList();
  // temp::Temp *reg = temp::TempFactory::NewTemp();
  temp::TempList *arg_regs = reg_manager->ArgRegs();
  int regs_num = arg_regs->GetList().size();
  int count = 0;
  // pop the args on the stack
  for (auto arg : args_->GetList()) {
    temp::Temp *tmp = arg->Munch(instr_list, fs);
    alloced_args->Append(tmp);
    if (count < regs_num) {
      // transfer pointer in args
      reg_manager->ArgRegs()->NthTemp(count)->store_pointer_ = cg::argIsPointer(count);
      auto move_instr = new assem::MoveInstr(
          std::string("movq `s0, `d0"),
          new temp::TempList(reg_manager->ArgRegs()->NthTemp(count)),
          new temp::TempList(tmp));
      instr_list.Append(move_instr);
      cg::pointerTransfer(move_instr);
    } else {
      instr_list.Append(new assem::OperInstr(
          std::string("subq $8, %rsp"),
          new temp::TempList(reg_manager->StackPointer()), nullptr, nullptr));
      auto move_instr = new assem::OperInstr(std::string("movq `s0, (%rsp)"),
                               new temp::TempList(reg_manager->StackPointer()),
                               new temp::TempList(tmp), nullptr);
      instr_list.Append(move_instr);
      cg::pointerTransfer(move_instr);
    }
    count++;
  }
  int formal_size = alloced_args->GetList().size(),
      max_size = arg_regs->GetList().size(); // normally it will be 6
  bool is_over_normal = formal_size > max_size;
  int extra_formal_size = formal_size - regs_num;

  std::string call_instr_str =
      "call " + static_cast<NameExp *>(fun_)->name_->Name();
  instr_list.Append(new assem::OperInstr(
      call_instr_str, reg_manager->CallerSaves(), nullptr, nullptr));

  /* For GC: 在call语句后面加入label(即return address为label) */
  temp::Label *return_addr_label_ = temp::LabelFactory::NewLabel();
  std::string ass = temp::LabelFactory::LabelString(return_addr_label_);
  assem::Instr *ins_label = new assem::LabelInstr(ass, return_addr_label_);
  instr_list.Append(ins_label);

  // set return value
  auto return_instr = new assem::MoveInstr("movq `s0, `d0", new temp::TempList(ret),
                           new temp::TempList(reg_manager->ReturnValue()));
  instr_list.Append(return_instr);
  cg::pointerTransfer(return_instr);
  // recover rsp
  if (is_over_normal) {
    std::string extra_arg_instr_str =
        "addq $" + std::to_string(extra_formal_size * reg_manager->WordSize()) +
        ", `d0";
    instr_list.Append(new assem::OperInstr(
        extra_arg_instr_str, new temp::TempList(reg_manager->StackPointer()),
        nullptr, nullptr));
  }
  return ret;
}

temp::TempList *ExpList::MunchArgs(assem::InstrList &instr_list,
                                   std::string_view fs) {
  temp::TempList *tempList = new temp::TempList();
  for (auto &exp : exp_list_) {
    temp::Temp *temp = exp->Munch(instr_list, fs);
    tempList->Append(temp);
  }
  return tempList;
}

} // namespace tree
