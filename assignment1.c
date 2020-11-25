#include <stdint.h>
#include <stdio.h>
typedef uint16_t tfl16_t;

tfl16_t  tfl_sign(tfl16_t value);
int8_t   tfl_exponent(tfl16_t value);
uint16_t tfl_significand(tfl16_t value);
uint8_t  tfl_equals(tfl16_t a, tfl16_t b);
uint8_t  tfl_greaterthan(tfl16_t a, tfl16_t b);
tfl16_t  tfl_normalize(uint8_t sign, int8_t exponent, uint16_t significand);
tfl16_t  tfl_add(tfl16_t a, tfl16_t b);
tfl16_t  tfl_mul(tfl16_t a, tfl16_t b);
int8_t tfl_exponent(tfl16_t value);
double power(int x,int n);
void test_tfl_sign();
void test_tfl_exponent();
void test_tfl_significand();
void test_tfl_equals();
void test_tfl_greaterthan();
void test_tfl_normalize();
void test_tfl_add();
void test_tfl_mul();

#define POSITIVE_ZERO (0b0000000000000000)
#define NEGATIVE_ZERO (0b1000000000000000)
#define POSITIVE_ONE (0b0000011000000000)
#define NEGATIVE_ONE (0b1000011000000000)
#define NEGATIVE_FIRST (0b1000000000000000)
#define ISOLATE_SIGNIFICANT (0b0000001111111111)
#define ISOLATE_EXPONENT (0b0000000000001111)
#define ISOLATE_EXPONENT_SIGN (0b0000000000000001)

double power(int x,int n){
    int i;
    int result = 1;
    double result2 = 1;
    if (n < 0){
        n = n * -1;
        for (i = 0; i < n; ++i){
            result2 *= x;        
        }
        result2 = (1/result2);
        return result2;
    }
    else{
        for (i = 0; i < n; ++i){
            result *= x;        
        }
        return(result);
    }
}

int main(){
    test_tfl_sign();
    test_tfl_exponent();
    test_tfl_significand();
    test_tfl_equals();
    test_tfl_greaterthan();
    test_tfl_normalize();
    test_tfl_add();
    test_tfl_mul();
    return 0;
}

tfl16_t  tfl_mul(tfl16_t a, tfl16_t b){
    tfl16_t a_sign = tfl_sign(a);
    tfl16_t b_sign = tfl_sign(b);
    int8_t a_exponent = tfl_exponent(a);
    int8_t b_exponent = tfl_exponent(b);   
    uint16_t a_significand = tfl_significand(a);
    uint16_t b_significand = tfl_significand(b);
    uint16_t sig_total;
    int8_t ex_total = 0;
    uint8_t sign = 0;

    double sum = 0.0;

    double a_total = ((power(2,a_exponent))* (a_significand / 1024.0));
    if (a_sign == NEGATIVE_ONE){
        a_total = a_total * -1.0;
    }

    double b_total = ((power(2,b_exponent))* (b_significand / 1024.0));
    if (b_sign == NEGATIVE_ONE){
        b_total = b_total * -1.0;
    }

    sum = a_total * b_total;

    if((sum > 32736)||((sum < 0.00000002980232238770) & (sum > -0.00000002980232238770 ) & (sum != 0))){
        printf("\nERROR: Bitmap overflow\n");
        return 0;
    }

    if (sum < 0){
        sign = 1;
        sum = sum * -1;
    }

    int sig_tot = sum * 1024;
    while (sig_tot > 1023){
        sig_tot = sig_tot / 2; 
        ex_total = ex_total + 1;
    }

    sig_total = sig_tot;
    tfl16_t result = tfl_normalize(sign, ex_total, sig_total);

    return result;
}

