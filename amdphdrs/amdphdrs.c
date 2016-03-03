#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif // _WIN32

#include <libelf.h>
#include <fcntl.h>
#include <gelf.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#define open _open
#define read _read
#define write _write
#define lseek _lseek
#define close _close
#else
#define O_BINARY 0
#endif // _WIN32

#define READ_SIZE 1024

void print_usage() {
  fprintf(stderr, "amdphdrs <input> <output>\n");
}

int main(int argc, char **argv) {

  Elf *elf;
  Elf64_Ehdr *ehdr;
  char read_buf[READ_SIZE];
  ssize_t read_bytes;
  size_t section_str_index;
  Elf_Scn *section = NULL;
  const char *in_file, *out_file;
  int infd, outfd;
  int has_data = 0;

  if (argc != 3) {
    print_usage();
    return EXIT_FAILURE;
  }

  in_file = argv[1];
  out_file = argv[2];
  infd = open(in_file, O_RDONLY | O_BINARY);
  outfd = open(out_file, O_RDWR | O_CREAT | O_BINARY, 0660);

  if (infd < 0) {
    fprintf(stderr, "Failed to open input file: %s\n", argv[1]);
    return EXIT_FAILURE;
  }

  if (outfd < 0) {
    fprintf(stderr, "Failed to open output file: %s\n", argv[2]);
    perror("");
    return EXIT_FAILURE;
  }

  while ((read_bytes = read(infd, read_buf, READ_SIZE)) > 0) {
    ssize_t write_bytes;
    if ((write_bytes = write(outfd, read_buf, READ_SIZE)) < 0) {
      fprintf(stderr, "Error writing output file: %s\n", out_file);
      return EXIT_FAILURE;
    }
  }

  if (read_bytes < 0) {
    fprintf(stderr, "Error reading input file: %s\n", in_file);
    return EXIT_FAILURE;
  }

  if (lseek(outfd, 0, SEEK_SET) != 0) {
    fprintf(stderr, "Failed to rewind file descriptor\n");
    return EXIT_FAILURE;
  }

  elf_version(EV_CURRENT);

  elf = elf_begin(outfd, ELF_C_RDWR, NULL);

  if (!elf) {
    fprintf(stderr, "Failed to create ELF handle for output.\n");
    return EXIT_FAILURE;
  }

  ehdr = elf64_getehdr(elf);

  if (!ehdr) {
    fprintf(stderr, "Failed to get ehdr: %s\n", elf_errmsg(-1));
    return EXIT_FAILURE;
  }

  ehdr->e_type = ET_EXEC;

  elf_getshdrstrndx(elf, &section_str_index);

  while (section = elf_nextscn(elf, section)) {
    GElf_Shdr section_header;
    const char *name;

    gelf_getshdr(section, &section_header);
    name = elf_strptr(elf, section_str_index, section_header.sh_name);
    if (!strcmp(name, ".hsadata_global_program")) {
      has_data = 1;
    }
  }

  if (!gelf_newphdr(elf, has_data ? 2 : 1)) {
    fprintf(stderr, "Error newphdr:%s \n", elf_errmsg(-1));
    return EXIT_FAILURE;
  }

  // Create the program headers

  if (!elf_update(elf, ELF_C_NULL)) {
    fprintf(stderr, "Error elf_update:%s \n", elf_errmsg(-1));
    return EXIT_FAILURE;
  }

  section = NULL;
  while (section = elf_nextscn(elf, section)) {
    GElf_Shdr section_header;
    const char *name;

    gelf_getshdr(section, &section_header);
    name = elf_strptr(elf, section_str_index, section_header.sh_name);
    if (!strcmp(name, ".hsatext")) {
      GElf_Phdr code_phdr;
      /*
      if (!gelf_getphdr(elf, 0, &code_phdr)) {
        fprintf(stderr, "Error getting code phdr:%s \n", elf_errmsg(-1));
        return EXIT_FAILURE;
      }
      */

      code_phdr.p_type = 0x60000003;
      code_phdr.p_filesz = section_header.sh_size;
      code_phdr.p_vaddr = section_header.sh_addr;
      code_phdr.p_offset = section_header.sh_offset;
      code_phdr.p_memsz = code_phdr.p_filesz;
      code_phdr.p_align = 0x100;
      code_phdr.p_flags = 0x00000005;

      if (!gelf_update_phdr(elf, 0, &code_phdr)) {
        fprintf(stderr, "Error updating code phdr:%s \n", elf_errmsg(-1));
        return EXIT_FAILURE;
      }
    }
    else  if (!strcmp(name, ".hsadata_global_program")) {
      GElf_Phdr global_program_phdr;
      /*
      if (!gelf_getphdr(elf, 1, &global_program_phdr)) {
        fprintf(stderr, "Error getting code phdr:%s \n", elf_errmsg(-1));
        return EXIT_FAILURE;
      }
      */
      global_program_phdr.p_type = 0x60000000;
      global_program_phdr.p_filesz = section_header.sh_size;
      global_program_phdr.p_filesz = section_header.sh_size;
      global_program_phdr.p_vaddr = section_header.sh_addr;
      global_program_phdr.p_offset = section_header.sh_offset;
      global_program_phdr.p_memsz = global_program_phdr.p_filesz;
      global_program_phdr.p_align = 4;
      global_program_phdr.p_flags = 0x00000006;
      if (!gelf_update_phdr(elf, 1, &global_program_phdr)) {
        fprintf(stderr, "Error updating code phdr:%s \n", elf_errmsg(-1));
        return EXIT_FAILURE;
      }
    }
  }

  if (!elf_update(elf, ELF_C_WRITE)) {
    fprintf(stderr, "Error elf_update:%s \n", elf_errmsg(-1));
    return EXIT_FAILURE;
  }

  elf_end(elf);
  close(outfd);
}
