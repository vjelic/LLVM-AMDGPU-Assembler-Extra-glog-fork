set(LLVM-MC ${LLVM_TOOLS_BINARY_DIR}/llvm-mc)

set(files asm-kernel)

foreach (f ${files})
add_custom_command(
  OUTPUT ${f}.o
  COMMAND
    ${LLVM-MC} -arch=amdgcn -mcpu=fiji -filetype=obj -o ${f}.o ${CMAKE_CURRENT_SOURCE_DIR}/${f}.s
  DEPENDS ${f}.s
  COMMENT "Assembling ${f}.s to ${f}.o"
)

set (AMDPHDRS $<TARGET_FILE:amdphdrs>)

add_custom_command(
  OUTPUT ${f}.co
  COMMAND
    ${AMDPHDRS} ${f}.o ${f}.co
  DEPENDS ${f}.o
  COMMENT "Linking ${f}.o to ${f}.co with amdphdrs"
)
endforeach(f)

add_custom_target(asm-kernel-assemble ALL DEPENDS asm-kernel.co)

include_directories(${HSA_HEADER_DIR})
file(GLOB sources *.cpp)
add_executable(asm-kernel ${sources})
target_link_libraries(asm-kernel examples_common)

install (TARGETS
  asm-kernel
  RUNTIME DESTINATION bin
  COMPONENT examples)
install (FILES
  ${CMAKE_CURRENT_BINARY_DIR}/asm-kernel.co
  DESTINATION bin
  COMPONENT examples)
