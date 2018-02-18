#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>

#define CLR 2560 //005000
#define CLRB 35328 //105000
#define COM 2624 //005100
#define COMB 35392 //105100
#define INC 2688 //005200
#define INCB 35456 //105200
#define DEC 2752 //005300
#define DECB 35520 //105300
#define NEG 2816 //005400
#define NEGB 35584 //105400
#define TST 3008 //005700
#define TSTB 35776 //105700
#define ASR 3200 //006200
#define ASRB 35968 //106200
#define ASL 3264 //006300
#define ASLB 36032 //106300
#define ROR 3072 //006000
#define RORB 35840 //106000
#define ROL 3136 //006100
#define ROLB 35904 //106100
#define SWAB 192 //000300
#define ADC 2880 //005500
#define ADCB 35648 //105500
#define SBC 2944 //005600
#define SBCB 35712 //105600
#define SXT 3520 //006700
#define MOV 512 //01000
#define MOVB 36864 //110000
#define CMP 8192 //020000
#define CMPB 40960 //120000
#define ADD 24576 //060000
#define SUB 57344 //160000
#define BIT 12288 //030000
#define BITB 45056 //130000
#define BIC 16384 //040000
#define BICB 49152 //140000
#define BIS 20480 //050000
#define BISB 53248 //150000
#define BR 256 //000400
#define BNE 512 //001000
#define BEQ 768 //001400
#define BPL 32768 //100000
#define BMI 33024 //100400
#define BVC 33792 //102000
#define BVS 34048 //102400
#define BCC 34304 //103000
#define BCS 34560 //103400
#define BGE 1024 //002000
#define BLT 1280 //002400
#define BGT 1536 //003000
#define BLE 1792 //003400
#define BHI 33280 //101000
#define BLOS 33536 //101400
#define JMP 64 //000100
#define JSP 2048 //004000
#define RTS 128 //000200
#define TRAP 34816 //104000
#define RTI 2 //000002
#define HALT 0 //000000
#define WAIT 1 //000001
#define RESET 5 //000005
#define CLC 161 //000241
#define CLV 162 //000242
#define CLZ 164 //000244
#define CLN 168 //000250
#define CCC 175 //000257
#define SEC 177 //000261
#define SEV 178 //000262
#define SEZ 180 //000264
#define SEN 184 //000270
#define SCC 191 //000277
#define NOP 160 //000240
#define MUL 28672 //070000
#define DIV 29184 //071000
#define ASH 29696 //072000
#define ASHC 30208 //073000
#define XOR 30720 //074000


class command {

    public:
        command();
        command(int to_data, char to_disposition);
        ~command();
        command (const command &to_copy);

        virtual void disp();


    protected:
        int data;
        char disposition;
};



class double_operand: public command
{
    public:
        double_operand();
        double_operand(int to_data);
        double_operand(int to_data, char to_disposition, int to_function_code, int to_source_mode, int to_source, int to_destination_mode, int to_destination);
        ~double_operand();
        double_operand(const double_operand &to_copy);

        void disp();




    protected:
        int function_code;
        int source;
        int source_mode;
        int destination;
        int destination_mode;
};

class single_operand: public command
{
    public:
        single_operand();
        single_operand(int to_data);
        single_operand(int to_data, char to_disposition, int to_function_code, int to_destination_mode, int to_destination);
        ~single_operand();
        single_operand(const single_operand &to_copy);
        void disp();

    protected:
        int function_code;
        int destination;
        int destination_mode;
};


class extended: public command
{
    public:
        extended();
        extended(int to_data);
        ~extended();
        extended(const extended &to_copy);

    protected:
        int function_code;
        int destination;
        int source;
        int source_mode;
};


class branch: public command
{
    public:
        branch();
        branch(int to_data);
        branch(int to_data, char disposition, int to_function_code, int to_offset);
        ~branch();
        branch(const branch &to_copy);
        void disp();

    protected:
        int function_code;
        int offset;
};


class jump_sub: public command
{
    public:
        jump_sub();
        jump_sub(int to_data);
        jump_sub(int to_data, char to_disposition, int to_function_code, int to_linkage_reg, int to_destination_code, int to_destination, int to_parameters);
        ~jump_sub();
        jump_sub(const jump_sub &to_copy);
        void disp();

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
        ~trapIntMiscCond();
        trapIntMiscCond(const trapIntMiscCond &to_copy);

    protected:
        int function_code;
};



