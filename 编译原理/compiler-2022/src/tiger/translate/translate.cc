#include "tiger/translate/translate.h"

#include <tiger/absyn/absyn.h>

#include "tiger/env/env.h"
#include "tiger/errormsg/errormsg.h"
#include "tiger/frame/frame.h"
#include "tiger/frame/temp.h"
#include "tiger/frame/x64frame.h"

extern frame::Frags *frags;
extern frame::RegManager *reg_manager;
extern std::vector<std::string> functions_ret_ptr;


namespace tr {

Access *Access::allocLocal(Level *level, bool escape) {
  return new Access(level, level->frame_->allocLocal(escape));
}

class Cx {
public:
  tr::PatchList trues_;
  tr::PatchList falses_;
  tree::Stm *stm_;

  Cx(tr::PatchList trues, tr::PatchList falses, tree::Stm *stm)
      : trues_(trues), falses_(falses), stm_(stm) {}
};

class Exp {
public:
  [[nodiscard]] virtual tree::Exp *UnEx() const = 0;
  [[nodiscard]] virtual tree::Stm *UnNx() const = 0;
  [[nodiscard]] virtual Cx UnCx(err::ErrorMsg *errormsg) const = 0;
};

class ExpAndTy {
public:
  tr::Exp *exp_;
  type::Ty *ty_;

  ExpAndTy(tr::Exp *exp, type::Ty *ty) : exp_(exp), ty_(ty) {}
};

class ExExp : public Exp {
public:
  tree::Exp *exp_;

  explicit ExExp(tree::Exp *exp) : exp_(exp) {}

  [[nodiscard]] tree::Exp *UnEx() const override { return exp_; }

  [[nodiscard]] tree::Stm *UnNx() const override {
    return new tree::ExpStm(exp_);
  }

  [[nodiscard]] Cx UnCx(err::ErrorMsg *errormsg) const override {
    tree::CjumpStm *cjStm = new tree::CjumpStm(
        tree::NE_OP, exp_, new tree::ConstExp(0), nullptr, nullptr);
    return Cx(*(new PatchList({&cjStm->true_label_})),
              *(new PatchList({&cjStm->false_label_})), cjStm);
  }
};

class NxExp : public Exp {
public:
  tree::Stm *stm_;

  explicit NxExp(tree::Stm *stm) : stm_(stm) {}

  [[nodiscard]] tree::Exp *UnEx() const override {
    return new tree::EseqExp(stm_, new tree::ConstExp(0));
  }
  [[nodiscard]] tree::Stm *UnNx() const override { return stm_; }
  [[nodiscard]] Cx UnCx(err::ErrorMsg *errormsg) const override {
    assert(false);
    printf("Nx cannot be a Cx\n");
  }
};

class CxExp : public Exp {
public:
  Cx cx_;

  CxExp(tr::PatchList trues, tr::PatchList falses, tree::Stm *stm)
      : cx_(trues, falses, stm) {}

  [[nodiscard]] tree::Exp *UnEx() const override {
    temp::Temp *r = temp::TempFactory::NewTemp();
    temp::Label *true_label = temp::LabelFactory::NewLabel();
    temp::Label *false_label = temp::LabelFactory::NewLabel();
    for (auto &patch : cx_.trues_.GetList())
      *patch = true_label;
    for (auto &patc : cx_.falses_.GetList())
      *patc = false_label;

    return new tree::EseqExp(
        new tree::MoveStm(new tree::TempExp(r),
                          new tree::ConstExp(1)), // pass 1 to temp r
        new tree::EseqExp(
            cx_.stm_,
            new tree::EseqExp(
                new tree::LabelStm(false_label), // mark false label
                new tree::EseqExp(
                    new tree::MoveStm(
                        new tree::TempExp(r),
                        new tree::ConstExp(0)), // if false, mark r as 0
                    new tree::EseqExp(
                        new tree::LabelStm(true_label),
                        new tree::TempExp(
                            r)))))); // else, mark true label, skip the
                                     // process of marking r as 1 'cause
                                     // default value is bigger than 0
  }
  [[nodiscard]] tree::Stm *UnNx() const override {
    // instead tran to Nx directly, fisrt tran to Ex, then tran Ex to Nx for it
    // is quite simple to multi-tran between Ex and Nx
    return new tree::ExpStm(UnEx());
  }

