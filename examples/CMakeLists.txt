set(LLVM-MC ${LLVM_TOOLS_BINARY_DIR}/llvm-mc)
set(AMDPHDRS $<TARGET_FILE:amdphdrs>)
set(SP3_TO_MC "${CMAKE_SOURCE_DIR}/bin/sp3_to_mc.pl")

macro(asm_assemble f)
add_custom_command(
  OUTPUT ${f}.o
  COMMAND
    ${LLVM-MC} -arch=amdgcn -mcpu=fiji -filetype=obj -o ${f}.o ${CMAKE_CURRENT_SOURCE_DIR}/${f}.s
  DEPENDS ${f}.s
  COMMENT "Assembling ${f}.s to ${f}.o"
)
endmacro(asm_assemble)

macro(asm_link f)
add_custom_command(
  OUTPUT ${f}.co
  COMMAND
    ${AMDPHDRS} ${f}.o ${f}.co
  DEPENDS ${f}.o
  COMMENT "Linking ${f}.o to ${f}.co with amdphdrs"
)
endmacro(asm_link)

macro(asm_example f)
  asm_assemble(${f})
  asm_link(${f})
  add_custom_target(${f}_co ALL DEPENDS ${f}.co)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${f}.co DESTINATION bin COMPONENT examples)
endmacro(asm_example)

macro(asm_sp3_example f)
  add_custom_command(
    OUTPUT ${f}.s
    COMMAND
      ${PERL_EXECUTABLE} ${SP3_TO_MC} ${CMAKE_CURRENT_SOURCE_DIR}/${f}.sp3 > ${CMAKE_CURRENT_SOURCE_DIR}/${f}.s
    DEPENDS ${f}.sp3
    COMMENT "Translating ${f}.sp3 to ${f}"
  )
  asm_example(${f})
endmacro(asm_sp3_example)

macro(asm_dispatch_example f)
  asm_example(${f})
  add_executable(${f} ${f}.cpp)
  target_link_libraries(${f} examples_common)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${f} DESTINATION bin COMPONENT examples)
endmacro(asm_dispatch_example)

add_subdirectory(common)
add_subdirectory(sp3)
add_subdirectory(asm-kernel)
add_subdirectory(gfx8)
