//
// Created by ruben on 28/03/23.
//

#include <stdio.h>
#include "keys/keys.h"

int main() {

    char gv1[MSG_CHAR_SIZE];
    int gv2 = 0;
    double gv3 = 0.0;

    printf("\tTESTS WITH ACTIVE SERVER\n");
    printf("init(): %d\n", init());

    printf("set_value(0, \"ASDF\", 78001, 2.0f): %d <must be 0>\n", set_value(0, "ASDF", 78001, 2.0f));
    printf("exist(0): %d <must be 1>\n", exist(0));
    printf("set_value(0, \"ASDF\", 1, 2.0f): %d <now it must be -1>\n", set_value(0, "ASDF", 1, 2.0f));

    int g = get_value(0, gv1, &gv2, &gv3);
    printf("get_value(key == 0): %d with values [%s], %d, %f\n", g, gv1, gv2, gv3);

    int g2 = get_value(1, gv1, &gv2, &gv3);
    printf("get_value(key == 1): %d with values [%s], %d, %f <now it must be -1, ignore other data>\n", g2, gv1, gv2, gv3);
    printf("modify_value(0, \"QWERTY\", 2, 4.0f): %d <now it must be 0>\n", modify_value(0, "QWERTY", 2, 4.0f));
    int g3 = get_value(0, gv1, &gv2, &gv3);
    printf("get_value(key == 0): %d with values [%s], %d, %f\n", g3, gv1, gv2, gv3);

    printf("exist(1): %d <now it must be 0>\n", exist(1));
    int cp = copy_key(0, 1);
    printf("copy_key(0,1): %d <it must be 0>\n", cp);
    int cp2 = copy_key(0, 0);
    printf("copy_key(0,0): %d <it must be -1>\n", cp2);

    int g4 = get_value(0, gv1, &gv2, &gv3);
    int del = delete_key(0);
    printf("get_value(key == 0): %d with values [%s], %d, %f\n", g4, gv1, gv2, gv3);
    printf("delete_key(0): %d <it must be 0>\n", del);
    printf("exist(0): %d <it must be 0>\n", exist(0));

    printf("exist(1): %d <it must be 1>\n", exist(1));
    int g5 = get_value(1, gv1, &gv2, &gv3);
    printf("get_value(key == 1): %d con valores [%s], %d, %f\n", g5, gv1, gv2, gv3);
    printf("delete_key(1): %d <it must be 0>\n", delete_key(1));
    printf("exist(1): %d <it must be 0>\n", exist(1));


    return 0;
}