tfl16_t  tfl_add(tfl16_t a, tfl16_t b){
    tfl16_t a_sign = tfl_sign(a);
    tfl16_t b_sign = tfl_sign(b);
    int8_t a_exponent = tfl_exponent(a);
    int8_t b_exponent = tfl_exponent(b);   
    uint16_t a_significand = tfl_significand(a);
    uint16_t b_significand = tfl_significand(b);
    uint16_t sig_total;
    int8_t ex_total = 0;
    uint8_t sign = 0;

    double sum = 0.0;

    double a_total = ((power(2,a_exponent))* (a_significand / 1024.0));
    if (a_sign == NEGATIVE_ONE){
        a_total = a_total * -1.0;
    }

    double b_total = ((power(2,b_exponent))* (b_significand / 1024.0));
    if (b_sign == NEGATIVE_ONE){
        b_total = b_total * -1.0;
    }

    sum = a_total + b_total;

    if((sum > 32736)||((sum < 0.00000002980232238770) & (sum > -0.00000002980232238770 ) & (sum != 0))){
        printf("\nERROR: Bitmap overflow\n");
        return 0;
    }

    if (sum < 0){
        sign = 1;
        sum = sum * -1;
    }

    int sig_tot = sum * 1024;
    while (sig_tot > 1023){
        sig_tot = sig_tot / 2; 
        ex_total = ex_total + 1;
    }

    sig_total = sig_tot;
    tfl16_t result = tfl_normalize(sign, ex_total, sig_total);

    return result;
}

tfl16_t  tfl_normalize(uint8_t sign, int8_t exponent, uint16_t significand){
    int singidicand_overflow = 0;
    int exponent_overflow = 0;
    tfl16_t normalized = 0b0000000000000000;

    while (significand > 1023){
        significand = significand / 2; 
        exponent = exponent + 1;
    }
    if (exponent > 31){
        printf("\nERROR: Bitmap overflow\n");
        return 0;
    }
    int i, c, k, c2, k2;
    if (significand != 0){
        for (c = 9; c >= 0; c--){
            k = significand >> c;
            if (k & 1)
                normalized ^= 1UL << c;
        }
    }
    if(exponent != 0){
        for (c2 = 4; c2 >= 0; c2--){
            k2 = exponent >> c2;
            if (k2 & 1)
                normalized ^= 1UL << (c2 + 10);
        }
    }

    if (sign == 1){
        normalized ^= 1UL << (15);
    }

    return normalized;
}

int8_t tfl_exponent(tfl16_t value){
    tfl16_t exponent = value >> 10;
    tfl16_t exponent_sign = value >> 14;
    exponent_sign = (exponent_sign & ISOLATE_EXPONENT_SIGN);
    exponent = (exponent & ISOLATE_EXPONENT);
    int8_t clean_exponent = exponent;
    if (exponent_sign == 1){
        clean_exponent = (-16 + (clean_exponent));
    }
    return clean_exponent;
}

uint16_t tfl_significand(tfl16_t value){
    uint16_t significand = (value & ISOLATE_SIGNIFICANT);
    return significand;
}

uint8_t tfl_equals(tfl16_t a, tfl16_t b){
    tfl16_t a_sign = tfl_sign(a);
    tfl16_t b_sign = tfl_sign(b);
    int8_t a_exponent = tfl_exponent(a);
    int8_t b_exponent = tfl_exponent(b);   
    uint16_t a_significand= tfl_significand(a);
    uint16_t b_significand = tfl_significand(b);  

    double a_total = ((power(2,a_exponent))* (a_significand / 1024.0));
    if (a_sign == NEGATIVE_ONE){
        a_total = a_total * -1.0;
    }

    double b_total = ((power(2,b_exponent))* (b_significand / 1024.0));
    if (b_sign == NEGATIVE_ONE){
        b_total = b_total * -1.0;
    }

    if (a_total == b_total){
        return 1;
    }
    else{
        return 0;
    }
}

