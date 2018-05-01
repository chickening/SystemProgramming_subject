/* 
 * my_assembler �Լ��� ���� ���� ���� �� ��ũ�θ� ��� �ִ� ��� �����̴�. 
 * 
 */
#define MAX_INST 256
#define MAX_LINES 5000
#define MAX_OPERAND 3

/* 
 * instruction ��� ���Ϸ� ���� ������ �޾ƿͼ� �����ϴ� ����ü �����̴�.
 * ������ ������ instruction set�� ��Ŀ� ���� ���� �����ϵ�
 * ���� ���� �ϳ��� instruction�� �����Ѵ�.
 */
struct inst_unit {
	char * name;	//��ɾ� �̸�
	int type;		//1,2,3,4����
	char * opcode;		//OPCODE
	int isHaveOperand;	//�ǿ����ڸ� �������ִ��� ����
};
typedef struct inst_unit inst;
inst *inst_table[MAX_INST];
int inst_index;		//��ɾ� ����

/*
 * ����� �� �ҽ��ڵ带 �Է¹޴� ���̺��̴�. ���� ������ ������ �� �ִ�.
 */
char *input_data[MAX_LINES];
static int line_num;	// ���� ����

int label_num; 

/* 
 * ����� �� �ҽ��ڵ带 ��ū������ �����ϱ� ���� ����ü �����̴�.
 * operator�� renaming�� ����Ѵ�.
 * nixbpe�� 8bit �� ���� 6���� bit�� �̿��Ͽ� n,i,x,b,p,e�� ǥ���Ѵ�.
 */
struct token_unit {
	char *label;	//���̺�
	char *operator;	//��ɾ� 
	char *operand[MAX_OPERAND];	//�ǿ�����
	char *comment;
	char nixbpe; // ���� ������Ʈ���� ���ȴ�. n : 2 ^ 5 i : 2 ^ 4 ... e = 1;

};

typedef struct token_unit token; 
token *token_table[MAX_LINES]; 
static int token_line;	// ��ū ����

/*
 * �ɺ��� �����ϴ� ����ü�̴�.
 * �ɺ� ���̺��� �ɺ� �̸�, �ɺ��� ��ġ�� �����ȴ�.
 */
struct symbol_unit {
	char symbol[10];
	int addr;
};

typedef struct symbol_unit symbol;
symbol sym_table[MAX_LINES];
int sym_table_size = 0;		//�ɺ� ���̺� ũ��
static int locctr;

/*
	���� ����
*/
typedef struct rervt_var_node
{
	int operator;	// + - �̷���
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

/* ���� ������Ʈ���� ����ϰ� �Ǵ� �Լ�*/
static int assem_pass2(void);
void make_objectcode_output(char *file_name);
void make_symtab_output(char * file_name);
int is_register(char * str);	//str �� ������������ Ȯ��
int find_symbol(char * str);	//Symbol index ã��