  [[nodiscard]] Cx UnCx(err::ErrorMsg *errormsg) const override { return cx_; }
};

void ProgTr::Translate() {
  // absyn_tree_->Translate(
  //     venv_.get(), tenv_.get(),
  //     new Level(new frame::X64Frame(temp::LabelFactory::NamedLabel("main"),
  //     {}),
  //               nullptr),
  //     temp::LabelFactory::NamedLabel("main"), errormsg_.get());
  temp::Label *main_label = temp::LabelFactory::NamedLabel("tigermain");
  // frame::Frame *new_frame = new frame::X64Frame(main_label,
  // std::list<bool>());
  tr::Level *new_level =
      new tr::Level(frame::newFrame(main_label, std::list<bool>()), nullptr);
  main_level_.reset(new_level);
  tenv_ = std::make_unique<env::TEnv>();
  venv_ = std::make_unique<env::VEnv>();
  FillBaseTEnv();
  FillBaseVEnv();
  tr::ExpAndTy *tree = absyn_tree_.get()->Translate(
      venv_.get(), tenv_.get(), main_level_.get(), main_label, errormsg_.get());
  // tree::Stm *stm = tree->exp_->UnNx();
  // frags->PushBack(new frame::ProcFrag(stm, new_frame));
  tree::Stm *stm = new tree::MoveStm(
      new tree::TempExp(reg_manager->ReturnValue()), tree->exp_->UnEx());
  frags->PushBack(new frame::ProcFrag(
      frame::procEntryExit1(new_level->frame_, stm), new_level->frame_));
}

tree::Exp *StaticLink(tr::Level *current, tr::Level *target) {
  //  Derive the calculated expression from the current and target stack frame
  //  level
  // let exp point to Frame Register %rbp
  tree::Exp *staticlink = new tree::TempExp(reg_manager->FramePointer());
  while (current != target) { // find frame access from frame bottom
    if (!current) {
      return nullptr;
    }
    //  The staticlink should be the first argument to the stack frame
    staticlink = current->frame_->formals_.front()->ToExp(staticlink);
    current = current->parent_;
  }
  return staticlink;
}

tr::Level *newLevel(Level *parent, temp::Label *name, std::list<bool> formals) {
  formals.push_front(true);
  frame::Frame *newFrame = frame::newFrame(name, formals);
  tr::Level *level = new tr::Level(newFrame, parent);

  return level;
}

} // namespace tr

namespace absyn {

/********** GC Protocol **********/

/*Put record descriptor into stringFrag
 * Structure: "$010101$"
 * LableName: "$typeName$_DESCRIPTOR"
 */
void emitRecordRecordTypeDescriptor(type::RecordTy *recordTy,
                                    sym::Symbol *name) {
  std::string pointMAP, recordNAME;
  recordNAME = name->Name() + "_DESCRIPTOR";
  std::list<type::Field *> field_list = recordTy->fields_->GetList();
  for (const type::Field *field : field_list) {
    if (typeid(*field->ty_->ActualTy()) == typeid(type::RecordTy) ||
        typeid(*field->ty_->ActualTy()) == typeid(type::ArrayTy))
      pointMAP += "1";
    else
      pointMAP += "0";
  }

  temp::Label *str_lable = temp::LabelFactory::NamedLabel(recordNAME);
  frame::StringFrag *str_frag = new frame::StringFrag(str_lable, pointMAP);
  frags->PushBack(str_frag);
}

bool IsPointer(type::Ty *ty_) {
  return typeid(*(ty_->ActualTy())) == typeid(type::RecordTy) ||
         typeid(*(ty_->ActualTy())) == typeid(type::ArrayTy);
}

/********** END GC Protocol **********/

tr::ExpAndTy *AbsynTree::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level, temp::Label *label,
                                   err::ErrorMsg *errormsg) const {
  return root_->Translate(venv, tenv, level, label, errormsg);
}

tr::ExpAndTy *SimpleVar::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level, temp::Label *label,
                                   err::ErrorMsg *errormsg) const {
  tr::Exp *exp = nullptr;
  type::Ty *ty;

  env::EnvEntry *entry = venv->Look(sym_);
  if (entry && typeid(*entry) == typeid(env::VarEntry)) {
    // errormsg->Error(pos_, "do varEntry");
    env::VarEntry *ventry = static_cast<env::VarEntry *>(entry);
    tr::Access *access = ventry->access_;
    tree::Exp *exp = new tree::TempExp(reg_manager->FramePointer());
    while (level != ventry->access_->level_) {
      exp = level->frame_->formals_.front()->ToExp(exp);
      level = level->parent_;
    }
    return new tr::ExpAndTy(new tr::ExExp(access->access_->ToExp(exp)),
                            ventry->ty_->ActualTy());
  } else {
    errormsg->Error(pos_, "undefined variable %s", sym_->Name().data());
    return new tr::ExpAndTy(nullptr, type::IntTy::Instance());
  }
}

