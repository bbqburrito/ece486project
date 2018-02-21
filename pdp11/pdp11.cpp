#include "pdp11.h"

using namespace std;


/*reads the file by line and returns the command by long
 * takes char pointer to store disposition of line
 */
long line_reader(char * filename, char *& disposition, int & filepos)
{
    char address[100];
    char * endptr;
    long data;

    ifstream filein;

    for(int i = 0; i < 100; ++i)
    {
        address[i] = 0;
    }

    filein.open(filename);

    if(!filein.is_open())
    {
        cout << "file not found\n";
        return 0;
    }

    filein.seekg(filepos);      //maintain file position

    filein.getline(address, 100);

    //while((strlen(address) != 7) && !filein.eof()) //if line length is not 7
                                                   //and file is not ended
    //{
    //    filein.getline(address, 100);
    //}

    if(filein.eof())
    {
        *disposition = '\0';
        data = -1;
        filepos = filein.tellg();   //pass file position.

        for(int i = 0; i < 100; ++i)
        {
            address[i] = '\0';
        }

        filein.close();
        return data;
    }

    while((strlen(address) != 7))
    {
        if(filein.eof())
        {
            *disposition = '\0';
            data = -1;
            filepos = filein.tellg();   //pass file position

            for(int i = 0; i < 100; ++i)
            {
                address[i] = '\0';
            }

            filein.close();
            return data;
        }

        for(int i = 0; i < 100; ++i)
        {
            address[i] = '\0';          //reset address
        }

        filein.getline(address, 100);
    }
    
    *disposition = address[0];          //get disposition
    address[0] = '0';                   //set first char to 0

    for(int i = 0; i < 7; ++i)
    {
        if(address[i] > '7')
            address[i] = '0';
    }

    data = strtol(address, &endptr, 8); //convert char to long
                                            //by octal
    //data = strtol(address, &endptr, 10); //convert char to long
    filepos = filein.tellg();           //pass file position

    filein.close();
        
    return data;
}

