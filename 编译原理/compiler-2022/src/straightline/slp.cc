#include "straightline/slp.h"
#include "slp.h"
#include <iostream>

namespace A {
int A::CompoundStm::MaxArgs() const {
  // TODO: put your code here (lab1).
  return max(stm1->MaxArgs(), stm2->MaxArgs());
}

Table *A::CompoundStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  // 先执行 stm1
  return stm2->Interp(stm1->Interp(t));
}

int A::AssignStm::MaxArgs() const {
  // TODO: put your code here (lab1).
  return exp->MaxArgs();
}

Table *A::AssignStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  // BUG! 图方便 调用了两次 exp->Interp(t), 由于此处的exp可能是 EseqExp，
  // 因此可能触发两次 print！！
  IntAndTable *intAndTable = exp->Interp(t);
  Table *table = intAndTable->t;
  return table->Update(this->id, intAndTable->i);
}

int A::PrintStm::MaxArgs() const {
  // TODO: put your code here (lab1).
  return exps->MaxArgs();
}

Table *A::PrintStm::Interp(Table *t) const {
  // TODO: put your code here (lab1).
  return exps->Interp(t)->t;
}

int A::IdExp::MaxArgs() const { return 1; }

IntAndTable *A::IdExp::Interp(Table *table) const {
  return new IntAndTable(table->Lookup(this->id), table);
}

int A::NumExp::MaxArgs() const { return 1; }

IntAndTable *A::NumExp::Interp(Table *table) const {
  return new IntAndTable(num, table);
}

int A::OpExp::MaxArgs() const { return 1; }

IntAndTable *A::OpExp::Interp(Table *table) const {
  //  printf("test ");
  switch (oper) {
  case PLUS:
    return new IntAndTable(this->left->Interp(table)->i +
                               this->right->Interp(table)->i,
                           this->right->Interp(table)->t);
  case MINUS:
    return new IntAndTable(this->left->Interp(table)->i -
                               this->right->Interp(table)->i,
                           this->right->Interp(table)->t);
  case TIMES:
    return new IntAndTable(this->left->Interp(table)->i *
                               this->right->Interp(table)->i,
                           this->right->Interp(table)->t);
  case DIV:
    return new IntAndTable(this->left->Interp(table)->i /
                               this->right->Interp(table)->i,
                           this->right->Interp(table)->t);
  }
  return nullptr;
}

int A::EseqExp::MaxArgs() const {
  return max(this->exp->MaxArgs(), this->stm->MaxArgs());
}

IntAndTable *A::EseqExp::Interp(Table *table) const {
  // 先执行stm
  return exp->Interp(stm->Interp(table));
}

int A::PairExpList::MaxArgs() const {
  return this->exp->MaxArgs() + this->tail->MaxArgs();
}

IntAndTable *A::PairExpList::Interp(Table *table) const {
  printf("%d ", exp->Interp(table)->i);
  return this->tail->Interp(this->exp->Interp(table)->t);
}

int A::LastExpList::MaxArgs() const { return 1; }

IntAndTable *A::LastExpList::Interp(Table *table) const {
  printf("%d\n", exp->Interp(table)->i);
  return exp->Interp(table);
}

int Table::Lookup(const std::string &key) const {
  if (id == key) {
    return value;
  } else if (tail != nullptr) {
    return tail->Lookup(key);
  } else {
    assert(false);
  }
}

Table *Table::Update(const std::string &key, int val) const {
  return new Table(key, val, this);
}
} // namespace A
