// Testing mutex errors
// --------------------
#include <pthread.h>
#include <stdlib.h>
#include <iostream>

pthread_mutex_t my_mutex;

class MutexHolder
   {
   public:

    pthread_mutex_t M;

    MutexHolder()
       {
       pthread_mutex_init(&M, NULL);
       }
   
    ~MutexHolder()
       {
       pthread_mutex_destroy(&M);
       }
   };

MutexHolder *MH;

int main(int argc, char **argv)
   {
   int error;

   if(argc==1)
      {
      std::cout << "\n Usage: tastmutex choice" << std::endl;
      exit(0);
      }
   int choice = atoi(argv[1]);

   if(choice!=2) 
      {
      std::cout << "\n Initilizing mutex " << std::endl;
      pthread_mutex_init(&my_mutex, NULL);
      }

   switch(choice)
      {
      case 1: std::cout << "\n Locking an initialized mutex" << std::endl;
              pthread_mutex_lock(&my_mutex);
              pthread_mutex_unlock(&my_mutex);
              break;
      case 2: std::cout << "\n Locking an uninitialized mutex" << std::endl;
              error = pthread_mutex_lock(&my_mutex);
              if(error)
                 {
                 std::cout << "\n Error in unitialized mutex lock" << std::endl;
                 exit(0);
                 }
              pthread_mutex_unlock(&my_mutex);
              break;
      case 3: std::cout << "\n Locking a destroyed mutex" << std::endl;
              pthread_mutex_destroy(&my_mutex);
              error = pthread_mutex_lock(&my_mutex);
              if(error)
                 {
                 std::cout << "\n Error, non existing mutex lock" << std::endl;
                 exit(0);
                 }
              pthread_mutex_unlock(&my_mutex);
              break;
      case 4: MH = new MutexHolder();
              delete MH;
              std::cout << "\n Locking a destroyed inner mutex" << std::endl;
              error = pthread_mutex_lock(&(MH->M));
              if(error)
                 {
                 std::cout << "\n Error, inner mutex lock" << std::endl;
                 exit(0);
                 }
              pthread_mutex_unlock(&(MH->M));
      case 5: MH = new MutexHolder();
              delete MH;
              std::cout << "\n Locking a destroyed inner mutex" << std::endl;
              pthread_mutex_lock(&(MH->M));
              pthread_mutex_unlock(&(MH->M));
      default: break;
      }
   }
