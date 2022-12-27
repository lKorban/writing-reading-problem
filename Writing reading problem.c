/******************************************************************************
 * 
 *	Solving the problem of Write/read using pthread.h librery
 *
 *
 *****************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define AVAILABLE 1 
#define FULL 0
#define WAITING 2
#define INDB 3
#define READER 4
#define WRITER 5
#define DONE 6


typedef struct User
{
		int id;
		int userType;
		int stat;
		int DBCount;
		float timeDB;
}user;


typedef struct DataBase
{
		int *users;
		int usersInDB;
		int userType;
		int capacity;
		int status;
		float writeTime;
		float readTime;
		int rwCount;
}dataBase;

dataBase *DB;


/*
 * 
 * 
 */
void *Process(void *p)
{
  
		clock_t time;
		float timeDiff;
		user *m;
		int flag = 0;
   m = (user*) p;
   printf("\n\n**************************************\n\n");
   printf("Thread = %d  - Beginning The Task\n\n",m->id);
   while(m->DBCount != DB->rwCount && m->stat != DONE)
   {
		   if(m->stat == WAITING)
		   {
				if(DB->usersInDB == 0)
				{
						//DB->users[0] = m->id;
						DB->usersInDB += 1;
						DB->userType = m->userType;
						m->stat = INDB;
						//m->DBCount += 1;
						time = clock();
						m->timeDB = (float)time;
						flag = 0;
				}
				else
				{
						if(DB->userType == READER && DB->usersInDB < DB->capacity && m->userType == READER)	   
						{
						//		DB->users[DB->usersInDB] = m->id;
								DB->usersInDB += 1;
								m->stat = INDB;
								time = clock();
								m->timeDB = (float)time;
								//m->DBCount += 1;
								flag = 0;
					   }
				}
		   }
		   else
		   {
				   time = clock();
				   if(m->userType == READER)
						   timeDiff = ((float)time - m->timeDB)/CLOCKS_PER_SEC - (float)DB->readTime/1000;
				   else
						   timeDiff = ((float)time - m->timeDB)/CLOCKS_PER_SEC - (float)DB->writeTime/1000;
				   if(timeDiff >= 0)
				   {
						   printf("Thread %d Saliendo de DB \n", m->id);
						  // DBExit(DB, m->id);
						   DB->usersInDB -= 1;
						   m->DBCount += 1;
						   if(m->DBCount < DB->rwCount)
								   m->stat = WAITING;
						   else
								   m->stat = DONE;
						   flag=0;
				   }
		   }
		   if(flag == 0)
		   {
			   printf("-------------------------- \n");
			   printf("User %d %d <", m->id, m->DBCount);

			   if(m->userType == READER)
			   {
					   if(m->stat == WAITING)
							   printf("Waiting for Read");
					   else
							   printf("Reading");
			   }
			   else
			   {
					   if(m->stat == WAITING)
							   printf("Waiting for Write");
					   else
							   printf("Writing");
			   }
			   printf("> \n");
			   flag = 1;
		   }
//		   printf("Users in DB: %d \n", DB->usersInDB);

   }


   printf("Thread:%d Ending The Task\n\n",m->id);   
   pthread_exit(0);
}   

void Usage(char *message) {
  
   printf("\nUsage: %s -O K < datafile",message);
   printf("\n\nO in {S,V}\n\n");
   printf("K: Theads Number\n\n");
}   
   
   
/*
 * 
 * 
 */
int main(int argc, char **argv) {

   int i, n, k, t;
   pthread_t *thread;
   pthread_attr_t attribute;
   user **m;
   void *exit_status;   


		DB = (dataBase*)calloc(1,sizeof(dataBase));

   if (argc == 5)
   {
      n = atoi(argv[1]);
	  k = atoi(argv[2]);
	  t = atoi(argv[3]);
	  i = atoi(argv[4]);

	  DB->users = (int*)calloc(n,sizeof(int));
	  DB->usersInDB = 0;
	  DB->capacity = k;
	  DB->status = AVAILABLE;
	  DB->writeTime = (float)t;
	  DB->readTime = (float)t*0.25;
	  DB->rwCount = i;
/*	  printf("Los argumentos son: \n");
	  printf("Número de usuarios: %d \n", n);
	  printf("Capacidad de lectura concurrente: %d \n", k);
	  printf("Tiempo de escritura: %d milisegundos\n", t);
	  printf("Número de accesos a la BD por usuario: %d \n", i);
	  */

      thread = calloc(n,sizeof(pthread_t));
      m = (user**)calloc(n,sizeof(user*));
      for (i = 0; i < n; i += 1)
         m[i] = (user*)calloc(1,sizeof(user));
      pthread_attr_init(&attribute);
      pthread_attr_setdetachstate(&attribute,PTHREAD_CREATE_JOINABLE);
      for (i = 0; i < n; i = i + 1)
	  {
         printf("Main: creating thread %d\n", i);
	     m[i]->id = i;
		 if(rand()%2 == 1)
				m[i]->userType = READER;
		 else
				m[i]->userType = WRITER;
		 m[i]->stat = WAITING;
		 m[i]->timeDB = 0;
		 m[i]->DBCount = 0;

         pthread_create(&thread[i],&attribute,Process,(void *) m[i]);
      }      
      pthread_attr_destroy(&attribute); 
      for (i = 0; i < k; i = i + 1)
         pthread_join(thread[i],&exit_status);
     // PrintMap(r,c);      
	  
  }
  else
     Usage(argv[0]);
  return 0;
}   
