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
#include <iostream>

using namespace amd::dispatch;

class SMemRealtimeDispatch : public Dispatch {
public:
  SMemRealtimeDispatch(int argc, const char **argv)
    : Dispatch(argc, argv) { }

  bool SetupCodeObject() override {
    return LoadCodeObjectFromFile("s_memrealtime.co");
  }

  bool Setup() override {
    if (!AllocateKernarg(8)) { return false; }
    uint64_t ticks_to_wait = 27000000; //should be 1 sec for 27MHz RTC
    KernargRaw(&ticks_to_wait, sizeof(ticks_to_wait), 1);
    SetGridSize(1);
    SetWorkgroupSize(1);
    return true;
  }

};

int main(int argc, const char** argv)
{
  return SMemRealtimeDispatch(argc, argv).RunMain();
}
