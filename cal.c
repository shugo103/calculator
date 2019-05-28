#include <stdio.h>
#include <fcntl.h>
void delete(double *ans); // コマンドD
void print(double *ans, double *mem); // コマンドP
void memory_plus(double *ans, double *mem); // コマンドM+
void memory_minus(double *ans, double *mem); // コマンドM-
void memory_clear(double *mem); // コマンドMC
void memory_renew(double *ans, double *mem); // コマンドMR
void parse_line(char *line, double *ans, double *mem); // 入力文によってコマンドか計算式かを判別
void exec_command(char *line, double *ans, double *mem); // コマンド実行関数
void exec_operator(char *line, double *ans); // (第一項)(演算子)(第二項)の場合
void exec_operator_cont(char *line, double *ans); // (演算子)(第二項)の場合
double myatoi(char *number); // 自作atoi
void over20_read(char *line); // 入力文が20文字を超えた場合の処理関数

const char Input[] = "> ";
const char Ans[] = "ans = ";
const char Mem[] = "mem = ";
const char Exit[] = "exit\n";
const char Error_1[] = "Error (input sentence is too long)\n"; // エラーメッセージの番号は仕様書の4章と対応
const char Error_2[] = "Error (input term is too big)\n";
const char Error_3[] = "Error (invalid command)\n";
const char Error_4[] = "Error (invalid sentence)\n";
const char Error_5[] = "Error (invalid term)\n";
const char Error_6[] = "Error (invalid operater)\n";
const char Error_7[] = "Error (devided by zero)\n";
const char Error_8[] = "Overflow (answer is too big)\n";
const char Error_9[] = "Underflow (answer is too small)\n";
const char Error_10[] = "Error (memory does not memorize number)\n";
const char Error_11[] = "Error (memory has already been initialized)\n";
const char Error_12[] = "Error (answer is almost 0)\n";

void delete(double *ans) { 
    *ans = 0;
}

void print(double *ans, double *mem) {
    if ((write(1, Ans, sizeof(Ans)-1)) == -1) {
        printf("Error (write)\n");
        return ;
    }
    printf("%lf\n", *ans);
    if ((write(1, Mem, sizeof(Mem)-1)) == -1) {
        printf("Error (write)\n");
        return ;
    }
    printf("%lf\n", *mem);
}

