#include "tiger/absyn/absyn.h"
#include "tiger/semant/semant.h"

namespace absyn {

void AbsynTree::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                           err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  root_->SemAnalyze(venv, tenv, 0, errormsg);
}

type::Ty *SimpleVar::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  env::EnvEntry *entry = venv->Look(sym_);
  if (entry && entry->kind_ == env::EnvEntry::VAR) {
    return ((env::VarEntry*)entry)->ty_->ActualTy();
  } else {
    errormsg->Error(pos_, "undefined variable %s", sym_->Name().c_str());
    return type::IntTy::Instance();
  }

}

type::Ty *FieldVar::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty *ty = var_->SemAnalyze(venv, tenv, labelcount, errormsg);

  if (ty->kind_ != type::Ty::Kind::RECORD) {
    errormsg->Error(pos_, "not a record type");
    return type::IntTy::Instance();
  };

  type::FieldList* fieldlist = ((type::RecordTy*)ty)->fields_;

  std::list<type::Field *>* contents = &fieldlist->GetList();
  for (auto iter = contents->begin(); iter != contents->end(); iter++){
    if ((*iter)->name_ == sym_) {
      return (*iter)->ty_;
    }
  }

  errormsg->Error(pos_, "field %s doesn't exist", sym_->Name().c_str());
  return type::IntTy::Instance();
}

type::Ty *SubscriptVar::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                   int labelcount,
                                   err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty *var_ty = var_->SemAnalyze(venv, tenv, labelcount, errormsg);
  type::Ty *exp_ty = subscript_->SemAnalyze(venv, tenv, labelcount, errormsg);

  if (var_ty->kind_ != type::Ty::Kind::ARRAY) {
    errormsg->Error(pos_, "array type required");
    return type::IntTy::Instance();
  };

  if (exp_ty->kind_ != type::Ty::Kind::INT) {
    errormsg->Error(pos_, "array index must be integer");
    return type::IntTy::Instance();
  };

  // why this ?
  return ((type::ArrayTy*) var_ty)->ty_->ActualTy();
}

type::Ty *VarExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  switch (var_->kind_)
  {
  case Var::Kind::SIMPLE:
    return ((SimpleVar*) var_)->SemAnalyze(venv, tenv, labelcount, errormsg);
  case Var::Kind::FIELD:
    return ((FieldVar*) var_)->SemAnalyze(venv, tenv, labelcount, errormsg);
  case Var::Kind::SUBSCRIPT:
    return ((SubscriptVar*) var_)->SemAnalyze(venv, tenv, labelcount, errormsg);
  };
}

type::Ty *NilExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return type::NilTy::Instance();
}

type::Ty *IntExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return type::IntTy::Instance();
}

type::Ty *StringExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return type::StringTy::Instance();
}

type::Ty *CallExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                              int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  env::EnvEntry * entry = venv->Look(func_);
  // function check
  if(!entry || entry->kind_ != env::EnvEntry::Kind::FUN) {
    errormsg->Error(pos_, "undefined function %s", func_->Name().c_str());
    return type::IntTy::Instance();
  }

