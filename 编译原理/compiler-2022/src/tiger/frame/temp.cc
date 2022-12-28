#include "tiger/frame/temp.h"

#include <cstdio>
#include <set>
#include <sstream>
#include "temp.h"

namespace temp {

LabelFactory LabelFactory::label_factory;
TempFactory TempFactory::temp_factory;

Label *LabelFactory::NewLabel() {
  char buf[100];
  sprintf(buf, "L%d", label_factory.label_id_++);
  return NamedLabel(std::string(buf));
}

/**
 * Get symbol of a label_. The label_ will be created only if it is not found.
 * @param s label_ string
 * @return symbol
 */
Label *LabelFactory::NamedLabel(std::string_view s) {
  return sym::Symbol::UniqueSymbol(s);
}

std::string LabelFactory::LabelString(Label *s) { return s->Name(); }

Temp *TempFactory::NewTemp() {
  Temp *p = new Temp(temp_factory.temp_id_++);
  std::stringstream stream;
  stream << 't';
  stream << p->num_;
  Map::Name()->Enter(p, new std::string(stream.str()));

  return p;
}

int Temp::Int() const { return num_; }


bool TempList::Contain(Temp *t) const {
  for (auto temp : temp_list_) {
    if (temp == t)
      return true;
  }
  return false;
}

bool TempList::IsSame(const TempList *tl) const {
  for (auto temp : tl->GetList()) {
    if (!Contain(temp)) {
      return false;
    }
  }
  for (auto temp : temp_list_) {
    if (!tl->Contain(temp)) {
      return false;
    }
  }
  return true;
}

bool TempList::Equal(temp::TempList *another) const {
  if (temp_list_.size() != another->temp_list_.size()) {
    return false;
  }
  for (auto t : temp_list_) {
    if (!another->Contain(t)) {
      return false;
    }
  }
  // 避免重复再比较一次
  for (auto t : another->GetList()) {
    if (!Contain(t)) {
      return false;
    }
  }
  return true;
}

TempList *TempList::Diff(TempList *tl) const {
  auto res = new TempList();
  for (auto temp : this->temp_list_) {
    if (!tl->Contain(temp)) {
      res->Append(temp);
    }
  }
  return res;
}

void TempList::Replace(temp::Temp *old_temp, temp::Temp *new_temp) {
  auto new_temp_list = new temp::TempList();
  for(const auto reg : this->temp_list_){
    if(reg == old_temp){
      new_temp_list->Append(new_temp);
    } else{
      new_temp_list->Append(reg);
    }
  }
  this->temp_list_ = new_temp_list->GetList();
}
TempList *TempList::Union(const TempList *tl) const {
  TempList *res = new TempList(this->temp_list_);
  // if(tl && !tl->temp_list_.empty()){
  //   tl->temp_list_.insert(tl->temp_list_.end(), temp_list_.begin(),
  //   temp_list_.end());
  // }
  for (auto temp : tl->GetList()) {
    if (!res->Contain(temp)) {
      res->Append(temp);
    }
  }
  return res;
}

Map *Map::Empty() { return new Map(); }

Map *Map::Name() {
  static Map *m = nullptr;
  if (!m)
    m = Empty();
  return m;
}

Map *Map::LayerMap(Map *over, Map *under) {
  if (over == nullptr)
    return under;
  else
    return new Map(over->tab_, LayerMap(over->under_, under));
}

void Map::Enter(Temp *t, std::string *s) {
  assert(tab_);
  tab_->Enter(t, s);
}

std::string *Map::Look(Temp *t) {
  std::string *s;
  assert(tab_);
  s = tab_->Look(t);
  if (s)
    return s;
  else if (under_)
    return under_->Look(t);
  else
    return nullptr;
}

void Map::DumpMap(FILE *out) {
  tab_->Dump([out](temp::Temp *t, std::string *r) {
    fprintf(out, "t%d -> %s\n", t->Int(), r->data());
  });
  if (under_) {
    fprintf(out, "---------\n");
    under_->DumpMap(out);
  }
}

} // namespace temp