void memory_plus(double *ans, double *mem) {
    if (*ans == 0.000000) {
        if ((write(1, Error_10, sizeof(Error_10)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ;
    }
    *mem = *mem + *ans;
}

void memory_minus(double *ans, double *mem) {
    if (*ans == 0.000000) {
        if ((write(1, Error_10, sizeof(Error_10)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ;
    }
    *mem = *mem - *ans;
}

void memory_clear(double *mem) {
    if (*mem == 0) {
        if ((write(1, Error_11, sizeof(Error_11)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ;
    }
    *mem = 0;
}

void memory_renew(double *ans, double *mem) {
    if (*mem == 0) {
        if ((write(1, Error_10, sizeof(Error_10)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ;
    }
    *ans = *mem;
}

void parse_line(char *line, double *ans, double *mem) { 
    int count = 0; // 入力文を一文字ずつチェックするとき，何番目の文字を調べているかを記憶
    int operator_check = 0; // 演算子が入力文にいくら含まれているかを記憶
    int operator_cont = 0; // 入力文の最初が演算子ならばフラグを立てる
    int error_check = 0; // 入力文に不適切な文字(英数字，コンマ，演算子以外)がいくつ含まれいるかを記憶

    while (line[count] != '\0') {
        if ((line[count] == '+') || (line[count] =='-') || (line[count] =='*') || (line[count] =='/')) { //入力された文字列に演算子がいくつ入っているか調べる
            operator_check++;
        }
        if (((line[count] < '0') || ('9' < line[count])) && (line[count] != '.') && (line[count] != '+') && (line[count] != '-') && (line[count] != '*') && (line[count] != '/')) { // 不適切な文字が含まれているかチェック
            error_check++;
        }
        count++;
    }
    if (line[0] == '%') {  // コマンドの場合はexec_commandに移行
        exec_command(line, ans, mem);
        return ;
    }
    if (error_check != 0) { // 入力文に不適切な文字が入力されている場合はエラー出力
        if ((write(1, Error_4, sizeof(Error_4)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ;
    }
    if ((line[0] == '+') || (line[0] =='-') || (line[0] =='*') || (line[0] =='/')) { // 入力文の一文字目が演算子か調べる
        operator_cont = 1;
    }
    if (operator_cont == 0) { // (第一項)(演算子)(第二項)の形の入力文の場合，exec_operatorに移行
        exec_operator(line, ans); 
        return ;
    }else {
        if (operator_check == 1) { // (演算子)(第二項)の形の入力文の場合，exec_operator_contに移行
            exec_operator_cont(line, ans);
            return ;
        }else if ((operator_check == 2) || (operator_check == 3)) { 
            if (line[1] == '-') { // 入力文の2文字目が‘-’の場合，exec_operator_contに移行
                exec_operator_cont(line, ans);
                return ;
            }else {
                exec_operator(line, ans);  
                return ;
            } 
        }else { // 演算子が一つの式に4つ以上入ることはないので，その場合はエラー出力
            if ((write(1, Error_4, sizeof(Error_4)-1)) == -1) {
                printf("Error (Write)\n");
            }
        return ;
        }
    }
    if ((write(1, Error_6, sizeof(Error_6)-1)) == -1) { //式でもコマンドでもない場合，エラー出力  
        printf("Error (write)\n");
        return ;
    }
}

void exec_command(char *line, double *ans, double *mem) { // コマンドの処理
    if ((line[1] == 'D')&&(line[2]=='\0')) {
        delete(ans);
    }else if ((line[1] == 'P') && (line[2] == '\0')) {
        print(ans, mem);
    }else if ((line[1] == 'M') && (line[2] == '+') && (line[3] == '\0')) {
        memory_plus(ans, mem);
    }else if ((line[1] == 'M') && (line[2] == '-') && (line[3] == '\0')) {
        memory_minus(ans, mem);
    }else if ((line[1] == 'M') && (line[2] == 'C') && (line[3 ]== '\0')) {
        memory_clear(mem);
    }else if ((line[1] == 'M') && (line[2] == 'R') && (line[3] == '\0')) {
        memory_renew(ans, mem);
    }else {
        if ((write(1, Error_3, sizeof(Error_3)-1)) == -1) {
            printf("Error (write)\n");
            return ;
        }
    }
}

void exec_operator (char *line, double *ans) {
    int count = 0; // 入力文字の何番目かを記憶
    int s_count = 0; // 項の何番目の文字かを記憶
    int minus_check1 = 0, minus_check2 = 0; // 第一項と第二項それぞれが負の値ならフラグを立てる
    char num1_s[11], num2_s[11];  // 第一項と第二項それぞれを格納する配列
    double num1_f = 0, num2_f = 0; // atoiを使って各項をdouble型に変形させた値を記憶
    int ope = 0; // 1=+, 2=-, 3=*, 4=/
    float pre_ans = 0; // 計算結果を一時的に保存
    int dpoint_check1 = 0, dpoint_check2 = 0; // 各項それぞれに小数点が含まれている場合フラグを立てる

    /* 第一項の処理開始 */
    if (line[0] == '-') {
        minus_check1 = 1;
        count++;
    }
    while ((('0' <= line[count]) && (line[count]<='9')) || (line[count] == '.')) { // 入力文から第一項を取り出し，num1_sに格納する
        num1_s[s_count] = line[count];
        count++;
        s_count++;
        if (line[count] == '.') {
            dpoint_check1++;
        }
    }
    num1_s[s_count] = '\0';
    if (num1_s[0] == '.') {
        if ((write(1, Error_5, sizeof(Error_5)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ; 
    }
    if ((dpoint_check1 != 0) && (dpoint_check1 != 1)) { 
        if ((write(1, Error_5, sizeof(Error_5)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ;
    }
    
    if (minus_check1 == 1) {
        s_count++;
    }
    if (s_count>9) {
        if ((write( 1, Error_2, sizeof(Error_2)-1)) == -1) {
            printf("Error (write)\n");
        }
        return;
    }
    /* 第一項の処理完了 */

    if (line[count] == '+') { // 演算子の確認
        ope = 1;
    }else if (line[count] == '-') {
        ope = 2;
    }else if (line[count] == '*') {
        ope = 3;
    }else if (line[count] == '/') {
        ope = 4;
    }else {
        if ((write(1, Error_6, sizeof(Error_6)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ;
    }
    count++;

    /* 第二項の処理開始 */
    if (line[count] == '-') { // 基本的には第一項と同じ処理を行っていく
        minus_check2++;
        count++;
    }
    s_count = 0;
    while ((('0' <= line[count]) && (line[count] <= '9')) || (line[count] == '.')) {
        num2_s[s_count] = line[count];
        count++;
        s_count++;
        if (line[count] == '.') {
            dpoint_check2++;
        }
    }
    num2_s[s_count] = '\0';
    if (num2_s[0] == '.') {
        if ((write(1, Error_5, sizeof(Error_5)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ; 
    }
    if ((dpoint_check2 != 0) && (dpoint_check2 != 1)) {
        if ((write(1, Error_5, sizeof(Error_5)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ;
    }
    if (s_count == 0) {
        if ((write( 1, Error_6, sizeof(Error_6)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ;
    }
    if (minus_check2 == 1) {
        s_count++;
    }
    if (s_count>9) {
        if ((write( 1, Error_2, sizeof(Error_2)-1)) == -1) {
            printf("Error (write)\n");
        };
        return;
    }
    /* 第二項の処理完了 */

    num1_f = myatoi(num1_s); // char型の項をatoiによってdouble型に変型
    if (minus_check1 == 1) {
        num1_f = num1_f * -1;
    }
    num2_f = myatoi(num2_s);
    if (minus_check2 == 1) {
        num2_f = num2_f * -1;
    }
    if (ope==1) { // 演算子によって計算方法を変更
        pre_ans = num1_f + num2_f;
    }else if (ope==2) {
        pre_ans = num1_f - num2_f;
    }else if (ope==3) {
        pre_ans = num1_f * num2_f;
    }else if (ope==4) {
        if (num2_f == 0) { // 0で割る場合，エラー
            if ((write(1, Error_7, sizeof(Error_7) -1 )) -1) {
                printf("Error (write)\n");
            }
            return ;
        }
        pre_ans = num1_f / num2_f;
    }

    if ((pre_ans <= 0.0000005) && (pre_ans >= -0.0000005)) { // 画面に0.000000としか出力されない場合に，エラー
        if (pre_ans != 0) {
            if ((write(1, Error_12, sizeof(Error_12)-1)) == -1) {
                printf("Error (write)\n");
            }
            return ;
        }
    }
    if (pre_ans > 999999999) { // 整数部が10桁を超える場合，エラー
        if ((write(1, Error_8, sizeof(Error_8)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ;
    }
    if (pre_ans < -999999999) {
        if ((write(1, Error_9, sizeof(Error_9)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ;
    }
    *ans = pre_ans;

    if ((write(1, Ans, sizeof(Ans)-1)) == -1) { // 計算結果を出力
        printf("Error (write)\n");
        return ;
    }
    printf("%lf\n", *ans);
    return ;
}

void exec_operator_cont (char *line, double *ans) { // exec_operatorとほぼ同じ処理を行う
    int count = 1;
    int s_count = 0;
    char num2_s[11];  
    double num2_f = 0;
    char ope[1]; // 1=+, 2=-, 3=*, 4=/
    int minus_check = 0;
    double pre_ans = 0;
    int dpoint_check = 0;

    if (line[1] == '-') {
        minus_check = 1;
        count++;
    }
    while ((('0' <= line[count]) && (line[count] <= '9')) || (line[count] == '.')) {
        num2_s[s_count] = line[count];
        count++;
        s_count++;
        if (line[count] == '.') {
            dpoint_check++;
        }
    }
    num2_s[s_count] = '\0';
    if (num2_s[0] == '.') {
        if ((write(1, Error_5, sizeof(Error_5)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ; 
    }
    if ((dpoint_check != 0) && (dpoint_check != 1)) {
        if ((write(1, Error_5, sizeof(Error_5)-1)) == -1) {
            printf( "Error (write)\n");
        }
        return ;
    }
    if (s_count == 0) {
        if ((write(1, Error_6, sizeof(Error_6)-1)) == -1) {
            printf("Error (write)\n");
        }
        return ;
    }

    if (count>10) {
        if ((write(1, Error_2, sizeof(Error_2)-1)) == -1) {
            printf("Error (write)\n");
        }
        return;
    }
    if (line[0]=='+') {
        ope = 1;
    }else if (line[0]=='-') {
        ope = 2;
    }else if (line[0]=='*') {
        ope = 3;
    }else if (line[0]=='/') {
        ope = 4;
    }else {
        if ((write(1, Error_6, sizeof(Error_6) -1)) == -1) {
            printf("Error (write)\n");
            return ;
        }
    }
    count++;
    num2_f = myatoi (num2_s);
    if (minus_check == 1) {
        num2_f = num2_f * -1;
    }
    if (ope==1) {
        pre_ans = *ans + num2_f;
    }else if (ope==2) {
        pre_ans = *ans - num2_f;
    }else if (ope==3) {
        pre_ans = *ans * num2_f;
    }else if (ope==4) {
        if (num2_f == 0) {
            if ((write(1, Error_7, sizeof( Error_7) -1)) == -1) {
                printf("Error (write)\n");
            }
            return ;
        }
        pre_ans = *ans / num2_f;
    }
    if ((pre_ans <= 0.0000005)&&(pre_ans >= -0.0000005)) {
        if (pre_ans != 0) {
            if ((write(1, Error_12, sizeof(Error_12) -1)) == -1) {
                printf("Error (write)\n");
            }
            return ;
        }
    }
    if (pre_ans > 999999999) {
        if ((write(1, Error_8, sizeof(Error_8) -1)) == -1) {
            printf("Error (write)\n");
        }
        return ;
    }
    if (pre_ans < -999999999) {
        if ((write(1, Error_9, sizeof(Error_9) -1)) == -1) {
            printf("Error (write)\n");
        }
        return ;
    }
    *ans = pre_ans;

    if ((write(1, Ans, sizeof(Ans) -1)) == -1) {
        printf("Error (write)\n");
        return ;
    };
    printf("%lf\n", *ans);
    return ;
}

double myatoi (char *number) {
    int count = 0,  inte_count = 0, digit = 0; // digitは小数点の位置を記憶
    int dot_num = 0;
    double integer = 0, decimals = 0;
    double answer = 0;
    for (count=0; number[count] != '\0'; count++) {  // 小数点が左から何番目に存在するか，もしくは存在しないかをチェックする
        if (number[count] == '.') {
            dot_num = count;
        }
        digit++;
    }

    /* 整数部の処理開始 */
    while ((number[inte_count] != '.') || (number[inte_count] != '\0')) {  
        if (number[inte_count] < '1' || number[inte_count] > '9') {
            break;
        }
        integer += number[inte_count] - 48;
        integer *= 10;
        inte_count++;
    }
    integer /= 10;
    /* 整数部の処理終了 */
    digit--;
    /* 小数部の処理開始 */
    if (dot_num != 0) { 
        while (number[digit] != '.') { 
           if (number[digit] < '1' || number[digit] > '9') {
               break;
           }
           decimals += number[digit] - 48;
           decimals /= 10;
           digit--;
        }
    }  
    /* 小数部の処理終了 */
    answer = integer + decimals;
    return answer;
}

void over20_read(char *line) {
    char trash[1]; // 次に捨てる文字を格納
    int count = 0; // 入力文の何文字目かを記憶
    if (line[20] == '\n') {
        if ((write(1, Error_1, sizeof(Error_1)-1)) == -1) {
            printf("Error (write)\n");
        };
        return ;
    }
    while (line[count] != '\0') { // 先にlineに書き込まれた文字を消去する
        line[count] = '\0';
        count++;
    }
    while (1) {
        if ((read(0, trash, 1)) == -1) { // 改行が読み込まれるまで1文字ずつ消去
            printf("Error (write)\n");
            break;
        }
        if (trash[0] == '\n') {
            break;
        }
    }
    if ((write(1, Error_1, sizeof(Error_1)-1)) == -1) { // エラーメッセージを出力
        printf("Error (write)\n");
        return ;
    }
}

int main(void) {
    double ans = 0;  // 計算結果を格納する変数
    double mem = 0;  // メモリ
    int line_count = 0;
    int reset_count = 0;

    while (1) {
        char line[21];  // 入力される文字列を格納
        if ((write(1, Input, sizeof(Input)-1)) == -1) { // 文を入力する前に"> "を出力
            printf("Error (write)\n");
            break ;
        }
        if ((line_count = read (0, line, 21)) == -1) { // 文を入力
            printf("Error (read)\n");
            break ;
        }
        if (line_count >= 21) { //21文字以上の場合，エラー処理
            over20_read(line);
            continue ;   
        }
        if (line_count == 1) { //改行文字だけを打ち込んだ場合，ループの最初に戻る
            continue ;
        }
        line[line_count-1] = '\0';
        if ((line[0] == '%') && (line[1] == 'Q')&&(line[2] == '\0')) { // コマンド%Qが入力された場合の処理
            if ((write(1, Exit, sizeof(Exit)-1)) == -1) {
                printf("Error (write)\n");
            }
            break;
        }

        parse_line(line, &ans, &mem);
        while (line[reset_count] != '\0') { // 入力文が格納された配列lineを初期化
            line[reset_count] = '\0';
            reset_count++;
        }
        reset_count = 0;    
    }
}
