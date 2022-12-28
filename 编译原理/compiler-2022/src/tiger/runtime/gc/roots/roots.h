#ifndef TIGER_RUNTIME_GC_ROOTS_H
#define TIGER_RUNTIME_GC_ROOTS_H

#include "tiger/liveness/liveness.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <vector>
#define DEBUG
namespace gc {

const std::string GC_ROOTS = "GLOBAL_GC_ROOTS";

/* 给PointerMap输出提供的结构 */
struct PointerMap {
  std::string label; // 本pointerMap的label, 为"L"+returnAddressLabel
  std::string returnAddressLabel;
  std::string nextPointerMapLable;
  std::string frameSize;
  std::string isMain = "0";
  std::vector<std::string> offsets;
  std::string endLabel = "-1";
};

class Roots {
public:
  Roots(assem::InstrList *il_, frame::Frame *frame_, fg::FGraphPtr flowgraph,
        std::vector<int> escapes_, temp::Map *color_)
      : escapes(escapes_), flowgraph_(flowgraph), color(color_), il(il_),
        frame(frame_) {}
  ~Roots() = default;

  /* 生成.data段的pointermap */
  std::vector<PointerMap> GetPointerMaps() {
    GenerateAddressLiveMap();
    GenerateTempLiveMap();
    BuildValidPointerMap();
    RewriteProgram();

    std::vector<PointerMap> pointerMaps;
    bool isMain = (frame->GetLabel() == "tigermain");
    for (auto pair : pointer_address_map) {
      std::cout << "offset size " << pair.second.size() << std::endl;
    }
    std::cout << "pointer size " << pointer_address_map.size() << std::endl;
    for (const auto &pair : pointer_address_map) {
      PointerMap newMap;
      newMap.frameSize = frame->GetLabel() + "_framesize";
      auto label_name = static_cast<assem::LabelInstr *>(pair.first)->label_;
      newMap.returnAddressLabel =
          static_cast<assem::LabelInstr *>(pair.first)->label_->Name();
      newMap.label = "L" + newMap.returnAddressLabel;
      newMap.nextPointerMapLable = "0";
      if (isMain){
        newMap.isMain = "1";
      }

      newMap.offsets = std::vector<std::string>();
      for (const auto &offset : pair.second)
        newMap.offsets.emplace_back(std::to_string(offset));

      pointerMaps.emplace_back(newMap);
    }

    for (int i = 0; i < (int)pointerMaps.size() - 1; i++){
      pointerMaps[i].nextPointerMapLable = pointerMaps[i + 1].label;
    }

    return pointerMaps;
  }

  assem::InstrList *GetInstrList() { return il; }

private:
  assem::InstrList *il;
  frame::Frame *frame;
  fg::FGraphPtr flowgraph_;
  std::vector<int> escapes;
  temp::Map *color;
  std::map<fg::FNodePtr, temp::TempList *> temp_in_;
  std::map<fg::FNodePtr, std::vector<int>> address_in_;
  std::map<fg::FNodePtr, std::vector<int>> address_out_;
  std::map<assem::Instr *, std::vector<int>> pointer_address_map;
  std::map<assem::Instr *, std::vector<std::string>> pointer_temp_map;

  std::vector<int> GetOffset(std::string mem_exp) {
    bool negative = mem_exp.find('-') != mem_exp.npos;
    int OffsetStart = negative ? mem_exp.find_first_of('-') + 1
                               : OffsetStart = mem_exp.find_first_of('+') + 1;
    int OffsetEnd = mem_exp.find_first_of(')');
    std::string offset = mem_exp.substr(OffsetStart, OffsetEnd - OffsetStart);
    return negative ? std::vector<int>(1, -std::stoi(offset))
                    : std::vector<int>(1, std::stoi(offset));
  }

  std::vector<int> AddressDef(assem::Instr *ins) { // dst
    if (typeid(*ins) == typeid(assem::OperInstr)) {
      std::string ass = static_cast<assem::OperInstr *>(ins)->assem_;
      if (ass.find("movq") != ass.npos && ass.find("_framesize") != ass.npos){
        int dstStart = ass.find_first_of(',') + 2;
        std::string dst = ass.substr(dstStart);
        if (dst[0] == '(') {
          return GetOffset(dst);
        }
      }
    }
    return std::vector<int>();
  }

  std::vector<int> AddressUse(assem::Instr *ins) { // src
    std::vector<int> addressUse;
    if (typeid(*ins) == typeid(assem::OperInstr)) {
      std::string ass = static_cast<assem::OperInstr *>(ins)->assem_;
      if (ass == "") {
        return escapes; // The life of the escape variable reaches the end of the function
      }
      if (ass.find("movq") != ass.npos && ass.find("_framesize") != ass.npos){
        int srcStart = ass.find_first_of(' ') + 1;
        int srcEnd = ass.find_first_of(',');
        std::string src = ass.substr(srcStart, srcEnd - srcStart);
        if (src[0] == '(') {
          addressUse = GetOffset(src);
        }
      }
    }
    return addressUse;
  }

