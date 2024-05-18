WeChat: cstutorcs
QQ: 749389476
Email: tutorcs@163.com

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>

#define CMD_EXIT  0
#define CMD_ALLOC 1
#define CMD_FREE  2
#define CMD_READ  3
#define CMD_WRITE 4
#define CMD_COPY  5
#define CMD_HELP  6
#define CMD_LOOP  7
#define CMD_ERROR -1

#define MAX_CELLS 64 
#define MAX_ARGS 3
#define MAX_SIZE 256

typedef struct cell_t {
   int size;   
   int mode; // = 0 (read-only), = 1 (read/write)  
   char *data; 
} cell_t; 


int out_of_bound(int i)
{
   if(i >= 0 && i < MAX_CELLS) return 0;
   printf("Index out of bound\n");
   return 1;
}

int free_cell(cell_t *cells[], int i)
{
   if(cells[i] == NULL ) {
      printf("Cell %d is empty\n",i); 
      return CMD_ERROR; 
   }

   free(cells[i]->data); 
	free(cells[i]);
   cells[i] = NULL; 
   return CMD_FREE;  
}

int alloc_cell(cell_t *cells[], int i, int val)
{	
   cell_t *c; 

   if(cells[i]) {
      printf("cell %d already allocated\n", i);
      return CMD_ERROR;
   } 

   if(val <= 0 || val > MAX_SIZE) {
      printf("Invalid size\n");
      return CMD_ERROR; 
   }

   c = (cell_t *) malloc(sizeof(cell_t));
   assert(c!=NULL); 

	cells[i] = c; 
   cells[i]->size = val;
   cells[i]->mode = 1; // read/write allowed 
   cells[i]->data = NULL; 
   return CMD_ALLOC;
}

int parse_args(char *input, int args[])
{
   char *s = strtok(input, " "); 
   int i; 

   if(!s) return 0;

   for(i=0; i < MAX_ARGS; ++i)
   {
      s = strtok(NULL, " ");
      if(!s) break; 
      args[i] = atoi(s); 
   }

   return i; 
}

void menu(void)
{
   puts("Heap smasher 2024 (v2)");
   puts("======================");   
   printf("You can allocate up to %d pointers, ", MAX_CELLS);
   puts("store them in an array of cells, ");
   puts("and smash the heap in the process!\n"); 
   
   puts("Available commands: ");
   puts("help         display this help message"); 
   puts("alloc i n    allocate n bytes to cell i"); 
   puts("free i       free the memory in cell i");
   puts("copy i j     copy cell i to cell j");
   puts("write i      write data to cell i"); 
   puts("read i       read the content of cell i"); 
   puts("chkdup       check for duplicate pointers");
   puts("exit         exit the program\n");
}

int cmd_alloc(char *input, cell_t * cells[])
{
   int args[MAX_ARGS]; 
   int n = parse_args(input, args); 

   if(n < 2) {
      printf("Missing argument(s)\n");
      return CMD_ERROR; 
   }

   if(out_of_bound(args[0]))  return CMD_ERROR; 
   return alloc_cell(cells, args[0], args[1]); 
}


int cmd_free(char *input, cell_t *cells[])
{
   int args[MAX_ARGS]; 
   int n = parse_args(input, args); 

   if(n < 1) {
      printf("Missing argument(s)\n");
      return CMD_ERROR; 
   }

   if(out_of_bound(args[0])) return CMD_ERROR; 

   return free_cell(cells, args[0]);    
}

int copy_cell(cell_t *cells[], int src, int dst)
{
   if(cells[src] == NULL) {
      printf("Can't copy empty cell %d\n", src);
      return CMD_ERROR;
   }

   if(cells[dst] != NULL) {
      printf("Cell %d already allocated\n", dst); 
      return CMD_ERROR;
   } 

   cell_t *c;
   c = malloc(sizeof(cell_t));
   assert(c!=NULL);
   c->size = cells[src]->size;
   c->mode = 0;
   c->data = cells[src]->data;
   cells[dst] = c; 
   cells[src]->mode=0; // once copied, set data to read-only

   return CMD_COPY; 
}

