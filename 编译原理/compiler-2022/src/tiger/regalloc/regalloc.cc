#include "tiger/regalloc/regalloc.h"

#include "regalloc.h"
#include "tiger/output/logger.h"

extern frame::RegManager *reg_manager;
#define MAX_COLOR_NUM reg_manager->Registers()->GetList().size()

namespace ra {
Result::~Result() {
  delete il_;
  delete coloring_;
}

RegAllocator::RegAllocator(frame::Frame *frame,
                           std::unique_ptr<cg::AssemInstr> assem_instr)
    : frame_(frame), assem_instr_(std::move(assem_instr)) {
  // init these
  precolored = new live::INodeList();
  initial = new live::INodeList();
  // init node list
  simplyWorklist = new live::INodeList();
  freezeWorklist = new live::INodeList();
  spillWorklist = new live::INodeList();
  spilledNodes = new live::INodeList();
  coalescedNodes = new live::INodeList();
  coloredNodes = new live::INodeList();
  stack_list = new live::INodeList();
  // init move list
  coalescedMoves = new live::MoveList();
  constrainedMoves = new live::MoveList();
  frozenMoves = new live::MoveList();
  worklistMoves = new live::MoveList();
  activeMoves = new live::MoveList();
}

void RegAllocator::RegAlloc() {

  InitGraph();
  Build();
  MakeWorkList();

loop:
  if (!simplyWorklist->GetList().empty()) {
    Simplify();
    goto judge;
  }
  if (!worklistMoves->GetList().empty()) {
    Coalesce();
    goto judge;
  }
  if (!freezeWorklist->GetList().empty()) {
    Freeze();
    goto judge;
  }
  if (!spillWorklist->GetList().empty()) {
    SelectSpill();
    goto judge;
  }

judge:
  if (!simplyWorklist->GetList().empty() || !worklistMoves->GetList().empty() ||
      !freezeWorklist->GetList().empty() || !spillWorklist->GetList().empty()) {
    goto loop;
  }

  AssignColor();
  if (!spilledNodes->GetList().empty()) {
    RewriteProgram();
    Reset();
    RegAlloc();
  } else {
    temp::Map *map = temp::Map::Empty();
    // with wrong!
    for (auto tmp : color) {
      std::string *name = temp::Map::Name()->Look(tmp.second);
      map->Enter(tmp.first, name);
    }
    for(auto tmp : reg_manager->Registers()->GetList()){
      map->Enter(tmp, reg_manager->temp_map_->Look(tmp));
    }
    map->Enter(reg_manager->StackPointer(), new std::string("%rsp"));

    simplifyInstrList();
    result =
        std::make_unique<ra::Result>(map, assem_instr_.get()->GetInstrList());
  }
}

std::unique_ptr<ra::Result> RegAllocator::TransferResult() {
  return std::move(result);
}

bool RegAllocator::Contain(live::INodePtr node, live::INodeListPtr list) {
  for (auto tmp : list->GetList()) {
    if (tmp->NodeInfo() == node->NodeInfo()) {
      return true;
    }
  }
  return false;
}

live::INodeListPtr RegAllocator::Union(live::INodeListPtr left,
                                       live::INodeListPtr right) {
  live::INodeListPtr result = new live::INodeList();
  for (auto l : left->GetList()) {
    if (!Contain(l, result)) {
      result->Append(l);
    }
  }
  for (auto r : right->GetList()) {
    if (!Contain(r, result)) {
      result->Append(r);
    }
  }

  return result;
}

live::INodeListPtr RegAllocator::Sub(live::INodeListPtr left,
                                     live::INodeListPtr right) {
  live::INodeListPtr result = new live::INodeList();
  for (auto l : left->GetList()) {
    if (!Contain(l, right)) {
      result->Append(l);
    }
  }

  return result;
}

live::INodeListPtr RegAllocator::Intersect(live::INodeListPtr left,
                                           live::INodeListPtr right) {
  live::INodeListPtr result = new live::INodeList();
  for (auto l : left->GetList()) {
    if (Contain(l, right)) {
      result->Append(l);
    }
  }

  return result;
}

bool RegAllocator::Contain(std::pair<live::INodePtr, live::INodePtr> node,
                           live::MoveList *list) {
  for (auto tmp : list->GetList()) {
    if (tmp == node) {
      return true;
    }
  }

  return false;
}

live::MoveList *RegAllocator::Union(live::MoveList *left,
                                    live::MoveList *right) {
  live::MoveList *result = new live::MoveList();
  for (auto l : left->GetList()) {
    if (!Contain(l, result)) {
      result->Append(l.first, l.second);
    }
  }
  for (auto r : right->GetList()) {
    if (!Contain(r, result)) {
      result->Append(r.first, r.second);
    }
  }

  return result;
}

live::MoveList *RegAllocator::Intersect(live::MoveList *left,
                                        live::MoveList *right) {
  live::MoveList *result = new live::MoveList();
  for (auto l : left->GetList()) {
    if (Contain(l, right)) {
      result->Append(l.first, l.second);
    }
  }

  return result;
}

void RegAllocator::Build() {
  for (const auto &move : liveGraph->GetLiveGraph().moves->GetList()) {
    live::INodePtr src = move.first;
    live::INodePtr dst = move.second;
    live::MoveList *src_list = moveList[src];
    if (!src_list) {
      src_list = new live::MoveList();
    }
    src_list->Append(src, dst);
    moveList[src] = src_list;
    live::MoveList *dst_list = moveList[dst];
    if (!dst_list) {
      dst_list = new live::MoveList();
    }
    dst_list->Append(src, dst);
    moveList[dst] = dst_list;
  }
  worklistMoves = liveGraph->GetLiveGraph().moves;
  for (auto node : liveGraph->GetLiveGraph().interf_graph->Nodes()->GetList()) {
    for (auto adj : node->Adj()->GetList()) {
      AddEdge(node, adj);
    }
  }

  // init precolored
  precolored->Clear();
  std::list<temp::Temp *> regs = reg_manager->Registers()->GetList();
  for (auto node : liveGraph->GetLiveGraph().interf_graph->Nodes()->GetList()) {
    for (auto reg : regs) {
      if (node->NodeInfo() == reg) {
        precolored->Append(node);
        color[node->NodeInfo()] = node->NodeInfo();
        break;
      }
    }
  }
  // for(auto reg : regs){
  //   color[reg] = reg;
  // }

  for (auto node : liveGraph->GetLiveGraph().interf_graph->Nodes()->GetList()) {
    bool flag = true;
    std::list<live::INodePtr> precolored_list = precolored->GetList();
    auto tmp = precolored_list.begin();
    for (; tmp != precolored_list.end(); tmp++) {
      if (*tmp == node) {
        color[node->NodeInfo()] = (*tmp)->NodeInfo();
        flag = false;
        break;
      }
    }
    if (flag) {
      initial->Append(node);
    }
    alias[node] = node;
  }
}

void RegAllocator::Reset() {
  precolored->Clear();
  initial->Clear();

  simplyWorklist->Clear();
  freezeWorklist->Clear();
  spillWorklist->Clear();
  spilledNodes->Clear();
  coalescedNodes->Clear();
  coloredNodes->Clear();
  stack_list->Clear();

  coalescedMoves = new live::MoveList();
  constrainedMoves = new live::MoveList();
  frozenMoves = new live::MoveList();
  worklistMoves = new live::MoveList();
  activeMoves = new live::MoveList();

  adjSet.clear();
  adjList.clear();
  degree.clear();
  moveList.clear();
  alias.clear();
  color.clear();
}

void RegAllocator::InitGraph() {
  fg::FlowGraphFactory *flowGraph =
      new fg::FlowGraphFactory(assem_instr_.get()->GetInstrList());
  flowGraph->AssemFlowGraph();
  liveGraph = new live::LiveGraphFactory(flowGraph->GetFlowGraph());
  liveGraph->Liveness();
  for (auto node : liveGraph->GetLiveGraph().interf_graph->Nodes()->GetList()) {
    adjList[node] = new live::INodeList();
    degree[node] = 0;
  }
}

void RegAllocator::AddEdge(live::INodePtr u, live::INodePtr v) {
  std::pair<live::INodePtr, live::INodePtr> edge = std::make_pair(u, v);
  if (adjSet.find(edge) == adjSet.end() && u != v) {
    adjSet.insert(std::make_pair(u, v));
    adjSet.insert(std::make_pair(v, u));
    if (!precolored->Contain(v)) {
      adjList[v]->Append(u);
      degree[v]++;
    }
    if (!precolored->Contain(u)) {
      adjList[u]->Append(v);
      degree[u]++;
    }
  }
}

void RegAllocator::MakeWorkList() {
  for (auto node : initial->GetList()) {
    if (degree[node] >= MAX_COLOR_NUM) {
      spillWorklist->Append(node);
    } else if (IsMoveRelated(node)) {
      freezeWorklist->Append(node);
    } else {
      simplyWorklist->Append(node);
    }
  }
  initial->Clear();
}

live::INodeListPtr RegAllocator::Adjacent(live::INodePtr node) {
  // adjacentList[n] - (selectStack ∪ coalescedNodes)
  return adjList[node]->Diff(stack_list->Union(coalescedNodes));
}

live::MoveList *RegAllocator::NodeMoves(live::INodePtr node) {
  if (!moveList[node]) {
    return new live::MoveList();
  }
  return moveList[node]->Intersect(Union(activeMoves, worklistMoves));
}

bool RegAllocator::IsMoveRelated(live::INodePtr node) {
  return !NodeMoves(node)->GetList().empty();
}

void RegAllocator::Simplify() {
  if (simplyWorklist->GetList().empty()) {
    return;
  }
  live::INodePtr min_degree_node = simplyWorklist->GetList().front();
  for (auto &node : spillWorklist->GetList()) {
    if (stack_list->Contain(node)) {
      continue;
    }
    if (degree.find(node)->second < degree.find(min_degree_node)->second) {
      min_degree_node = node;
    }
    if (degree.find(min_degree_node)->second <
        MAX_COLOR_NUM) { // find the approprate node
      break;
    }
  }

  simplyWorklist->DeleteNode(min_degree_node);
  stack_list->Append(min_degree_node);
  // update degree
  for (auto node : Adjacent(min_degree_node)->GetList()) {
    DecrementDegree(node);
  }
}

void RegAllocator::DecrementDegree(live::INodePtr node) {
  if (--degree[node] == MAX_COLOR_NUM - 1) {
    auto interf_list = Adjacent(node);
    interf_list->Append(node);
    EnableMoves(interf_list);
    spillWorklist->DeleteNode(node);
    if (IsMoveRelated(node)) {
      freezeWorklist->Append(node);
    } else {
      simplyWorklist->Append(node);
    }
  }
}

void RegAllocator::EnableMoves(live::INodeListPtr nodeList) {
  for (auto n : nodeList->GetList()) {
    for (auto m : NodeMoves(n)->GetList()) {
      if (activeMoves->Contain(m.first, m.second)) {
        activeMoves->Delete(m.first, m.second);
        worklistMoves->Append(m.first, m.second);
      }
    }
  }
}

void RegAllocator::Coalesce() {
  if (worklistMoves->GetList().empty()) {
    return;
  }

  auto move = worklistMoves->GetList().front();
  live::INodePtr u, v;
  if (precolored->Contain(move.second)) { // if des is precolored
    u = GetAlias(move.second);
    v = GetAlias(move.first);
  } else {
    u = GetAlias(move.first);
    v = GetAlias(move.second);
  }
  worklistMoves->Delete(move.first, move.second);

  if (u == v) {
    if (!coalescedMoves->Contain(move.first, move.second)) {
      coalescedMoves->Append(move.first, move.second);
    }
    if (IsSimplifyNode(u)) {
      FromFreezeToSimply(u);
    }
  } else if (precolored->Contain(v) ||
             adjSet.find(std::make_pair(u, v)) !=
                 adjSet.end()) { // useless move related
    if (!constrainedMoves->Contain(move.first, move.second)) {
      constrainedMoves->Append(move.first, move.second);
    }
    constrainedMoves->Append(move.first, move.second);
    if (IsSimplifyNode(u)) {
      FromFreezeToSimply(u);
    }
    if (IsSimplifyNode(v)) {
      FromFreezeToSimply(v);
    }
  } else if (GeogreOrBriggs(u, v)) { // satisfy combine condition, do coalesce
    coalescedMoves->Append(move.first, move.second);
    Combine(u, v);
    if (IsSimplifyNode(u)) {
      FromFreezeToSimply(u);
    }
  } else if (!activeMoves->Contain(move.first, move.second)) {
    activeMoves->Append(move.first, move.second);
  }
}

live::INodePtr RegAllocator::GetAlias(live::INodePtr node) {
  return coalescedNodes->Contain(node) ? GetAlias(alias[node]) : node;
}

void RegAllocator::Combine(live::INodePtr u, live::INodePtr v) {
  if (freezeWorklist->Contain(v)) {
    freezeWorklist->DeleteNode(v);
  } else {
    spillWorklist->DeleteNode(v);
  }
  coalescedNodes->Append(v);
  alias[v] = u;
  moveList[u] = moveList[u]->Union(moveList[v]);
  live::INodeListPtr v_list = new live::INodeList();
  v_list->Append(v);
  EnableMoves(v_list);
  for (auto t : Adjacent(v)->GetList()) {
    AddEdge(t, u);
    AddEdge(u, t);
    DecrementDegree(t);
  }
  if (degree[u] >= MAX_COLOR_NUM && freezeWorklist->Contain(u)) {
    FromFreezeToSpill(u);
  }
}

void RegAllocator::Freeze() {
  if (freezeWorklist->GetList().empty()) {
    return;
  }
  auto u = freezeWorklist->GetList().front();
  FromFreezeToSimply(u);
  FreezeMoves(u);
}

void RegAllocator::FreezeMoves(live::INodePtr u) {
  for (auto m : NodeMoves(u)->GetList()) {
    live::INodePtr x = m.first;
    live::INodePtr y = m.second;
    live::INodePtr v = nullptr;
    if (GetAlias(y) == GetAlias(u)) {
      v = GetAlias(x);
    } else {
      v = GetAlias(y);
    }
    activeMoves->Delete(m.first, m.second);
    if (!frozenMoves->Contain(m.first, m.second)) {
      frozenMoves->Append(m.first, m.second);
    }
    if (NodeMoves(v)->GetList().empty() && degree[v] < MAX_COLOR_NUM) {
      FromFreezeToSimply(v);
    }
  }
}

void RegAllocator::SelectSpill() {
  // find max degree node to spill
  live::INodePtr spill_node = *(spillWorklist->GetList().begin());
  int max_degree = degree[spill_node];
  for (auto node : spillWorklist->GetList()) {
    // don't spill new temp reg in rewrite program
    if (notSpill.find(node->NodeInfo()) != notSpill.end()) {
      continue;
    }
    if (!precolored->Contain(node)) {
      int d = degree[node];
      if (d >= max_degree) {
        spill_node = node;
        max_degree = d;
      }
    }
  }
  FromSpillToSimply(spill_node);
  FreezeMoves(spill_node);
}

void RegAllocator::AssignColor() {
  spilledNodes->Clear();
  auto list = stack_list->GetList();
  auto n = list.end();
  std::vector<temp::Temp *> available_colors;
  while (!stack_list->isEmpty()) {
    auto node = stack_list->GetList().back();
    stack_list->DeleteNode(node);
    available_colors.clear();
    for (const auto &reg : reg_manager->Registers()->GetList()) {
      available_colors.emplace_back(reg);
    }
    // delete used colors by adjacent node
    for (auto n : adjList[node]->GetList()) {
      auto alias_node = GetAlias(n);
      if ((coloredNodes->Contain(alias_node)) ||
          precolored->Contain(alias_node)) {
        auto alias_node_color = color.find(alias_node->NodeInfo());
        if (alias_node_color == color.end()) {
          continue;
        }
        for (auto del_color = available_colors.begin();
             del_color != available_colors.end(); del_color++) {
          if (*del_color == (*alias_node_color).second) {
            available_colors.erase(del_color);
            break;
          }
        }
      }
    }
    if (available_colors.empty()) { // all colors are used, spill this node
      spilledNodes->Append(node);
    } else { // select one color for the node
      coloredNodes->Append(node);
      color.insert(
          std::make_pair(node->NodeInfo(), *(available_colors.begin())));
    }
  }
  // assign color for its coalesced nodes
  for (auto n : coalescedNodes->GetList()) {
    color[n->NodeInfo()] = color[GetAlias(n)->NodeInfo()];
  }
}

void RegAllocator::RewriteProgram() {
  assem::InstrList *new_instrList = new assem::InstrList();
  std::vector<int> offset_vec;
  for (auto node : spilledNodes->GetList()) {
    auto access = frame_->allocLocal(true);
    offset_vec.push_back(frame_->offset);
    // For GC
    if (node->NodeInfo()->store_pointer_) {
      access->SetStorePointer();
    }
  }
  assem::InstrList *instr_list = assem_instr_.get()->GetInstrList();
  for (auto il : instr_list->GetList()) {
    int i = 0;
    assem::InstrList *temp_instrList = new assem::InstrList();
    std::vector<temp::Temp *> node_temp_vec;
    for (auto node : spilledNodes->GetList()) {
      temp::TempList *use = il->Use();
      if (live::Contain(node->NodeInfo(), use)) {
        temp::Temp *t = temp::TempFactory::NewTemp();
        if (node->NodeInfo()->store_pointer_) {
          t->store_pointer_ = true;
        }
        node_temp_vec.push_back(t);
        notSpill.insert(t);
        std::string instr = std::string("movq (") + frame_->GetLabel() +
                            std::string("_framesize-") +
                            std::to_string(-(offset_vec[i])) +
                            std::string(")(`s0), `d0");
        //  dst : new temp reg, src : frame_ptr - offset, insert before il
        new_instrList->Append(new assem::OperInstr(
            instr, new temp::TempList(t),
            new temp::TempList({reg_manager->StackPointer()}), nullptr));

        temp::TempList *src_list_new = new temp::TempList();
        if (typeid(*il) == typeid(assem::MoveInstr)) {
          temp::TempList *src_list = ((assem::MoveInstr *)il)->src_;
          src_list->Replace(node->NodeInfo(), t);
        } else if (typeid(*il) == typeid(assem::OperInstr)) {
          temp::TempList *src_list = ((assem::OperInstr *)il)->src_;
          src_list->Replace(node->NodeInfo(), t);
        }
      } else {
        node_temp_vec.push_back(nullptr);
      }
      i++;
    }

    i = 0;
    for (auto node : spilledNodes->GetList()) {
      temp::TempList *def = il->Def();
      if (live::Contain(node->NodeInfo(), def)) {
        temp::Temp *t = node_temp_vec[i];
        if (!t) {
          t = temp::TempFactory::NewTemp();
          notSpill.insert(t);
        }
        if (node->NodeInfo()->store_pointer_) {
          t->store_pointer_ = true;
        }
        std::string instr = std::string("movq `s0, (") + frame_->GetLabel() +
                            std::string("_framesize-") +
                            std::to_string(-(offset_vec[i])) +
                            std::string(")(`d0)");
        temp_instrList->Append(new assem::OperInstr(
            instr, new temp::TempList(reg_manager->StackPointer()),
            new temp::TempList(t), nullptr));

        temp::TempList *dst_list_new = new temp::TempList();
        if (typeid(*(il)) == typeid(assem::MoveInstr)) {
          temp::TempList *dst_list = ((assem::MoveInstr *)(il))->dst_;
          dst_list->Replace(node->NodeInfo(), t);
        } else if (typeid(*(il)) == typeid(assem::OperInstr)) {
          temp::TempList *dst_list = ((assem::OperInstr *)(il))->dst_;
          dst_list->Replace(node->NodeInfo(), t);
        }
      }
      i++;
    }

    // insert il
    new_instrList->Append(il);

    // insert after il
    for (auto instr : temp_instrList->GetList()) {
      new_instrList->Append(instr);
    }
  }

  assem_instr_ = std::make_unique<cg::AssemInstr>(new_instrList);
  // spilledNodes->Clear();
  // // and initial??
  // coloredNodes->Clear();
  // coalescedNodes->Clear();
}

void RegAllocator::simplifyInstrList() {
  assem::InstrList *instr_list = assem_instr_.get()->GetInstrList();
  assem::InstrList *instr_list_new = new assem::InstrList();
    for(auto reg : reg_manager->Registers()->GetList()){
    color[reg] = reg;
  }
  for (const auto &il : instr_list->GetList()) {
    if (typeid(*il) == typeid(assem::MoveInstr)) {
      // if (*color.find(*il->Use()->GetList().begin()) ==
      //     *color.find(*il->Def()->GetList().begin())) {
      //   continue;
      // }
      temp::Temp *src = static_cast<assem::MoveInstr*>(il)->src_->GetList().front();
      temp::Temp *dst = static_cast<assem::MoveInstr*>(il)->dst_->GetList().front();
       if(color[src] == color[dst]){
        continue;
       }
    }
    instr_list_new->Append(il);
  }
  assem_instr_ = std::make_unique<cg::AssemInstr>(instr_list_new);
}

bool RegAllocator::IsSimplifyNode(live::INodePtr node) {
  return !precolored->Contain(node) && !IsMoveRelated(node) &&
         degree[node] < MAX_COLOR_NUM;
}

bool RegAllocator::GeogreOrBriggs(live::INodePtr u, live::INodePtr v) {
  bool flag = true;
  // judge Geogre condition
  auto interf_list = Adjacent(v);
  // return true if all its interf node are low degree node or has conflicts
  // with the other node
  for (auto interf_node : interf_list->GetList()) {
    if (precolored->Contain(interf_node) &&
        degree[interf_node] >= MAX_COLOR_NUM &&
        adjSet.find(std::make_pair(interf_node, u)) == adjSet.end()) {
      flag = false;
      break;
    }
  }
  // judge Briggs conditon
  auto coalesceNodeList = Adjacent(u)->Union(Adjacent(v));
  int new_degree = 0;
  for (auto node : coalesceNodeList->GetList()) {
    if (precolored->Contain(node) || degree[node] >= MAX_COLOR_NUM) {
      new_degree++;
    }
  }
  // return true if there would not be a high degree node after coalescing
  if (new_degree >= MAX_COLOR_NUM || precolored->Contain(u)) {
    flag = false;
  }
  return flag;
}

void RegAllocator::FromFreezeToSimply(live::INodePtr node) {
  freezeWorklist->DeleteNode(node);
  simplyWorklist->Append(node);
}

void RegAllocator::FromFreezeToSpill(live::INodePtr node) {
  freezeWorklist->DeleteNode(node);
  spillWorklist->Append(node);
}

void RegAllocator::FromSpillToSimply(live::INodePtr node) {
  spillWorklist->DeleteNode(node);
  simplyWorklist->Append(node);
}

} // namespace ra