tr::ExpAndTy *FieldVar::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level, temp::Label *label,
                                  err::ErrorMsg *errormsg) const {
  // translate var
  tr::ExpAndTy *tr_var = var_->Translate(venv, tenv, level, label, errormsg);
  // cast ty to record_ty
  type::RecordTy *record_ty =
      static_cast<type::RecordTy *>(tr_var->ty_->ActualTy());

  int i = 0;
  const auto &field_list = (record_ty->fields_->GetList());
  for (auto field : field_list) {
    if (field->name_->Name() == sym_->Name()) {
      tree::MemExp *exp = tree::getMemByBaseAndIndex(
          tr_var->exp_->UnEx(), (i * reg_manager->WordSize()));
      return new tr::ExpAndTy(new tr::ExExp(exp), tr_var->ty_->ActualTy());
    }
    i++;
  }
  return new tr::ExpAndTy(nullptr, type::IntTy::Instance());
}

tr::ExpAndTy *SubscriptVar::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                      tr::Level *level, temp::Label *label,
                                      err::ErrorMsg *errormsg) const {
  tr::ExpAndTy *var = var_->Translate(venv, tenv, level, label, errormsg);
  tr::ExpAndTy *subscript =
      subscript_->Translate(venv, tenv, level, label, errormsg);
  tr::Exp *subscriptVar = new tr::ExExp(new tree::MemExp(new tree::BinopExp(
      tree::PLUS_OP, var->exp_->UnEx(),
      new tree::BinopExp(tree::MUL_OP, subscript->exp_->UnEx(),
                         new tree::ConstExp(reg_manager->WordSize())))));
  return new tr::ExpAndTy(
      subscriptVar, (static_cast<type::ArrayTy *>(var->ty_))->ty_->ActualTy());
}

tr::ExpAndTy *VarExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level, temp::Label *label,
                                err::ErrorMsg *errormsg) const {
  return this->var_->Translate(venv, tenv, level, label, errormsg);
}

tr::ExpAndTy *NilExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level, temp::Label *label,
                                err::ErrorMsg *errormsg) const {
  // errormsg->Error(pos_, "begin nil exp");
  return new tr::ExpAndTy(new tr::ExExp(new tree::ConstExp(0)),
                          type::NilTy::Instance());
}

tr::ExpAndTy *IntExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level, temp::Label *label,
                                err::ErrorMsg *errormsg) const {
  // printf("begin int exp %d", val_);
  return new tr::ExpAndTy(new tr::ExExp(new tree::ConstExp(val_)),
                          type::IntTy::Instance());
}

tr::ExpAndTy *StringExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level, temp::Label *label,
                                   err::ErrorMsg *errormsg) const {
  temp::Label *string_label = temp::LabelFactory::NewLabel();
  frags->PushBack(new frame::StringFrag(string_label, str_));
  return new tr::ExpAndTy(new tr::ExExp(new tree::NameExp(string_label)),
                          type::StringTy::Instance());
}

tr::ExpAndTy *CallExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                 tr::Level *level, temp::Label *label,
                                 err::ErrorMsg *errormsg) const {
  env::FunEntry *entry = (env::FunEntry *)(venv->Look(func_));
  tr::Exp *exp = nullptr;
  if (!entry) {
    return new tr::ExpAndTy(exp, type::VoidTy::Instance());
  }
  env::FunEntry *fun_entry = static_cast<env::FunEntry *>(entry);
  type::Ty *ty = fun_entry->result_ ? fun_entry->result_->ActualTy()
                                    : type::VoidTy::Instance();
  std::list<type::Ty *> formal_list = fun_entry->formals_->GetList();
  std::list<Exp *> arg_list = args_->GetList();
  int size = std::min(arg_list.size(), formal_list.size());
  auto list = new tree::ExpList();
  auto formal_iter = formal_list.begin();
  auto arg_iter = arg_list.begin();
  for (auto i = 0; i < size; i++) {
    list->Append((*arg_iter)
                     ->Translate(venv, tenv, level, label, errormsg)
                     ->exp_->UnEx());
    arg_iter++;
    // formal_iter++;
  }
  if (fun_entry->level_->parent_) {
    tree::Exp *sl = new tree::TempExp(reg_manager->FramePointer());
    while (level != entry->level_->parent_) {
      sl = level->frame_->formals_.front()->ToExp(sl);
      level = level->parent_;
    }
    list->Insert(sl);
    exp = new tr::ExExp(
        new tree::CallExp(new tree::NameExp(entry->label_), list));
  } else { // call external functions
    exp = new tr::ExExp(
        frame::externalCall(temp::LabelFactory::LabelString(func_), list));
  }
  return new tr::ExpAndTy(exp, ty);
}

