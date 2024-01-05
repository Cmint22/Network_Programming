#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

void printBoard(int board[8][8], int pieces_gamer1, int pieces_gamer2)
{
    int line, column;
    for(line = -1; line < 8; line++)
    {
        if(line >= 0)
            printf("  %d ", line + 1); //hien tung do ban co
        else
            printf("   ");

        for(column = 0; column < 8; column++)
        {
            if(line >= 0)
            {
                switch(board[line][column])
                {
                case 1: printf("  H "); break; 		// Ma 1
                case 2: printf("  T "); break; 		// Xe 1
                case 3: printf("  B "); break;		// Tuong 1
                case 4: printf("  Q "); break;		// Hau 1
                case 5: printf("  K "); break;		// Vua 1
                case 6: printf("  P "); break;		// Tot 1

                case 7: printf("  h "); break; 		// Ma 2
                case 8: printf("  t "); break; 		// Xe 2
                case 9: printf("  b "); break;		// Tuong 2
                case 10: printf("  q "); break;		// Hau 2
                case 11: printf("  k "); break;		// Vua 2
                case 12: printf("  p ");break;		// Tot 2

                case 13: printf("    "); break;		// Khoang trang

                }
            }
            else
                printf("   %d", column + 1); // hien hoanh do ban co
            if(column == 7 && line == -1)
                printf("                           -----------------------------");
            else if(column == 7 && line == 0)
                printf("                          |           SCORE           |");
            else if(column == 7 && line == 1)
                printf("                          |                           |");
            else if(column == 7 && line == 2) // So quan gamer1 da an duoc
            {
                if((16 - pieces_gamer2) <= 10)
                    printf("                          |        GAMER 1 = %d        |", (16 - pieces_gamer2));
                else
                    printf("                          |        GAMER 1 = %d       |", (16 - pieces_gamer2));
            }
            else if(column == 7 && line == 3) // So quan gamer2 da an duoc
            {
                if((16 - pieces_gamer2) <= 10)
                    printf("                          |        GAMER 2 = %d        |", (16 - pieces_gamer1));
                else
                    printf("                          |        GAMER 2 = %d       |", (16 - pieces_gamer1));
            }
            else if(column == 7 && line == 4)

                printf("                          |                           |");
            else if(column == 7 && line == 5)
                printf("                          -----------------------------");
        }
        printf("\n");
    }
    printf("\n\n");
}
