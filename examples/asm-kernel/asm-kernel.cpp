#include "dispatch.hpp"

using namespace amd::dispatch;

class AsmKernelDispatch : public Dispatch {
private:
  Buffer* out;

public:
  AsmKernelDispatch(int argc, const char **argv)
    : Dispatch(argc, argv) { }

  bool SetupCodeObject() override {
    return LoadCodeObjectFromFile("asm-kernel.co");
  }

  bool Setup() override {
    if (!AllocateKernarg(1024)) { return false; }
    out = AllocateBuffer(1024);
    Kernarg(out);
    SetGridSize(1);
    SetWorkgroupSize(1);
    return true;
  }

  bool Verify() override {
    if (!CopyFrom(out)) {
      output << "Error: failed to copy from local" << std::endl;
      return false;
    }
    if (((const float*)out->SystemPtr())[0] == 3.14159f) {
      output << "Success" << std::endl;
      return true;
    } else {
      output << "Error: validation failed: got " << (((const float*) out->SystemPtr())[0]) << " expected " << 3.14159 << std::endl;
      return false;
    }
  }
};

int main(int argc, const char** argv)
{
  return AsmKernelDispatch(argc, argv).Run() ? 0 : 1;
}