tr::ExpAndTy *OpExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                               tr::Level *level, temp::Label *label,
                               err::ErrorMsg *errormsg) const {
  tr::ExpAndTy *left = left_->Translate(venv, tenv, level, label, errormsg);
  tr::ExpAndTy *right = right_->Translate(venv, tenv, level, label, errormsg);
  tr::Exp *exp = nullptr;
  tree::CjumpStm *cjstm = nullptr;

  if (oper_ == absyn::AND_OP || oper_ == absyn::OR_OP) {
    tr::Cx left_cx = left->exp_->UnCx(errormsg);
    tr::Cx right_cx = right->exp_->UnCx(errormsg);
    temp::Label *next_label = temp::LabelFactory::NewLabel();
    // Do Patch
    tr::PatchList true_list = tr::PatchList();
    tr::PatchList false_list = tr::PatchList();
    tree::Stm *s1 = nullptr;
    if (oper_ == OR_OP) {
      static_cast<tree::CjumpStm *>(left_cx.stm_)->false_label_ = next_label;
      left_cx.falses_.DoPatch(next_label);
      s1 = new tree::SeqStm(
          left_cx.stm_,
          new tree::SeqStm(new tree::LabelStm(next_label), right_cx.stm_));
      true_list = tr::PatchList::JoinPatch(left_cx.trues_, right_cx.trues_);
      false_list =
          tr::PatchList::JoinPatch(right_cx.falses_, *(new tr::PatchList()));
    } else {
      static_cast<tree::CjumpStm *>(left_cx.stm_)->true_label_ = next_label;
      left_cx.trues_.DoPatch(next_label);
      s1 = new tree::SeqStm(
          left_cx.stm_,
          new tree::SeqStm(new tree::LabelStm(next_label), right_cx.stm_));
      true_list =
          tr::PatchList::JoinPatch(right_cx.trues_, *(new tr::PatchList()));
      false_list = tr::PatchList::JoinPatch(left_cx.falses_, right_cx.falses_);
    }
    return new tr::ExpAndTy(new tr::CxExp(true_list, false_list, s1),
                            type::IntTy::Instance());
  }

  tree::Exp *l = left->exp_->UnEx();
  tree::Exp *r = right->exp_->UnEx();
  if (oper_ == absyn::TIMES_OP || oper_ == absyn::DIVIDE_OP ||
      oper_ == absyn::PLUS_OP || oper_ == absyn::MINUS_OP) {
    switch (oper_) {
    case absyn::PLUS_OP:
      exp = new tr::ExExp(new tree::BinopExp(tree::BinOp::PLUS_OP, l, r));
      break;
    case absyn::MINUS_OP:
      exp = new tr::ExExp(new tree::BinopExp(tree::BinOp::MINUS_OP, l, r));
      break;
    case absyn::TIMES_OP:
      exp = new tr::ExExp(new tree::BinopExp(tree::BinOp::MUL_OP, l, r));
      break;
    case absyn::DIVIDE_OP:
      exp = new tr::ExExp(new tree::BinopExp(tree::BinOp::DIV_OP, l, r));
      break;
    default:
      break;
    }
  }

  else if (oper_ == absyn::GE_OP || oper_ == absyn::GT_OP ||
           oper_ == absyn::LE_OP || oper_ == absyn::LT_OP) {
    switch (oper_) {
    case absyn::GE_OP:
      cjstm = new tree::CjumpStm(tree::RelOp::GE_OP, l, r, nullptr, nullptr);
      break;
    case absyn::GT_OP:
      cjstm = new tree::CjumpStm(tree::RelOp::GT_OP, l, r, nullptr, nullptr);
      break;
    case absyn::LE_OP:
      cjstm = new tree::CjumpStm(tree::RelOp::LE_OP, l, r, nullptr, nullptr);
      break;
    case absyn::LT_OP:
      cjstm = new tree::CjumpStm(tree::RelOp::LT_OP, l, r, nullptr, nullptr);
      break;
    default:
      break;
    }
    // tr::PatchList trues = tr::PatchList({&(cjstm->true_label_)});
    // tr::PatchList falses = tr::PatchList({&(cjstm->false_label_)});
    std::list<temp::Label **> true_label_list;
    std::list<temp::Label **> false_label_list;
    true_label_list.emplace_back(&(cjstm->true_label_));
    false_label_list.emplace_back(&(cjstm->false_label_));
    exp = new tr::CxExp(*(new tr::PatchList(true_label_list)),
                        *(new tr::PatchList(false_label_list)), cjstm);
  }

  if (oper_ == absyn::EQ_OP || oper_ == absyn::NEQ_OP) {
    if (typeid(*(left->ty_)) != typeid(type::StringTy)) {
      switch (oper_) {
      case EQ_OP:
        cjstm = new tree::CjumpStm(tree::RelOp::EQ_OP, left->exp_->UnEx(),
                                   right->exp_->UnEx(), nullptr, nullptr);
        break;
      case NEQ_OP:
        cjstm = new tree::CjumpStm(tree::RelOp::NE_OP, left->exp_->UnEx(),
                                   right->exp_->UnEx(), nullptr, nullptr);
      }
    } else {
      tree::ExpList *expList =
          new tree::ExpList({left->exp_->UnEx(), right->exp_->UnEx()});
      cjstm = new tree::CjumpStm(
          tree::EQ_OP,
          new tree::CallExp(
              new tree::NameExp(temp::LabelFactory::NamedLabel("string_equal")),
              expList),
          new tree::ConstExp(1), nullptr, nullptr);
    }
    // tr::PatchList trues = tr::PatchList({&(cjstm->true_label_)});
    // tr::PatchList falses = tr::PatchList({&(cjstm->false_label_)});
    std::list<temp::Label **> true_label_list;
    std::list<temp::Label **> false_label_list;
    true_label_list.emplace_back(&(cjstm->true_label_));
    false_label_list.emplace_back(&(cjstm->false_label_));
    exp = new tr::CxExp(*(new tr::PatchList(true_label_list)),
                        *(new tr::PatchList(false_label_list)), cjstm);
  }

  return new tr::ExpAndTy(exp, type::IntTy::Instance());
}

