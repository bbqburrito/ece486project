#include "pdp11.h"

using namespace std;


/*reads the file by line and returns the command by long
 * takes char pointer to store disposition of line
 */
long line_reader(char * filename, char * disposition)
{
    char address[100];
    char * endptr;
    long data;

    ifstream filein;

    filein.open(filename);

    if(!filein.is_open())
    {
        cout << "file not found\n";
        return 0;
    }

    filein.getline(address, 100);

    while((strlen(address) != 7) && !filein.eof()) //if line length is not 7
                                                   //and file is not ended
    {
        filein.getline(address, 100);
    }

    *disposition = address[0];          //get disposition

    address[0] = '0';

    data = strtol(address, &endptr, 8); //convert char to long
                                        //by octal
    return data;
}


int interpreter(int to_interpret, int * firstbit, command *& new_command)
{
    int i;
    string instruction;
    int code;
    //int firstbit;
    int bit1214;
    int bit0911;
    int bit0608;
    int bit0305;
    int bit0002;
    double_operand * make_command;

    //first bit indicates whether byte instruction
    //for some instructions
    *firstbit = to_interpret >> 15;

    cout << *firstbit << endl;
    
    //next 3 bits indicate function code for double 
    //operand commands
    bit1214 = (to_interpret >> 12) - *firstbit*8;

    cout << bit1214 << endl;

    //next 3 bits indicate more of function code for 
    //extended instruction set
    bit0911 = (to_interpret >> 9) - *firstbit*pow(8, 2) - bit1214*8;

    cout << bit0911 << endl;

    //next 3 bits indicate function code for 
    //single instruction set
    bit0608 = (to_interpret >> 6) - *firstbit*pow(8, 3) - bit1214*pow(8, 2) - bit0911*8;

    cout << (to_interpret >> 6) << " - " << *firstbit*512 << " - " << bit1214*64 << " - " << bit0911*8 << endl;

    cout << bit0608 << endl;

    //next 3 bits indicate function code for
    //most control functions
    bit0305 = (to_interpret >> 3)- *firstbit*pow(8, 4) - bit1214*pow(8, 3) - bit0911*64 - bit0608*8;

    cout << bit0305 << endl;

    //next 3 bits indicate function code for
    //some control functions
    bit0002 = to_interpret - *firstbit*pow(8, 5) - bit1214*pow(8, 4) - bit0911*pow(8, 3) - bit0608*64 - bit0305*8;

    cout << bit0002 << endl;

    if(!((to_interpret & ADD) ^ ADD))
    {
        instruction = "ADD";
        code = ADD;
        cout << "ADD" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        new_command->disp();
        //new_command = make_command;
    }
    else if(!((to_interpret & CLR) ^ CLR))
    {
        instruction = "CLR";
        code = CLR;
        cout << "CLR" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & CLRB) ^ CLRB))
    {
        instruction = "CLRB";
        code = CLRB;
        cout << "CLRB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & COM) ^ COM))
    {
        instruction = "COM";
        code = COM;
        cout << "COM" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
        else if(!((to_interpret & COMB) ^ COMB0))
    {
        instruction = "COMB";
        code = COMB;
        cout << "COMB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & INC) ^ INC))
    {
        instruction = "INC";
        code = INC;
        cout << "INC" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & INCB) ^ INCB))
    {
        instruction = "INCB";
        code = INCB;
        cout << "INCB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & DEC) ^ DEC))
    {
        instruction = "DEC";
        code = DEC;
        cout << "DEC" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & DECB) ^ DECB))
    {
        instruction = "DECB";
        code = DECB;
        cout << "DECB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & NEG) ^ NEG))
    {
        instruction = "NEG";
        code = NEG;
        cout << "NEG" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & NEGB) ^ NEGB))
    {
        instruction = "NEGB";
        code = NEGB;
        cout << "NEGB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & TST) ^ TST))
    {
        instruction = "TST";
        code = TST;
        cout << "TST" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & TSTB) ^ TSTB))
    {
        instruction = "TSTB";
        code = TSTB;
        cout << "TSTB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & ASR) ^ ASR))
    {
        instruction = "ASR";
        code = ASR;
        cout << "ASR" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & ASRB) ^ ASRB))
    {
        instruction = "ASRB";
        code = ASRB;
        cout << "ASRB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & ASL) ^ ASL))
    {
        instruction = "ASL";
        code = ASL;
        cout << "ASL" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & ASLB) ^ ASLB))
    {
        instruction = "ASLB";
        code = ASLB;
        cout << "ASLB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & ROR) ^ ROR))
    {
        instruction = "ROR";
        code = ROR;
        cout << "ROR" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & RORB) ^ RORB))
    {
        instruction = "RORB";
        code = RORB;
        cout << "RORB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & ROL) ^ ROL))
    {
        instruction = "ROL";
        code = ROL;
        cout << "ROL" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & ROLB) ^ ROLB))
    {
        instruction = "ROLB";
        code = ROLB;
        cout << "ROLB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
     else if(!((to_interpret & SWAB) ^ SWAB))
    {
        instruction = "SWAB";
        code = SWAB;
        cout << "SWAB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
     else if(!((to_interpret & ADC) ^ ADC))
    {
        instruction = "ADC";
        code = ADC;
        cout << "ADC" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & ADCB) ^ ADCB))
    {
        instruction = "ADCB";
        code = ADCB;
        cout << "ADCB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & SBC) ^ SBC))
    {
        instruction = "SBC";
        code = SBC;
        cout << "SBC" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & SBCB) ^ SBCB))
    {
        instruction = "SBCB";
        code = SBCB;
        cout << "SBCB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & SXT) ^ SXT))
    {
        instruction = "SXT";
        code = SXT;
        cout << "SXT" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & MOV) ^ MOV))
    {
        instruction = "MOV";
        code = MOV;
        cout << "MOV" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & MOVB) ^ MOVB))
    {
        instruction = "MOVB";
        code = MOVB;
        cout << "MOVB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & CMP) ^ CMP))
    {
        instruction = "CMP";
        code = CMP;
        cout << "CMP" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }
    else if(!((to_interpret & CMPB) ^ CMPB))
    {
        instruction = "CMPB";
        code = CMPB;
        cout << "CMPB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        new_command->disp();
    }

    else if(!((to_interpret & SUB) ^ SUB))
    {
        instruction = "SUB";
        code = SUB;
        cout << "SUB" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        new_command->disp();
        //new_command = make_command;
    }

    else if(!((to_interpret & BIT) ^ BIT))
    {
        instruction = "BIT";
        code = BIT;
        cout << "BIT" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        new_command->disp();
        //new_command = make_command;
    }

    else if(!((to_interpret & BITB) ^ BITB))
    {
        instruction = "BITB";
        code =  BITB;
        cout << "BITB" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        new_command->disp();
        //new_command = make_command;
    }

    else if(!((to_interpret & BIC) ^ BIC))
    {
        instruction = "BIC";
        code = BIC;
        cout << "BIC" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        new_command->disp();
        //new_command = make_command;
    }

    else if(!((to_interpret & BICB) ^ BICB))
    {
        instruction = "BICB";
        code = BICB;
        cout << "BICB" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        new_command->disp();
        //new_command = make_command;
    }

    else if(!((to_interpret & BIS) ^ BIS))
    {
        instruction = "BIS";
        code = BIS;
        cout << "BIS" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        new_command->disp();
        //new_command = make_command;
    }

    else if(!((to_interpret & BISB) ^ BISB))
    {
        instruction = "BISB";
        code = BISB;
        cout << "BISB" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        new_command->disp();
        //new_command = make_command;
    }
    
    else if(!((to_interpret & BR) ^ BR))
    {
        instruction = "BR";
        code = BR;
        cout << "BR" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }
    
    else if(!((to_interpret & BNE) ^ BNE))
    {
        instruction = "BNE";
        code = BNE;
        cout << "BNE" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }
    
    else if(!((to_interpret & BEQ) ^ BEQ))
    {
        instruction = "BEQ";
        code = BEQ;
        cout << "BEQ" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }
    
    else if(!((to_interpret & BPL) ^ BPL))
    {
        instruction = "BPL";
        code = BPL;
        cout << "BPL" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }
    
    else if(!((to_interpret & BMI) ^ BMI))
    {
        instruction = "BMI";
        code = BMI;
        cout << "BMI" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }
    
    else if(!((to_interpret & BVC) ^ BVC))
    {
        instruction = "BVC";
        code = BVC;
        cout << "BVC" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }
    
    else if(!((to_interpret & BVS) ^ BVS))
    {
        instruction = "BVS";
        code = BVS;
        cout << "BVS" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }
    
    else if(!((to_interpret & BCC) ^ BCC))
    {
        instruction = "BCC";
        code = BCC;
        cout << "BCC" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }
    
    else if(!((to_interpret & BCS) ^ BCS))
    {
        instruction = "BCS";
        code = BCS;
        cout << "BCS" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }
    
    else if(!((to_interpret & BGE) ^ BGE))
    {
        instruction = "BGE";
        code = BGE;
        cout << "BGE" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }
    
    else if(!((to_interpret & BLT) ^ BLT))
    {
        instruction = "BLT";
        code = BLT;
        cout << "BLT" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }
    
    else if(!((to_interpret & BGT) ^ BGT))
    {
        instruction = "BGT";
        code = BGT;
        cout << "BGT" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }
    
    else if(!((to_interpret & BLE) ^ BLE))
    {
        instruction = "BLE";
        code = BLE;
        cout << "BLE" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }
    
    else if(!((to_interpret & BHI) ^ BHI))
    {
        instruction = "BHI";
        code = BHI;
        cout << "BHI" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }
    
    else if(!((to_interpret & BLOS) ^ BLOS))
    {
        instruction = "BLOS";
        code = BLOS;
        cout << "BLOS" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0503 * 8 + bit0002);
        new_command->disp();
    }

    else if(!((to_interpret & JMP) ^ JMP))
    {
        instruction = "JMP";
        code = JMP;
        cout << "JMP" << endl;
        new_command = new jump_sub(to_interpret, '-', code, 0, bit0503, bit0002, 0);
        
        new_command->disp();
    }

    else 
    {
        instruction = "nope";
        code = -1;
        cout << "nope\n";
    }

    return code;
}

