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

    else 
    {
        instruction = "nope";
        code = 0;
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

//double_operand
double_operand::double_operand(): function_code(0), source(0), source_mode(0), destination(0), destination_mode(0)
{
}

double_operand::double_operand(int to_data, char disposition, int to_function_code, int to_source_mode, int to_source, int to_destination_mode, int to_destination): command(to_data, disposition), function_code(to_function_code), source(to_source), source_mode(to_source_mode), destination(to_destination), destination_mode(to_destination_mode)
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




