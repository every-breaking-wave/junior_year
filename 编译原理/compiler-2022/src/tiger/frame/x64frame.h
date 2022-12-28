
#ifndef TIGER_COMPILER_X64FRAME_H
#define TIGER_COMPILER_X64FRAME_H

#include "tiger/frame/frame.h"
extern frame::RegManager *reg_manager;
namespace frame {
class X64RegManager : public RegManager {
private:
  temp::Temp *rax;
  temp::Temp *rbx;
  temp::Temp *rcx;
  temp::Temp *rdx;
  temp::Temp *rsi;
  temp::Temp *rdi;
  temp::Temp *rbp;
  temp::Temp *rsp;
  temp::Temp *r8;
  temp::Temp *r9;
  temp::Temp *r10;
  temp::Temp *r11;
  temp::Temp *r12;
  temp::Temp *r13;
  temp::Temp *r14;
  temp::Temp *r15;
  
public:
  X64RegManager(){
    rax = temp::TempFactory::NewTemp();
    rbx = temp::TempFactory::NewTemp();
    rcx = temp::TempFactory::NewTemp();
    rdx = temp::TempFactory::NewTemp();
    rsi = temp::TempFactory::NewTemp();
    rdi = temp::TempFactory::NewTemp();
    rbp = temp::TempFactory::NewTemp();
    rsp = temp::TempFactory::NewTemp();
    r8 = temp::TempFactory::NewTemp();
    r9 = temp::TempFactory::NewTemp();
    r10 = temp::TempFactory::NewTemp();
    r11 = temp::TempFactory::NewTemp();
    r12 = temp::TempFactory::NewTemp();
    r13 = temp::TempFactory::NewTemp();
    r14 = temp::TempFactory::NewTemp();
    r15 = temp::TempFactory::NewTemp();

    // regs_=std::vector<temp::Temp*>{rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, 
    //   r8, r9, r10, r11, r12, r13, r14, r15};

    temp::Map::Name()->Enter(rax,new std::string("%rax"));
    temp::Map::Name()->Enter(rbx,new std::string("%rbx"));
    temp::Map::Name()->Enter(rcx,new std::string("%rcx"));
    temp::Map::Name()->Enter(rdx,new std::string("%rdx"));
    temp::Map::Name()->Enter(rsi,new std::string("%rsi"));
    temp::Map::Name()->Enter(rdi,new std::string("%rdi"));
    temp::Map::Name()->Enter(rbp,new std::string("%rbp"));
    temp::Map::Name()->Enter(rsp,new std::string("%rsp"));
    temp::Map::Name()->Enter(r8,new std::string("%r8"));
    temp::Map::Name()->Enter(r9,new std::string("%r9"));
    temp::Map::Name()->Enter(r10,new std::string("%r10"));
    temp::Map::Name()->Enter(r11,new std::string("%r11"));
    temp::Map::Name()->Enter(r12,new std::string("%r12"));
    temp::Map::Name()->Enter(r13,new std::string("%r13"));
    temp::Map::Name()->Enter(r14,new std::string("%r14"));
    temp::Map::Name()->Enter(r15,new std::string("%r15"));

    
    temp_map_->Enter(rax,new std::string("%rax"));
    temp_map_->Enter(rbx,new std::string("%rbx"));
    temp_map_->Enter(rcx,new std::string("%rcx"));
    temp_map_->Enter(rdx,new std::string("%rdx"));
    temp_map_->Enter(rsi,new std::string("%rsi"));
    temp_map_->Enter(rdi,new std::string("%rdi"));
    temp_map_->Enter(rbp,new std::string("%rbp"));
    temp_map_->Enter(rsp,new std::string("%rsp"));
    temp_map_->Enter(r8,new std::string("%r8"));
    temp_map_->Enter(r9,new std::string("%r9"));
    temp_map_->Enter(r10,new std::string("%r10"));
    temp_map_->Enter(r11,new std::string("%r11"));
    temp_map_->Enter(r12,new std::string("%r12"));
    temp_map_->Enter(r13,new std::string("%r13"));
    temp_map_->Enter(r14,new std::string("%r14"));
    temp_map_->Enter(r15,new std::string("%r15"));
  }

  // temp::Temp *GetRegister(int regno) { return regs_[regno]; }