//command
command::command(): data(0), disposition(0)
{
}

command::command(int to_data, char to_disposition): data(to_data), disposition(to_disposition)
{
}

void command::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
}

//jump_sub
jump_sub::jump_sub(): command(), function_code(0), linkage_reg(0), destination_code(0), destination(0), parameters(0)
{
}

jump_sub::jump_sub(int to_data, char to_disposition, int to_function_code, int to_linkage_reg, int to_destination_code, int to_destination, int to_parameters): command(to_data, to_disposition), function_code(to_function_code), linkage_reg(to_linkage_reg), destination_code(to_destination_code), destination(to_destination), parameters(to_parameters)
{
}

void jump_sub()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
    cout << "function_code: " << function_code << endl;
    cout << "linkage_reg: " << linkage_reg << endl;
    cout << "destination_code: " << destination_code << endl;
    cout << "destination: " << destination << endl;
    cout << "parameters: " << parameters << endl;
}

//branch
branch::branch(): command(), function_code(0), offset(0)
{
}

branch::branch(int to_data, char to_disposition, int to_function_code, int to_offset): command(to_data, to_disposition), function_code(to_function_code), offset(to_offset)
{
}

void branch::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
    cout << "function code: " << function_code << endl;
    cout << "offset: " << offset << endl;
}

