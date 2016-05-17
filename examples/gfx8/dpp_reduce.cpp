////////////////////////////////////////////////////////////////////////////////
//
// The University of Illinois/NCSA
// Open Source License (NCSA)
//
// Copyright (c) 2016, Advanced Micro Devices, Inc. All rights reserved.
//
// Developed by:
//
//                 AMD Research and AMD HSA Software Development
//
//                 Advanced Micro Devices, Inc.
//
//                 www.amd.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal with the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
//  - Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimers.
//  - Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimers in
//    the documentation and/or other materials provided with the distribution.
//  - Neither the names of Advanced Micro Devices, Inc,
//    nor the names of its contributors may be used to endorse or promote
//    products derived from this Software without specific prior written
//    permission.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS WITH THE SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////

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
    return ok;
  }
};

int main(int argc, const char** argv)
{
  return DppReduce(argc, argv).RunMain();
}
