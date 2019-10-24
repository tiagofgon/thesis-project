
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <CpuTimer.h>
#include <SPool.h>
#include <ABarrier.h>
#include <Barrier.h>

using namespace std;
SPool *TH;

FILE *inFile, *outFile;
char * line = NULL;
size_t len = 0;
ssize_t read;
char header[4][2000];
int width, height;

ABarrier  *B;

int **inImage;
int **outImage;

int **allocate_dynamic_matrix(int row, int col)
{
    int **ret_val;
    int i;
 
    ret_val = (int **)malloc(sizeof(int *) * row);
    if (ret_val == NULL) {
        perror("memory allocation failure");
        exit(EXIT_FAILURE);
    }
 
    for (i = 0; i < row; ++i) {
        ret_val[i] = (int *)malloc(sizeof(int) * col);
        if (ret_val[i] == NULL) {
            perror("memory allocation failure");
            exit(EXIT_FAILURE);
        }
    }
 
    return ret_val;
}
 
void deallocate_dynamic_matrix(int **matrix, int row)
{
    int i;
 
    for (i = 0; i < row; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}


void thread_fct(void *P) {

    int nrPixelsChanged = 1;
    int no_BPs;
    int no_Swaps;
    float tempo=0;
    int n_lines = height;
    int beg;
    int sss=0;

    {
    // while(nrPixelsChanged){
    while(sss<335){
      sss++;
      nrPixelsChanged = 0;

      // Values in the sides do not have 8 neighbours schedule(static)
      
       //reduction(+:tempo)
      std::pair<int, int> par = TH->shedule_static(height);
      beg=par.first;
      n_lines=par.second;
      if(beg==0)
        beg++;
      for(int h=beg; h<n_lines; h++){
        for(int w=1; w<width-1; w++){
          if(inImage[h][w]==1){
            // Here we save the Point neighbours into a local array for 
            int P[8] = {inImage[h][w-1],inImage[h+1][w-1],inImage[h+1][w],inImage[h+1][w+1],inImage[h][w+1],inImage[h-1][w+1],inImage[h-1][w],inImage[h-1][w-1]};
            no_BPs = P[0]+P[1]+P[2]+P[3]+P[4]+P[5]+P[6]+P[7];
            if(no_BPs>=2 && no_BPs<=6){
              no_Swaps = (!P[0] && P[0] != P[1]) + (!P[1] && P[1] != P[2]) + (!P[2] && P[2] != P[3]) + (!P[3] && P[3] != P[4]) + (!P[4] && P[4] != P[5]) + (!P[5] && P[5] != P[6]) + (!P[6] && P[6] != P[7]) + (!P[7] && P[7] != P[0]);
              if(no_Swaps == 1){
                if(!P[0] + !P[2] + !P[4]){
                  if(!P[2] + !P[4] + !P[6]){
                    outImage[h][w] = 0;
                    nrPixelsChanged=1;
                  }
                }
              }
            }
          }
        }
      }
      
      B->Wait();
      
      par = TH->shedule_static(height);
      beg=par.first;
      n_lines=par.second;
      for(int h=beg; h<n_lines; h++){
        for(int w=0; w<width; w++){
          inImage[h][w] = outImage[h][w];
        }
      }
      
      B->Wait();

      // Values in the sides do not have 8 neighbours
      par = TH->shedule_static(height);
      beg=par.first;
      n_lines=par.second;
      if(beg==0)
        beg++;
      for(int h=beg; h<n_lines; h++){
        for(int w=1; w<width-1; w++){
          if(inImage[h][w]==1){
            // Here we save the Point neighbours into a local array for 
            int P[8] = {inImage[h][w-1],inImage[h+1][w-1],inImage[h+1][w],inImage[h+1][w+1],inImage[h][w+1],inImage[h-1][w+1],inImage[h-1][w],inImage[h-1][w-1]};
            no_BPs = P[0]+P[1]+P[2]+P[3]+P[4]+P[5]+P[6]+P[7];
            if(no_BPs>=2 && no_BPs<=6){
              no_Swaps = (!P[0] && P[0] != P[1]) + (!P[1] && P[1] != P[2]) + (!P[2] && P[2] != P[3]) + (!P[3] && P[3] != P[4]) + (!P[4] && P[4] != P[5]) + (!P[5] && P[5] != P[6]) + (!P[6] && P[6] != P[7]) + (!P[7] && P[7] != P[0]);
              if(no_Swaps == 1){
                if((P[0]+P[6]+(P[2]*P[4])) == 1){
                    outImage[h][w] = 0;
                    nrPixelsChanged=1;
                  }
                if(!P[0] + !P[2] + !P[6]){
                  if(!P[0] + !P[4] + !P[6]){
                    outImage[h][w] = 0;
                    nrPixelsChanged=1;
                  }
                }
              }
            }
          }
        }
      }
      B->Wait();

      
      par = TH->shedule_static(height);
      beg=par.first;
      n_lines=par.second;
      for(int h=beg; h<n_lines; h++){
        for(int w=0; w<width; w++){
          inImage[h][w] = outImage[h][w];
        }
      }
      
      B->Wait();
    }
    }






    // {
    // // while(nrPixelsChanged){
    
    // while(sss<452){
    //   sss++;
    //   nrPixelsChanged = 0;

    //   // Values in the sides do not have 8 neighbours schedule(static)
      
    //    //reduction(+:tempo)
    //   n_lines=0;
    //   while(n_lines<height) {
    //   std::pair<int, int> par = TH->shedule_dynamic(n_lines, height, 2);
    //   beg=par.first;
    //   n_lines=par.second;
    //   //cout << beg << "  " << n_lines << endl;
    //   if(beg==0)
    //     beg++;
    //   for(int h=beg; h<n_lines; h++){
    //     for(int w=1; w<width-1; w++){
    //       if(inImage[h][w]==1){
    //         // Here we save the Point neighbours into a local array for 
    //         int P[8] = {inImage[h][w-1],inImage[h+1][w-1],inImage[h+1][w],inImage[h+1][w+1],inImage[h][w+1],inImage[h-1][w+1],inImage[h-1][w],inImage[h-1][w-1]};
    //         no_BPs = P[0]+P[1]+P[2]+P[3]+P[4]+P[5]+P[6]+P[7];
    //         if(no_BPs>=2 && no_BPs<=6){
    //           no_Swaps = (!P[0] && P[0] != P[1]) + (!P[1] && P[1] != P[2]) + (!P[2] && P[2] != P[3]) + (!P[3] && P[3] != P[4]) + (!P[4] && P[4] != P[5]) + (!P[5] && P[5] != P[6]) + (!P[6] && P[6] != P[7]) + (!P[7] && P[7] != P[0]);
    //           if(no_Swaps == 1){
    //             if(!P[0] + !P[2] + !P[4]){
    //               if(!P[2] + !P[4] + !P[6]){
    //                 outImage[h][w] = 0;
    //                 nrPixelsChanged=1;
    //               }
    //             }
    //           }
    //         }
    //       }
    //     }
    //   }
    //   }
      
    //   B->Wait();
      
    //   n_lines=0;
    //   while(n_lines<height) {
    //   std::pair<int, int> par = TH->shedule_dynamic(n_lines, height, 2);
    //   beg=par.first;
    //   n_lines=par.second;
      
    //   for(int h=beg; h<n_lines; h++){
    //     for(int w=0; w<width; w++){
    //       inImage[h][w] = outImage[h][w];
    //     }
    //   }
    //   }
      
    //   B->Wait();

    //   n_lines=0;
    //   while(n_lines<height) {
    //   std::pair<int, int> par = TH->shedule_dynamic(n_lines, height, 2);
    //   beg=par.first;
    //   n_lines=par.second;
    //   if(beg==0)
    //     beg++;
    //   for(int h=beg; h<n_lines; h++){
    //     for(int w=1; w<width-1; w++){
    //       if(inImage[h][w]==1){
    //         // Here we save the Point neighbours into a local array for 
    //         int P[8] = {inImage[h][w-1],inImage[h+1][w-1],inImage[h+1][w],inImage[h+1][w+1],inImage[h][w+1],inImage[h-1][w+1],inImage[h-1][w],inImage[h-1][w-1]};
    //         no_BPs = P[0]+P[1]+P[2]+P[3]+P[4]+P[5]+P[6]+P[7];
    //         if(no_BPs>=2 && no_BPs<=6){
    //           no_Swaps = (!P[0] && P[0] != P[1]) + (!P[1] && P[1] != P[2]) + (!P[2] && P[2] != P[3]) + (!P[3] && P[3] != P[4]) + (!P[4] && P[4] != P[5]) + (!P[5] && P[5] != P[6]) + (!P[6] && P[6] != P[7]) + (!P[7] && P[7] != P[0]);
    //           if(no_Swaps == 1){
    //             if((P[0]+P[6]+(P[2]*P[4])) == 1){
    //                 outImage[h][w] = 0;
    //                 nrPixelsChanged=1;
    //               }
    //             if(!P[0] + !P[2] + !P[6]){
    //               if(!P[0] + !P[4] + !P[6]){
    //                 outImage[h][w] = 0;
    //                 nrPixelsChanged=1;
    //               }
    //             }
    //           }
    //         }
    //       }
    //     }
    //   }
    //   }
    //   B->Wait();

    //   n_lines=0;
    //   while(n_lines<height) {
    //   std::pair<int, int> par = TH->shedule_dynamic(n_lines, height, 2);
    //   beg=par.first;
    //   n_lines=par.second;
    //   //cout << beg << "  " << n_lines << endl;
    //   for(int h=beg; h<n_lines; h++){
    //     for(int w=0; w<width; w++){
    //       inImage[h][w] = outImage[h][w];
    //     }
    //   }
    //   }
      
    //   B->Wait();
    // }
    // }






    // {
    // // while(nrPixelsChanged){
    
    // while(sss<452){
    //   sss++;
    //   nrPixelsChanged = 0;

    //   // Values in the sides do not have 8 neighbours schedule(static)
      
    //    //reduction(+:tempo)
    //   n_lines=0;
    //   while(n_lines<height) {
    //   std::pair<int, int> par = TH->shedule_guided(n_lines, height, 1);
    //   beg=par.first;
    //   n_lines=par.second;
    //   //cout << beg << "  " << n_lines << endl;
    //   if(beg==0)
    //     beg++;
    //   for(int h=beg; h<n_lines; h++){
    //     for(int w=1; w<width-1; w++){
    //       if(inImage[h][w]==1){
    //         // Here we save the Point neighbours into a local array for 
    //         int P[8] = {inImage[h][w-1],inImage[h+1][w-1],inImage[h+1][w],inImage[h+1][w+1],inImage[h][w+1],inImage[h-1][w+1],inImage[h-1][w],inImage[h-1][w-1]};
    //         no_BPs = P[0]+P[1]+P[2]+P[3]+P[4]+P[5]+P[6]+P[7];
    //         if(no_BPs>=2 && no_BPs<=6){
    //           no_Swaps = (!P[0] && P[0] != P[1]) + (!P[1] && P[1] != P[2]) + (!P[2] && P[2] != P[3]) + (!P[3] && P[3] != P[4]) + (!P[4] && P[4] != P[5]) + (!P[5] && P[5] != P[6]) + (!P[6] && P[6] != P[7]) + (!P[7] && P[7] != P[0]);
    //           if(no_Swaps == 1){
    //             if(!P[0] + !P[2] + !P[4]){
    //               if(!P[2] + !P[4] + !P[6]){
    //                 outImage[h][w] = 0;
    //                 nrPixelsChanged=1;
    //               }
    //             }
    //           }
    //         }
    //       }
    //     }
    //   }
    //   }
      
    //   B->Wait();
      
    //   n_lines=0;
    //   while(n_lines<height) {
    //   std::pair<int, int> par = TH->shedule_guided(n_lines, height, 1);
    //   beg=par.first;
    //   n_lines=par.second;
      
    //   for(int h=beg; h<n_lines; h++){
    //     for(int w=0; w<width; w++){
    //       inImage[h][w] = outImage[h][w];
    //     }
    //   }
    //   }
      
    //   B->Wait();

    //   n_lines=0;
    //   while(n_lines<height) {
    //   std::pair<int, int> par = TH->shedule_guided(n_lines, height, 1);
    //   beg=par.first;
    //   n_lines=par.second;
    //   if(beg==0)
    //     beg++;
    //   for(int h=beg; h<n_lines; h++){
    //     for(int w=1; w<width-1; w++){
    //       if(inImage[h][w]==1){
    //         // Here we save the Point neighbours into a local array for 
    //         int P[8] = {inImage[h][w-1],inImage[h+1][w-1],inImage[h+1][w],inImage[h+1][w+1],inImage[h][w+1],inImage[h-1][w+1],inImage[h-1][w],inImage[h-1][w-1]};
    //         no_BPs = P[0]+P[1]+P[2]+P[3]+P[4]+P[5]+P[6]+P[7];
    //         if(no_BPs>=2 && no_BPs<=6){
    //           no_Swaps = (!P[0] && P[0] != P[1]) + (!P[1] && P[1] != P[2]) + (!P[2] && P[2] != P[3]) + (!P[3] && P[3] != P[4]) + (!P[4] && P[4] != P[5]) + (!P[5] && P[5] != P[6]) + (!P[6] && P[6] != P[7]) + (!P[7] && P[7] != P[0]);
    //           if(no_Swaps == 1){
    //             if((P[0]+P[6]+(P[2]*P[4])) == 1){
    //                 outImage[h][w] = 0;
    //                 nrPixelsChanged=1;
    //               }
    //             if(!P[0] + !P[2] + !P[6]){
    //               if(!P[0] + !P[4] + !P[6]){
    //                 outImage[h][w] = 0;
    //                 nrPixelsChanged=1;
    //               }
    //             }
    //           }
    //         }
    //       }
    //     }
    //   }
    //   }
    //   B->Wait();

    //   n_lines=0;
    //   while(n_lines<height) {
    //   std::pair<int, int> par = TH->shedule_guided(n_lines, height, 1);
    //   beg=par.first;
    //   n_lines=par.second;
    //   //cout << beg << "  " << n_lines << endl;
    //   for(int h=beg; h<n_lines; h++){
    //     for(int w=0; w<width; w++){
    //       inImage[h][w] = outImage[h][w];
    //     }
    //   }
    //   }
      
    //   B->Wait();
    // }
    // }
    

}




int main(int argc, char *argv[]) {

  CpuTimer TR;         // object to measure execution times
  int nTh = 4;
  TH = new SPool(nTh);
  B = new ABarrier(nTh);

  // Number of arguments 2, Function and the image to skeletonize
	if( argc == 2 ) {

		//printf("Opening image %s\n", argv[1]);
		char outFileName[] = "new_image.pgm";

		inFile = fopen(argv[1], "r");

		if (inFile==NULL){
			fprintf(stderr, "Error opening image!\n");
			exit(1);
		}

		outFile = fopen(outFileName, "w");

		if (outFile==NULL){
			fprintf(stderr, "Problem trying to open file for saving image!");
			exit(1);
		}

    // Reading header of Image, in case the format is not the same, there might be problems
    for (int i=0; i<4;i++) {
        if(getline(&line, &len, inFile)>0)
          sprintf(header[i],"%s",line );
        else {
          fprintf(stderr, "Error reading image header!\n");
          exit(1);
        }
    }

    // Writing header of Image in our output image
    for (int i=0; i<4;i++){
      //printf("%s",header[i] );
      fprintf(outFile,"%s",header[i] );
    }

    // Saving Image dimentions
    width = atoi(strtok(header[2], " "));
    height = atoi(strtok(NULL, " "));

    inImage = allocate_dynamic_matrix(height, width);
    outImage = allocate_dynamic_matrix(height, width);


    // This step is only to make the matrix binary
    for (int h=0;h<height;h++){
      for (int w=0;w<width;w++){
        if(getline(&line, &len, inFile)>0){
          int aux = atoi(line)/255;
          inImage[h][w] = aux;
          outImage[h][w] = aux;
        }
      }
    }
    

    // #pragma omp parallel
    // for(int i=0; i<100; i++) {
		//   int id = omp_get_thread_num();
		//   printf("T%d:i%d \n", id, i );
    // }
    



    // Skeletenization 

  TR.Start();
  TH->Dispatch(thread_fct, NULL);
  TH->WaitForIdle();
  TR.Stop();

  TR.Report();





    // copy to file
    for (int h=0;h<height;h++){
      for (int w=0;w<width;w++){
        fprintf(outFile, "%d\n", outImage[h][w]*255);
      }
    }

    fclose(inFile);
    fclose(outFile);

    deallocate_dynamic_matrix(inImage, height);
    deallocate_dynamic_matrix(outImage, height);

    //printf("%f\n", tempo);
	}

	else if( argc > 2 ) {
		printf("Too many arguments supplied.\n");
	}

	else {
 		printf("Pass the image filename as argument!\n");
	}

  return 0;
}