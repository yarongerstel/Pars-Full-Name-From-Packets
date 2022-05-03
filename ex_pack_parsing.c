#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
// IF YOU NEED MORE INCLUDES, ADD HERE


#ifdef _MSC_VER
#define DYNAMIC_SIZE 1
#else
#define DYNAMIC_SIZE 
#endif

typedef enum
{
    FIRST_NAME, // char array                   
    LAST_NAME,  // char array                   
    ID_NUMBER,  // uint64, unsigned long long   
    PHOTO,      // binary data                  
} data_type_t;  

typedef struct 
{
    data_type_t type;                    
    size_t length;                      
    unsigned char buffer[DYNAMIC_SIZE]; // binary data, not have to be null terminated string
} entry_data_t;

typedef struct 
{
    size_t length; 
    entry_data_t data_array[];
} packet_t;


size_t parse_full_name(packet_t *p, char **p_full_name)
/*
    Parse "full name" from a given packet.

    p - packet object
        Assume the packet data include one FIRST_NAME and one LAST_NAME.

    p_full_name - returned with point to "full name" string.
        The function use at "malloc()" function to allocate memory for "full name" string.
        Full name format: "<first name> <last name>"
    
    Return packet size by bytes.
*/
{
    /*************/
    // Your implement ...
    
    int packet_size = 0;
    size_t * ptr = &p->data_array->type;
    char type;
    char* p_str_f = NULL; //ponter to start of firs name string
    char* p_str_l = NULL; //ponter to start of last name string
    size_t length_of_string_f = 0; // lengthof string first name
    size_t length_of_string_l = 0; //length of string last name
    size_t length_of_string = 0;

    for (size_t i = 0; i < p->length; i++)
    {
        type = *(ptr);
        length_of_string = *(ptr + 1);
        packet_size += 16;
        switch (type)
        {
        case FIRST_NAME:
            p_str_f = ptr + 2;
            length_of_string_f = length_of_string;
            break;
        case LAST_NAME:
            p_str_l = ptr + 2;
            length_of_string_l = length_of_string;
            break;
        case ID_NUMBER:
            packet_size += 16; // uint64, unsigned long long
        default: //  case PHOTO..
            break;
        }

        char* temp = ptr + 2; // 16 byte : type and length (size_t == 8)

        if(type!=ID_NUMBER)  // In order not to calculate twice - once the size of 16 bytes and once the length of the string (contained within the 16 bytes)
            packet_size += length_of_string;
        ptr = temp + length_of_string;
    }

    // allocate memory for "full name" string.
    char* full_name = (char*)malloc(length_of_string_f+length_of_string_l+2);

    // Filling first name
    for (size_t i = 0; i < length_of_string_f; i++)
    {
        full_name[i] = p_str_f[i];
    }

    // Space between names
    full_name[length_of_string_f] = ' ';

    // Filling last name
    for (size_t i = 0; i < length_of_string_l; i++)
    {
        full_name[length_of_string_f + 1+i] = p_str_l[i];
    }

    // Null terminated string
    full_name[length_of_string_f + 1 + length_of_string_l] = '\0';
    
    *p_full_name = &full_name[0];
    
   

    return packet_size;
   
   
}

int main(int argc, char *argv[])
{
    char packet_data[0x1000] = {0};
    char *full_name = NULL;

    if (argc < 2)
    {
        printf("usage: %s <packet file>\n", argv[0]);
        goto cleanup;
    }

    printf("read packet data...\n");
    // read packet data
    {
        FILE *f = fopen(argv[1], "rb");
        if (NULL == f)
        {
            printf("fopen() fialed\n");
            goto cleanup;
        }
        fread(packet_data, sizeof(packet_data), 1, f);
        fclose(f);        
    }

    // parse
    parse_full_name((packet_t*) packet_data, &full_name);

    // print 
    printf("The full name: %s\n", full_name);

cleanup:
    // cleanup
    if (full_name)
    {
        free(full_name);
        full_name = NULL;
    }
}