#pragma once

#include <assert.h>
#include <stdint.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#include "heap.h"
// Used to locate the start of ptrmap, simply get the address by
// &GLOBAL_GC_ROOTS
extern uint64_t GLOBAL_GC_ROOTS;

namespace gc {

// constexpr long END_MARK = 0;

class DerivedHeap : public TigerHeap {
 public:
  /***************** necessary protocols********************/
  struct recordInfo {
    char *recordBeginPtr;
    int recordSize;
    int descriptorSize;
    unsigned char *descriptor;
    recordInfo()=default;
    recordInfo(char *recordBeginPtr_, int recordSize_, int descriptorSize_, unsigned char* descriptor_):
      recordBeginPtr(recordBeginPtr_), recordSize(recordSize_), descriptorSize(descriptorSize_), descriptor(descriptor_){}
  };

  struct arrayInfo {
    char *arrayBeginPtr;
    int arraySize;
    arrayInfo()=default;
    arrayInfo(char * arrayBeginPtr_, int arraySize_) : arrayBeginPtr(arrayBeginPtr_), arraySize(arraySize_) {}
  };

  struct freeIntervalInfo {
    char *intervalStart;
    uint64_t intervalSize;
    char *intervalEnd;
    freeIntervalInfo()=default;
    freeIntervalInfo(char* intervalStart_, char* intervalEnd_, uint64_t intervalSize_):
      intervalStart(intervalStart_), intervalEnd(intervalEnd_), intervalSize(intervalSize_){}
  };

  struct markResult {
    std::vector<int> arraiesActiveBitMap;
    std::vector<int> recordsActiveBitMap;
    markResult(std::vector<int> arrayBitMap, std::vector<int> recordBitMap):
      arraiesActiveBitMap(arrayBitMap), recordsActiveBitMap(recordBitMap){}
  };

  /* ascending order*/
  static inline bool compareIntervalBySize(freeIntervalInfo interval1,
                                           freeIntervalInfo interval2) {
    return interval1.intervalSize < interval2.intervalSize;
  }

  /* ascending order */
  static inline bool compareIntervalByStartAddr(freeIntervalInfo interval1,
                                                freeIntervalInfo interval2) {
    return (uint64_t)interval1.intervalStart <
           (uint64_t)interval2.intervalStart;
  }

  struct PointerMapBin {
    uint64_t returnAddress;
    uint64_t nextPointerMapAddress;
    uint64_t frameSize;
    uint64_t isMain;
    std::vector<int64_t> offsets;
    PointerMapBin(uint64_t returnAddress_, uint64_t nextPointerMapAddress_,
                  uint64_t frameSize_, uint64_t isMain_,
                  std::vector<int64_t> offsets_)
        : returnAddress(returnAddress_),
          nextPointerMapAddress(nextPointerMapAddress_),
          frameSize(frameSize_),
          isMain(isMain_),
          offsets(offsets_) {}
  };

  /***************** end necessary protocols ********************/

  DerivedHeap() = default;

  ~DerivedHeap() = default;

  void Coalesce();

  freeIntervalInfo FindFit(int size);

  char *Allocate(uint64_t size);

  char *AllocateRecord(uint64_t size, int des_size, unsigned char *des_ptr,
                       uint64_t *sp);

  char *AllocateArray(uint64_t size, uint64_t *sp);

  uint64_t Used() const;

  uint64_t MaxFree() const;

  void Initialize(uint64_t size);

  markResult Mark();

  void Sweep(markResult bitMaps);

  inline void ScanARecord(recordInfo record,
                          std::vector<int> &arraiesActiveBitMap,
                          std::vector<int> &recordsActiveBitMap);

  void MarkAnAddress(uint64_t address, std::vector<int> &arraiesActiveBitMap,
                     std::vector<int> &recordsActiveBitMap);

  void GC();

  static constexpr uint64_t WORD_SIZE = 8;

  void GetAllPointerMaps();

  std::vector<uint64_t> addressToMark();

 private:
  // void printPointerMap();

  char *heap_root;
  std::vector<recordInfo> recordsInHeap;
  std::vector<arrayInfo> arraiesInHeap;
  std::vector<freeIntervalInfo> free_intervals;
  std::vector<PointerMapBin> pointerMaps;
  uint64_t *tigerStack;
};

}  // namespace gc

