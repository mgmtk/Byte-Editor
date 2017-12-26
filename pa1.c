#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>


/****************************
Mitchell Mesecher PA1 September 12, 2017

This program begins in main which parses the users
choice of read, write or quit. Once state is chosen
the application will read in the corresponding user
input to be written to a file. Any invalid input
will be met with a re-prompt to the user.

*****************************/

//declare functions
void parse_input(long* val);
int check_EOF();
void read_fd();
void write_fd();
int value_check(int byte_num);
int pos_check(int byte_num);
int byte_check();

//create global variables
int file_sz; 
unsigned char flag = 0x00;
FILE* edit_file;

//create enumerations for checking input errors
enum
{
    CONTINIOUS_BIT = 0x01,
    READ_BIT = 0x02,
    WRITE_BIT = 0x04,
    QUIT_BIT = 0x08,
    INCORR_VAL_BIT = 0x10,
    NEG_NUM_BIT = 0x20,
    ERROR_BIT = 0x40,
    RESET_BITS = 0x70
    
} INPUT_ENUM;

//Beginning of program
int main(int arg, char* args[])
{
    //check for incorrect number of arguements
    if(arg <= 1)
    {
        puts("Usage: ./pa1 file_name");
        return 0;
    
    }
    //check for file existance
    edit_file = fopen(args[1], "r+");
    if(edit_file == NULL)
    {
        printf("Error opening file \"%s\": No such file or directory\n", args[1] );
        return 0;
    }
    
    //declare pointer and determine file size
    char *c;
    file_sz = check_EOF();
    printf("File size: %d \n", file_sz); 
    flag |= CONTINIOUS_BIT; //append the continue bit

//program do while loop    
do{
    
    c =  calloc(256, sizeof(char)); //initialze line memory block
    //choice do while loop
    do   
    {
        flag &= CONTINIOUS_BIT; //clear bits except for continue bit
        printf("Enter 'r' to read, 'w' to write, or 'q' to quit: ");
        scanf("%s", c); //scan input
        switch(c[0] + c[1]) //check for only one character
        {
            case 'r': flag |= READ_BIT;  break;
            case 'w': flag |= WRITE_BIT; break;
            case 'q': flag |= QUIT_BIT; break;
            default: flag |= ERROR_BIT; printf("You must enter 'r', 'w', or 'q'\n"); break; //append error bit
            
        }    
  
    }
    while(flag & ERROR_BIT); //while the error bit is assigned re-prompt
  
    if(flag & READ_BIT)
    {
        //if the user chose to read
        read_fd();
        
    }
    else if(flag & WRITE_BIT)
    {
        //if the user chose to write
        write_fd();
    
    }
    else if(flag & QUIT_BIT)
    {
        //if the user chose to quit remove flags
        flag = 0;
    }
    
 }while(flag & CONTINIOUS_BIT); //restart program if quit was not chosen
 
//free memory to prevent leaks
free(c);  
fclose(edit_file);  
 
return 1;

}

/****************************
Read Fd() 
    Contains file reading prompt while loops that
    contniue to exact if unvalid input
    is submitted, reads bytes from file.
*****************************/

void read_fd()
{

    int byte_num = 0;
    int pos = 0;
    int value = 0;
    do
    { 
        //prompt for number of bytes to read
        flag &= ~RESET_BITS; //remove byte error bit
        printf("Enter the number of bytes to be read: ");
        byte_num = byte_check();
   
    }
    while(flag & ERROR_BIT); //loop while value is incorrect

    do
    {   //prompt for file position
        flag &= ~RESET_BITS;//remove file position error bit
        printf("Enter the byte position of which to read from: ");
        pos = pos_check(byte_num); 
    }
    while(flag & ERROR_BIT); //loop while value is incorrect

    fseek(edit_file, pos, SEEK_SET); //seek to the correct file location
    fread(&value, byte_num, 1, edit_file); //read the bytes into the variable
    printf("The value is %d\n", value); //print the read value


}

/****************************
check_EOF() 
    Returns the size of file
    Returns: the EOF byte
*****************************/
int check_EOF()
{
    int size;
    fseek(edit_file, 0, SEEK_END); //seek to end of file
    size = ftell(edit_file); //save the file size
    rewind(edit_file); //reset file stream
    return size;

}