uint8_t  tfl_greaterthan(tfl16_t a, tfl16_t b){
    int i, s;
    int a_ex = 1;
    int b_ex = 1;
    double a_total = 0.0;
    double b_total = 0.0;
    tfl16_t a_sign = tfl_sign(a);
    tfl16_t b_sign = tfl_sign(b);
    int8_t a_exponent = tfl_exponent(a);
    int8_t b_exponent = tfl_exponent(b);   
    uint16_t a_significand= tfl_significand(a);
    uint16_t b_significand = tfl_significand(b);  

    if (a_exponent > 0){
        for (i=0; i< a_exponent; ++i){
            a_ex = 2 * a_ex;
        } 
    }
    else{
        for (i=0; i > a_exponent; --i){
            a_ex = 2 * a_ex;
        }
    }
    if (b_exponent > 0){
        for (i=0; i< b_exponent; ++i){
            b_ex = 2 * b_ex;
        } 
    }
    else{
        for (i=0; i > b_exponent; --i){
            b_ex = 2 * b_ex;
        }
    }

    if (a_sign == POSITIVE_ONE){
        a_total = (a_ex * (a_significand/1024.0));
    }
    else if (a_sign == NEGATIVE_ONE){
        a_total = (-1.0 * a_ex * (a_significand/1024.0));
    }
    else{
        a_total = 0.0;
    }

    if (b_sign == POSITIVE_ONE){
        b_total = (b_ex * (b_significand/1024.0));
    }
    else if (b_sign == NEGATIVE_ONE){
        b_total = (-1.0 * b_ex * (b_significand/1024.0));
    }
    else{
        b_total = 0.0;
    }

    if (a_total > b_total){
        return 1;
    }
    else{
        return 0;
    }
}

tfl16_t tfl_sign(tfl16_t value){
    uint16_t neg_sign = (value & NEGATIVE_FIRST);
    if (value == POSITIVE_ZERO){
        return POSITIVE_ZERO;
    }
    else if (value == NEGATIVE_ZERO){
        return POSITIVE_ZERO;
    }
    else if (neg_sign == NEGATIVE_FIRST ){
        return NEGATIVE_ONE;        
    }
    else{
        return POSITIVE_ONE;
    }
}

void test_tfl_sign(){
    tfl16_t test_1 = 0b1000000000000101; // negative number
    tfl16_t test_2 = 0b0000000000000000; // zero
    tfl16_t test_3 = 0b1000000000000000; // negative zero
    tfl16_t test_4 = 0b0000000000000101; // positive number
    tfl16_t test_5 = 0b1011111111111111; // large negative number
    tfl16_t test_6 = 0b0011111111111111; // large positive number
    tfl16_t test_7 = 0b1111110000000001; // small negative number
    tfl16_t test_8 = 0b0111110000000001; // small positive number
    printf("\n---------- tfl_sign Test ----------\n");

    if (tfl_sign(test_1) != NEGATIVE_ONE){
        printf("\ntfl_sign Test 1 failed.\n");}
    else{
        printf("\ntfl_sign Test 1 passed.\n");}
    if (tfl_sign(test_2) != POSITIVE_ZERO){
        printf("\ntfl_sign Test 2 failed.\n");}
    else{
        printf("\ntfl_sign Test 2 passed.\n");}
    if (tfl_sign(test_3) != POSITIVE_ZERO){
        printf("\ntfl_sign Test 3 failed.\n");}
    else{
        printf("\ntfl_sign Test 3 passed.\n");}
    if (tfl_sign(test_4) != POSITIVE_ONE){
        printf("\ntfl_sign Test 4 failed.\n");}
    else{
        printf("\ntfl_sign Test 4 passed.\n");}
    if (tfl_sign(test_5) != NEGATIVE_ONE){
        printf("\ntfl_sign Test 5 failed.\n");}
    else{
        printf("\ntfl_sign Test 5 passed.\n");}
    if (tfl_sign(test_6) != POSITIVE_ONE){
        printf("\ntfl_sign Test 6 failed.\n");}
    else{
        printf("\ntfl_sign Test 6 passed.\n");}
    if (tfl_sign(test_7) != NEGATIVE_ONE){
        printf("\ntfl_sign Test 7 failed.\n");}
    else{
        printf("\ntfl_sign Test 7 passed.\n");}
    if (tfl_sign(test_8) != POSITIVE_ONE){
        printf("\ntfl_sign Test 8 failed.\n");}
    else{
        printf("\ntfl_sign Test 8 passed.\n");}
}