tr::ExpAndTy *RecordExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level, temp::Label *label,
                                   err::ErrorMsg *errormsg) const {
  tree::ExpList *explist = new tree::ExpList();
  for (const auto &field : this->fields_->GetList()) {
    auto tr_exp = field->exp_->Translate(venv, tenv, level, label, errormsg);
    explist->Append(tr_exp->exp_->UnEx());
  }
  int num = static_cast<int>(fields_->GetList().size());
  auto reg = temp::TempFactory::NewTemp();
  auto exp_list = new tree::ExpList();
  exp_list->Append(new tree::ConstExp(num * reg_manager->WordSize()));
  // Add a descriptor for record
  exp_list->Append(new tree::NameExp(temp::LabelFactory::NamedLabel(typ_->Name() + "_DESCRIPTOR")));
  tree::Stm *stm = new tree::MoveStm(
      new tree::TempExp(reg), frame::externalCall("alloc_record", exp_list));

  num = 0;
  auto expressions = explist->GetList();
  for (auto &expr : expressions) {
    auto dst = tree::getMemByBaseAndIndex(new tree::TempExp(reg),
                                          (num * reg_manager->WordSize()));
    stm = (new tree::SeqStm(stm, new tree::MoveStm(dst, expr)));
    ++num;
  }

  tr::ExExp *exp =
      new tr::ExExp(new tree::EseqExp(stm, new tree::TempExp(reg)));
  return new tr::ExpAndTy(exp, tenv->Look(typ_)->ActualTy());
}

tr::ExpAndTy *SeqExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level, temp::Label *label,
                                err::ErrorMsg *errormsg) const {
  ExpList *seq = seq_;
  std::list<Exp *> exp_list = seq->GetList();
  std::list<Exp *>::iterator it = exp_list.begin();
  tr::Exp *exp = nullptr;
  type::Ty *ty;
  for (auto exp_ele : exp_list) {
    tr::ExpAndTy *tmp = exp_ele->Translate(venv, tenv, level, label, errormsg);
    if (!exp) {
      exp = new tr::ExExp(tmp->exp_->UnEx());
    } else {
      exp = new tr::ExExp(new tree::EseqExp(exp->UnNx(), tmp->exp_->UnEx()));
    }
    ty = tmp->ty_->ActualTy();
  }
  return new tr::ExpAndTy(exp, ty);
}

tr::ExpAndTy *AssignExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   tr::Level *level, temp::Label *label,
                                   err::ErrorMsg *errormsg) const {
  tr::ExpAndTy *tr_var = var_->Translate(venv, tenv, level, label, errormsg);
  tr::ExpAndTy *tr_exp = exp_->Translate(venv, tenv, level, label, errormsg);
  // execute move instruction, exp->var
  tree::MoveStm *move_stm =
      new tree::MoveStm(tr_var->exp_->UnEx(), tr_exp->exp_->UnEx());
  return new tr::ExpAndTy(new tr::NxExp(move_stm), type::VoidTy::Instance());
}