int cmd_copy(char *input, cell_t *cells[])
{
   int args[MAX_ARGS]; 
   int n = parse_args(input, args); 
   int i; 

   if(n < 2) {
      printf("Missing argument(s)\n");
      return CMD_ERROR; 
   }

   if(out_of_bound(args[0]) || out_of_bound(args[1]) )
      return CMD_ERROR; 

   copy_cell(cells, args[0], args[1]); 

   return CMD_COPY; 
}

int cmd_read(char *input, cell_t *cells[])
{
   int args[MAX_ARGS]; 
   int n = parse_args(input, args); 
   int i; 

   if(n < 1) {
      printf("Missing argument(s)\n");
      return CMD_ERROR; 
   }
     
   if(out_of_bound(args[0])) return CMD_ERROR; 
   i = args[0];

   if(!cells[i]) {
      printf("Cell %d is empty\n", i);
      return CMD_ERROR;
   }

   write(1, cells[i]->data, cells[i]->size); 

   return CMD_READ; 
}

int write_cell(cell_t *cells[], int i)
{
   ssize_t n; 

   if(cells[i]->mode == 0)
   {
      printf("Write failed; cell is read-only\n");
      return CMD_ERROR; 
   }

   if(cells[i]->size == 0xdeadbeef) {
      write(1,"flag{Heap_Level_6}\n", 19);
      _exit(0);       
   }

   if(cells[i]->data == NULL) {
      cells[i]->data = malloc(cells[i]->size);
      assert(cells[i]->data != NULL); 
   }

   printf("Enter data: "); 
   n = read(0, cells[i]->data, cells[i]->size);
   if(n < 0) return CMD_ERROR; 

   return CMD_WRITE; 
}

int cmd_write(char *input, cell_t *cells[])
{
   char *s = strtok(input, " "); 
   int i; 

   if(!s) return CMD_ERROR; 
   
   s = strtok(NULL, " ");

   if(!s) return CMD_ERROR; 
   i = atoi(s); 

   if(out_of_bound(i)) return CMD_ERROR;

   if(!cells[i]) {
      printf("Cell %d is not empty\n", i);
      return CMD_ERROR;
   }

   return write_cell(cells, i); 
}

void cmd_chkdup(cell_t * cells[])
{
   int i,j;
   for(i=0; i < MAX_CELLS-1; ++i)
   {
      if(!cells[i]) continue; 

      for(j=i+1; j < MAX_CELLS; ++i)
      {
         if(cells[i] == cells[j]) {
            printf("flag{Heap_Level_5}\n");
            _exit(0);
         }
      }
   }
   printf("No duplicate pointers detected\n"); 
}

int process_cmd(char input[], cell_t *cells[])
{
   if(strncmp(input, "exit", 4) == 0) return CMD_EXIT; 

   if(strncmp(input, "help", 4) == 0) {
      menu();
      return CMD_HELP; 
   }

   if(strncmp(input, "alloc", 5) == 0)
      return cmd_alloc(input, cells); 
   
   if(strncmp(input, "free", 4) == 0)
      return cmd_free(input, cells); 
   
   if(strncmp(input, "read", 4) == 0)
      return cmd_read(input, cells); 

   if(strncmp(input, "write", 5) == 0)
      return cmd_write(input, cells); 

   if(strncmp(input, "copy", 4) == 0)
      return cmd_copy(input, cells);

   if(strncmp(input, "chkdup", 6) == 0)
      cmd_chkdup(cells);
   
   return CMD_ERROR;
}


int main()
{
   char cmd[64];
   cell_t *cells[MAX_CELLS] = {}; 

   setbuf(stdin, NULL);
   setbuf(stdout, NULL);
   setbuf(stderr, NULL);

   menu();
   
   while(1) {
      printf("\n>> "); 
    	fgets(cmd, 64, stdin);
      if(process_cmd(cmd, cells) == CMD_EXIT)
         break;  
      if(feof(stdin)) break;
	}

   return 0;
}

