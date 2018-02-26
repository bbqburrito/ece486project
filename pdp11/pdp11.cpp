#include "pdp11.h"

using namespace std;

int findstart(i_cache * prog_mem, int prog_length)
{
	char answer[25];
	int start;
    int i;
    int j;

    cin.sync();
    cout << "input start point? (y/N) ";    //if start point not inputted as arg
                                            //ask user for start point
                                            //if no, find * disposition
                                            //if no *, start from 0
    cin.getline(answer, 25);
    cin.sync();

    if(toupper(answer[0]) == 'Y')
    {
        cin.sync();
        cout << "enter start: ";
        cin.getline(answer, 25);
        cin.sync();
        start = strtol(answer, NULL, 10);
        if(start > prog_length)
        {
            cout << "program only " << prog_length << " lines. Start set to 0\n";
            start = 0;
        }
    }
    else {
        cout << "find '*' to start at or start at 0." << endl;
        i = 0;
        j = 0;                              //indicate '*' found
        while(prog_mem[i].disposition != '*' && i < I_SIZE)
        {
            ++i;
            if(prog_mem[i].disposition == '*')
            {
                start = i;
                j = 1;
            }
        }

        if(!j)
            start = 0;
	}
	return start;
}



//writes to file specified by filename. writes address parameter
//as an octal. returns an int to indicate outcome.
//takes a char to indicate filename, an int to indicate
//the type write, and an int to indicate the address write
int trace_file(char * filename, int type, int address)
{
    char make_address[100];
    ofstream outfile;
    int i;
    int outcome;

    //initialize make_address to all NULL
    for(i = 0; i < 100; ++i)
    {
        make_address[i] = '\0';
    }

    outfile.open(filename, ios::app | ios::out);

    if(!outfile.is_open())
    {
        cout << "file error\n";
        return -1;
    }

    outcome = sprintf(make_address, "%d\t\t%o\n", type, address);

    outfile << make_address;

    return outcome;
}


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
        return -1;
    }

    filein.seekg(filepos);      //maintain file position

    filein.getline(address, 100);
    if(address[7] == '\r')
    {
        address[7] = '\0';
    }

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

    while((strlen(address) != 7))       //if string loaded not 7 characters
    {                                   //read next line
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
        if(address[7] == '\r')
            address[7] = '\0';
    }
    
    *disposition = address[0];          //get disposition
    address[0] = '0';                   //set first char to 0

    for(int i = 0; i < 7; ++i)          //invalid characters converted to 0
    {
        if(address[i] > '7' && address[i] < '0')
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
    bit1214 = (to_interpret >> 12) & 7;

    //cout << bit1214 << endl;

    //next 3 bits indicate more of function code for 
    //extended instruction set
    bit0911 = (to_interpret >> 9) & 7; 

    //cout << bit0911 << endl;

    //next 3 bits indicate function code for 
    //single instruction set
    bit0608 = (to_interpret >> 6) & 7;

    //cout << (to_interpret >> 6) << " - " << *firstbit*512 << " - " << bit1214*64 << " - " << bit0911*8 << endl;

    //cout << bit0608 << endl;

    //next 3 bits indicate function code for
    //most control functions
    bit0305 = (to_interpret >> 3) & 7;

    //cout << bit0305 << endl;

    //next 3 bits indicate function code for
    //some control functions
    bit0002 = to_interpret & 7;

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
        cout << "instruction code: " << to_interpret << ": nope\n";
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
CPSR::CPSR(): T(0), N(0), Z(0), V(0), C(0), priority(0)
{
}

CPSR::~CPSR()
{
}

CPSR::CPSR(const CPSR &to_copy)
{
    int condition = to_copy.get_condition();
    priority = condition >> 5;
    T = (condition >> 4) & 1;
    N = (condition >> 3) & 1;
    Z = (condition >> 2) & 1;
    V = (condition >> 1) & 1;
    C = condition & 1;
}

int CPSR::get_condition() const
{
    int result = (priority << 5) + (T << 4) + (N << 3) + (Z << 2) + (V << 1) + C;

    return result;
}

int CPSR::set_condition(int to_set)
{
    T = (to_set >> 4) & 1;
    N = (to_set >> 3) & 1;
    Z = (to_set >> 2) & 1;
    V = (to_set >> 1) & 1;
    C = to_set & 1;
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


int command::instruction(int *regs, CPSR *states, i_cache *program, int position, int * mem, char * tracefile)
{
    return 0;
}

int command::instructionB(int *regs, CPSR *states, i_cache * program, int position, int * mem, char * tracefile)
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

int extended::instruction(int *regs, CPSR *states, i_cache * program, int position, int * mem, char * tracefile)
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

int trapIntMiscCond::instruction(int *regs, CPSR *states, i_cache *program, int position, int * mem, char * tracefile)
{
    int outcome = -1;

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
                states->set_condition(states->get_condition() & 0xFE);
                break;
            }
        case CLV:
            {
                cout << "CLV" << endl;
                states->set_condition(states->get_condition() & 0xFD);
                break;
            }
        case CLZ:
            {
                cout << "CLZ" << endl;
                states->set_condition(states->get_condition() & 0xFB);
                break;
            }
        case CLN:
            {
                cout << "CLN" << endl;
                states->set_condition(states->get_condition() & 0xF7);
                break;
            }
        case CCC:
            {
                cout << "CCC" << endl;
                states->set_condition(states->get_condition() & 0xF0);
                break;
            }
        case SEC:
            {
                cout << "SEC" << endl;
                states->set_condition(states->get_condition() | 1);
                break;
            }
        case SEV:
            {
                cout << "SEV" << endl;
                states->set_condition(states->get_condition() | 2);
                break;
            }
        case SEZ:
            {
                cout << "SEZ" << endl;
                states->set_condition(states->get_condition() | 4);
                break;
            }
        case SEN:
            {
                cout << "SEN" << endl;
                states->set_condition(states->get_condition() | 8);
                break;
            }
        case SCC:
            {
                cout << "SCC" << endl;
                states->set_condition(states->get_condition() | 0xF);
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

int jump_sub::instruction(int *regs, CPSR *states, i_cache *program, int position, int * mem, char * tracefile)
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

int branch::instruction(int *regs, CPSR *states, i_cache *program, int position, int * mem, char * tracefile)
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

int single_operand::instruction(int *regs, CPSR *states, i_cache *program, int position, int * mem, char * tracefile)
{
    int outcome = -1;

    cout << "function_code: " << function_code << ": ";

    switch(function_code)
    {
        case CLR:
            {
                cout << "CLR" << endl;
                switch (destination_mode) {
                    case 0: 
                        {
                            regs[destination] = 0;
                            states->set_condition(4);
                            break;
                        }
                    case 1:
                        {
                            mem[regs[destination]] = 0;
                            states->set_condition(4);
                            trace_file(tracefile, 1, regs[destination]);   //write data write to trace file
                            outcome = 1;
                            break;
                        }
                    case 2:
                        {
                            mem[regs[destination]] = 0;
                            regs[destination] += 1;
                            states->set_condition(4);
                            trace_file(tracefile, 1, regs[destination] - 1);   //write data write to trace file
                            outcome = 1;
                            break;
                        }
                    default:
                        {
                             cout << "invalid destination mode" << endl;
                             break;
                        }
                }


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

int double_operand::instruction(int *regs, CPSR *states, i_cache *program, int position, int * mem, char * tracefile)
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
    char answer[25];
    int start;
    i_cache prog_mem[I_SIZE];
    int memory[MEM_SIZE];
    int gps[8];
    CPSR status_reg;
    int firstbit;
    int make_instruction;
    int to_interpret;
    int filepos = 0;
    int prog_pos;
    int prog_size;
    int i = 0;
    int j;
    command * new_command;
    char make_disposition = '0';
    char * disposition = &make_disposition;         //initialize disposition
    char * trace;
    ofstream trfile;
    ifstream mkfile;
    time_t timer;
    struct tm * timeinfo;

    if(argc != 3)
    {
        cout << "usage: pdp11 [input file] [output file]" << endl;

        return 0;
    }

    trace = argv[2];

    mkfile.open(trace);

    if(mkfile.is_open())
    {
        cin.sync();
        cout << "trace file already exists. erase and use this filename (Y/n)";
        cin.getline(answer, 25);
        cin.sync();
        if(toupper(answer[0]) == 'N')
        {
            //get filename from stdin
            while(mkfile.is_open())
            {
                mkfile.close();
                cin.sync();
                cout << "enter name for new trace file: ";
                cin.getline(trace, 100);
                cin.sync();
                mkfile.open(trace);
            }
        }
    }

    //erase existing file
    trfile.open(trace, ios::trunc | ios::out);
    if(!trfile.is_open())
    {
        cout << "cannot open\n";                    
        return 0;
    }

    //write to file. put timestamp and column headers
    time(&timer);
    timeinfo = localtime(&timer);

    trfile << asctime(timeinfo) << endl;

    trfile << "type\taddress" << endl;
    trfile.close();


    //to_interpret = atoi(argv[1]);

    //instruction = interpreter(to_interpret, &firstbit, new_command);

    to_interpret = line_reader(argv[1], disposition, filepos);

    //read file into memory
    while(to_interpret != -1)
    {
        prog_mem[i].disposition = *disposition;
        prog_mem[i].data = to_interpret;
        ++i;

        to_interpret = line_reader(argv[1], disposition, filepos);
    }

    prog_size = i;

    filepos = 0;

    for(j = 0; j <= i; ++j)
    {
        cout << prog_mem[j].disposition << prog_mem[j].data << endl;
    }

    to_interpret = line_reader(argv[1], disposition, filepos);

    cout << to_interpret << endl;

    while(to_interpret != -1)
    {
        make_instruction = interpreter(to_interpret, &firstbit, new_command);

        //new_command->disp();
        //cout << new_command->instruction(gps, &status_reg) << endl;

        new_command->instruction(gps, &status_reg, prog_mem, prog_pos, memory, trace);

        to_interpret = line_reader(argv[1], disposition, filepos);
    }

    start = findstart(prog_mem, prog_size);       //get start point

    cout << "start point: " << start << endl;


    return 0;
}


