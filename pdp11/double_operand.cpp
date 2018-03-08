//file to contain all double operand instructions
//double_operand

#include "pdp11.h"

using namespace std;

double_operand::double_operand(): command(), function_code(0), source_mode(0), source(0), destination_mode(0), destination(0)
{
}

double_operand::double_operand(int to_data, char to_disposition, char * tracefile, int to_function_code, int to_source_mode, int to_source, int to_destination_mode, int to_destination): command(to_data, to_disposition, tracefile), function_code(to_function_code), source_mode(to_source_mode), source(to_source), destination_mode(to_destination_mode), destination(to_destination)
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

int double_operand::make_dest(uint16_t *regs, i_cache *program)
{
    int index = 0;

    switch(destination_mode)
    {
        case 0:
            {   //register
                return (int) destination;
                break;
            }

        case 1:
            {   //register deferred
                //memory read for BIT and CMP
                //memory write for all others
                switch(function_code) {
                    case BIT: {
                        trace_file(tracefile, 0, regs[destination]);
                        break;
                    }
                    case BITB: {
                        trace_file(tracefile, 0, regs[destination]);
                        break;
                    }
                    case CMP: {
                        trace_file(tracefile, 0, regs[destination]);
                        break;
                    }
                    case CMPB: {
                        trace_file(tracefile, 0, regs[destination]);
                        break;
                    }
                    default: {
                        trace_file(tracefile, 1, regs[destination]);
                        break;
                    }
                }
                return (int)program[regs[destination]].data;
                break;
            }


        case 2:
            {   //register deferred
                //memory read for BIT and CMP
                //memory write for all others
                switch(function_code) {
                    case BIT: {
                        trace_file(tracefile, 0, regs[destination]);
                        break;
                    }
                    case BITB: {
                        trace_file(tracefile, 0, regs[destination]);
                        break;
                    }
                    case CMP: {
                        trace_file(tracefile, 0, regs[destination]);
                        break;
                    }
                    case CMPB: {
                        trace_file(tracefile, 0, regs[destination]);
                        break;
                    }
                    default: {
                        trace_file(tracefile, 1, regs[destination]);
                        break;
                    }
                }
                return (int)program[regs[destination]].data;
                break;
            }


        case 3: {   //register deferred
            //memory read for BIT and CMP
            //memory write for all others
            switch (function_code) {
                case BIT: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, program[regs[destination]].data);
                    break;
                }
                case BITB: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, program[regs[destination]].data);
                    break;
                }
                case CMP: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, program[regs[destination]].data);
                    break;
                }
                case CMPB: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, program[regs[destination]].data);
                    break;
                }
                default: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 1, program[regs[destination]].data);
                    break;
                }
            }
            return (int) program[program[regs[destination]].data].data;
            break;
        }


        case 4: {   //register deferred
                    //memory read for BIT and CMP
                    //memory write for all others
                switch (function_code) {
                    case BIT: {
                        trace_file(tracefile, 0, regs[destination]);
                        break;
                    }
                    case BITB: {
                        trace_file(tracefile, 0, regs[destination]);
                        break;
                    }
                    case CMP: {
                        trace_file(tracefile, 0, regs[destination]);
                        break;
                    }
                    case CMPB: {
                        trace_file(tracefile, 0, regs[destination]);
                        break;
                    }
                    default: {
                        trace_file(tracefile, 1, regs[destination]);
                        break;
                    }
                }
            return (int) program[regs[destination]].data;
            break;
        }
        case 5: {   //register deferred
                    //memory read for BIT and CMP
                    //memory write for all others
            switch (function_code) {
                case BIT: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, program[regs[destination]].data);
                    break;
                }
                case BITB: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, program[regs[destination]].data);
                    break;
                }
                case CMP: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, program[regs[destination]].data);
                    break;
                }
                case CMPB: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, program[regs[destination]].data);
                    break;
                }
                default: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 1, program[regs[destination]].data);
                    break;
                }
            }
            return (int) program[program[regs[destination]].data].data;
            break;
        }

        case 6: {   //register deferred
                        //memory read for BIT and CMP
                        //memory write for all others
            if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2)) {
                cout << "boundary error\n";
                return -1;
            }

            switch (function_code) {
                case BIT: {
                    trace_file(tracefile, 0, regs[PC]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    break;
                }
                case BITB: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    break;
                }
                case CMP: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    break;
                }
                case CMPB: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    break;
                }
                default: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 1, (uint16_t) index);
                    break;
                }
            }
            return (int) program[index].data;
            break;
        }

        case 7: {   //register deferred
                        //memory read for BIT and CMP
                        //memory write for all others
            if (destination == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[destination] + program[regs[PC]].data;
            if ((regs[destination] % 2) || (index % 2) || (program[index].data % 2)) {
                cout << "boundary error\n";
                return -1;
            }
            index = program[index].data;
            switch (function_code) {
                case BIT: {
                    trace_file(tracefile, 0, regs[PC]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    trace_file(tracefile, 0, program[index].data);
                    break;
                }
                case BITB: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    trace_file(tracefile, 0, program[index].data);
                    break;
                }
                case CMP: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    trace_file(tracefile, 0, program[index].data);
                    break;
                }
                case CMPB: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    trace_file(tracefile, 0, program[index].data);
                    break;
                }
                default: {
                    trace_file(tracefile, 0, regs[destination]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    trace_file(tracefile, 1, program[index].data);
                    break;
                }
            }

            return (int) program[index].data;
            break;
        }
        default: {
            cout << "invalid source mode\n";
            return -1;
            break;
        }
    }

    return -1;
}