//single operand
single_operand::single_operand(): command(), function_code(0), destination_mode(0), destination(0)
{
}

single_operand::single_operand(int to_data, char to_disposition, int to_function_code, int to_destination_mode, int to_destination): command(to_data, to_disposition), function_code(to_function_code), destination(to_destination), destination_mode(to_destination_mode)
{
}

void single_operand::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
    cout << "function code: " << function_code << endl;
    cout << "destination_mode: " << destination_mode << endl;
    cout << "destination: " << destination << endl;
}

//double_operand
double_operand::double_operand(): command(), function_code(0), source(0), source_mode(0), destination(0), destination_mode(0)
{
}

double_operand::double_operand(int to_data, char to_disposition, int to_function_code, int to_source_mode, int to_source, int to_destination_mode, int to_destination): command(to_data, to_disposition), function_code(to_function_code), source(to_source), source_mode(to_source_mode), destination(to_destination), destination_mode(to_destination_mode)
{
}

void double_operand::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
    cout << "function_code: " << function_code << endl;
    cout << "source_mode: " << source_mode << endl;
    cout << "source: " << source << endl;
    cout << "destination_mode: " << destination_mode << endl;
    cout << "destination: " << destination << endl;
}


int main(int argc, char* argv[])
{
    int firstbit;
    string instruction;
    int to_interpret;
    command * new_command;

    to_interpret = atoi(argv[1]);

    instruction = interpreter(to_interpret, &firstbit, new_command);

    new_command->disp();

    return 0;
}




    /*switch(bit1214)             //check 3 most significant bits
    {
        case 1: instruction = "MOV";
                break;

        case 2: instruction = "CMP";
                break;

        case 3: instruction = "BIT";
                break;

        case 4: instruction = "BIC";
                break;

        case 5: instruction = "BIS";
                break;
    
        case 6: if(*firstbit)
                {
                    instruction = "SUB";
                }
                else instruction = "ADD";
                break;

        case 7: switch(bit0911)         //extended instruction set
                {
                    case 0: instruction = "MUL";
                            break;
                            
                    case 1: instruction = "DIV";
                            break;

                    case 2: instruction = "ASH";
                            break;

                    case 3: instruction = "ASHC";
                            break;

                    case 4: instruction = "XOR";
                            break;

                    //case 7: instruction = "SOB";          //not in 11/20 ISA

                    case default: instruction = "invalid";
                                  break;
                }
                break;

        case 0: switch(bit0911)         //all other commands
                {
                    case 1: switch(*firstbit)
                            {
                                case 0: if(!bit0608)
                                            instruction = "BNE";
                                        else if(bit608 == 4)
                                            instruction = "BEQ";
                                        else instruction = "invalid";
                                        break;

                                case 1: if(!bit0608)
                                            instruction = "BHI";
                                        else if(bit0608 == 4)
                                            instruction = "BLOS";
                                        else instruction = "invalid";
                                        break;
                            }
                            break;

                    case 2: switch(*firstbit)
                            {
                                case 0: if(!bit0608)
                                            instruction = "BGE";
                                        else if(bit0608)
                                            instruction = "BLT";
                                        else instruction = "invalid"
                                            */




