#include <fcntl.h>
#include <sys/stat.h>
int main(int argc, char **argv)
{  
  if(argc>=2) 		//ak mame aspon jeden argument 
  {               
     int des;                                                                          
   des=open(argv[1] , O_CREAT | O_WRONLY , S_IRUSR | S_IWUSR);  
   close(1);                                               
   dup(des);   // vymenime standardny vystup za subor, ktory je ako prvy arg. progr.
   close(des);
  }
  write(1,"Nejaky ten vypis...\n",20);        //vypisy pojdu bud na povodny standardny vystup alebo do suboru.  
  return 0;
} 