//decode source mode
int double_operand::make_source(uint16_t *regs, i_cache *program)
{
    int index = 0;

    switch(source_mode)
    {
        case 0:
            {   //register
                return (int) source;
                break;
            }

        case 1:
            {   //register deferred
                //memory read for BIT and CMP
                //memory write for all others
                switch(function_code) {
                    case BIT: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                    case BITB: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                    case CMP: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                    case CMPB: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                    default: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                }
                return (int)program[regs[source]].data;
                break;
            }


        case 2:
            {   //register deferred
                //memory read for BIT and CMP
                //memory write for all others
                switch(function_code) {
                    case BIT: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                    case BITB: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                    case CMP: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                    case CMPB: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                    default: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                }
                return (int)program[regs[source]].data;
                break;
            }


        case 3: {   //register deferred
            //memory read for BIT and CMP
            //memory write for all others
            switch (function_code) {
                case BIT: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, program[regs[source]].data);
                    break;
                }
                case BITB: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, program[regs[source]].data);
                    break;
                }
                case CMP: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, program[regs[source]].data);
                    break;
                }
                case CMPB: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, program[regs[source]].data);
                    break;
                }
                default: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, program[regs[source]].data);
                    break;
                }
            }
            return (int) program[program[regs[source]].data].data;
            break;
        }


        case 4: {   //register deferred
                    //memory read for BIT and CMP
                    //memory write for all others
                switch (function_code) {
                    case BIT: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                    case BITB: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                    case CMP: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                    case CMPB: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                    default: {
                        trace_file(tracefile, 0, regs[source]);
                        break;
                    }
                }
            return (int) program[regs[source]].data;
            break;
        }
        case 5: {   //register deferred
                    //memory read for BIT and CMP
                    //memory write for all others
            switch (function_code) {
                case BIT: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, program[regs[source]].data);
                    break;
                }
                case BITB: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, program[regs[source]].data);
                    break;
                }
                case CMP: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, program[regs[source]].data);
                    break;
                }
                case CMPB: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, program[regs[source]].data);
                    break;
                }
                default: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, program[regs[source]].data);
                    break;
                }
            }
            return (int) program[program[regs[source]].data].data;
            break;
        }

        case 6: {   //register deferred
                        //memory read for BIT and CMP
                        //memory write for all others
            if (source == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[source] + program[regs[PC]].data;
            if ((regs[source] % 2) || (index % 2)) {
                cout << "boundary error\n";
                return -1;
            }

            switch (function_code) {
                case BIT: {
                    trace_file(tracefile, 0, regs[PC]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    break;
                }
                case BITB: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    break;
                }
                case CMP: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    break;
                }
                case CMPB: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    break;
                }
                default: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    break;
                }
            }
            return (int) program[index].data;
            break;
        }

        case 7: {   //register deferred
                        //memory read for BIT and CMP
                        //memory write for all others
            if (source == PC)
                index = regs[PC] + 2 + program[regs[PC]].data;
            else index = regs[source] + program[regs[PC]].data;
            if ((regs[source] % 2) || (index % 2) || (program[index].data % 2)) {
                cout << "boundary error\n";
                return -1;
            }
            index = program[index].data;
            switch (function_code) {
                case BIT: {
                    trace_file(tracefile, 0, regs[PC]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    trace_file(tracefile, 0, program[index].data);
                    break;
                }
                case BITB: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    trace_file(tracefile, 0, program[index].data);
                    break;
                }
                case CMP: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    trace_file(tracefile, 0, program[index].data);
                    break;
                }
                case CMPB: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    trace_file(tracefile, 0, program[index].data);
                    break;
                }
                default: {
                    trace_file(tracefile, 0, regs[source]);
                    trace_file(tracefile, 0, (uint16_t) index);
                    trace_file(tracefile, 0, program[index].data);
                    break;
                }
            }

            return (int) program[index].data;
            break;
        }
        default: {
            cout << "invalid source mode\n";
            return -1;
            break;
        }
    }

    return -1;
}




int double_operand::instruction(uint16_t *regs, CPSR *states, i_cache *program)
{
    int source;
    int destination;
    int result;
    cout << "function_code: " << function_code << ": ";

    switch(function_code)
    {
        case MOV:
            {
                cout << "MOV" << endl;
                source = make_source(regs, program);
                destination = make_dest(regs, program);
                destination = source;

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
