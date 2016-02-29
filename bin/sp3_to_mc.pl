#!/usr/bin/perl

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
