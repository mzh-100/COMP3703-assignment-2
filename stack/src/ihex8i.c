WeChat: cstutorcs
QQ: 749389476
Email: tutorcs@163.com

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <readline/readline.h> 
#include <readline/history.h> 
#include "ihex8i.h"

#define BLOCKSIZE 16
#define MAPSIZE (64 * BLOCKSIZE)
#define WRITESIZE 32
#define READSIZE 2 

#define MAGIC 0xdeadbeef 

uint64_t magic=0; 

int enforce_crc = 0; 
int enable_write = 0; 
void start(void); 

typedef struct hexline_t {
    uint8_t size;
    int offset; 
    uint8_t bytes[WRITESIZE];
} hexline_t; 

void Win1()
{
    write(1,"flag{Stack_Level_1}\n",19);
    _exit(0); 
}

void Win2()
{
    char flag[32] = {};
    int i; 

    if(magic != MAGIC)
    {
        write(1,"Try again\n",9);
        _exit(1); 
    }    

    for(i=0; i < 20; ++i) 
        flag[i] = "flag{Stack_Level_4}"[i]; 

    asm(
        "mov rax, 1; "
        "mov rdi, 1;"
        "mov rdx, 19;"
        "syscall;"
        "mov rax, 0x3c;"
        "mov rdi, 0;"
        "syscall"
        : 
        : "S" (flag)
    );

}


int hex2byte(char* s, uint8_t *n)
{   
    int i; 
    char c; 
    *n = 0; 
    for(i=0; i < 2; ++i)
    {
        c=tolower(s[i]); 

        if(c >= '0' && c <= '9') 
        {
            *n = (*n<<4) + (c-'0');
        }
        else if(c >= 'a' && c <= 'f')
        {
            *n = (*n << 4) + (c-'a'+10);
        }
        else return 0;  
    }
    return 1; 
}

hexline_t * convert(char *input) {
    char* line=NULL; 
    hexline_t *hline=NULL; 
    uint8_t b; 
    uint8_t crc; 
    int i,k; 
    crc = 0; 

    line = (char *) malloc(strlen(input) + 1);
    hline = (hexline_t *) malloc(sizeof(hexline_t)); 
    assert(line != NULL && hline != NULL);

    strcpy(line,input); 

    // parse header 
    if(strlen(line) < 11 || line[0] != ':')
    {  
        printf("Syntax error\n"); 
        goto FAIL; 
    }

    // parse length
    if(!hex2byte(line+1, &hline->size))
    {
        printf("Syntax error\n");
        goto FAIL; 
    }
    if(hline->size > WRITESIZE ) {
        printf("You can only write up to 32 bytes at a time\n"); 
        goto FAIL; 
    }

    // get offset
    crc = hline->size; 
    if(!hex2byte(line+3, &b))
    {
        printf("Syntax error\n");
        goto FAIL; 
    }

    hline->offset = b*256;
    crc += b; 
    if(!hex2byte(line+5, &b))
    {
        printf("Syntax error\n");
        goto FAIL; 
    }

    hline->offset += b; 
    
    // parse code 
    crc += b; 
    if(!hex2byte(line+7, &b))
    {
        printf("Syntax error\n");
        goto FAIL; 
    }
    crc += b; 

    if(b != 0)
    {
        printf("Record type %02x not supported.\n", b);
        goto FAIL; 
    }

    // parse data
    for(i = 0; i < hline->size; ++i)
    {
        k = 9+i*2; 
        if(k+4 > strlen(line)) 
        {
            printf("\nThe number of data bytes does not match the length field.\n"); 
            goto FAIL;
        }
        
        if(!hex2byte(line+k, &b))
        {
            printf("\nSyntax error\n");
            goto FAIL; 
        }
        hline->bytes[i] = b;
        crc += b;  
    }
    crc = (crc ^ 0xff) + 1; 
    // parse checksum
    if(!hex2byte(line+9+hline->size*2, &b))
    {
        printf("\nSyntax error\n");
        goto FAIL;
    }

    if (enforce_crc && b != crc) {
        printf(" -- Checksum error!\n");
        goto FAIL; 
    }

    return hline; 
FAIL: 
    free(line);
    free(hline);
    return NULL;
}