void test_tfl_exponent(){
    tfl16_t test_1 = 0b1000010000000101; // negative number
    tfl16_t test_2 = 0b0000000000000000; // zero
    tfl16_t test_3 = 0b1000000000000000; // negative zero
    tfl16_t test_4 = 0b0000010000000101; // positive number
    tfl16_t test_5 = 0b1011111111111111; // large negative number
    tfl16_t test_6 = 0b0011111111111111; // large positive number
    tfl16_t test_7 = 0b1100010000000001; // small negative number
    tfl16_t test_8 = 0b0100010000000001; // small positive number
    printf("\n---------- tfl_exponent Test ----------\n");

    if (tfl_exponent(test_1) != 1){
        printf("\ntfl_exponent Test 1 failed.\n");}
    else{
        printf("\ntfl_exponent Test 1 passed.\n");}
    if (tfl_exponent(test_2) != 0){
        printf("\ntfl_exponent Test 2 failed.\n");}
    else{
        printf("\ntfl_exponent Test 2 passed.\n");}
    if (tfl_exponent(test_3) != 0){
        printf("\ntfl_exponent Test 3 failed.\n");}
    else{
        printf("\ntfl_exponent Test 3 passed.\n");}
    if (tfl_exponent(test_4) != 1){
        printf("\ntfl_exponent Test 4 failed.\n");}
    else{
        printf("\ntfl_exponent Test 4 passed.\n");}
    if (tfl_exponent(test_5) != 15){
        printf("\ntfl_exponent Test 5 failed.\n");}
    else{
        printf("\ntfl_exponent Test 5 passed.\n");}
    if (tfl_exponent(test_6) != 15){
        printf("\ntfl_exponent Test 6 failed.\n");}
    else{
        printf("\ntfl_exponent Test 6 passed.\n");}
    if (tfl_exponent(test_7) != -15){
        printf("\ntfl_exponent Test 7 failed.\n");}
    else{
        printf("\ntfl_exponent Test 7 passed.\n");}
    if (tfl_exponent(test_8) != -15){
        printf("\ntfl_exponent Test 8 failed.\n");}
    else{
        printf("\ntfl_exponent Test 8 passed.\n");}

}

void test_tfl_significand(){
    tfl16_t test_1 = 0b1000010000000101; // negative number
    tfl16_t test_2 = 0b0000000000000000; // zero
    tfl16_t test_3 = 0b1000000000000000; // negative zero
    tfl16_t test_4 = 0b0000010000000101; // positive number
    tfl16_t test_5 = 0b1011111111111111; // large negative number
    tfl16_t test_6 = 0b0011111111111111; // large positive number
    tfl16_t test_7 = 0b1100010000000001; // small negative number
    tfl16_t test_8 = 0b0100010000000001; // small positive number
    printf("\n---------- tfl_significand Test ----------\n");

    if (tfl_significand(test_1) != 5){
        printf("\ntfl_significand Test 1 failed.\n");}
    else{
        printf("\ntfl_significand Test 1 passed.\n");}
    if (tfl_significand(test_2) != 0){
        printf("\ntfl_significand Test 2 failed.\n");}
    else{
        printf("\ntfl_significand Test 2 passed.\n");}
    if (tfl_significand(test_3) != 0){
        printf("\ntfl_significand Test 3 failed.\n");}
    else{
        printf("\ntfl_significand Test 3 passed.\n");}
    if (tfl_significand(test_4) != 5){
        printf("\ntfl_significand Test 4 failed.\n");}
    else{
        printf("\ntfl_significand Test 4 passed.\n");}
    if (tfl_significand(test_5) != 1023){
        printf("\ntfl_significand Test 5 failed.\n");}
    else{
        printf("\ntfl_significand Test 5 passed.\n");}
    if (tfl_significand(test_6) != 1023){
        printf("\ntfl_significand Test 6 failed.\n");}
    else{
        printf("\ntfl_significand Test 6 passed.\n");}
    if (tfl_significand(test_7) != 1){
        printf("\ntfl_significand Test 7 failed.\n");}
    else{
        printf("\ntfl_significand Test 7 passed.\n");}
    if (tfl_significand(test_8) != 1){
        printf("\ntfl_significand Test 8 failed.\n");}
    else{
        printf("\ntfl_significand Test 8 passed.\n");}
}
    
