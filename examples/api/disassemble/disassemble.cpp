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

// Simple self-contained example that uses LLVM API to disassemble AMDGCN instructions.

#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCParser/AsmLexer.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Format.h"
#include "llvm/ADT/STLExtras.h"

using namespace llvm;

typedef std::pair<std::vector<unsigned char>, std::vector<const char *>>
    ByteArrayTy;

uint8_t Instructions[] = {
  0x00, 0x00, 0x80, 0xBF,
  0x7F, 0x00, 0x8C, 0xBF,
  0x00, 0x00, 0x81, 0xBF,
};

class DisassemblerExample {
private:

  const char* ArchName = "amdgcn";
  const char* TripleName = "";
  const char* MCPU = "fiji";

  std::string Error;

  std::string Disassembly;


  void Init() {
    // Initialize targets and assembly printers/parsers.
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllDisassemblers();
  }

  int RunDisassemble() {
    Triple TheTriple(Triple::normalize(TripleName));
    const Target* TheTarget =
      TargetRegistry::lookupTarget(ArchName, TheTriple, Error);
    if (!TheTarget) { outs() << Error; return 2; }

    ArrayRef<uint8_t> Data(Instructions, sizeof(Instructions));

    outs() << "Instruction stream (length " << Data.size() << ")\n";
    for (size_t i = 0; i < Data.size(); ++i) {
      char c = Data[i];
      outs() << format_hex((uint8_t) c, 4);
      if ((i+1) % 16 == 0) { outs() << '\n'; } else { outs() << " "; }
    }
    outs() << "\n";

    outs() << "Running disassembly" << '\n';

    std::string FeaturesStr;

    std::unique_ptr<MCSubtargetInfo> STI(TheTarget->createMCSubtargetInfo(TripleName, MCPU, FeaturesStr));

    std::unique_ptr<const MCRegisterInfo> MRI(TheTarget->createMCRegInfo(TripleName));
    if (!MRI) {
      outs() << "error: no register info for target " << TripleName << "\n";
      return -1;
    }

    std::unique_ptr<const MCAsmInfo> MAI(TheTarget->createMCAsmInfo(*MRI, TripleName));
    if (!MAI) {
      outs() << "error: no assembly info for target " << TripleName << "\n";
      return -1;
    }

    // Set up the MCContext for creating symbols and MCExpr's.
    MCObjectFileInfo MOFI;
    MCContext Ctx(MAI.get(), MRI.get(), &MOFI, nullptr);
    MOFI.InitMCObjectFileInfo(TheTriple, true, CodeModel::Default, Ctx);

    std::unique_ptr<const MCDisassembler> DisAsm(
      TheTarget->createMCDisassembler(*STI, Ctx));
    if (!DisAsm) {
      outs() << "error: no disassembler for target " << TripleName << "\n";
      return -1;
    }

    std::unique_ptr<MCInstrInfo> MCII(TheTarget->createMCInstrInfo());
    MCCodeEmitter *CE = nullptr; // TheTarget->createMCCodeEmitter(*MCII, *MRI, Ctx);
    MCAsmBackend *MAB = nullptr; // TheTarget->createMCAsmBackend(*MRI, TripleName, MCPU);

    std::unique_ptr<raw_string_ostream> DataStream(make_unique<raw_string_ostream>(Disassembly));
    //std::unique_ptr<buffer_ostream> BOS(make_unique<buffer_ostream>(*DataStream));
    std::unique_ptr<formatted_raw_ostream> FOut(new formatted_raw_ostream(*DataStream));

    unsigned OutputAsmVariant = 0;
    bool ShowInst = false;

    MCInstPrinter *IP = nullptr;
    IP = TheTarget->createMCInstPrinter(Triple(TripleName), OutputAsmVariant,
                                        *MAI, *MCII, *MRI);

    std::unique_ptr<MCStreamer> Streamer(TheTarget->createAsmStreamer(
        Ctx, std::move(FOut), /*asmverbose*/ true,
        /*useDwarfDirectory*/ true, IP, CE, MAB, ShowInst));

    Streamer->InitSections(false);

    uint64_t Size;
    uint64_t Index;

    // Tell SrcMgr about this buffer, which is what the parser will pick up.

    for (Index = 0; Index < Data.size(); Index += Size) {
      MCInst Inst;

      MCDisassembler::DecodeStatus S;
      S = DisAsm->getInstruction(Inst, Size, Data.slice(Index), Index, nulls(), nulls());
      switch (S) {
      case MCDisassembler::Fail:
      case MCDisassembler::SoftFail:
        outs() << "Disassembly failed" << '\n';
        return 1;

      case MCDisassembler::Success:
        Streamer->EmitInstruction(Inst, *STI);
        break;
      }
    }

    outs() << "Disassembled instructions:\n";
    outs() << Disassembly << '\n';

    return 0;
  }


public:
  int RunMain() {
    int res;
    Init();
    if ((res = RunDisassemble()) != 0) { return res; }
    return 0;
  }
};

int main(int argc, char **argv)
{
  llvm_shutdown_obj Y;  // Call llvm_shutdown() on exit.
  return DisassemblerExample().RunMain();
}

