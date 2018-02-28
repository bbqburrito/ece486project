#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <ctime>
#include <getopt.h>


const int CLR = 2560; //005000
const int CLRB = 35328; //105000
const int COM = 2624; //005100
const int COMB = 35392; //105100
const int INC = 2688; //005200
const int INCB = 35456; //105200
const int DEC = 2752; //005300
const int DECB = 35520; //105300
const int NEG = 2816; //005400
const int NEGB = 35584; //105400
const int TST = 3008; //005700
const int TSTB = 35776; //105700
const int ASR = 3200; //006200
const int ASRB = 35968; //106200
const int ASL = 3264; //006300
const int ASLB = 36032; //106300
const int ROR = 3072; //006000
const int RORB = 35840; //106000
const int ROL = 3136; //006100
const int ROLB = 35904; //106100
const int SWAB = 192; //000300
const int ADC = 2880; //005500
const int ADCB = 35648; //105500
const int SBC = 2944; //005600
const int SBCB = 35712; //105600
const int SXT = 3520; //006700
const int MOV = 4096; //010000
const int MOVB = 36864; //110000
const int CMP = 8192; //020000
const int CMPB = 40960; //120000
const int ADD = 24576; //060000
const int SUB = 57344; //160000
const int BIT = 12288; //030000
const int BITB = 45056; //130000
const int BIC = 16384; //040000
const int BICB = 49152; //140000
const int BIS = 20480; //050000
const int BISB = 53248; //150000
const int BR = 256; //000400
const int BNE = 512; //001000
const int BEQ = 768; //001400
const int BPL = 32768; //100000
const int BMI = 33024; //100400
const int BVC = 33792; //102000
const int BVS = 34048; //102400
const int BCC = 34304; //103000
const int BCS = 34560; //103400
const int BGE = 1024; //002000
const int BLT = 1280; //002400
const int BGT = 1536; //003000
const int BLE = 1792; //003400
const int BHI = 33280; //101000
const int BLOS = 33536; //101400
const int JMP = 64; //000100
const int JSR = 2048; //004000
const int RTS = 128; //000200
const int TRAP = 34816; //104000
const int RTI = 2; //000002
const int HALT = 0; //000000
const int WAIT = 1; //000001
const int RESET = 5; //000005
const int CLC = 161; //000241
const int CLV = 162; //000242
const int CLZ = 164; //000244
const int CLN = 168; //000250
const int CCC = 175; //000257
const int SEC = 177; //000261
const int SEV = 178; //000262
const int SEZ = 180; //000264
const int SEN = 184; //000270
const int SCC = 191; //000277
const int NOP = 160; //000240
const int MUL = 28672; //070000
const int DIV = 29184; //071000
const int ASH = 29696; //072000
const int ASHC = 30208; //073000
const int XOR = 30720; //074000


const int I_SIZE = 32768;
const int MEM_SIZE = 32768;



struct i_cache {

    int data;
    char disposition;
};

class CPSR {
    public:
        CPSR();
        ~CPSR();
        CPSR(const CPSR &to_copy);

        int get_condition() const;
        int set_condition(int to_set);
        int get_priority() const;
        int set_priority(int to_set);
        void display();

    protected:
        int T;
        int N;
        int Z;
        int V;
        int C;
        int priority;
};



class command {

    public:
        command();
        command(int to_data, char to_disposition, char * to_tracefile);
        ~command();
        command (const command &to_copy);

        virtual void disp();
        virtual int instruction(int *regs, CPSR * states, i_cache *program);
        virtual int instructionB(int *regs, CPSR * states, i_cache *program);



    protected:
        int data;
        char disposition;
        char * tracefile;
        int program_counter;
};



class double_operand: public command
{
    public:
        double_operand();
        double_operand(int to_data);
        double_operand(int to_data, char to_disposition, char * tracefile, int to_function_code, int to_source_mode,
                       int to_source, int to_destination_mode, int to_destination);
        ~double_operand();
        double_operand(const double_operand &to_copy);

        void disp();
        int instruction(int *regs, CPSR * states, i_cache *program);
        //int instructionB(int *regs, CPSR * states, i_cache *program);




    protected:
        int function_code;
        int source_mode;
        int source;
        int destination_mode;
        int destination;
};

class single_operand: public command
{
    public:
        single_operand();
        single_operand(int to_data);
        single_operand(int to_data, char to_disposition, char * tracefile, int to_function_code,
                       int to_destination_mode, int to_destination);
        ~single_operand();
        single_operand(const single_operand &to_copy);
        void disp();
        int instruction(int *regs, CPSR *states, i_cache *program);
        //int instructionB(int *regs, CPSR * states, i_cache *program);


    protected:
        int function_code;
        int destination_mode;
        int destination;
};


class extended: public command
{
    public:
        extended();
        extended(int to_data);
        extended(int to_data, char to_disposition, char * tracefile, int to_function_code, int to_destination,
                 int to_source_code, int to_source);
        ~extended();
        extended(const extended &to_copy);
        void disp();
        int instruction(int *regs, CPSR * states, i_cache *program);
        //int instructionB(int *regs, CPSR * states, i_cache *program);


    protected:
        int function_code;
        int destination;
        int source_mode;
        int source;
};


class branch: public command
{
    public:
        branch();
        branch(int to_data);
        branch(int to_data, char disposition, char * tracefile, int to_function_code, int to_offset);
        ~branch();
        branch(const branch &to_copy);
        void disp();
        int instruction(int *regs, CPSR * states, i_cache *program);
        //int instructionB(int *regs, CPSR * states, i_cache *program);


    protected:
        int function_code;
        int offset;
};


class jump_sub: public command
{
    public:
        jump_sub();
        jump_sub(int to_data);
        jump_sub(int to_data, char to_disposition, char * tracefile, int to_function_code, int to_linkage_reg,
                 int to_destination_code, int to_destination, int to_parameters);
        ~jump_sub();
        jump_sub(const jump_sub &to_copy);
        void disp();
        int instruction(int *regs, CPSR * states, i_cache *program);
        //int instructionB(int *regs, CPSR * states, i_cache *program);


    protected:
        int function_code;
        int linkage_reg;
        int destination_code;
        int destination;
        int parameters;

};


class trapIntMiscCond: public command
{
    public:
        trapIntMiscCond();
        trapIntMiscCond(int to_data);
        trapIntMiscCond(int to_data, char to_disposition, char * tracefile, int to_function_code, int to_trap_code);
        ~trapIntMiscCond();
        trapIntMiscCond(const trapIntMiscCond &to_copy);
        void disp();
        int instruction(int *regs, CPSR * states, i_cache *program);
        //int instructionB(int *regs, CPSR * states, i_cache *program);


    protected:
        int function_code;
        int trap_code;
};



int findstart(i_cache * prog_mem, int prog_length);
long line_reader(char * filename, char *& disposition, int & filepos);
int trace_file(char * filename, int type, int address);
int interpreter(int to_interpret, int * firstbit, command *& new_command, char * tracefile);