void test_tfl_equals(){
    tfl16_t test_1a,test_1b = 0b1000010000000101; // negative number
    tfl16_t test_2a = 0b0000000000000000; // zero
    tfl16_t test_2b = 0b1000000000000000; // negative zero
    tfl16_t test_3a = 0b1000000000000000; // negative zero
    tfl16_t test_3b = 0b1000000000000000; // zero
    tfl16_t test_4a, test_4b = 0b0000010000000101; // positive number
    tfl16_t test_5a = 0b0000011000000000; // = 1
    tfl16_t test_5b = 0b0000100100000000; // = 1 but different pattern
    tfl16_t test_6a = 0b1000011000000000; // = -1
    tfl16_t test_6b = 0b1000100100000000; // = -1 but different pattern
    tfl16_t test_7a = 0b0000011000000000; // Same exponent different significand
    tfl16_t test_7b = 0b0000011000000101; // Same exponent different significand
    tfl16_t test_8a = 0b0000011000000000; // Same significand different exponent
    tfl16_t test_8b = 0b0001011000000000; // Same significand different exponent
    printf("\n---------- tfl_equals Test ----------\n");

    if (tfl_equals(test_1a,test_1b) != 1){
        printf("\ntfl_equals Test 1 failed.\n");}
    else{
        printf("\ntfl_equals Test 1 passed.\n");}
    if (tfl_equals(test_2a,test_2b) != 1){
        printf("\ntfl_equals Test 2 failed.\n");}
    else{
        printf("\ntfl_equals Test 2 passed.\n");}
    if (tfl_equals(test_3a,test_3b) != 1){
        printf("\ntfl_equals Test 3 failed.\n");}
    else{
        printf("\ntfl_equals Test 3 passed.\n");}
    if (tfl_equals(test_4a,test_4b) != 1){
        printf("\ntfl_equals Test 4 failed.\n");}
    else{
        printf("\ntfl_equals Test 4 passed.\n");}
    if (tfl_equals(test_5a,test_5b) != 1){
        printf("\ntfl_equals Test 5 failed.\n");}
    else{
        printf("\ntfl_equals Test 5 passed.\n");}
    if (tfl_equals(test_6a,test_6b) != 1){
        printf("\ntfl_equals Test 6 failed.\n");}
    else{
        printf("\ntfl_equals Test 6 passed.\n");}
    if (tfl_equals(test_7a,test_7b) != 0){
        printf("\ntfl_equals Test 7 failed.\n");}
    else{
        printf("\ntfl_equals Test 7 passed.\n");}
    if (tfl_equals(test_8a,test_8b) != 0){
        printf("\ntfl_equals Test 8 failed.\n");}
    else{
        printf("\ntfl_equals Test 8 passed.\n");}
    
}