//  const std::list<type::Ty *>*  formal_list = &((((env::FunEntry*) entry)->formals_)->GetList());
//  const std::list<Exp *>* arg_list = &(args_->GetList());
//
//  if(arg_list->size() != formal_list->size()) {
//    errormsg->Error(pos_, "num of args and formals are different in function %s", func_->Name().c_str());
//    return type::IntTy::Instance();
//  }
//
//  auto formal_iter = formal_list->begin();
//  auto arg_iter = arg_list->begin();
//
//  for (int i = 0; i < arg_list->size(); ++i) {
//    type::Ty* ty = (*arg_iter)->SemAnalyze(venv, tenv, labelcount, errormsg);
//    if (!ty->IsSameType(*formal_iter)) {
//      errormsg->Error(pos_, "para type mismatch");
//      return type::IntTy::Instance();
//    };
//    formal_iter ++;
//    arg_iter ++;
//  }
  if(entry && entry->kind_ == env::EnvEntry::FUN){
    type::TyList *formal = ((env::FunEntry*) entry)->formals_;
    const std::list<type::Ty *>* formalist = &(formal->GetList());
    const std::list<Exp *> * args = &(args_->GetList());

    auto formal_iter = formalist->begin();
    auto args_iter = args->begin();

    for(;formal_iter!=formalist->end()&&args_iter!=args->end();formal_iter++, args_iter++){
      type::Ty *ty = (*args_iter)->SemAnalyze(venv, tenv, labelcount, errormsg);
      if(!ty->IsSameType(*formal_iter)){
        errormsg->Error(pos_, "para type mismatch");
        return type::IntTy::Instance();
      }
    }

    if(formal_iter!=formalist->end()){
      errormsg->Error(pos_,"too few params in function %s", func_->Name().c_str());
      return type::IntTy::Instance();
    }

    if(args_iter!=args->end()){
      errormsg->Error(pos_,"too many params in function %s", func_->Name().c_str());
      return type::IntTy::Instance();
    }
    return ((env::FunEntry *)entry)->result_->ActualTy();
  }
  return type::IntTy::Instance();
}

type::Ty *OpExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                            int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty *left_ty = left_->SemAnalyze(venv, tenv, labelcount, errormsg);
  type::Ty *right_ty = right_->SemAnalyze(venv, tenv, labelcount, errormsg);
  switch (oper_) {
  case Oper::PLUS_OP: case Oper::MINUS_OP: case Oper::TIMES_OP: case Oper::DIVIDE_OP: case Oper::AND_OP :case Oper::OR_OP:
  {
    if (left_ty->kind_ != type::Ty::Kind::INT && left_ty->kind_ != type::Ty::Kind::NIL) errormsg->Error(left_->pos_, "integer required");
    if (right_ty->kind_ != type::Ty::Kind::INT && right_ty->kind_ != type::Ty::Kind::NIL) errormsg->Error(right_->pos_, "integer required");
    break;
  }

  case Oper::LT_OP: case Oper::LE_OP: case Oper::GT_OP: case Oper::GE_OP: case Oper::EQ_OP: case Oper::NEQ_OP:
  {
    if (!left_ty->IsSameType(right_ty) && left_ty->kind_ != type::Ty::Kind::VOID && right_ty->kind_ != type::Ty::Kind::NIL) {
      errormsg->Error(this->pos_, "same type required");
    };
    break;
  }
  };
  return type::IntTy::Instance();
}

type::Ty *RecordExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* ty = tenv->Look(typ_);

  if (!ty) {
    errormsg->Error(pos_, "undefined type %s", typ_->Name().c_str());
    return type::IntTy::Instance();
  };
  return ty;


  //////////////////////////

}

type::Ty *SeqExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  const std::list<Exp *> *explist = &(seq_->GetList());
  // TODO: check break ???
  type::Ty *ty;
  for(auto iter = explist->begin(); iter != explist->end(); iter++){
    ty = (*iter)->SemAnalyze(venv, tenv, labelcount, errormsg);
    // if(labelcount==0) errormsg->Error((*iter)->pos_, "break is not inside any loop");
    if((*iter)->kind_ == BREAK && !venv->inLoop())
      errormsg->Error((*iter)->pos_, "break is not inside any loop");
  }
  return ty;  // what is the seqexp ty?
}

type::Ty *AssignExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                                int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  // read only var can't be assigned
  if (typeid(*var_) == typeid(absyn::SimpleVar)) {
    env::EnvEntry* entry = venv->Look((static_cast<SimpleVar*>(var_))->sym_);
    env::VarEntry* var_entry = static_cast<env::VarEntry*>(entry);
    if (var_entry->readonly_) {
      errormsg->Error(pos_, "loop variable can't be assigned");
      return type::IntTy::Instance();
    };
  };

  type::Ty* var_ty = var_->SemAnalyze(venv, tenv, labelcount, errormsg);
  type::Ty* exp_ty = exp_->SemAnalyze(venv, tenv, labelcount, errormsg);

  if (!var_ty->IsSameType(exp_ty))
    errormsg->Error(pos_, "unmatched assign exp");

  return type::VoidTy::Instance();
}

