/*
 * 화일명 : my_assembler_00000000.c 
 * 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
 * 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
 * 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을 기입한다.
 */

/*
 *
 * 프로그램의 헤더를 정의한다. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

// 파일명의 "00000000"은 자신의 학번으로 변경할 것.
#include "my_assembler_20160323.h"

/* ----------------------------------------------------------------------------------
 * 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
 * 매계 : 실행 파일, 어셈블리 파일 
 * 반환 : 성공 = 0, 실패 = < 0 
 * 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다. 
 *		   또한 중간파일을 생성하지 않는다. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[]) 
{
	if(init_my_assembler()< 0)
	{
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n"); 
		return -1 ; 
	}

	if(assem_pass1() < 0 ){
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n") ; 
		return -1 ; 
	}
	//make_opcode_output("output_20160323");
	make_symtab_output(NULL);
	assem_pass2();
	make_objectcode_output("output_20160323");
	/*
	* 추후 프로젝트에서 사용되는 부분
	*
	if(assem_pass2() < 0 ){
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n") ; 
		return -1 ; 
	}

	make_objectcode_output("output") ; 
	*/
	return 0;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다. 
 * 매계 : 없음
 * 반환 : 정상종료 = 0 , 에러 발생 = -1
 * 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기 
 *		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
 *		   구현하였다. 
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
	int result ; 

	if((result = init_inst_file("inst.data")) < 0 )
		return -1 ;
	if((result = init_input_file("input.txt")) < 0 )
		return -1 ; 
	return result ; 
}

/* ----------------------------------------------------------------------------------
 * 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을 
 *        생성하는 함수이다. 
 * 매계 : 기계어 목록 파일
 * 반환 : 정상종료 = 0 , 에러 < 0 
 * 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
 *	
 *	===============================================================================
 *		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
 *	===============================================================================	   
 *		
 * ----------------------------------------------------------------------------------
 */
