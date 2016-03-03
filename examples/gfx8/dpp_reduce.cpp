#include "dispatch.hpp"

using namespace amd::dispatch;

class DppReduce : public Dispatch {
private:
  Buffer* in;
  Buffer* out;
  unsigned length;

public:
  DppReduce(int argc, const char **argv)
    : Dispatch(argc, argv), length(64) { }

  bool SetupCodeObject() override {
    return LoadCodeObjectFromFile("dpp_reduce.co");
  }

  bool Setup() override {
    if (!AllocateKernarg(16)) { return false; }
    in = AllocateBuffer(length * sizeof(float));
    for (unsigned i = 0; i < length; ++i) {
      in->Data<float>(i) = i;
    }
    if (!CopyTo(in)) { output << "Error: failed to copy to local" << std::endl; return false; }
    out = AllocateBuffer(length * sizeof(float));
    Kernarg(in);
    Kernarg(out);
    SetGridSize(64);
    SetWorkgroupSize(64);
    return true;
  }

  bool Verify() override {
    if (!CopyFrom(out)) { output << "Error: failed to copy from local" << std::endl; return false; }
    bool ok = true;
    float expected = 0.0;
    for (unsigned i = 0; i < length; ++i) {
      if (i % 64 == 0) { expected = 0; }
      expected += i;
      if (expected != out->Data<float>(i)) {
        output << "Error: validation failed at " << i << ": got " << out->Data<float>(i) << " expected " << expected << std::endl;
        ok = false;
      }
    }
    output << (ok ? "Success" : "Failure") << std::endl;
    return 0;
  }
};

int main(int argc, const char** argv)
{
  return DppReduce(argc, argv).RunMain();
}