type::Ty *IfExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                            int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  if (test_->SemAnalyze(venv, tenv, labelcount, errormsg)->kind_ != type::Ty::INT) {
    errormsg->Error(pos_, "integer required");
    return type::IntTy::Instance();
  };

  // TODO : why then and else should have the same type?
  type::Ty* ty = then_->SemAnalyze(venv, tenv, labelcount, errormsg);
  if (elsee_ == nullptr) {
    if (ty->kind_ != type::Ty::VOID) {
      errormsg->Error(pos_, "if-then exp's body must produce no value");
      return type::IntTy::Instance();
    };
    return type::VoidTy::Instance();
  } else {
    if (ty->IsSameType(elsee_->SemAnalyze(venv, tenv, labelcount, errormsg))) return ty;
    else {
      errormsg->Error(pos_, "then exp and else exp type mismatch");
      return type::VoidTy::Instance();
    };
  };
}

type::Ty *WhileExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* test_ty = test_->SemAnalyze(venv, tenv, labelcount, errormsg);
  if (test_ty->kind_ != type::Ty::Kind::INT)
    errormsg->Error(test_->pos_, "integer required");

  /* tolerate 'break' sentence */
  venv->BeginLoop();
  tenv->BeginLoop();

  if(body_){
    type::Ty *body = body_->SemAnalyze(venv,tenv,labelcount,errormsg);
    if(body->kind_!=type::Ty::VOID){
      errormsg->Error(body_->pos_, "while body must produce no value");
    }
  }

  venv->EndLoop();
  tenv->EndLoop();

  return type::VoidTy::Instance();


}

type::Ty *ForExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* lo_ty = lo_->SemAnalyze(venv, tenv, labelcount, errormsg);
  type::Ty* hi_ty = hi_->SemAnalyze(venv, tenv, labelcount, errormsg);

  if (lo_ty->kind_ != type::Ty::Kind::INT)
    errormsg->Error(lo_->pos_, "for exp's range type is not integer");
  if (hi_ty->kind_ != type::Ty::Kind::INT)
    errormsg->Error(hi_->pos_, "for exp's range type is not integer");

  // TODO : how to handle loop ??
  venv->BeginScope();
  tenv->BeginScope();

  venv->BeginLoop();
  tenv->BeginLoop();

  venv->Enter(var_, new env::VarEntry(type::IntTy::Instance(), true));
  type::Ty* body_ty = body_->SemAnalyze(venv, tenv, labelcount, errormsg);
  venv->EndScope();
  tenv->EndScope();

  venv->EndLoop();
  tenv->EndLoop();
  return type::VoidTy::Instance();

}

type::Ty *BreakExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return type::VoidTy::Instance();
}

type::Ty *LetExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  venv->BeginScope();
  tenv->BeginScope();
  const std::list<Dec *>* decslist = &decs_->GetList();

  // check type for each dec
  for (auto iter = decslist->begin(); iter != decslist->end(); iter++)
    (*iter)->SemAnalyze(venv, tenv, labelcount, errormsg);

  type::Ty* ty = body_->SemAnalyze(venv, tenv, labelcount, errormsg);

  venv->EndScope();
  tenv->EndScope();
  return ty;

}

