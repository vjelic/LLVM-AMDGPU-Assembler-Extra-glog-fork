#include "dispatch.hpp"
#include <iostream>

using namespace amd::dispatch;

class DsBPermuteDispatch : public Dispatch {
private:
  Buffer* in;
  Buffer* index;
  Buffer* out;

public:
  DsBPermuteDispatch(int argc, const char **argv)
    : Dispatch(argc, argv) { }

  bool SetupCodeObject() override {
    return LoadCodeObjectFromFile("ds_bpermute.co");
  }

  bool Setup() override {
    if (!AllocateKernarg(24)) { return false; }
    in = AllocateBuffer(4 * 64);
    index = AllocateBuffer(4 * 64);
    out = AllocateBuffer(4 * 64);
    for (unsigned i = 0; i < 64; ++i) {
      in->Data<uint32_t>(i) = i;
      index->Data<uint32_t>(i) = 63-i;
    }
    if (!CopyTo(in) || !CopyTo(index)) { output << "Error: failed to copy to local" << std::endl; return false; }
    Kernarg(in);
    Kernarg(index);
    Kernarg(out);
    SetGridSize(64);
    SetWorkgroupSize(64);
    return true;
  }

  bool Verify() override {
    if (!CopyFrom(out)) { output << "Error: failed to copy from local" << std::endl; return false; }
    bool ok = true;
    for (unsigned i = 0; i < 64; ++i) {
      uint32_t expected = 63-i;
      if (out->Data<uint32_t>(i) != expected) {
        output << "Error: validation failed at " << i << ": got " << out->Data<uint32_t>(i) << " expected " << expected << std::endl;
        ok = false;
      }
    }
    return ok;
  }
};

int main(int argc, const char** argv)
{
  return DsBPermuteDispatch(argc, argv).RunMain();
}