void test_tfl_greaterthan(){
    tfl16_t test_1a,test_1b = 0b1000010000000101; // same negative number
    tfl16_t test_2a = 0b0000000000000000; // zero
    tfl16_t test_2b = 0b1000000000000000; // negative zero
    tfl16_t test_3a = 0b1000000000000000; // negative zero
    tfl16_t test_3b = 0b1000000000000000; // zero
    tfl16_t test_4a, test_4b = 0b0000010000000101; // same positive number
    tfl16_t test_5a = 0b0001011000000000; // = 1 (a is greater, same number with different sign)
    tfl16_t test_5b = 0b1000100100000000; // = -1
    tfl16_t test_6a = 0b0000011100000000; // = 1.5 (b is greater, both are positive)
    tfl16_t test_6b = 0b0000101100100100; // = 3.1406
    tfl16_t test_7a = 0b1000011100000000; // = - 1.5 (a is greater and both are negative)
    tfl16_t test_7b = 0b1000101100100100; // = - 3.1406
    tfl16_t test_8a = 0b1000101100100100; // = - 3.1406 (b is greater and both are negative)
    tfl16_t test_8b = 0b1000011100000000; // = - 1.5 

    printf("\n---------- tfl_greaterthan Test ----------\n");

    if (tfl_greaterthan(test_1a,test_1b) != 0){
        printf("\ntfl_greaterthan Test 1 failed.\n");}
    else{
        printf("\ntfl_greaterthan Test 1 passed.\n");}
    if (tfl_greaterthan(test_2a,test_2b) != 0){
        printf("\ntfl_greaterthan Test 2 failed.\n");}
    else{
        printf("\ntfl_greaterthan Test 2 passed.\n");}
    if (tfl_greaterthan(test_3a,test_3b) != 0){
        printf("\ntfl_greaterthan Test 3 failed.\n");}
    else{
        printf("\ntfl_greaterthan Test 3 passed.\n");}
    if (tfl_greaterthan(test_4a,test_4b) != 0){
        printf("\ntfl_greaterthan Test 4 failed.\n");}
    else{
        printf("\ntfl_greaterthan Test 4 passed.\n");}
    if (tfl_greaterthan(test_5a,test_5b) != 1){
        printf("\ntfl_greaterthan Test 5 failed.\n");}
    else{
        printf("\ntfl_greaterthan Test 5 passed.\n");}
    if (tfl_greaterthan(test_6a,test_6b) != 0){
        printf("\ntfl_greaterthan Test 6 failed.\n");}
    else{
        printf("\ntfl_greaterthan Test 6 passed.\n");}
    if (tfl_greaterthan(test_7a,test_7b) != 1
    ){
        printf("\ntfl_greaterthan Test 7 failed.\n");}
    else{
        printf("\ntfl_greaterthan Test 7 passed.\n");}
    if (tfl_greaterthan(test_8a,test_8b) != 0){
        printf("\ntfl_greaterthan Test 8 failed.\n");}
    else{
        printf("\ntfl_greaterthan Test 8 passed.\n");}
    
}

void test_tfl_normalize(){
    tfl16_t test_1 = tfl_normalize(1,1,512); // negative number
    tfl16_t test_2 = tfl_normalize(0,0,0); // zero
    tfl16_t test_3 = tfl_normalize(1,0,0); // negative zero
    tfl16_t test_4 = tfl_normalize(0,1,512); // positive number
    tfl16_t test_5 = tfl_normalize(0,1,6600); // significand overflow
    tfl16_t test_6 = tfl_normalize(0,15,1023); // large number
    tfl16_t test_7 = tfl_normalize(1,31,1); // small number
    printf("\n---------- tfl_normalize Test ----------\n");

    if (test_1 != 0b1000011000000000){
        printf("\ntfl_normalize Test 1 failed.\n");}
    else{
        printf("\ntfl_normalize Test 1 passed.\n");}
    if (test_2 != 0b0000000000000000){
        printf("\ntfl_normalize Test 2 failed.\n");}
    else{
        printf("\ntfl_normalize Test 2 passed.\n");}
    if (test_3 != 0b1000000000000000){
        printf("\ntfl_normalize Test 3 failed.\n");}
    else{
        printf("\ntfl_normalize Test 3 passed.\n");}
    if (test_4 != 0b0000011000000000){
        printf("\ntfl_normalize Test 4 failed.\n");}
    else{
        printf("\ntfl_normalize Test 4 passed.\n");}
    if (test_5 != 0b0001001100111001){
        printf("\ntfl_normalize Test 5 failed.\n");}
    else{
        printf("\ntfl_normalize Test 5 passed.\n");}
    if (test_6 != 0b0011111111111111){
        printf("\ntfl_normalize Test 6 failed.\n");}
    else{
        printf("\ntfl_normalize Test 6 passed.\n");}
    if (test_7 != 0b1111110000000001){
        printf("\ntfl_normalize Test 7 failed.\n");}
    else{
        printf("\ntfl_normalize Test 7 passed.\n");}

    tfl16_t test_8 = tfl_normalize(0,30,6600); // bitmap overflow
    if (test_8 != 0){ // 0 is failure
        printf("\ntfl_normalize Test 8 failed.\n");}
    else{
        printf("\ntfl_normalize Test 8 passed.\n");}


}