//takes int as a command to parse, an int pointer to pass firstbit, and a command 
//pointer to assign to command class object by upcasting. returns an int representing
//the function code of passed command. parses command, then creates an object of 
//class type and stores in new_command pointer
int interpreter(int to_interpret, int * firstbit, command *& new_command)
{
    //int i;
    string instruction;
    int code;
    //int firstbit;
    int bit1214;
    int bit0911;
    int bit0608;
    int bit0305;
    int bit0002;
    //double_operand * make_command;

    //first bit indicates whether byte instruction
    //for some instructions
    *firstbit = to_interpret >> 15;

    //cout << *firstbit << endl;
    
    //next 3 bits indicate function code for double 
    //operand commands
    bit1214 = (to_interpret >> 12) - *firstbit*8;

    //cout << bit1214 << endl;

    //next 3 bits indicate more of function code for 
    //extended instruction set
    bit0911 = (to_interpret >> 9) - *firstbit*pow(8, 2) - bit1214*8;

    //cout << bit0911 << endl;

    //next 3 bits indicate function code for 
    //single instruction set
    bit0608 = (to_interpret >> 6) - *firstbit*pow(8, 3) - bit1214*pow(8, 2) - bit0911*8;

    //cout << (to_interpret >> 6) << " - " << *firstbit*512 << " - " << bit1214*64 << " - " << bit0911*8 << endl;

    //cout << bit0608 << endl;

    //next 3 bits indicate function code for
    //most control functions
    bit0305 = (to_interpret >> 3)- *firstbit*pow(8, 4) - bit1214*pow(8, 3) - bit0911*64 - bit0608*8;

    //cout << bit0305 << endl;

    //next 3 bits indicate function code for
    //some control functions
    bit0002 = to_interpret - *firstbit*pow(8, 5) - bit1214*pow(8, 4) - bit0911*pow(8, 3) - bit0608*64 - bit0305*8;

    //cout << bit0002 << endl;

    if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ int(ADD)))
    {
        instruction = "ADD";
        code = ADD;
        //cout << "ADD" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(CLR)))
    {
        instruction = "CLR";
        code = CLR;
        //cout << "CLR" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(CLRB)))
    {
        instruction = "CLRB";
        code = CLRB;
        //cout << "CLRB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(COM)))
    {
        instruction = "COM";
        code = COM;
        //cout << "COM" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(COMB)))
    {
        instruction = "COMB";
        code = COMB;
        //cout << "COMB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(INC)))
    {
        instruction = "INC";
        code = INC;
        //cout << "INC" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(INCB)))
    {
        instruction = "INCB";
        code = INCB;
        //cout << "INCB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(DEC)))
    {
        instruction = "DEC";
        code = DEC;
        //cout << "DEC" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(DECB)))
    {
        instruction = "DECB";
        code = DECB;
        //cout << "DECB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(NEG)))
    {
        instruction = "NEG";
        code = NEG;
        //cout << "NEG" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(NEGB)))
    {
        instruction = "NEGB";
        code = NEGB;
        //cout << "NEGB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(TST)))
    {
        instruction = "TST";
        code = TST;
        //cout << "TST" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(TSTB)))
    {
        instruction = "TSTB";
        code = TSTB;
        //cout << "TSTB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(ASR)))
    {
        instruction = "ASR";
        code = ASR;
        //cout << "ASR" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(ASRB)))
    {
        instruction = "ASRB";
        code = ASRB;
        //cout << "ASRB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(ASL)))
    {
        instruction = "ASL";
        code = ASL;
        //cout << "ASL" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(ASLB)))
    {
        instruction = "ASLB";
        code = ASLB;
        //cout << "ASLB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(ROR)))
    {
        instruction = "ROR";
        code = ROR;
        //cout << "ROR" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(RORB)))
    {
        instruction = "RORB";
        code = RORB;
        //cout << "RORB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(ROL)))
    {
        instruction = "ROL";
        code = ROL;
        //cout << "ROL" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(ROLB)))
    {
        instruction = "ROLB";
        code = ROLB;
        //cout << "ROLB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(SWAB)))
    {
        instruction = "SWAB";
        code = SWAB;
        //cout << "SWAB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(ADC)))
    {
        instruction = "ADC";
        code = ADC;
        //cout << "ADC" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(ADCB)))
    {
        instruction = "ADCB";
        code = ADCB;
        //cout << "ADCB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(SBC)))
    {
        instruction = "SBC";
        code = SBC;
        //cout << "SBC" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(SBCB)))
    {
        instruction = "SBCB";
        code = SBCB;
        //cout << "SBCB" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(SXT)))
    {
        instruction = "SXT";
        code = SXT;
        //cout << "SXT" << endl;
        new_command = new single_operand(to_interpret, '-', code, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ int(MOV)))
    {
        instruction = "MOV";
        code = MOV;
        //cout << "MOV" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ int(MOVB)))
    {
        instruction = "MOVB";
        code = MOVB;
        //cout << "MOVB" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ int(CMP)))
    {
        instruction = "CMP";
        code = CMP;
        //cout << "CMP" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
    }
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ int(CMPB)))
    {
        instruction = "CMPB";
        code = CMPB;
        //cout << "CMPB" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ int(SUB)))
    {
        instruction = "SUB";
        code = SUB;
        //cout << "SUB" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ int(BIT)))
    {
        instruction = "BIT";
        code = BIT;
        //cout << "BIT" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ int(BITB)))
    {
        instruction = "BITB";
        code =  BITB;
        //cout << "BITB" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ int(BIC)))
    {
        instruction = "BIC";
        code = BIC;
        //cout << "BIC" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ int(BICB)))
    {
        instruction = "BICB";
        code = BICB;
        //cout << "BICB" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ int(BIS)))
    {
        instruction = "BIS";
        code = BIS;
        //cout << "BIS" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4)) ^ int(BISB)))
    {
        instruction = "BISB";
        code = BISB;
        //cout << "BISB" << endl;
        new_command = new double_operand(to_interpret, '-', code, bit0911, bit0608, bit0305, bit0002);

        //new_command->disp();
        //new_command = make_command;
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BR)))
    {
        instruction = "BR";
        code = BR;
        //cout << "BR" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BNE)))
    {
        instruction = "BNE";
        code = BNE;
        //cout << "BNE" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BEQ)))
    {
        instruction = "BEQ";
        code = BEQ;
        //cout << "BEQ" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BPL)))
    {
        instruction = "BPL";
        code = BPL;
        //cout << "BPL" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BMI)))
    {
        instruction = "BMI";
        code = BMI;
        //cout << "BMI" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BVC)))
    {
        instruction = "BVC";
        code = BVC;
        //cout << "BVC" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BVS)))
    {
        instruction = "BVS";
        code = BVS;
        //cout << "BVS" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BCC)))
    {
        instruction = "BCC";
        code = BCC;
        //cout << "BCC" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BCS)))
    {
        instruction = "BCS";
        code = BCS;
        //cout << "BCS" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BGE)))
    {
        instruction = "BGE";
        code = BGE;
        //cout << "BGE" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BLT)))
    {
        instruction = "BLT";
        code = BLT;
        //cout << "BLT" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BGT)))
    {
        instruction = "BGT";
        code = BGT;
        //cout << "BGT" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BLE)))
    {
        instruction = "BLE";
        code = BLE;
        //cout << "BLE" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BHI)))
    {
        instruction = "BHI";
        code = BHI;
        //cout << "BHI" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }
    
    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + (bit0608 & 4)*pow(8, 2)) ^ int(BLOS)))
    {
        instruction = "BLOS";
        code = BLOS;
        //cout << "BLOS" << endl;
        new_command = new branch(to_interpret, '-', code, (bit0608 & 3) * 64 + bit0305 * 8 + bit0002);
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2)) ^ int(JMP)))
    {
        instruction = "JMP";
        code = JMP;
        //cout << "JMP" << endl;
        new_command = new jump_sub(to_interpret, '-', code, 0, bit0305, bit0002, 0);
        
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3)) ^ int(JSR)))
    {
        instruction = "JSR";
        code = JSR;
        //cout << "JSR" << endl;
        new_command = new jump_sub(to_interpret, '-', code, bit0608, bit0305, bit0002, 0);
        
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3) + bit0608*pow(8, 2) + bit0305*8) ^ int(RTS)))
    {
        instruction = "RTS";
        code = RTS;
        //cout << "RTS" << endl;
        new_command = new jump_sub(to_interpret, '-', code, bit0002, 0, 0, 0);
        
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3)) ^ int(TRAP)))
    {
        instruction = "TRAP";
        code = TRAP;
        //cout << "TRAP" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, bit0608*pow(8, 2) + bit0305*pow(8, 1) + bit0002);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(RTI)))
    {
        instruction = "RTI";
        code = RTI;
        //cout << "RTI" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(HALT)))
    {
        instruction = "HALT";
        code = HALT;
        //cout << "HALT" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(WAIT)))
    {
        instruction = "WAIT";
        code = WAIT;
        //cout << "WAIT" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(RESET)))
    {
        instruction = "RESET";
        code = RESET;
        //cout << "RESET" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(CLC)))
    {
        instruction = "CLC";
        code = CLC;
        //cout << "CLC" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(CLV)))
    {
        instruction = "CLV";
        code = RTI;
        //cout << "CLV" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(CLZ)))
    {
        instruction = "CLZ";
        code = CLZ;
        //cout << "CLZ" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(CLN)))
    {
        instruction = "CLN";
        code = CLN;
        //cout << "CLN" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(CCC)))
    {
        instruction = "CCC";
        code = CCC;
        //cout << "CCC" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(SEC)))
    {
        instruction = "SEC";
        code = SEC;
        //cout << "SEC" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(SEV)))
    {
        instruction = "SEV";
        code = SEV;
        //cout << "SEV" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(SEZ)))
    {
        instruction = "SEZ";
        code = SEZ;
        //cout << "SEZ" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(SEN)))
    {
        instruction = "SEN";
        code = SEN;
        //cout << "SEN" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(SCC)))
    {
        instruction = "SCC";
        code = SCC;
        //cout << "SCC" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(to_interpret) ^ int(NOP)))
    {
        instruction = "NOP";
        code = NOP;
        //cout << "NOP" << endl;
        new_command = new trapIntMiscCond(to_interpret, '-', code, 0);
        
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3)) ^ int(MUL)))
    {
        instruction = "MUL";
        code = MUL;
        //cout << "MUL" << endl;
        new_command = new extended(to_interpret, '-', code, bit0608, bit0305, bit0002);
        
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3)) ^ int(DIV)))
    {
        instruction = "DIV";
        code = DIV;
        //cout << "DIV" << endl;
        new_command = new extended(to_interpret, '-', code, bit0608, bit0305, bit0002);
        
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3)) ^ int(ASH)))
    {
        instruction = "ASH";
        code = ASH;
        //cout << "ASH" << endl;
        new_command = new extended(to_interpret, '-', code, bit0608, bit0305, bit0002);
        
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3)) ^ int(ASHC)))
    {
        instruction = "ASHC";
        code = ASHC;
        //cout << "ASHC" << endl;
        new_command = new extended(to_interpret, '-', code, bit0608, bit0305, bit0002);
        
        //new_command->disp();
    }

    else if(!(int(*firstbit*pow(8, 5) + bit1214*pow(8, 4) + bit0911*pow(8, 3)) ^ int(XOR)))
    {
        instruction = "XOR";
        code = XOR;
        //cout << "XOR" << endl;
        new_command = new extended(to_interpret, '-', code, bit0608, bit0305, bit0002);
        
        //new_command->disp();
    }

    else 
    {
        instruction = "nope";
        code = -1;
        cout << "instruction code: " << to_interpret << " nope\n";
        new_command = new command(to_interpret, '-');
    }

    return code;
}

//gp_register
gp_register::gp_register(): contents(0)
{
}

gp_register::~gp_register()
{
}

gp_register::gp_register(const gp_register &to_copy)
{
    contents = to_copy.get_gp();
    return;
}


void gp_register::display()
{
    cout << "contents: " << contents << endl;
}

int gp_register::set_gp(int to_set)
{
    contents = to_set;

    return contents;
}

int gp_register::get_gp() const
{
    return contents;
}

//CPSR
CPSR::CPSR(): T(0), C(0), N(0), V(0), Z(0), priority(0)
{
}

CPSR::~CPSR()
{
}

CPSR::CPSR(const CPSR &to_copy)
{
    int condition = to_copy.get_condition();
    priority = condition >> 5;
    T = (condition >> 4) - (priority << 1);
    N = (condition >> 3) - (T << 1) - (priority << 2);
    Z = (condition >> 2) - (N << 1) - (T << 2) - (priority << 3);
    V = (condition >> 1) - (Z << 1) - (N << 2) - (T << 3) - (priority << 4);
    C = condition - (V << 1) - (Z << 2) - (N << 3) - (T << 4) - (priority << 5);
}

int CPSR::get_condition() const
{
    int result = (priority << 5) + (T << 4) + (N << 3) + (Z << 2) + (V << 1) + C;

    return result;
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


int command::instruction(gp_register *regs, CPSR *states)
{
    return 0;
}

int command::instructionB(gp_register *regs, CPSR *states)
{
    return 0;
}

//extended
extended::extended(): command(), function_code(0), destination(0), source_mode(0), source(0)
{
}

extended::extended(int to_data, char disposition, int to_function_code, int to_destination, int to_source_mode, int to_source): command(to_data, disposition), function_code(to_function_code), source_mode(to_source_mode), source(to_source)
{
}

void extended::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
    cout << "function code: " << function_code << endl;
    cout << "source_mode: " << source_mode << endl;
    cout << "source: " << source << endl;
}

int extended::instruction(gp_register *regs, CPSR *states)
{

    cout << "function_code: " << function_code << ": ";
   switch(function_code)
    {
        case MUL:
            {
                cout << "MUL" << endl;
                break;
            }
        case DIV:
            {
                cout << "DIV" << endl;
                break;
            }
        case ASH:
            {
                cout << "ASH" << endl;
                break;
            }
        case ASHC:
            {
                cout << "ASHC" << endl;
                break;
            }
        case XOR:
            {
                cout << "XOR" << endl;
                break;
            }
        default:
            {
                cout << "invalid" << endl;
                break;
            }
    }

    return 0;
}

//trapIntMiscCond
trapIntMiscCond::trapIntMiscCond(): command(), function_code(0), trap_code(0)
{
}


trapIntMiscCond::trapIntMiscCond(int to_data, char to_disposition, int to_function_code, int to_trap_code): command(to_data, to_disposition), function_code(to_function_code), trap_code(to_trap_code)
{
}

void trapIntMiscCond::disp()
{
    cout << "data: " << data << endl;
    cout << "dispostion: " << disposition << endl;
    cout << "function code: " << function_code << endl;
    cout << "trap code: " << trap_code << endl;
}

int trapIntMiscCond::instruction(gp_register *regs, CPSR *states)
{

    cout << "function_code: " << function_code << ": ";
   switch(function_code)
    {
        case TRAP:
            {
                cout << "TRAP" << endl;
                break;
            }
        case RTI:
            {
                cout << "RTI" << endl;
                break;
            }
        case HALT:
            {
                cout << "HALT" << endl;
                break;
            }
        case WAIT:
            {
                cout << "WAIT" << endl;
                break;
            }
        case RESET:
            {
                cout << "RESET" << endl;
                break;
            }
        case CLC:
            {
                cout << "CLC" << endl;
                break;
            }
        case CLV:
            {
                cout << "CLV" << endl;
                break;
            }
        case CLZ:
            {
                cout << "CLZ" << endl;
                break;
            }
        case CLN:
            {
                cout << "CLN" << endl;
                break;
            }
        case CCC:
            {
                cout << "CCC" << endl;
                break;
            }
        case SEC:
            {
                cout << "SEC" << endl;
                break;
            }
        case SEV:
            {
                cout << "SEV" << endl;
                break;
            }
        case SEZ:
            {
                cout << "SEZ" << endl;
                break;
            }
        case SEN:
            {
                cout << "SEN" << endl;
                break;
            }
        case SCC:
            {
                cout << "SCC" << endl;
                break;
            }
        case NOP:
            {
                cout << "NOP" << endl;
                break;
            }
        default:
            {
                cout << "nope" << endl;
                break;
            }
    }
    return 0;
}

//jump_sub
jump_sub::jump_sub(): command(), function_code(0), linkage_reg(0), destination_code(0), destination(0), parameters(0)
{
}

jump_sub::jump_sub(int to_data, char to_disposition, int to_function_code, int to_linkage_reg, int to_destination_code, int to_destination, int to_parameters): command(to_data, to_disposition), function_code(to_function_code), linkage_reg(to_linkage_reg), destination_code(to_destination_code), destination(to_destination), parameters(to_parameters)
{
}

void jump_sub::disp()
{
    cout << "data: " << data << endl;
    cout << "disposition: " << disposition << endl;
    cout << "function_code: " << function_code << endl;
    cout << "linkage_reg: " << linkage_reg << endl;
    cout << "destination_code: " << destination_code << endl;
    cout << "destination: " << destination << endl;
    cout << "parameters: " << parameters << endl;
}

int jump_sub::instruction(gp_register *regs, CPSR *states)
{

    cout << "function_code: " << function_code << ": ";
   switch(function_code)
    {
        case JMP:
            {
                cout << "JMP" << endl;
                break;
            }
        case JSR:
            {
                cout << "JSR" << endl;
                break;
            }
        case RTS:
            {
                cout << "RTS" << endl;
                break;
            }
        default:
            {
                cout << "nope" << endl;
                break;
            }
    }

    return 0;
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

int branch::instruction(gp_register *regs, CPSR *states)
{

    cout << "function_code: " << function_code << ": ";
   switch(function_code)
    {
        case BR:
            {
                cout << "BR" << endl;
                break;
            }
        case BNE:
            {
                cout << "BNE" << endl;
                break;
            }
        case BEQ:
            {
                cout << "BEQ" << endl;
                break;
            }
        case BPL:
            {
                cout << "BPL" << endl;
                break;
            }
        case BMI:
            {
                cout << "BMI" << endl;
                break;
            }
        case BVC:
            {
                cout << "BVC" << endl;
                break;
            }
        case BVS:
            {
                cout << "BVS" << endl;
                break;
            }
        case BCC:
            {
                cout << "BCC" << endl;
                break;
            }
        case BCS:
            {
                cout << "BCS" << endl;
                break;
            }
        case BGE:
            {
                cout << "BGE" << endl;
                break;
            }
        case BLT:
            {
                cout << "BLT" << endl;
                break;
            }
        case BGT:
            {
                cout << "BGT" << endl;
                break;
            }
        case BLE:
            {
                cout << "BLE" << endl;
                break;
            }
        case BHI:
            {
                cout << "BHI" << endl;
                break;
            }
        case BLOS:
            {
                cout << "BLOS" << endl;
                break;
            }
        default:
            {
                cout << "nope" << endl;
                break;
            }
    }
    return 0;
}

//single operand
single_operand::single_operand(): command(), function_code(0), destination_mode(0), destination(0)
{
}

single_operand::single_operand(int to_data, char to_disposition, int to_function_code, int to_destination_mode, int to_destination): command(to_data, to_disposition), function_code(to_function_code), destination_mode(to_destination_mode), destination(to_destination)
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

int single_operand::instruction(gp_register *regs, CPSR *states)
{

    cout << "function_code: " << function_code << ": ";

    switch(function_code)
    {
        case CLR:
            {
                cout << "CLR" << endl;
                break;
            }
        case CLRB:
            {
                cout << "CLRB" << endl;
                break;
            }
        case COM:
            {
                cout << "COM" << endl;
                break;
            }
        case COMB:
            {
                cout << "COMB" << endl;
                break;
            }
        case INC:
            {
                cout << "INC" << endl;
                break;
            }
        case INCB:
            {
                cout << "INCB" << endl;
                break;
            }
        case DEC:
            {
                cout << "DEC" << endl;
                break;
            }
        case DECB:
            {
                cout << "DECB" << endl;
                break;
            }
        case NEG:
            {
                cout << "NEG" << endl;
                break;
            }
        case NEGB:
            {
                cout << "NEGB" << endl;
                break;
            }
        case TST:
            {
                cout << "TST" << endl;
                break;
            }
        case TSTB:
            {
                cout << "TSTB" << endl;
                break;
            }
        case ASR:
            {
                cout << "ASR" << endl;
                break;
            }
        case ASRB:
            {
                cout << "ASRB" << endl;
                break;
            }
        case ASL:
            {
                cout << "ASL" << endl;
                break;
            }
        case ASLB:
            {
                cout << "ASLB" << endl;
                break;
            }
        case ROR:
            {
                cout << "ROR" << endl;
                break;
            }
        case RORB:
            {
                cout << "RORB" << endl;
                break;
            }
        case ROL:
            {
                cout << "ROL" << endl;
                break;
            }
        case ROLB:
            {
                cout << "ROLB" << endl;
                break;
            }
        case SWAB:
            {
                cout << "SWAB" << endl;
                break;
            }
        case ADC:
            {
                cout << "ADC" << endl;
                break;
            }
        case ADCB:
            {
                cout << "ADCB" << endl;
                break;
            }
        case SBC:
            {
                cout << "SBC" << endl;
                break;
            }
        case SBCB:
            {
                cout << "SBCB" << endl;
                break;
            }
        case SXT:
            {
                cout << "SXT" << endl;
                break;
            }
        default:
            {
                cout << "nope" << endl;
                break;
            }
    }

    return 0;
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

int double_operand::instruction(gp_register *regs, CPSR *states)
{

    cout << "function_code: " << function_code << ": ";

    switch(function_code)
    {
        case MOV:
            {
                cout << "MOV" << endl;
                break;
            }
        case MOVB:
            {
                cout << "MOVB" << endl;
                break;
            }
        case CMP: 
            {
                cout << "CMP" << endl;
                break;
            }
        case CMPB:
            {
                cout << "CMPB" << endl;
                break;
            }
        case ADD:
            {
                cout << "ADD" << endl;
                break;
            }
        case SUB:
            {
                cout << "SUB" << endl;
                break;
            }
        case BIT:
            {
                cout << "BIT" << endl;
                break;
            }
        case BITB:
            {
                cout << "BITB" << endl;
                break;
            }
        case BIC:
            {
                cout << "BIC" << endl;
                break;
            }
        case BICB:
            {
                cout << "BICB" << endl;
                break;
            }
        case BIS:
            {
                cout << "BIS" << endl;
                break;
            }
        case BISB:
            {
                cout << "BISB" << endl;
                break;
            }
        default:
            {
                cout << "nope" << endl;
                break;
            }
    }
    return 0;
}

int main(int argc, char* argv[])
{
    gp_register gps[8];
    CPSR status_reg;
    int firstbit;
    int make_instruction;
    int to_interpret;
    int filepos = 0;
    command * new_command;
    char make_disposition = '0';
    char * disposition = &make_disposition;         //initialize disposition

    //to_interpret = atoi(argv[1]);

    //instruction = interpreter(to_interpret, &firstbit, new_command);
   
    to_interpret = line_reader(argv[1], disposition, filepos);

    cout << to_interpret << endl;

    while(to_interpret != -1)
    {
        make_instruction = interpreter(to_interpret, &firstbit, new_command);

        //new_command->disp();
        //cout << new_command->instruction(gps, &status_reg) << endl;

        new_command->instruction(gps, &status_reg);

        to_interpret = line_reader(argv[1], disposition, filepos);
    }


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




