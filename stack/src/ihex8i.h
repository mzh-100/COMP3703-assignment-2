WeChat: cstutorcs
QQ: 749389476
Email: tutorcs@163.com
__attribute__ ((__optimize__ ("-fno-stack-protector"))) void unlock(void);

long int get_key()
{
    long int n; 
    char buf[4]; 
    int fd = open("/dev/urandom", O_RDONLY);
    int r;

    if(fd < 0) {
        fprintf(stderr, "Failed opening /dev/urandom\n"); 
        exit(1);
    }
    
    r = read(fd, buf, 4); 
    if(r < 0) 
    {
        fprintf(stderr, "Failed generating random number\n"); 
        exit(1);
    }
    n = *((int *)buf); 
    close(fd);
    return (n & 0xffffffff); 
}

void check_canary(void)
{
    uint64_t canary_on_stack; 
    uint64_t guess;
    char input[64];

    printf("Enter the canary value: "); 
    fgets(input, 64, stdin);
    guess = strtoul(input,NULL, 16); 

    asm volatile ("mov %0, qword ptr [rbp - 0x8]" : "=r" (canary_on_stack)); 
    if(guess == canary_on_stack) {
        printf("flag{Stack_Level_3}\n");
        _exit(0);
    }
    printf("Wrong canary value\n"); 
}