void show_map(uint8_t * map)
{
    int i,j;
    int n = MAPSIZE/BLOCKSIZE; 
    for(i=0; i < n; ++i)
    {
        printf("%04x: ", i*16); 
        for(j=0; j < BLOCKSIZE; ++j)
            printf("%02x ", map[i*16 + j]); 
        printf("\n"); 
    }
}

void read_map(char *line, uint8_t *map)
{
    uint32_t i = strtoul(line,NULL,16);
    int j,k,b; 
    if(i > MAPSIZE) {
        printf("Read failed -- offset out of bound.\n");
        return; 
    }

    for(j=0; j < READSIZE; ++j) 
    {
        i = i + j*BLOCKSIZE; 
        printf("%04x: ", i);

        for(k=0; k < BLOCKSIZE; ++k)
            printf("%02x ", map[i+k]);
        printf("\n");
    }
}

void menu()
{
    puts("Interactive Intel HEX format translator");
    puts("Commands:");
    printf("  read <N>      Read %d bytes starting at offset N in the buffer\n", READSIZE*BLOCKSIZE);
    puts(  "  write <line>  Convert Intel HEX string <line> to bytes and write to buffer."); 
    printf("                You can write at most %d bytes at a time.\n", WRITESIZE); 
    puts("  dump          Show all bytes in the buffer");
    puts("  unlock        Enter a purchase key and unlock the write feature"); 
    puts("  canary        Guess the canary value");
    puts("  help          Show the available commands"); 
    puts("  exit          Exit the program\n"); 

}

void print_hexline(hexline_t *hline)
{
    int i; 

    for(i=0; i < hline->size; ++i) {
        if(i%BLOCKSIZE == 0) {
            printf("\n%04x: ", hline->offset+i); 
        }
        printf("%02x ", hline->bytes[i]); 
    }
    printf("\n"); 
}

void do_write(char *input, uint8_t map[])
{
    hexline_t *hline;
    int i; 

    if(!enable_write) {
        printf("The write functionality is currently disabled.\n");
        printf("To enable it, use the unlock menu, and enter a correct key.\n");
        return; 
    }
    hline=convert(input);
    if(!hline) return; 

    if(hline->offset > MAPSIZE) {
        printf("Write failed -- offset out of bound!\n");
        free(hline);
        return; 
    }

    for(i=0; i < hline->size; ++i)
        map[hline->offset+i] = hline->bytes[i]; 
    print_hexline(hline); 
    free(hline);
}

void unlock(void)
{
    char input[10];
    long int key;  

    printf("Enter key (in hex): ");
    key = get_key();
    if(fgets(input, 30, stdin) == NULL) {
        printf("Error reading input\n");
        return; 
    }

    if(strtol(input, NULL, 16) != key) {
        printf("Invalid key\n");
        return; 
    } else {
        printf("flag{Stack_Level_2}\n"); 
        enable_write = 1; 
    }
}


void print_config()
{
    puts("Configuration:"); 
    printf("  Write:     %s\n", enable_write? "enabled." : "disabled");
    printf("  Checksum:  %s\n", enable_write? "enabled." : "disabled");
    printf("  libc base: %p\n", (void *)(puts - 541728));
}

void start(void) {
    uint8_t out_buffer[MAPSIZE]; 
    char *line = NULL; 
    char *rest=NULL; 

    memset(out_buffer, 0, MAPSIZE); 
    print_config();

    while(line = readline(">> ")) { 
        if(!strncmp(line, "exit",4)) break; 
        if(!strncmp(line, "read",4)) {
            rest = line + 5; 
            read_map(rest, out_buffer); 
            continue; 
        }
        if(!strncmp(line, "dump",4)) {
            show_map(out_buffer); 
            continue; 
        } 
        if(!strncmp(line, "write", 5)) {
            rest=line+6; 
            do_write(rest, out_buffer); 
            continue; 
        }
        if(!strncmp(line, "unlock", 6)) {
            unlock(); 
            continue; 
        }
        if(!strncmp(line, "canary", 6)) {
            check_canary(); 
            continue; 
        }
        if(!strncmp(line, "help", 4)) {
            menu();
            continue; 
        }
        printf("Unknown command \"%s\"\n", line);
    } 
}

 
int main(int argc, char* argv[])
{
    setbuf(stdin,NULL);
    setbuf(stderr,NULL);
    setbuf(stdout,NULL);

    menu(); 
    start();
    return 0;
}