void test_tfl_add(){
    tfl16_t test_1a = 0b1000011000000000; // two negative numbers added together (-1)
    tfl16_t test_1b = 0b1000011000000000; // (-1)
    tfl16_t test_2a = 0b0000011000000000; // two positive numbers added together (+1)
    tfl16_t test_2b = 0b0000011000000000; // +1)
    tfl16_t test_3a = 0b0000011000000000; // positive and negative (+1)
    tfl16_t test_3b = 0b1000101000000000; // (-2)
    tfl16_t test_4a = 0b0000000000000000; // positive 0 and negative 0 (+0)
    tfl16_t test_4b = 0b1000000000000000; // (-0)
    tfl16_t test_5a = 0b0010101111101000; // = significand overflow (+1000)
    tfl16_t test_5b = 0b0010101111101000; // = (+1000)
    tfl16_t test_6a = 0b0111000001111011; // = small numbers (+0.00750)
    tfl16_t test_6b = 0b0111000001111011; // = (+0.00750)
    tfl16_t test_7a = 0b0011101111111111; // = big numbers (+16368)
    tfl16_t test_7b = 0b0011101111111111; // = (+16368)

    printf("\n---------- tfl_add Test ----------\n");

    if (tfl_add(test_1a,test_1b) != 0b1000101000000000){
        printf("\ntfl_add Test 1 failed.\n");}
    else{
        printf("\ntfl_add Test 1 passed.\n");}
    if (tfl_add(test_2a,test_2b) != 0b0000101000000000){
        printf("\ntfl_add Test 2 failed.\n");}
    else{
        printf("\ntfl_add Test 2 passed.\n");}
    if (tfl_add(test_3a,test_3b) != 0b1000011000000000){
        printf("\ntfl_add Test 3 failed.\n");}
    else{
        printf("\ntfl_add Test 3 passed.\n");}
    if (tfl_add(test_4a,test_4b) != 0b0000000000000000){
        printf("\ntfl_add Test 4 failed.\n");}
    else{
        printf("\ntfl_add Test 4 passed.\n");}
    if (tfl_add(test_5a,test_5b) != 0b0010111111101000){
        printf("\ntfl_add Test 5 failed.\n");}
    else{
        printf("\ntfl_add Test 5 passed.\n");}
    if (tfl_add(test_6a,test_6b) != 0b0000000000001111){
        printf("\ntfl_add Test 6 failed.\n");}
    else{
        printf("\ntfl_add Test 6 passed.\n");}
    if (tfl_add(test_7a,test_7b) != 0b0011111111111111){
        printf("\ntfl_add Test 7 failed.\n");}
    else{
        printf("\ntfl_add Test 7 passed.\n");}

    tfl16_t test_8a = 0b0011111111111111; // = Numbers too big for bitmap (+16368)
    tfl16_t test_8b = 0b0011111111111111; // = (+16368)

    if (tfl_add(test_8a,test_8b) != 0){ // 0 is failure
        printf("\ntfl_add Test 8 failed.\n");}
    else{
        printf("\ntfl_add Test 8 passed.\n");}

    tfl16_t test_9a = 0b0100010000000001; // = Numbers too small for bitmap (+0.00000002980232238770)
    tfl16_t test_9b = 0b1100010000000010; // = (-0.00000005960464477539)

    if (tfl_add(test_9a,test_9b) != 0){ // 0 is failure
        printf("\ntfl_add Test 9 failed.\n");}
    else{
        printf("\ntfl_add Test 9 passed.\n");}
    
}

