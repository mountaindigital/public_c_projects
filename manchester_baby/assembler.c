/*
 *  Author: Brian D'Ostilio
 *  Description: Assembler for Manchester Baby computer.  Binary output
 *  is reversed compared to modern binary represenation.
 *
 *  Example asm line:
 *  00: NUM 19   -- jump address
 */

#include <err.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Machine code dimensions
#define OUTPUT_FILE_LC      32
#define INSTR_WIDTH         32
#define INSTR_FILLER_WIDTH  16
#define OPERAND_WIDTH       13
#define OPCODE_WIDTH        3

// ASM mnemonics 
#define MNEM_NUM  "NUM" // opcode: 0
#define MNEM_JMP  "JMP" // opcode: 0
#define MNEM_JRP  "JRP" // opcode: 1
#define MNEM_LDN  "LDN" // opcode: 2
#define MNEM_STO  "STO" // opcode: 3
#define MNEM_SUB  "SUB" // opcode: 4 or 5
#define MNEM_SKN  "SKN" // opcode: 6
#define MNEM_HLT  "HLT" // opcode: 7

// misc consts
#define BUFFER_SIZE       BUFSIZ
#define EMPTY_INSTRUCTION "00000000000000000000000000000000"

int mnemtoopcode(char* mnem, char opcode[OPCODE_WIDTH + 1]) {
  char* opc = {0}; 
  if (!mnem) {
    fprintf(stderr, "empty pointer passed to opcode converter\n");
    return 1;
  } else if (!strcmp(mnem, MNEM_NUM)) {
    // data should not be executed so do not write it into buffer
    return 0; 
  } else if (!strcmp(mnem, MNEM_JMP)) {
    opc = "000";
  } else if (!strcmp(mnem, MNEM_JRP)) {
    opc = "100";
  } else if (!strcmp(mnem, MNEM_LDN)) {
    opc = "010";
  } else if (!strcmp(mnem, MNEM_STO)) {
    opc = "110";
  } else if (!strcmp(mnem, MNEM_SUB)) {
    opc = "001";
  } else if (!strcmp(mnem, MNEM_SKN)) {
    opc = "011";
  } else if (!strcmp(mnem, MNEM_HLT)) {
    opc = "111";
  } else {
    fprintf(stderr, "mnemonic '%s' unknown\n", mnem);
    return 1;
  }

  // overwrite chars without setting nul byte
  // to avoid string truncation
  for (size_t i = 0; i < OPCODE_WIDTH; i++) {
    opcode[i] = opc[i];
  }

  return 0;
}

// convert operand to reverse binary and write to output buffer
int revbin(int32_t operand, size_t width, char out[width + 1]) {
  double maxoperand = pow(2, width - 1);
  if (operand > maxoperand) {
    fprintf(
      stderr, "operand (%d) exceeds max size (2^%zu=%g)\n",
      operand, width, maxoperand);
    return 1;
  }

  int bit = 0;
  size_t idx = 0;
  while (operand) {
    bit = operand % 2;
    if (bit)
      out[idx] = '1';
    operand /= 2;
    idx++;
  }
  
  return 0;
}

int writeinstr(char* mnem, int32_t operand, char instruction[INSTR_WIDTH + 1]) {
  strcpy(instruction, EMPTY_INSTRUCTION);

  // handle data and instruction differently
  if (!strcmp(mnem, MNEM_NUM)) {
    // data lines can take up all 32 bits with data
    if (revbin(operand, INSTR_WIDTH, instruction)) {
      fprintf(stderr, "could not convert data to reverse binary\n");
      return 1;
    }
  } else {
    // instruction line operands can only take up (first) 13 bits
    if (revbin(operand, OPERAND_WIDTH, instruction)) {
      fprintf(stderr, "could not convert operand to reverse binary\n");
      return 1;
    }
  }

  if (mnemtoopcode(mnem, instruction + OPERAND_WIDTH)) {
    fprintf(stderr, "could not convert mnemonic '%s' to opcode\n", mnem);
    return 1;
  }

  printf("instruction: %s\n", instruction);
  return 0;
}

int assemble(FILE* inputF, char asmOut[OUTPUT_FILE_LC][INSTR_WIDTH + 1]) {
  char buf[BUFFER_SIZE] = {0};
  char* mnemonic = {0};
  char* operand_str = {0};
  long operand = 0;
  size_t iter = 0;
  while (fgets(buf, BUFFER_SIZE, inputF)) {
    strtok(buf, ": ");
    mnemonic = strtok(NULL, " ");
    operand_str = strtok(NULL, " ");
    operand = strtol(operand_str, NULL, 0);
    if (operand > INT32_MAX) {
      fprintf(stderr, 
          "operand (%ld) exceeds max data size for baby registers (%d)\n",
          operand,
          INT32_MAX);
      return 1;
    }
    if (writeinstr(mnemonic, (int32_t)operand, asmOut[iter])) {
      fprintf(stderr, "failed to write instruction at line %zu\n", iter); 
      return 1;
    }
    iter++;
  }

  return 0;
}


int main(int argc, char* argv[argc + 1]) {
  if (argc < 2) {
    fprintf(stderr, "please provide assembly filename as only argument\n");
    return EXIT_FAILURE;
  }
  FILE* fp = fopen(argv[1], "r");
  if (!fp) {
    err(EXIT_FAILURE, "could not open file: %s\n", argv[1]);
  }

  char asmOut[OUTPUT_FILE_LC][INSTR_WIDTH + 1] = {0};
  if (assemble(fp, asmOut)) {
    if (fclose(fp))
      err(EXIT_FAILURE, "could not close file: %s\n", argv[1]);

    fprintf(stderr, "could not write machine code\n");
    return EXIT_FAILURE;
  }

  if (fclose(fp))
    err(EXIT_FAILURE, "could not close file: %s\n", argv[1]);

  return EXIT_SUCCESS;
}