int init_inst_file(char *inst_file)
{
	FILE * file;
	int errno;
	errno = -1;
	if ((file = fopen(inst_file, "r")) == NULL)
	{
		return errno;
	}

	char line[1000];
	inst_index = 0;
	while (fgets(line, 999, file) != NULL)
	{
		inst_table[inst_index] = (inst*)malloc(sizeof(inst));
		char tName[100];
		char tOpcode[100];
		sscanf(line,"%s %d %s %d", tName, &(inst_table[inst_index]->type), tOpcode, &(inst_table[inst_index]->isHaveOperand));
		inst_table[inst_index]->name = (char*)malloc(sizeof(char) * (strlen(tName) + 1));
		inst_table[inst_index]->opcode = (char*)malloc(sizeof(char) * (strlen(tOpcode) + 1));
		strcpy(inst_table[inst_index]->name, tName);
		strcpy(inst_table[inst_index]->opcode, tOpcode);
		++inst_index;
	}
	errno = 1;
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 어셈블리 할 소스코드를 읽어 소스코드 테이블(input_data)를 생성하는 함수이다. 
 * 매계 : 어셈블리할 소스파일명
 * 반환 : 정상종료 = 0 , 에러 < 0  
 * 주의 : 라인단위로 저장한다.
 *		
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char *input_file)
{
	FILE * file;
	int errno;
	errno = 1;
	if ((file = fopen(input_file, "r")) == NULL)
		return -1;
	int i = 0;
	char line[1000];
	line_num = 0;
	while (fgets(line, 999, file) != NULL)
	{
		if (line[strlen(line) - 1] == '\n')	// 개행 문자 없애기
			line[strlen(line) - 1] = '\0';
		input_data[line_num] = (char*)malloc(sizeof(char) * (strlen(line) + 1));
		strcpy(input_data[line_num], line);
		line_num++;
	}
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다. 
 *        패스 1로 부터 호출된다. 
 * 매계 : 파싱을 원하는 문자열  
 * 반환 : 정상종료 = 0 , 에러 < 0 
 * 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다. 
 * ----------------------------------------------------------------------------------
 */
int is_register(char * str)
{
	char registers[8][3] = { "PC","SW","X","A","S","T","B","F" };
	int r = 0;
	for (int i = 0; i < 8; i++)
		if (!strcmp(str, registers[i]))
			r = 1;
	return r;

}
int token_parsing(char *str)
{
	token_table[token_line] = (token*)malloc(sizeof(token));
	token_table[token_line]->label = NULL;
	token_table[token_line]->operator = NULL;
	for (int i = 0; i < MAX_OPERAND; i++)
		token_table[token_line]->operand[i] = NULL;
	token_table[token_line]->comment = NULL;
	char * tStr = (char*)malloc(sizeof(char) * (strlen(str) + 1));
	char * ptr;
	strcpy(tStr, str);	// 임시 str 만든이유 strtok 쓰면 내부 데이터 값이 변경되서
	char * word[100];
	int wordIndex = 0;
	ptr = strtok(tStr, "\t");
	while (ptr != NULL)
	{
		word[wordIndex] = (char*)malloc(sizeof(char) * (strlen(ptr) + 1));
		strcpy(word[wordIndex], ptr);
		ptr = strtok(NULL, "\t");
		++wordIndex;
	}

	int state = 0;	// 0 : Label 1 : Instruction 2 : Ope rand 3 : Comment 4 : 전부 Comment
	static int resv_addr = 0;	// 리터럴 변수 크기
	if (wordIndex == 1)	//단어가 한개면 바로 instruction 부터 체크 예외
		state = 1;
	for (int i = 0; i < wordIndex; i++)
	{
		if (state == 0 || state == 1)
		{
			//printf("word[%d] : %s state : %d\n", i, word[i], state);
			int findIndex;
			findIndex = search_opcode(word[i]);
			if (findIndex != -1)	//insruction 인것을 확인했을때
			{
				token_table[token_line]->operator = (char *)malloc(sizeof(char) * (strlen(word[i]) + 1));
				strcpy(token_table[token_line] -> operator, word[i]);
				if (inst_table[findIndex]->isHaveOperand == 0)	// 피연산자가 없는경우 바로 Comment로 
					state = 3;
				else
					state = 2;
			}
			else if (state == 0)	//Label 인경우 Label 채우기
			{
				token_table[token_line]->label = (char *)malloc(sizeof(char) * (strlen(word[i]) + 1));
				strcpy(token_table[token_line]->label, word[i]);
				state = 1;

			}
			else if (state == 1)	//2번째 까지 Instruction이 아닌경우 이행은 전부 Comment
			{
				free(token_table[token_line]->label);
				token_table[token_line]->label = NULL;
				state = 4;	//모두 코멘트인 상황
				i = -1;
			}
		}
		else if (state == 2)	//피연산자 받아오기
		{
			int operandIndex = 0;
			char * tWord = (char*)malloc(sizeof(char) *(strlen(word[i]) + 1));
			char * wordPtr;
			strcpy(tWord, word[i]);
			wordPtr = strtok(tWord, ",");
			while (wordPtr != NULL)
			{
				token_table[token_line]->operand[operandIndex] = (char*)malloc(sizeof(char) * (strlen(wordPtr) + 1));
				strcpy(token_table[token_line]->operand[operandIndex], wordPtr);
				operandIndex++;
				wordPtr = strtok(NULL, ",");
			}
			state = 3;
		}
		else if (state == 3 || state == 4)	//state == 4 일경우 i = 0 부터
		{
			char tLabel[1000] = "";
			for (; i < wordIndex; i++)	//남은 모든 word를 Comment에 넣는 작업
			{
				strcat(tLabel, word[i]);
				if (i != wordIndex - 1)
					strcat(tLabel, " ");
			}
			token_table[token_line]->comment = (char*)malloc(sizeof(char) * (strlen(tLabel) + 1));
			strcpy(token_table[token_line]->comment, tLabel);
			break;
		}
	}
	if (token_table[token_line]->operator != NULL)	// START 와 CSECT
	{
		if (!strcmp("START", token_table[token_line]->operator))
			locctr = atoi(token_table[token_line]->operand[0]);
		else if (!strcmp("CSECT", token_table[token_line]->operator))
			locctr = 0;
	}
	/*추가 부분 예외처리*/
	if (token_table[token_line]->label != NULL)
	{
		/*추가 부분 레이블 심볼테이블에 추가 하기*/
		strcpy(sym_table[sym_table_size].symbol, token_table[token_line]->label);
		sym_table[sym_table_size].addr = locctr;

		/*추가 부분 예외 사항*/
		if (!strcmp("EQU" , token_table[token_line]->operator) && strcmp("*", token_table[token_line]->operand[0]))	//EQU 처리
		{
			int len = strlen(token_table[token_line]->operand[0]);
			int operator = 1;	// 0 : -  , 1 : +
			int fh = -1;
			sym_table[sym_table_size].addr = 0;
			for (int j = 0; j < len; j++)
			{
				if (token_table[token_line]->operand[0][j] == '+' || token_table[token_line]->operand[0][j] == '-' || j == len - 1)	//마지막일때 예외사항
				{
					if (j == len - 1)	//예외 사항
						++j;
					char temp_str[10] = { 0 };
					strncpy(temp_str, token_table[token_line]->operand[0] + fh + 1, j - fh - 1);
					for (int k = 0; k < sym_table_size; k++)	// k = 0 아마도 나중에 수정
						if (!strcmp(sym_table[k].symbol, temp_str))
						{
							if (operator == 0)
								sym_table[sym_table_size].addr -= sym_table[k].addr;
							else if (operator == 1)
								sym_table[sym_table_size].addr += sym_table[k].addr;
						}
					if (token_table[token_line]->operand[0][j] == '+')
						operator = 1;
					else if (token_table[token_line]->operand[0][j] == '-')
						operator = 0;
					fh = j;
				}
			}
		}
		++sym_table_size;
	}
	if (token_table[token_line]->operand[0] != NULL)	// 리터럴 체크
	{
		if (token_table[token_line]->operand[0][0] == '=')	//일단 꼼수
		{
			if (token_table[token_line]->operand[0][1] == 'X')
				resv_addr += 1;
			else if (token_table[token_line]->operand[0][1] == 'C')
				resv_addr += 3;
		}
	}
	if (token_table[token_line]->operator != NULL)	// 현재 주소 더하기
	{

		int add_addr = 0;
		if (!strcmp("RESB", token_table[token_line]->operator))
			locctr += atoi(token_table[token_line]->operand[0]);
		else if (!strcmp("RESW", token_table[token_line]->operator))
			locctr += 3 * atoi(token_table[token_line]->operand[0]);
		else if (!strcmp("LTORG", token_table[token_line]->operator))
		{
			locctr += resv_addr;
			resv_addr = 0;
		}
		else if (token_table[token_line]->operator[0] == '+')	//주의
			locctr += (add_addr = 4);
		else
			locctr += (add_addr = inst_table[search_opcode(token_table[token_line]->operator)]->type);\
	}

	token_table[token_line]->nixbpe = 0;
	if (token_table[token_line]->operator != NULL && inst_table[search_opcode(token_table[token_line]->operator)]->type >= 3)	// nixbpe
	{
		if (token_table[token_line]->operator[0] == '+')
			token_table[token_line]->nixbpe |= MODE_E;
		
		if (token_table[token_line]->operand[0] != NULL)
		{
			if (token_table[token_line]->operand[0][0] == '@')
				token_table[token_line]->nixbpe |= MODE_N;
			else if (token_table[token_line]->operand[0][0] == '#')
				token_table[token_line]->nixbpe |= MODE_I;
			else
			{
				token_table[token_line]->nixbpe |= MODE_N;
				token_table[token_line]->nixbpe |= MODE_I;
			}
			if(!is_register(token_table[token_line]->operand[0]) && token_table[token_line]->operand[0][0] != '#')
				token_table[token_line]->nixbpe |= MODE_P;
		}
		if(token_table[token_line]->operand[1] != NULL)
			if(!strcmp("X", token_table[token_line]->operand[1]))
				token_table[token_line]->nixbpe |= MODE_X;
	}

	//printf("%s\t%X %X\n", str, (token_table[token_line]->nixbpe & (MODE_N | MODE_I)) >> 4 , token_table[token_line]->nixbpe & (MODE_I - 1));
	
	/*추가 부분 현재 주소값에다가 주소 크기값 더하기*/
	++token_line;	//다음 토큰을 파싱하기위해서
}

/* ----------------------------------------------------------------------------------
 * 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다. 
 * 매계 : 토큰 단위로 구분된 문자열 
 * 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0 
 * 주의 : 
 *		
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char *str)
{
	for(int i = 0; i < inst_index; i++)
		if ((((str[0] == '+' || str[0] == '@') && strcmp(str + 1, inst_table[i]->name) == 0)) || strcmp(str, inst_table[i]->name) == 0)
			return i;
	return -1;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */

	/* input_data의 문자열을 한줄씩 입력 받아서 
	 * token_parsing()을 호출하여 token_unit에 저장
	 */
	token_line = 0;
	for (int i = 0; i < line_num; i++)	//토큰수는 라인수와 같음
		token_parsing(input_data[i]);
}


