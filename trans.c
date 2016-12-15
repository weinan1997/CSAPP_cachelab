// English Name: Nathaniel Wei		Chinese Name: Wei Nan		School Number: 151220120

/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	int i, j, a, b, tmp;
	if(M == 32 && N == 32)
	{	
		for(a = 0; a < N; a += 8)
			for(b = 0; b < M; b += 8)
   				for (i = a; i < a + 8; i++)
				{
        				for (j = b; j < b + 8; j++)
					{
						if(i != j)
            						B[j][i] = A[i][j];
						else
							tmp = A[i][j];
					}
					if(a == b)
						B[i][i] = tmp;
				}
	}
	else if(M == 61 && N == 67)
	{	
		for(a = 0; a < N; a += 16)
			for(b = 0; b < M; b += 16)
   				for (i = a; (i < a + 16) && (i < N); i++)
				{
        				for (j = b; (j < b + 16) && (j < M); j++)
					{
						if(i != j)
            						B[j][i] = A[i][j];
						else
							tmp = A[i][j];
					}
					if(a == b)
						B[i][i] = tmp;
				}
	}
	else if(M == 64 && N == 64)
	{	
		for(a = 0; a < N; a += 8)
			for(b = 0; b < M; b += 8)
			{
   				for (i = a; i < a + 4; i++)
				{
        				for (j = b; j < b + 4; j++)
					{
						if(i != j)
            						B[j][i] = A[i][j];
						else
							tmp = A[i][j];
						B[j][i+4] = A[i][j+4];
					}
					if(a == b)
						B[i][i] = A[i][i];
				}
				i = a + 4;
				for(j = b; j < b + 4; j++)
				{
					int t0, t1, t2, t3;
					t0 = B[j][i];
					t1 = B[j][i+1];
					t2 = B[j][i+2];
					t3 = B[j][i+3];
					B[j][i] = A[i][j];
					B[j][i+1] = A[i+1][j];
					B[j][i+2] = A[i+2][j];
					B[j][i+3] = A[i+3][j];
					B[j+4][a] = t0;
					B[j+4][a+1] = t1;
					B[j+4][a+2] = t2;
					B[j+4][a+3] = t3;
				}
				for(i = a + 4; i < a + 8; i++)
				{
					for(j = b + 4; j < b + 8; j++)
					{
						if(i != j)
							B[j][i] = A[i][j];
						else
							tmp = A[i][j];
					}
					if(a == b)
						B[i][i] = A[i][i];
				}

			}
	}

}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