tr::ExpAndTy *IfExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                               tr::Level *level, temp::Label *label,
                               err::ErrorMsg *errormsg) const {
  auto tr_test = this->test_->Translate(venv, tenv, level, label, errormsg);
  auto tr_then = this->then_->Translate(venv, tenv, level, label, errormsg);
  tr::ExpAndTy *tr_else(nullptr); // tr_else may not exist
  if (this->elsee_) {
    tr_else = this->elsee_->Translate(venv, tenv, level, label, errormsg);
  }

  temp::Temp *ret = temp::TempFactory::NewTemp(); // register save return value

  temp::Label *true_label = temp::LabelFactory::NewLabel();
  temp::Label *false_label = temp::LabelFactory::NewLabel();
  temp::Label *done_label = temp::LabelFactory::NewLabel();
  auto done_label_vector = new std::vector<temp::Label *>;
  done_label_vector->emplace_back(done_label); // setting done label list

  // generate a new Cx by test
  tr::Cx cx = tr_test->exp_->UnCx(errormsg);
  // do patch
  cx.falses_.DoPatch(false_label);
  cx.trues_.DoPatch(true_label);
  tr::Exp *exp;
  if (elsee_) {
    exp = new tr::ExExp(new tree::EseqExp(
        cx.stm_,
        new tree::EseqExp(
            // if test is true, pass then value to ret register
            new tree::LabelStm(true_label),
            new tree::EseqExp(
                new tree::MoveStm(new tree::TempExp(ret),
                                  tr_then->exp_->UnEx()),
                new tree::EseqExp(
                    // if test is true, jump to done label directly
                    new tree::JumpStm(new tree::NameExp(done_label),
                                      done_label_vector),
                    new tree::EseqExp(
                        // if test is false, pass else value to ret register
                        new tree::LabelStm(false_label),
                        new tree::EseqExp(
                            new tree::MoveStm(new tree::TempExp(ret),
                                              tr_else->exp_->UnEx()),
                            new tree::EseqExp(
                                new tree::JumpStm(new tree::NameExp(done_label),
                                                  done_label_vector),
                                // setting ret register as return valuee
                                new tree::EseqExp(
                                    new tree::LabelStm(done_label),
                                    new tree::TempExp(ret))))))))));
  } else {
    exp = new tr::NxExp(new tree::SeqStm(
        cx.stm_,
        new tree::SeqStm(new tree::LabelStm(true_label),
                         new tree::SeqStm(tr_then->exp_->UnNx(),
                                          // if test is false, skip then
                                          new tree::LabelStm(false_label)))));
  }
  return new tr::ExpAndTy(exp, tr_then->ty_->ActualTy());
}

tr::ExpAndTy *WhileExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level, temp::Label *label,
                                  err::ErrorMsg *errormsg) const {
  auto tr_test = this->test_->Translate(venv, tenv, level, label, errormsg);
  auto done_label = temp::LabelFactory::NewLabel();
  auto test_label = temp::LabelFactory::NewLabel();
  auto body_label = temp::LabelFactory::NewLabel();
  auto tr_body =
      this->body_->Translate(venv, tenv, level, done_label, errormsg);
  tr::Cx cx = tr_test->exp_->UnCx(errormsg);
  cx.trues_.DoPatch(body_label);  // if test true, goto body
  cx.falses_.DoPatch(done_label); // else go to done
  std::vector<temp::Label *> *test_vector_ptr = new std::vector<temp::Label *>;
  test_vector_ptr->emplace_back(test_label);
  // consider while_exp as no elsee_ if_exp, test label as done label
  tr::Exp *exp = new tr::NxExp(new tree::SeqStm(
      new tree::LabelStm(test_label),
      new tree::SeqStm(
          cx.stm_, new tree::SeqStm(
                       new tree::LabelStm(body_label),
                       new tree::SeqStm(
                           tr_body->exp_->UnNx(),
                           new tree::SeqStm(
                               new tree::JumpStm(new tree::NameExp(test_label),
                                                 test_vector_ptr),
                               new tree::LabelStm(done_label)))))));
  return new tr::ExpAndTy(exp, type::VoidTy::Instance());
}

