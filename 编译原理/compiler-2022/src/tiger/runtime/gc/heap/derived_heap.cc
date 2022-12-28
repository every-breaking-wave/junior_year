#include "derived_heap.h"
// #define DEBUG
namespace gc {

/* Coalesce adjacent free intervals */
void DerivedHeap::Coalesce() {
  std::sort(free_intervals.begin(), free_intervals.end(),
            DerivedHeap::compareIntervalByStartAddr);
  auto iter = free_intervals.begin();
  while ((iter + 1) != free_intervals.end()) {
    auto iter_next = iter;
    iter_next++;
    if ((uint64_t)(*iter).intervalEnd == (uint64_t)(*iter_next).intervalStart) {
      freeIntervalInfo newIterval(
          (*iter).intervalStart, (*iter_next).intervalEnd,
          (*iter).intervalSize + (*iter_next).intervalSize);
      iter = free_intervals.erase(iter);              // iter指向iter_next
      iter = free_intervals.erase(iter);              // iter指向下一个item
      iter = free_intervals.insert(iter, newIterval); // iter指向newInterval
      continue;
    }
    iter++;
  }

  std::sort(free_intervals.begin(), free_intervals.end(),
            DerivedHeap::compareIntervalBySize);
}

/* 寻找interval_size最接近(>=)size的空闲区间, 并将其从free_intervals中删除 */
DerivedHeap::freeIntervalInfo DerivedHeap::FindFit(int size) {
  auto iter = free_intervals.begin();
  while (iter != free_intervals.end()) {
    if ((*iter).intervalSize == size) {
      freeIntervalInfo bestFit = *iter;
      free_intervals.erase(iter);
      return bestFit;
    } else if ((*iter).intervalSize > size) {
      freeIntervalInfo bestFit((*iter).intervalStart,
                               (*iter).intervalStart + size, size);
      freeIntervalInfo restInterval(bestFit.intervalEnd, (*iter).intervalEnd,
                                    (*iter).intervalSize - size);
      free_intervals.erase(iter);
      free_intervals.emplace_back(restInterval);
      std::sort(free_intervals.begin(), free_intervals.end(),
                DerivedHeap::compareIntervalBySize);
      return bestFit;
    }
    iter++;
  }
}

char *DerivedHeap::Allocate(uint64_t size) {
  return size > MaxFree() ? nullptr : FindFit(size).intervalStart;
}

char *DerivedHeap::AllocateRecord(uint64_t size, int des_size,
                                  unsigned char *des_ptr, uint64_t *sp) {
  tigerStack = sp;
  char *record_begin = Allocate(size);
  if (!record_begin) {
    return nullptr;
  }
  recordInfo info(record_begin, size, des_size, des_ptr);
  recordsInHeap.emplace_back(info);
  return record_begin;
}

/* To ease your burden, you don't need to consider the situation that
   program allocate pointer in array. */
char *DerivedHeap::AllocateArray(uint64_t size, uint64_t *sp) {
  tigerStack = sp;
  char *array_begin = Allocate(size);
  if (!array_begin){
    return nullptr;
  }
  arrayInfo info(array_begin, size);
  arraiesInHeap.emplace_back(info);
  return array_begin;
}

uint64_t DerivedHeap::Used() const {
  uint64_t used = 0;
  for (const auto &info_rec : recordsInHeap) {
    used += info_rec.recordSize;
  }
  for (const auto &info_arr : arraiesInHeap) {
    used += info_arr.arraySize;
  }
  return used;
}

uint64_t DerivedHeap::MaxFree() const {
  int maxFree = 0;
  for (const auto &interval : free_intervals) {
    if (interval.intervalSize > maxFree) {
      maxFree = interval.intervalSize;
    }
  }
  // std::cout << "maxFree = " << maxFree << std::endl;
  return maxFree;
}

void DerivedHeap::Initialize(uint64_t size) {
  heap_root = (char *)malloc(size);
  freeIntervalInfo freeInterval(heap_root, heap_root + size, size);
  free_intervals.emplace_back(freeInterval);
  GetAllPointerMaps();
}

void DerivedHeap::Sweep(markResult bitMaps) {
  std::vector<arrayInfo> new_arraiesInHeap;
#ifdef DEBUG
  printf("bitmap size %d\n", bitMaps.arraiesActiveBitMap.size());
  fflush(stdout);
#endif
  for (int i = 0; i < bitMaps.arraiesActiveBitMap.size(); i++) {
    if (bitMaps.arraiesActiveBitMap[i]) { // marked, cannot sweep
      new_arraiesInHeap.emplace_back(arraiesInHeap[i]);
    } else {
      freeIntervalInfo freedInterval((char *)arraiesInHeap[i].arrayBeginPtr,
                                     (char *)arraiesInHeap[i].arrayBeginPtr +
                                         arraiesInHeap[i].arraySize,
                                     arraiesInHeap[i].arraySize);
      free_intervals.emplace_back(freedInterval);
    }
  }
  std::vector<recordInfo> new_recordsInHeap;
  for (int i = 0; i < bitMaps.recordsActiveBitMap.size(); i++) {
    if (bitMaps.recordsActiveBitMap[i])
      new_recordsInHeap.emplace_back(recordsInHeap[i]);
    else {
      freeIntervalInfo freedInterval((char *)recordsInHeap[i].recordBeginPtr,
                                     (char *)recordsInHeap[i].recordBeginPtr +
                                         recordsInHeap[i].recordSize,
                                     recordsInHeap[i].recordSize);
      free_intervals.emplace_back(freedInterval);
    }
  }
  recordsInHeap = new_recordsInHeap;
  arraiesInHeap = new_arraiesInHeap;
#ifdef DEBUG
  std::cout << "interval free size=" << free_intervals.size() << std::endl;
  std::cout << "new_recordsInHeap  size=" << new_recordsInHeap.size()
            << std::endl;
#endif
  Coalesce();
}

DerivedHeap::markResult DerivedHeap::Mark() {
  std::vector<int> arraiesActiveBitMap(arraiesInHeap.size(), 0);
  std::vector<int> recordsActiveBitMap(recordsInHeap.size(), 0);
#ifdef DEBUG
  printf("array in heap size %d\n", arraiesInHeap.size());
  fflush(stdout);
  printf("pointer size %d\n", pointers.size());
#endif
  for (uint64_t pointer : addressToMark()){
    MarkAnAddress(pointer, arraiesActiveBitMap, recordsActiveBitMap);
  }
  // 以pointer为root开始mark
  markResult bitMaps(arraiesActiveBitMap, recordsActiveBitMap);
#ifdef DEBUG
  printf("array size %d\n", arraiesActiveBitMap.size());
#endif
  return bitMaps;
}

inline void DerivedHeap::ScanARecord(recordInfo record,
                                     std::vector<int> &arraiesActiveBitMap,
                                     std::vector<int> &recordsActiveBitMap) {
  long beginAddress = (long)record.recordBeginPtr;
  for (int i = 0; i < record.descriptorSize; i++) {
    if (record.descriptor[i] == '1') {
      uint64_t targetAddress = *((uint64_t *)(beginAddress + WORD_SIZE * i));
      // Store the value stored at the frame address of the pointer
      MarkAnAddress(targetAddress, arraiesActiveBitMap, recordsActiveBitMap);
    }
  }
}

void DerivedHeap::MarkAnAddress(uint64_t address,
                                std::vector<int> &arraiesActiveBitMap,
                                std::vector<int> &recordsActiveBitMap) {
  int arraiesNum = arraiesActiveBitMap.size();
  int recordsNum = recordsActiveBitMap.size();
  int i = 0;
  for (const auto &array : arraiesInHeap) {
    int delta = address - (uint64_t)array.arrayBeginPtr;
    if (delta >= 0 && delta <= (array.arraySize - WORD_SIZE)) {
      arraiesActiveBitMap[i] = 1;
      return;
    }
    i++;
  }
  i = 0;
  for (const auto &record : recordsInHeap) {
    int64_t delta = (int64_t)address - (int64_t)record.recordBeginPtr;
    if (delta >= 0 && delta <= (record.recordSize - WORD_SIZE)) {
      if (recordsActiveBitMap[i])
        return; // It has been scanned and marked
      else {
        ScanARecord(record, arraiesActiveBitMap, recordsActiveBitMap);
        recordsActiveBitMap[i] = 1;
        return;
      }
    }
    i++;
  }
}

void DerivedHeap::GC() {
  markResult bitMaps = Mark();
#ifdef DEBUG
  printf("bitmap size %d\n", bitMaps.arraiesActiveBitMap.size());
  std::cout << "prev heap size " << MaxFree() << " ";
  fflush(stdout);
  std::cout << bitMaps.recordsActiveBitMap.size() << "  ---- "
            << bitMaps.arraiesActiveBitMap.size() << std::endl;
#endif
  Sweep(bitMaps);
#ifdef DEBUG
  std::cout << "after heap size " << MaxFree() << " ";
  printf("finish GC\n");
  fflush(stdout);
#endif
}

/*************** Root Protocol ***************/
/* read pointerMaps */
void DerivedHeap::GetAllPointerMaps() {
  uint64_t *pointerMapStart = &GLOBAL_GC_ROOTS;
  uint64_t *iter = pointerMapStart;
  while (1) {
    uint64_t returnAddress = *iter;
    iter += 1;
    uint64_t nextPointerMapAddress = *iter;
    iter += 1;
    uint64_t frameSize = *iter;
    iter += 1;
    uint64_t isMain = *iter;
    iter += 1;
    std::vector<int64_t> offsets;
    while (true) {
      int64_t offset = *iter;
      iter += 1;
      if (offset == -1) { //.quad -1(end label)
        break;
      } else {
        offsets.emplace_back(offset);
      }
    }

    pointerMaps.emplace_back(PointerMapBin(returnAddress, nextPointerMapAddress,
                                           frameSize, isMain, offsets));
    if (nextPointerMapAddress == 0) {
      break;
    }
  }
}

/* Return roots in the stack */
std::vector<uint64_t> DerivedHeap::addressToMark() {
  std::vector<uint64_t> pointers;
  uint64_t *sp = tigerStack;
  bool isMain = false;
  while (!isMain) {
    uint64_t returnAddress = *(sp - 1);
    for (const auto &pointMap : pointerMaps)
      if (pointMap.returnAddress == returnAddress) {
        for (int64_t offset : pointMap.offsets) {
          uint64_t *pointerAddress =
              (uint64_t *)(offset + (int64_t)sp +
                           (int64_t)pointMap.frameSize); // address = offset +
                                                         // %rsp + framesize
          pointers.emplace_back(*pointerAddress);
        }
        sp += (pointMap.frameSize / WORD_SIZE + 1);
        isMain = pointMap.isMain;
        break;
      }
  }
  return pointers;
}
/*************** End Root Protocol ***************/

} // namespace gc