  /**
   * Get general-purpose registers except RSP
   * NOTE: returned temp list should be in the order of calling convention
   * @return general-purpose registers
   */
  [[nodiscard]] temp::TempList *Registers(){
    temp::TempList *tempList = new temp::TempList({rax, rbx, rcx, rdx,
      rdi, rbp, r8, r9, r10, r11, r12, r13, r14, r15});

    return tempList;
  }

  /**
   * Get registers which can be used to hold arguments
   * NOTE: returned temp list must be in the order of calling convention
   * @return argument registers
   */
  [[nodiscard]] temp::TempList *ArgRegs(){
    temp::TempList *tempList = new temp::TempList({rdi, rsi, rdx, rcx, r8, r9});
    return tempList;
  }

  /**
   * Get caller-saved registers
   * NOTE: returned registers must be in the order of calling convention
   * @return caller-saved registers
   */
  [[nodiscard]] temp::TempList *CallerSaves(){
    temp::TempList *tempList = new temp::TempList({rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11});
    // these register set can't overlap each other?
    // temp::TempList *tempList = new temp::TempList({r10, r11});
    return tempList;
  }

  /**
   * Get callee-saved registers
   * NOTE: returned registers must be in the order of calling convention
   * @return callee-saved registers
   */
  [[nodiscard]] temp::TempList *CalleeSaves(){
    temp::TempList *tempList = new temp::TempList({rbx, rbp, r12, r13, r14, r15});
    return tempList;
  }

  /**
   * Get return-sink registers
   * @return return-sink registers
   */
  //TODO: not finish in lab5-part1 here
  [[nodiscard]] temp::TempList *ReturnSink(){
    temp::TempList *tempList = CalleeSaves();
    tempList->Append(StackPointer());
    tempList->Append(ReturnValue());

    return tempList;
  }

  /**
   * Get word size
   */
  [[nodiscard]] int WordSize(){
    return 8;
  }

  [[nodiscard]] temp::Temp *FramePointer(){
    return rbp;
  }

  [[nodiscard]] temp::Temp *StackPointer(){
    return rsp;
  }

  [[nodiscard]] temp::Temp *ReturnValue(){
    return rax;
  }
};


class InFrameAccess : public Access {
public:
  int offset;
  bool store_pointer_;
  explicit InFrameAccess(int offset) : offset(offset), store_pointer_(false) {}
  tree::Exp *ToExp(tree::Exp *framePtr) const override{
    tree::Exp * binopExp = new tree::BinopExp(tree::BinOp::PLUS_OP, framePtr, new tree::ConstExp(offset));
    return new tree::MemExp(binopExp);
  }
  void SetStorePointer(){store_pointer_ = false;}
};


class InRegAccess : public Access {
public:
  temp::Temp *reg;

  explicit InRegAccess(temp::Temp *reg) : reg(reg) {}
  tree::Exp *ToExp(tree::Exp *framePtr) const override{
    // printf("framePtr is ");
    return new tree::TempExp(reg);
  }
  void SetStorePointer(){
    reg->SetStorePointer();
  }
};

class X64Frame : public Frame {
  private:
  int reg_alloced = 1;

  public:
  X64Frame(){
    stms_ = new tree::StmList();
    offset -= reg_manager->WordSize();
  }

  virtual ~X64Frame(){
    delete stms_;
  }

  Access *allocLocal(bool escape){
    frame::Access *access;
    int reg_num = reg_manager->ArgRegs()->GetList().size();
    if(!escape && reg_alloced < reg_num){
      access = new InRegAccess(temp::TempFactory::NewTemp());
      reg_alloced++;
    }
    else{
      access = new InFrameAccess(offset);
      offset -= reg_manager->WordSize();
    }
    return access;
  }


  tree::Exp *externalCall(std::string s, tree::ExpList *args){
    // printf("external %s\n", s.data());
    return new tree::CallExp(new tree::NameExp(temp::LabelFactory::NamedLabel(s)), args);
  }
  
  tree::Exp *getFramePtr(){
    return new tree::TempExp(reg_manager->FramePointer());
  }

  std::string GetLabel(){
    return name->Name();
  }
};


tree::Exp* externalCall(std::string str, tree::ExpList* args);


} // namespace frame
#endif // TIGER_COMPILER_X64FRAME_H