#!/usr/bin/perl

################################################################################
##
## The University of Illinois/NCSA
## Open Source License (NCSA)
## 
## Copyright (c) 2016, Advanced Micro Devices, Inc. All rights reserved.
## 
## Developed by:
## 
##                 AMD Research and AMD HSA Software Development
## 
##                 Advanced Micro Devices, Inc.
## 
##                 www.amd.com
## 
## Permission is hereby granted, free of charge, to any person obtaining a copy
## of this software and associated documentation files (the "Software"), to
## deal with the Software without restriction, including without limitation
## the rights to use, copy, modify, merge, publish, distribute, sublicense,
## and#or sell copies of the Software, and to permit persons to whom the
## Software is furnished to do so, subject to the following conditions:
## 
##  - Redistributions of source code must retain the above copyright notice,
##    this list of conditions and the following disclaimers.
##  - Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimers in
##    the documentation and#or other materials provided with the distribution.
##  - Neither the names of Advanced Micro Devices, Inc,
##    nor the names of its contributors may be used to endorse or promote
##    products derived from this Software without specific prior written
##    permission.
## 
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
## THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
## OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
## ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
## DEALINGS WITH THE SOFTWARE.
##
################################################################################

# Convert legacy AMD sp3 assembler syntax into LLVM MC assembler syntax.
# This concerns mostly non-standard macros.
#
# Some examples of this conversion are:
# 1. function 


my @scope;
my @args;
while (<>) {
  if (m/for var (\w+) = (\w+); (\w+) < (\w+); (\w+) \+= (\w+)/) {
    $s = ".irp $1";
    for (my $v = $2; $v < $4; $v += $6) {
      $s .= ",";
      $s .= $v;
    }
    print $s; print "\n";
    push @scope, 'for';
    next;
  }
  s/var(\s+)(\w+)(\s+)=(\s+)(.*)$/.set$1$2$3,$4$5/g;
  if (m/if /) { push @scope, 'if'; s/if /.if /g; }
  if (m/function\s+(\w+)\s*\((.*)\)/) {
    push @scope, 'function';
    s/function\s+(\w+)\s*\((.*)\)/.macro $1 $2/g;
    @args = split(', ', $2);
  } else {
    foreach $arg (@args) {
      s/($arg[\s,])/\\$1/g;
    }
  }
  s/else /.else /g;
  if (m/(\s*)end$/) {
    my $s = pop @scope;
    if ($s eq 'if') { s/end/.endif/g; }
    if ($s eq 'function') {
      s/end/.endm/g;
      @args = [];
    }
    if ($s eq 'for') { s/end/.endr/; }
  }
  s/(\w+)\((.*)\)/$1 $2/g;
  print;
}