void test_tfl_mul(){
    tfl16_t test_1a = 0b1000011000000000; // two negative numbers multiplied together (-1)
    tfl16_t test_1b = 0b1000011000000000; // (-1)
    tfl16_t test_2a = 0b0000011000000000; // two positive numbers multiplied together (+1)
    tfl16_t test_2b = 0b0000011000000000; // +1)
    tfl16_t test_3a = 0b0000011000000000; // positive and negative (+1)
    tfl16_t test_3b = 0b1000101000000000; // (-2)
    tfl16_t test_4a = 0b0000000000000000; // positive 0 and negative 0 (+0)
    tfl16_t test_4b = 0b1000000000000000; // (-0)
    tfl16_t test_5a = 0b0001011111101000; // = big numbers significand overflow (+32.5)
    tfl16_t test_5b = 0b0010101111101000; // = (+1000)
    tfl16_t test_6a = 0b0111000001111011; // = small numbers (+0.00750)
    tfl16_t test_6b = 0b0000100010000000; // = (+0.5)

    printf("\n---------- tfl_add Test ----------\n");

    if (tfl_mul(test_1a,test_1b) != 0b0000011000000000){
        printf("\ntfl_mul Test 1 failed.\n");}
    else{
        printf("\ntfl_mul Test 1 passed.\n");}
    if (tfl_mul(test_2a,test_2b) != 0b0000011000000000){
        printf("\ntfl_mul Test 2 failed.\n");}
    else{
        printf("\ntfl_mul Test 2 passed.\n");}
    if (tfl_mul(test_3a,test_3b) != 0b1000101000000000){
        printf("\ntfl_mul Test 3 failed.\n");}
    else{
        printf("\ntfl_mul Test 3 passed.\n");}
    if (tfl_mul(test_4a,test_4b) != 0b0000000000000000){
        printf("\ntfl_mul Test 4 failed.\n");}
    else{
        printf("\ntfl_mul Test 4 passed.\n");}
    if (tfl_mul(test_5a,test_5b) != 0b0011111111010000){
        printf("\ntfl_mul Test 5 failed.\n");}
    else{
        printf("\ntfl_mul Test 5 passed.\n");}
    if (tfl_mul(test_6a,test_6b) != 0b0000000000000011){
        printf("\ntfl_mul Test 6 failed.\n");}
    else{
        printf("\ntfl_mul Test 6 passed.\n");}

    tfl16_t test_7a = 0b0011111111111111; // = Numbers too big for bitmap (+32736)
    tfl16_t test_7b = 0b0000101000000000; // = (+2)

    if (tfl_add(test_7a,test_7b) != 0){ // 0 is failure
        printf("\ntfl_add Test 7 failed.\n");}
    else{
        printf("\ntfl_add Test 7 passed.\n");}

    tfl16_t test_8a = 0b0100010000000001; // = Numbers too small for bitmap (+0.00000002980232238770)
    tfl16_t test_8b = 0b1100010000000010; // = (-0.00000005960464477539)

    if (tfl_add(test_8a,test_8b) != 0){ // 0 is failure
        printf("\ntfl_add Test 9 failed.\n");}
    else{
        printf("\ntfl_add Test 9 passed.\n");}
    
}