type::Ty *ArrayExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                               int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  // check array type
  type::Ty* ty = tenv->Look(typ_)->ActualTy();
  if (!ty || ty->kind_ != type::Ty::ARRAY) {
    errormsg->Error(pos_, "undefined array type %s", typ_->Name().c_str());
    return type::VoidTy::Instance();
  };
  // size exp should be int type
  if (size_->SemAnalyze(venv, tenv, labelcount, errormsg)->kind_ != type::Ty::INT) {
    errormsg->Error(pos_, "undefined type %s", typ_->Name().c_str());
    return type::VoidTy::Instance();
  };
  // check init value type
  type::ArrayTy* array_ty = (type::ArrayTy*) ty;
  if (!init_->SemAnalyze(venv, tenv, labelcount, errormsg)->IsSameType(array_ty->ty_)) {
    errormsg->Error(pos_, "type mismatch");
    return type::VoidTy::Instance();
  };
  return array_ty;
}

type::Ty *VoidExp::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                              int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  return type::VoidTy::Instance();
}

void FunctionDec::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv,
                             int labelcount, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  // trans functionDec to funDecList
  const std::list<FunDec *>* fun_dec_list = &(functions_->GetList());
  // check there is no duplicate fundec
  for(auto it = fun_dec_list->begin(); it != fun_dec_list->end(); it++){
    auto next_it = it;
    if(next_it != fun_dec_list->end()){
      next_it++;
    }
    for(; next_it != fun_dec_list->end(); next_it++){
      if((*next_it)->name_->Name() == (*it)->name_->Name()){
        errormsg->Error((*it)->pos_, "two functions have the same name");
      }
    }
  }
  // check each fundec
  for(auto it = fun_dec_list->begin(); it != fun_dec_list->end(); it++){
    type::Ty* result;
    // check result
    if ((*it)->result_ != nullptr) {
      result = tenv->Look((*it)->result_);
      if (result == nullptr) {
        errormsg->Error(pos_, "FunctionDec undefined result type");
      }
    } else {
      // function is void
      result = type::VoidTy::Instance();
    }
    // check each param
    type::TyList* ans = new type::TyList();
    if((*it)->params_ == nullptr) {
      ans = nullptr;
    } else{
      const std::list<absyn::Field *> * param_list = &((*it)->params_->GetList());
      for (auto it_field = param_list->begin(); it_field != param_list->end(); it_field++) {
        type::Ty* ty = tenv->Look((*it_field)->typ_);
        if (ty == nullptr){
          errormsg->Error((*it_field)->pos_, "undefined field type %s in function %s", (*it_field)->typ_->Name().c_str(), (*it)->name_->Name().c_str());
        }
        ans->Append(ty->ActualTy());
      };
    }

    // add new function_dec to Venv
    venv->Enter((*it)->name_, new env::FunEntry(ans, result));
  }

  // check each body in venv
  for (auto it = fun_dec_list->begin(); it != fun_dec_list->end(); it++) {
    venv->BeginScope();
    const std::list<Field *>* field_list = &((*it)->params_->GetList());
    for (auto it_field = field_list->begin(); it_field != field_list->end(); it_field++) {
      type::Ty* ty = tenv->Look((*it_field)->typ_);
      if (!ty){
        errormsg->Error((*it_field)->pos_, "undefined type %s", (*it_field)->typ_->Name().c_str());
      }
      // add field to venv
      venv->Enter((*it_field)->name_, new env::VarEntry(ty));
    };
    type::Ty* body_ty = (*it)->body_->SemAnalyze(venv, tenv, labelcount, errormsg);
    type::Ty* dec_ty = ((env::FunEntry*) venv->Look((*it)->name_))->result_;
    //  check return value and declared return value
    if (!body_ty->IsSameType(dec_ty)) {
      if (dec_ty->kind_ == type::Ty::VOID) {
        errormsg->Error((*it)->body_->pos_, "procedure returns value");
      }
      else {
        errormsg->Error((*it)->body_->pos_, "return type mismatch");
      }
    };
    venv->EndScope();
  };


}

