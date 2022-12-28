#include "tiger/frame/x64frame.h"
#include "frame.h"

#include <map>



namespace frame {

// x64 Frame part

frame::Frame *newFrame(temp::Label *name, std::list<bool> formals){
  // printf("\nnew frame name is %s\n", name->Name().data());
  frame::X64Frame *newFrame = new frame::X64Frame();
  newFrame->name = name;
  newFrame->offset = -reg_manager->WordSize();
  std::list<temp::Temp *> argRegList = reg_manager->ArgRegs()->GetList();
  std::list<temp::Temp *>::iterator reg = argRegList.begin();
  for(bool escape : formals){
    frame::Access* access = newFrame->allocLocal(escape);
    newFrame->formals_.emplace_back(access);
  }
    // newFrame->formals = new AccessList();
  // newFrame->view_shift= new tree::StmList();
  // int count = 0;
  // int max_size = reg_manager->ArgRegs()->GetList().size();
  // for (const auto &formal : newFrame->formals->GetList()) {
  //   tree::Exp *dst_exp;
  //   if (formal->kind == Access::INREG) {
  //     dst_exp = new tree::TempExp(static_cast<InRegAccess *>(formal)->reg);
  //   } else {
  //     dst_exp = new tree::MemExp(new tree::BinopExp(
  //         tree::PLUS_OP, new tree::TempExp(reg_manager->FramePointer()),
  //         new tree::ConstExp(static_cast<InFrameAccess *>(formal)->offset)));
  //   }
  //   tree::Stm *stm;
  //   if (count < max_size) {
  //     stm = new tree::MoveStm(
  //         dst_exp, new tree::TempExp(reg_manager->GetRegister(count)));
  //   } else {
  //     int offset = (max_size - 1 - count) * reg_manager->WordSize();
  //     stm = new tree::MoveStm(
  //         dst_exp,
  //         new tree::MemExp(new tree::BinopExp(
  //             tree::PLUS_OP, new tree::TempExp(reg_manager->FramePointer()),
  //             new tree::ConstExp(offset))));
  //   }
  //   count++;
  //   // newFrame->view_shift->Linear(stm);
  // }

  return newFrame;
}

tree::Stm *procEntryExit1(frame::Frame *frame, tree::Stm *stm){
  int reg_num = reg_manager->ArgRegs()->GetList().size();
  int count = 0;
  tree::Stm* exp=new tree::ExpStm(new tree::ConstExp(0));
  std::list<temp::Temp *> regs= reg_manager->ArgRegs()->GetList();
  std::list<temp::Temp *>::iterator reg = regs.begin();
  for(auto formal:frame->formals_)
  {
    if(count < reg_num){
      exp = new tree::SeqStm(exp, new tree::MoveStm(
        formal->ToExp(new tree::TempExp(reg_manager->FramePointer())),
          new tree::TempExp(*reg)));
      reg++;
    }
    else{
      int view_offset=(frame->formals_.size() - 1 - count) * reg_manager->WordSize() + reg_manager->WordSize();
      exp = new tree::SeqStm(exp, new tree::MoveStm(
        formal->ToExp(new tree::TempExp(reg_manager->FramePointer())),
          new tree::MemExp(new tree::BinopExp(tree::PLUS_OP,
            new tree::TempExp(reg_manager->FramePointer()),new tree::ConstExp(view_offset)))));
    }
    count++;
  }

  return new tree::SeqStm(exp, stm);
}

assem::InstrList *procEntryExit2(assem::InstrList *body){
  body->Append(new assem::OperInstr("", nullptr, reg_manager->ReturnSink(), nullptr));

  return body;
}

assem::Proc *procEntryExit3(frame::Frame *frame, assem::InstrList *body){
  std::string prolog = frame->name->Name() + std::string(":\n");
  std::string instr = std::string(".set ") + frame->name->Name() + std::string("_framesize, ") 
    + std::to_string(-frame->offset) + std::string("\n");
  prolog += instr;
  instr = std::string("subq $") + std::to_string(-frame->offset) + std::string(", %rsp\n");
  prolog += instr;

  std::string epilog = std::string("addq $") + std::to_string(-frame->offset) + std::string(", %rsp\n");
  epilog += std::string("retq\n");

  return new assem::Proc(prolog, body, epilog);
}

tree::Exp *externalCall(std::string str, tree::ExpList *args) {
  return new tree::CallExp(new tree::NameExp(temp::LabelFactory::NamedLabel(str)), args);
}
} // namespace frame
