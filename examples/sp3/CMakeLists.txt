find_package(Perl REQUIRED)

set(SP3_TO_MC "${CMAKE_SOURCE_DIR}/bin/sp3_to_mc.pl")
set(LLVM-MC ${LLVM_TOOLS_BINARY_DIR}/llvm-mc)

set(files if function for)

foreach (f ${files})
add_custom_command(
  OUTPUT ${f}.s
  COMMAND 
    ${PERL_EXECUTABLE} ${SP3_TO_MC} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.sp3 > ${CMAKE_CURRENT_BINARY_DIR}/${f}.s
  DEPENDS ${f}.sp3
  COMMENT "Translating ${f}.sp3 to ${f}"
)

add_custom_command(
  OUTPUT ${f}.o
  COMMAND
    ${LLVM-MC} -arch=amdgcn -mcpu=fiji -filetype=obj -o ${f}.o ${f}.s
  DEPENDS ${f}.s
  COMMENT "Assembling ${f}.s to ${f}.o"
)

set (AMDPHDRS $<TARGET_FILE:amdphdrs>)

add_custom_command(
  OUTPUT ${f}
  COMMAND
    ${AMDPHDRS} ${f}.o ${f}
  DEPENDS ${f}.o
  COMMENT "Linking ${f}.o to ${f} with amdphdrs"
)
endforeach(f)

add_custom_target(sp3_convert ALL DEPENDS ${files})