tr::ExpAndTy *ForExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level, temp::Label *label,
                                err::ErrorMsg *errormsg) const {
  venv->BeginScope();
  // translate lo and hi, body
  auto tr_lo = this->lo_->Translate(venv, tenv, level, label, errormsg);
  auto tr_hi = this->hi_->Translate(venv, tenv, level, label, errormsg);
  tr::Access *loop_access = tr::Access::allocLocal(
      level, escape_); // loop var in for_loop, considered as i
  venv->Enter(this->var_,
              new env::VarEntry(loop_access, tr_lo->ty_->ActualTy()));
  temp::Label *body_label = temp::LabelFactory::NewLabel();
  temp::Label *test_label = temp::LabelFactory::NewLabel();
  temp::Label *done_label = temp::LabelFactory::NewLabel();
  // translate body in a new venv
  auto tr_body =
      this->body_->Translate(venv, tenv, level, done_label, errormsg);
  venv->EndScope();
  // consider for loop as if_exp, test if i<hi, if true, goto body,i++ if false,
  // goto done

  // assign loop var to value of lo, if it is escape, it will be store in frame,
  // else in register
  tree::Exp *i = loop_access->access_->ToExp(
      new tree::TempExp(reg_manager->FramePointer()));
  tr::Exp *exp = new tr::NxExp(new tree::SeqStm(
      new tree::MoveStm(i, tr_lo->exp_->UnEx()),
      new tree::SeqStm(
          new tree::LabelStm(test_label),
          new tree::SeqStm(
              new tree::CjumpStm(tree::RelOp::LE_OP, i, tr_hi->exp_->UnEx(),
                                 body_label, done_label),
              new tree::SeqStm(
                  new tree::LabelStm(body_label),
                  new tree::SeqStm(
                      tr_body->exp_->UnNx(),
                      new tree::SeqStm(
                          new tree::MoveStm(
                              i, new tree::BinopExp(tree::BinOp::PLUS_OP, i,
                                                    new tree::ConstExp(1))),
                          new tree::SeqStm(
                              new tree::JumpStm(new tree::NameExp(test_label),
                                                new std::vector<temp::Label *>(
                                                    1, test_label)),
                              new tree::LabelStm(done_label)))))))));
  return new tr::ExpAndTy(exp, type::VoidTy::Instance());
}

tr::ExpAndTy *BreakExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level, temp::Label *label,
                                  err::ErrorMsg *errormsg) const {
  // jump to break target label
  auto jump_labels = new std::vector<temp::Label *>(1, label);
  return new tr::ExpAndTy(
      new tr::NxExp(new tree::JumpStm(new tree::NameExp(label), jump_labels)),
      type::VoidTy::Instance());
}

tr::ExpAndTy *LetExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level, temp::Label *label,
                                err::ErrorMsg *errormsg) const {
  const auto &decs = this->decs_->GetList();
  std::list<tr::Exp *> tr_dec_list; // store translated decs
  // translate decs
  int num = decs_->GetList().size();
  for (auto dec : decs) {
    tr_dec_list.emplace_back(
        dec->Translate(venv, tenv, level, label, errormsg));
  }
  tree::Exp *exp = nullptr;
  tree::Stm *stm = nullptr;
  if (tr_dec_list.size() > 0) {
    // res = tr_dec_list.front();
    for (auto &dec : tr_dec_list) {
      stm = stm ? new tree::SeqStm(stm, dec->UnNx()) : dec->UnNx();
    }
  }
  // translate bod
  tr::ExpAndTy *tr_body = body_->Translate(venv, tenv, level, label, errormsg);
  exp = stm ? new tree::EseqExp(stm, tr_body->exp_->UnEx())
            : tr_body->exp_->UnEx();
  return new tr::ExpAndTy(new tr::ExExp(exp), tr_body->ty_);
}

tr::ExpAndTy *ArrayExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                  tr::Level *level, temp::Label *label,
                                  err::ErrorMsg *errormsg) const {
  type::Ty *ty = tenv->Look(typ_); // get array type
  tr::ExpAndTy *tr_size = size_->Translate(venv, tenv, level, label, errormsg);
  tr::ExpAndTy *tr_init = init_->Translate(venv, tenv, level, label, errormsg);
  // alloc array on heap by externel call
  tree::Exp *array_exp = frame::externalCall(
      "init_array",
      new tree::ExpList({tr_size->exp_->UnEx(), tr_init->exp_->UnEx()}));
  return new tr::ExpAndTy(new tr::ExExp(array_exp), ty->ActualTy());
}

tr::ExpAndTy *VoidExp::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                 tr::Level *level, temp::Label *label,
                                 err::ErrorMsg *errormsg) const {
  return new tr::ExpAndTy(nullptr, type::VoidTy::Instance());
}

