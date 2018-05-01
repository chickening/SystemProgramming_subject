/* 
 * my_assembler 함수를 위한 변수 선언 및 매크로를 담고 있는 헤더 파일이다. 
 * 
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

/* 
 * instruction 목록 파일로 부터 정보를 받아와서 생성하는 구조체 변수이다.
 * 구조는 각자의 instruction set의 양식에 맞춰 직접 구현하되
 * 라인 별로 하나의 instruction을 저장한다.
 */
struct inst_unit {
	char * name;	//명령어 이름
	int type;		//1,2,3,4형식
	char * opcode;		//OPCODE
	int isHaveOperand;	//피연산자를 가지고있는지 여부
};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;		//명령어 갯수

/*
 * 어셈블리 할 소스코드를 입력받는 테이블이다. 라인 단위로 관리할 수 있다.
 */
char *input_data[MAX_LINES];
static int line_num;	// 라인 길이

int label_num; 

/* 
 * 어셈블리 할 소스코드를 토큰단위로 관리하기 위한 구조체 변수이다.
 * operator는 renaming을 허용한다.
 * nixbpe는 8bit 중 하위 6개의 bit를 이용하여 n,i,x,b,p,e를 표시한다.
 */
struct token_unit {
	char *label;	//레이블
	char *operator;	//명령어 
	char *operand[MAX_OPERAND];	//피연산자
	char *comment;
	char nixbpe; // 추후 프로젝트에서 사용된다. n : 2 ^ 5 i : 2 ^ 4 ... e = 1;

};

typedef struct token_unit token; 
token *token_table[MAX_LINES]; 
static int token_line;	// 토큰 갯수

/*
 * 심볼을 관리하는 구조체이다.
 * 심볼 테이블은 심볼 이름, 심볼의 위치로 구성된다.
 */
struct symbol_unit {
	char symbol[10];
	int addr;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
int sym_table_size = 0;		//심볼 테이블 크기
static int locctr;

/*
	변수 저장
*/
typedef struct rervt_var_node
{
	int operator;	// + - 이런거
	char target[10];
	struct rervt_var_node * next;
}rvn;

typedef struct rsrvt_var
{
	char symbol[10];
	rvn * next;
}rv;
enum nixbpe_
{
	MODE_E = 1 << 0,		// m : mode 
	MODE_P = 1 << 1,
	MODE_B = 1 << 2,
	MODE_X = 1 << 3,
	MODE_I = 1 << 4,
	MODE_N = 1 << 5
};
//--------------

static char *input_file;
static char *output_file;
int init_my_assembler(void);
int init_inst_file(char *inst_file);
int init_input_file(char *input_file);
int token_parsing(char *str);
int search_opcode(char *str);
static int assem_pass1(void);
void make_opcode_output(char *file_name);

/* 추후 프로젝트에서 사용하게 되는 함수*/
static int assem_pass2(void);
void make_objectcode_output(char *file_name);
void make_symtab_output(char * file_name);
int is_register(char * str);	//str 이 레지스터인지 확인
int find_symbol(char * str);	//Symbol index 찾기