  /* Active frame address */
  void GenerateAddressLiveMap() {
    std::list<fg::FNodePtr> flowgrapg_nodes = flowgraph_->Nodes()->GetList();
    std::map<fg::FNodePtr, std::vector<int>> pre_in, pre_out;

    for (const auto &node_ : flowgrapg_nodes) {
      address_in_[node_] = std::vector<int>();
      address_out_[node_] = std::vector<int>();
    }
    bool changed = true;
    while(changed) {
      pre_in = address_in_;
      pre_out = address_out_;
      changed = false;
      for (const auto& node_ : flowgrapg_nodes) {
        assem::Instr *node_ins = node_->NodeInfo();
        std::vector<int> use_list = AddressUse(node_ins);
        std::vector<int> def_list = AddressDef(node_ins);

        address_out_[node_] = std::vector<int>();
        std::list<fg::FNodePtr> succ = node_->Succ()->GetList();
        for (fg::FNodePtr succ_node : succ)
          address_out_[node_] =
              VectorUnion(address_out_[node_], address_in_[succ_node]);
        address_in_[node_] = VectorUnion(
            VectorDifference(address_out_[node_], def_list), use_list);
        if (address_out_[node_] != pre_out[node_] ||
            address_in_[node_] != pre_in[node_]){ 
          changed = true;
            }
      }
    } 
  }

  void GenerateTempLiveMap() {
    auto *liveGraphFacPtr =
        new live::LiveGraphFactory(flowgraph_);
    liveGraphFacPtr->LiveMap();
    auto *in_ = liveGraphFacPtr->GetIn();
    std::list<fg::FNodePtr> flowgrapg_nodes = flowgraph_->Nodes()->GetList();
    for (const auto &node_ : flowgrapg_nodes) {
      temp_in_[node_] = in_->Look(node_);
    }
  }

  /* callee saved and frame address active after call */
  void BuildValidPointerMap() {
    std::vector<std::string> calleeSaved = {"%r13", "%rbp", "%r12",
                                            "%rbx", "%r14", "%r15"};
    auto flowgrapg_nodes = flowgraph_->Nodes()->GetList();
    bool nextReturnLabel = false;
    for (const auto &node_ : flowgrapg_nodes) {
      assem::Instr *ins = node_->NodeInfo();
      if (typeid(*ins) == typeid(assem::OperInstr)) {
        std::string ass = static_cast<assem::OperInstr *>(ins)->assem_;
        if (ass.find("call") != ass.npos) {
          nextReturnLabel = true;
          continue;
        }
      }
      if (nextReturnLabel) {
        // The current node must be LabelInstr
        nextReturnLabel = false;
        // The frame address that stores the pointer
        pointer_address_map[ins] = address_in_[node_];
        // Filter out the callee saves register, which stores Pointers
        pointer_temp_map[ins] = std::vector<std::string>();
        for (const auto &temp : temp_in_[node_]->GetList())
          if (temp->store_pointer_) {
            std::string regName = *color->Look(temp);
            if (std::find(calleeSaved.begin(), calleeSaved.end(), regName) !=
                    calleeSaved.end() &&
                std::find(pointer_temp_map[ins].begin(),
                          pointer_temp_map[ins].end(),
                          regName) == pointer_temp_map[ins].end()) {
              pointer_temp_map[ins].emplace_back(
                  regName); // The combined register is not inserted twice
            }
          }
      }
    }
  }

  /* Place the pointer in the callee saves register into the frame */
  void RewriteProgram() {
    std::list<assem::Instr *> ins_list = il->GetList();
    auto iter = ins_list.begin();
    while (iter != ins_list.end()) {
      if (typeid(*(*iter)) == typeid(assem::OperInstr)) {
        std::string ass = static_cast<assem::OperInstr *>(*iter)->assem_;
        if (ass.find("call") == ass.npos) {
          iter++;
          continue;
        }
        auto labelnode = iter;
        labelnode++;
#ifdef DEBUG
        printf("pointer address map vector size %d \n", pointer_address_map[*labelnode].size());
#endif
        for (const auto &reg : pointer_temp_map[(*labelnode)]) {
#ifdef DEBUG
          printf("push offset\n");
#endif
          int offset = frame->offset;
          frame::Access *access = frame->allocLocal(true);
          access->SetStorePointer();
          pointer_address_map[(*labelnode)].emplace_back(offset);
          std::string move_instr = "movq " + reg + ", (" + frame->GetLabel() +
                                   "_framesize" + std::to_string(offset) +
                                   ")(%rsp)";
          iter = ins_list.insert(iter, new assem::OperInstr(move_instr, nullptr,
                                                            nullptr, nullptr));
          iter++;
        }
      }
      iter++;
    }
    il = new assem::InstrList(ins_list);
  }

  std::vector<int> VectorUnion(std::vector<int> vec1, std::vector<int> vec2) {
    std::vector<int> result;
    std::sort(vec1.begin(), vec1.end());
    std::sort(vec2.begin(), vec2.end());
    std::set_union(
        vec1.begin(), vec1.end(), vec2.begin(), vec2.end(),
        std::back_inserter(
            result)); // Find the union of two sets, for vectors, but in order
    return result;
  }

  std::vector<int> VectorDifference(std::vector<int> vec1,
                                    std::vector<int> vec2) {
    std::vector<int> result;
    for (const auto &item : vec1) {
      if (std::find(vec2.begin(), vec2.end(), item) == vec2.end()) {
        result.emplace_back(item);
      }
    }
    return result;
  }
};

} // namespace gc

#endif // TIGER_RUNTIME_GC_ROOTS_H