tr::Exp *FunctionDec::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                                tr::Level *level, temp::Label *label,
                                err::ErrorMsg *errormsg) const {
  std::list<FunDec *> functions = functions_->GetList();
  for (auto func_dec : functions) {
    // build escape table
    std::list<bool> esacpes;
    for (auto field : func_dec->params_->GetList()) {
      esacpes.emplace_back(field->escape_);
    }
    // add a level for this func
    temp::Label *new_label =
        temp::LabelFactory::NamedLabel(func_dec->name_->Name());
    tr::Level *new_level = tr::newLevel(level, new_label, esacpes);
    if (func_dec->result_) { // fun has a return value
      type::Ty *result_ty = tenv->Look(func_dec->result_);
      venv->Enter(
          func_dec->name_,
          new env::FunEntry(new_level, new_label,
                            func_dec->params_->MakeFormalTyList(tenv, errormsg),
                            result_ty));
    } else {
      venv->Enter(
          func_dec->name_,
          new env::FunEntry(new_level, new_label,
                            func_dec->params_->MakeFormalTyList(tenv, errormsg),
                            type::VoidTy::Instance()));
    }
  }

  for (FunDec *function : functions) {
    // env::EnvEntry *fun = venv->Look(function->name_);
    env::FunEntry *funentry =
        static_cast<env::FunEntry *>(venv->Look(function->name_));
    type::TyList *formals = funentry->formals_;
    const auto &field_list = function->params_->GetList();
    auto formal_it = formals->GetList().begin();
    auto param_it = (function->params_->GetList()).begin();
    venv->BeginScope();
    std::list<frame::Access *> accessList = funentry->level_->frame_->formals_;
    std::list<frame::Access *>::iterator access_it = accessList.begin();
    // Skip the staticlink!
    access_it++;
    for (; param_it != field_list.end(); param_it++) {
      if(IsPointer(tenv->Look((*param_it)->typ_))){
        (*access_it)->SetStorePointer();
      }
      venv->Enter(
          (*param_it)->name_,
          new env::VarEntry(new tr::Access(funentry->level_, *access_it),
                            *formal_it));
      formal_it++;
      access_it++;
    }
    tr::ExpAndTy *body = function->body_->Translate(
        venv, tenv, funentry->level_, funentry->label_, errormsg);
    venv->EndScope();
    // For GC
    if(IsPointer(funentry->result_)){
      functions_ret_ptr.emplace_back(funentry->label_->Name());
    }
    tree::Stm *stm = new tree::MoveStm(
        new tree::TempExp(reg_manager->ReturnValue()), body->exp_->UnEx());
    stm = frame::procEntryExit1((funentry)->level_->frame_, stm);
    frags->PushBack(new frame::ProcFrag(stm, (funentry)->level_->frame_));
  }

  return new tr::ExExp(new tree::ConstExp(0));
}

tr::Exp *VarDec::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                           tr::Level *level, temp::Label *label,
                           err::ErrorMsg *errormsg) const {
  tr::ExpAndTy *tr_init = init_->Translate(venv, tenv, level, label, errormsg);
  // decide var type
  type::Ty *var_ty;
  if (!typ_) {
    var_ty = tr_init->ty_->ActualTy();
  } else {
    var_ty = tenv->Look(typ_);
  }
  // alloc local for var
  tr::Access *access = tr::Access::allocLocal(level, escape_);
  venv->Enter(var_, new env::VarEntry(access, var_ty));
  // For GC
  if(IsPointer(tr_init->ty_)){
    access->access_->SetStorePointer();
  }
  // translate access to exp by staticlink
  tr::Exp *tr_var = new tr::ExExp(
      access->access_->ToExp(tr::StaticLink(access->level_, level)));
  return new tr::NxExp(
      new tree::MoveStm(tr_var->UnEx(), tr_init->exp_->UnEx()));
}

tr::Exp *TypeDec::Translate(env::VEnvPtr venv, env::TEnvPtr tenv,
                            tr::Level *level, temp::Label *label,
                            err::ErrorMsg *errormsg) const {
  const auto &name_and_ty_list = this->types_->GetList();
  for (auto &name_and_ty : name_and_ty_list) {
    tenv->Enter(name_and_ty->name_,
                new type::NameTy(name_and_ty->name_, nullptr));
  }
  for (auto &name_and_ty : name_and_ty_list) {
    type::NameTy *nameTy = (type::NameTy *)tenv->Look(name_and_ty->name_);
    nameTy->ty_ = name_and_ty->ty_->Translate(tenv, errormsg);
    tenv->Set(name_and_ty->name_, nameTy);
    // For GC
    if(typeid(*nameTy->ty_->ActualTy()) == typeid(type::RecordTy)){
      emitRecordRecordTypeDescriptor(static_cast<type::RecordTy *>(nameTy->ty_),
          nameTy->sym_);
    }
  }
  return new tr::ExExp(new tree::ConstExp(0));
}

type::Ty *NameTy::Translate(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  type::Ty *name_ty = tenv->Look(name_);
  if (!name_ty) {
    return type::VoidTy::Instance();
  }
  return new type::NameTy(name_, name_ty);
}

type::Ty *RecordTy::Translate(env::TEnvPtr tenv,
                              err::ErrorMsg *errormsg) const {
  type::FieldList *fields = record_->MakeFieldList(tenv, errormsg);

  return new type::RecordTy(fields);
}

type::Ty *ArrayTy::Translate(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  type::Ty *array_ty = tenv->Look(array_);
  if (!array_ty) {
    return type::VoidTy::Instance();
  }
  return new type::ArrayTy(array_ty);
}

} // namespace absyn