/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 4번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 4번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/
void make_opcode_output(char *file_name)
{
	FILE * file;
	if (file_name == NULL)
		file = stdout;
	else if ((file = fopen(file_name, "w")) == NULL)
		return;
	for (int i = 0; i < token_line; i++)
	{
		if (token_table[i]->label == NULL && token_table[i]->operator == NULL)	//Comment만 있는경우
			continue;
		if(token_table[i]->label != NULL)
			fprintf(file, "%s", token_table[i]->label);
		fprintf(file, "\t");
		if (token_table[i]->operator != NULL)
			fprintf(file, "%s", token_table[i]->operator);
		fprintf(file, "\t");
		for (int j = 0; j < 3;j++)
		{
			if (token_table[i]->operand[j] != NULL)
			{

				if(j == 0)
					fprintf(file, "%s", token_table[i]->operand[j]);
				else
					fprintf(file, ",%s", token_table[i]->operand[j]);
			}
		}
		fprintf(file,"\t");
		if (token_table[i]->operator != NULL)
		{
			char *op = inst_table[search_opcode(token_table[i]->operator)]->opcode;
			if (strcmp(op, "-1") != 0)
				fprintf(file, "\t%s", op);
		}
		fprintf(file,"\n");
	}

}



/* --------------------------------------------------------------------------------*
* ------------------------- 추후 프로젝트에서 사용할 함수 --------------------------*
* --------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/

static int assem_pass2(void)
{
	int mc_lang = 0;	//기게어
	for (int i = 0; i < line_num; i++)
	{
		if (token_table[i] ->operator != NULL)
		{
			int op = 0;
			sscanf(inst_table[search_opcode(token_table[i]->operator)]->opcode, "%d", &op);

			int format_type = inst_table[search_opcode(token_table[i]->operator)]->type;
			if (token_table[i]->nixbpe & MODE_E)
				format_type = 4;

			if (format_type == 4)
			{
				mc_lang |= op << 25;
				mc_lang |= token_table[i]->nixbpe << 19;

			}
			else if (format_type == 3)	// 0 이 아니면 3 형식
			{
				mc_lang |= op << 17;
				mc_lang |= token_table[i]->nixbpe << 11;
			}
			else if (format_type == 2)
				mc_lang |= op << 7;
			else if (format_type == 1)
				mc_lang |= op;
		}
	}
	

}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char *file_name)
{
	/* add your code here */

}

void make_symtab_output(char * file_name)
{
	FILE * fp = stdout;
	if (file_name != NULL && (fp = fopen(file_name, "w")) == NULL)
		return;
	for (int i = 0; i < sym_table_size; i++)
		fprintf(fp,"%s\t%X\n", sym_table[i].symbol, sym_table[i].addr);
}
// 심볼 이름으로 심볼 인덱스를 찾아주는 함수
// return 값
// 정상 : 심볼의 인덱스 값
// 실패 : -1
int find_symbol(char * str)
{
	for (int i = 0; i < sym_table_size; i++)
	{
		if (!strcmp(str, sym_table[i].symbol))
			return i;
	}
	return -1;
}