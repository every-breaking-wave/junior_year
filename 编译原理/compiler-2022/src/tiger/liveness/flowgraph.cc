
#include "tiger/liveness/flowgraph.h"

extern frame::RegManager *reg_manager;

namespace fg {

void FlowGraphFactory::AssemFlowGraph() {
  /* TODO: Put your lab6 code here */
  std::list<assem::Instr *> il = instr_list_->GetList();
  tab::Table<assem::Instr, FNode> instr_to_node;
  FNode *prev = nullptr;
  for(auto instr : il){
    FNode *cur = flowgraph_->NewNode(instr);
    instr_to_node.Enter(instr, cur);
    if(prev){
      flowgraph_->AddEdge(prev, cur);
    }
    if(typeid(*instr) == typeid(assem::LabelInstr)){
      if(((assem::LabelInstr *)instr)->label_ != nullptr){
        label_map_->Enter(((assem::LabelInstr *)instr)->label_, cur);
      }
    }
    if(typeid(*instr) == typeid(assem::OperInstr)){
      if(((assem::OperInstr*)instr)->assem_.find("jmp")==0){
        prev = nullptr;
        continue;
      }
    }
    prev = cur;
  }
  
  FNodeListPtr nodes = flowgraph_->Nodes();
  std::list<FNodePtr> node_list = nodes->GetList();
  for(auto node: node_list){
    if(typeid(*(node->NodeInfo())) == typeid(assem::OperInstr)){
      assem::Targets *jumps = ((assem::OperInstr *)(node->NodeInfo()))->jumps_;
      if(!jumps){
        continue;
      }
      int size = jumps->labels_->size();
      for(int i = 0; i < size; i++){
        FNode *lable_node = label_map_->Look(jumps->labels_->at(i));
        flowgraph_->AddEdge(instr_to_node.Look(node->NodeInfo()), lable_node);
      }
    }
  }
}



} // namespace fg


namespace assem {

temp::TempList *LabelInstr::Def() const {
  /* TODO: Put your lab6 code here */
  return new temp::TempList();
}

temp::TempList *MoveInstr::Def() const {
  /* TODO: Put your lab6 code here */
  return dst_ ? dst_ : new temp::TempList();
}

temp::TempList *OperInstr::Def() const {
  /* TODO: Put your lab6 code here */
  return dst_ ? dst_ : new temp::TempList();
}

temp::TempList *LabelInstr::Use() const {
  /* TODO: Put your lab6 code here */
  return new temp::TempList();
}

temp::TempList *MoveInstr::Use() const {
  /* TODO: Put your lab6 code here */
  return src_ ? src_ : new temp::TempList();
}

temp::TempList *OperInstr::Use() const {
  /* TODO: Put your lab6 code here */
  return src_ ? src_ : new temp::TempList();
}
} // namespace assem