void VarDec::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv, int labelcount,
                        err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  if(typ_ == nullptr) { // simple var
    type::Ty* ty = init_->SemAnalyze(venv, tenv, labelcount, errormsg);
    if (ty->kind_ == type::Ty::NIL) {
      errormsg->Error(pos_, "init should not be nil without type specified");
    }
    venv->Enter(var_, new env::VarEntry(ty->ActualTy()));
  }
  else {
    type::Ty* ty = tenv->Look(typ_);
    if (ty == nullptr) {
      errormsg->Error(pos_, "undefined type %s", typ_->Name().c_str());
      return;
    };
    if (ty->IsSameType(init_->SemAnalyze(venv, tenv, labelcount, errormsg))) {
      venv->Enter(var_, new env::VarEntry(tenv->Look(typ_)));
    }
    else {
      errormsg->Error(pos_, "type mismatch");
    }
  }
}

void TypeDec::SemAnalyze(env::VEnvPtr venv, env::TEnvPtr tenv, int labelcount,
                         err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  // trans NameAndTyList to std::list
    const std::list<NameAndTy*> * name_and_ty_list = &(types_->GetList());
    // check there is no duplicate name_and_ty_dec
    for(auto iter=name_and_ty_list->begin(); iter != name_and_ty_list->end(); iter ++){
      if(tenv->Look((*iter)->name_)) {
        errormsg->Error(pos_, "two types have the same name");
      } else {
        tenv->Enter((*iter)->name_, new type::NameTy((*iter)->name_, nullptr));
      }
    }

    ////////

    for (auto it = name_and_ty_list->begin(); it != name_and_ty_list->end(); it++) {
      type::NameTy* name_ty = (type::NameTy*) tenv->Look((*it)->name_);
      name_ty->ty_ = (*it)->ty_->SemAnalyze(tenv, errormsg);
    };

    bool cycle = false;
    for (auto it = name_and_ty_list->begin(); it != name_and_ty_list->end(); it++) {
      type::Ty* ty = tenv->Look((*it)->name_);
      if (ty->kind_ == type::Ty::NAME) {
        type::Ty* ty_ty = ((type::NameTy*) ty)->ty_;
        while (ty_ty->kind_ == type::Ty::NAME) {
          type::NameTy* name_ty = (type::NameTy*) ty_ty;
          if (name_ty->sym_->Name() == (*it)->name_->Name()) {
            errormsg->Error(pos_, "illegal type cycle");
            cycle = true;
            break;
          };
          ty_ty = name_ty->ty_;
        };
      };
      if (cycle) break;
    };
    return;


}

type::Ty *NameTy::SemAnalyze(env::TEnvPtr tenv, err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* ty = tenv->Look(name_);
  if (!ty) {
    errormsg->Error(pos_, "undefined type %s", name_->Name().c_str());
    return type::VoidTy::Instance();
  };
  return new type::NameTy(name_, ty);
}

type::Ty *RecordTy::SemAnalyze(env::TEnvPtr tenv,
                               err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::FieldList* field_list;
  if(record_ == nullptr){
      field_list = nullptr;
      return new type::RecordTy(field_list);
  }

  field_list = new type::FieldList();

  const std::list<absyn::Field *> * list_field = &(record_->GetList());
  for (auto it = list_field->begin(); it != list_field->end(); it++) {
    type::Ty* ty = tenv->Look((*it)->typ_);
    if (ty == nullptr) {
      errormsg->Error((*it)->pos_, "undefined type %s", (*it)->typ_->Name().c_str());
    }
    field_list->Append(new type::Field((*it)->name_, ty));
  };
  return new type::RecordTy(field_list);
};



type::Ty *ArrayTy::SemAnalyze(env::TEnvPtr tenv,
                              err::ErrorMsg *errormsg) const {
  /* TODO: Put your lab4 code here */
  type::Ty* ty = tenv->Look(array_);
  if (!ty) {
    errormsg->Error(pos_, "undefined array type %s", array_->Name().c_str());
    return type::VoidTy::Instance();
  };
  return new type::ArrayTy(ty);
}

} // namespace absyn

namespace sem {

void ProgSem::SemAnalyze() {
  FillBaseVEnv();
  FillBaseTEnv();
  absyn_tree_->SemAnalyze(venv_.get(), tenv_.get(), errormsg_.get());
}

} // namespace tr