/****************************
Write Fd() 
    Contains file writing prompt while loops that
    contniue to exact if unvalid input
    is submitted, writes bytes to file.
*****************************/
void write_fd()
{
unsigned int byte_num = 0;
unsigned int pos = 0;
unsigned int value = 0;

   do{
        //prompt for number of bytes to write
        flag &= ~RESET_BITS; //remove byte error bit
        printf("Enter the number of bytes to be written: ");
        byte_num = byte_check();
    }
    while(flag & ERROR_BIT); //loop while value is incorrect
    
    
    do{
        //prompt for value to write
        flag &= ~RESET_BITS; //remove byte error bit
        printf("Enter the value you want to write: ");   
        value = value_check(byte_num);
    
    }
    while(flag & ERROR_BIT); //loop while value is incorrect
    
    do{
        //prompt for file position
        flag &= ~RESET_BITS; //remove byte error bit
        printf("Enter the byte position of which to write: ");
        pos = pos_check(byte_num);
    }
    while(flag & ERROR_BIT); //loop while value is incorrect
   
    fseek(edit_file, pos, SEEK_SET); //seek to file position
    fwrite(&value, 1, sizeof(int), edit_file); //write the user's value


}

/****************************
Value Check() 
    Checks for a correct value
    Sets flag if not
    Returns: write value
*****************************/
int value_check(int byte_num)
{
    long value;  
    parse_input(&value); //parse the value
    if(flag & INCORR_VAL_BIT) 
    {
        //check for invalid value
        puts("You must enter a valid number");
        flag |= ERROR_BIT;
        return 0;

    }
    else if(flag & NEG_NUM_BIT)
    {
        //check for negative number
         puts("No negative numbers");
         flag |= ERROR_BIT;
         return 0;
    
    }
    
    switch(byte_num)
    {
        case 1: if(value > UCHAR_MAX)
                {
                    puts("You must enter a value between 0 and 255");
                    flag |= ERROR_BIT; 
                    return 0;
                }
        case 2: if(value > USHRT_MAX)
                {
                     puts("You must enter a value between 0 and 65535"); 
                     flag |= ERROR_BIT; 
                     return 0;
                }
        case 4: if(value > UINT_MAX)
                { 
                    puts("You must enter a value between 0 and 4294967295"); 
                    flag |= ERROR_BIT; 
                    return 0;
                }    
    }
    return value;

}

/****************************
Byte Check() 
    Checks for a correct byte number
    Sets flag if not
    Returns: byte num
*****************************/
int byte_check()
{
    long byte_num = 0;
    parse_input(&byte_num);
    if(byte_num != 2 && byte_num != 4 && byte_num != 1)
     {
        puts("You must enter a power of 2 <= 4!");
        flag |= ERROR_BIT;

    }
    return byte_num;

}

/****************************
Parse Input() 
    Reads in user input from command line 
    and checks for errors and invalid inputs
    Returns: Error code
*****************************/
void parse_input(long* val)
{
    int i = 0;
    char* input = calloc(256, sizeof(char)); //malloc size of line input
    scanf("%s", input); //scan user input
    for(i = 0; i < sizeof(input); i++)
    {
        //check if any input is a letter
        if(isalpha(input[i]))
        {
            free(input);
            flag |= INCORR_VAL_BIT;
            return;
        }
    }
    
    *val = strtol(input, NULL, 10);
    if(*val < 0)
    {
        //check for negative
        free(input);
        flag |= NEG_NUM_BIT;
        return;
       
    }
}

/****************************
Pos check() 
    Checks the returned error code from parse_input 
    Sets Position flag on invalid input   
    Returns: position value
*****************************/

int pos_check(int byte_num)
{

    long pos = 0;
    parse_input(&pos);
    if(flag & INCORR_VAL_BIT)
    {
        //prints invalid num prompt
        puts("You must enter a valid number");
        flag |= ERROR_BIT;      
        return 0;
    
    }
    else if(flag & NEG_NUM_BIT)
    {
        //prints neg num prompt
        puts("No negative numbers");
        flag |= ERROR_BIT;
        return 0;
    
    }
    else if(pos + byte_num > file_sz)
    {
        //checks for position above EOF
        printf("You must enter a value between 0 and %d\n", file_sz - byte_num);
        flag |= ERROR_BIT;
        return 0;
    }
    return pos;

}



