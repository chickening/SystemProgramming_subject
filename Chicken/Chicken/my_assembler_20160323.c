/*
 * ȭ�ϸ� : my_assembler_00000000.c 
 * ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
 * �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
 * ���� ������ ���Ǵ� ���ڿ� "00000000"���� �ڽ��� �й��� �����Ѵ�.
 */

/*
 *
 * ���α׷��� ����� �����Ѵ�. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

// ���ϸ��� "00000000"�� �ڽ��� �й����� ������ ��.
#include "my_assembler_20160323.h"

/* ----------------------------------------------------------------------------------
 * ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
 * �Ű� : ���� ����, ����� ���� 
 * ��ȯ : ���� = 0, ���� = < 0 
 * ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�. 
 *		   ���� �߰������� �������� �ʴ´�. 
 * ----------------------------------------------------------------------------------
 */
int main(int args, char *arg[]) 
{
	if(init_my_assembler()< 0)
	{
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n"); 
		return -1 ; 
	}

	if(assem_pass1() < 0 ){
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n") ; 
		return -1 ; 
	}
	//make_opcode_output("output_20160323");
	make_symtab_output(NULL);
	assem_pass2();
	make_objectcode_output("output_20160323");
	/*
	* ���� ������Ʈ���� ���Ǵ� �κ�
	*
	if(assem_pass2() < 0 ){
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n") ; 
		return -1 ; 
	}

	make_objectcode_output("output") ; 
	*/
	return 0;
}

/* ----------------------------------------------------------------------------------
 * ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�. 
 * �Ű� : ����
 * ��ȯ : �������� = 0 , ���� �߻� = -1
 * ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ� 
 *		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
 *		   �����Ͽ���. 
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
 * ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)�� 
 *        �����ϴ� �Լ��̴�. 
 * �Ű� : ���� ��� ����
 * ��ȯ : �������� = 0 , ���� < 0 
 * ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
 *	
 *	===============================================================================
 *		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
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
 * ���� : ����� �� �ҽ��ڵ带 �о� �ҽ��ڵ� ���̺�(input_data)�� �����ϴ� �Լ��̴�. 
 * �Ű� : ������� �ҽ����ϸ�
 * ��ȯ : �������� = 0 , ���� < 0  
 * ���� : ���δ����� �����Ѵ�.
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
		if (line[strlen(line) - 1] == '\n')	// ���� ���� ���ֱ�
			line[strlen(line) - 1] = '\0';
		input_data[line_num] = (char*)malloc(sizeof(char) * (strlen(line) + 1));
		strcpy(input_data[line_num], line);
		line_num++;
	}
	return errno;
}

/* ----------------------------------------------------------------------------------
 * ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�. 
 *        �н� 1�� ���� ȣ��ȴ�. 
 * �Ű� : �Ľ��� ���ϴ� ���ڿ�  
 * ��ȯ : �������� = 0 , ���� < 0 
 * ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�. 
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
	strcpy(tStr, str);	// �ӽ� str �������� strtok ���� ���� ������ ���� ����Ǽ�
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

	int state = 0;	// 0 : Label 1 : Instruction 2 : Ope rand 3 : Comment 4 : ���� Comment
	static int resv_addr = 0;	// ���ͷ� ���� ũ��
	if (wordIndex == 1)	//�ܾ �Ѱ��� �ٷ� instruction ���� üũ ����
		state = 1;
	for (int i = 0; i < wordIndex; i++)
	{
		if (state == 0 || state == 1)
		{
			//printf("word[%d] : %s state : %d\n", i, word[i], state);
			int findIndex;
			findIndex = search_opcode(word[i]);
			if (findIndex != -1)	//insruction �ΰ��� Ȯ��������
			{
				token_table[token_line]->operator = (char *)malloc(sizeof(char) * (strlen(word[i]) + 1));
				strcpy(token_table[token_line] -> operator, word[i]);
				if (inst_table[findIndex]->isHaveOperand == 0)	// �ǿ����ڰ� ���°�� �ٷ� Comment�� 
					state = 3;
				else
					state = 2;
			}
			else if (state == 0)	//Label �ΰ�� Label ä���
			{
				token_table[token_line]->label = (char *)malloc(sizeof(char) * (strlen(word[i]) + 1));
				strcpy(token_table[token_line]->label, word[i]);
				state = 1;

			}
			else if (state == 1)	//2��° ���� Instruction�� �ƴѰ�� ������ ���� Comment
			{
				free(token_table[token_line]->label);
				token_table[token_line]->label = NULL;
				state = 4;	//��� �ڸ�Ʈ�� ��Ȳ
				i = -1;
			}
		}
		else if (state == 2)	//�ǿ����� �޾ƿ���
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
		else if (state == 3 || state == 4)	//state == 4 �ϰ�� i = 0 ����
		{
			char tLabel[1000] = "";
			for (; i < wordIndex; i++)	//���� ��� word�� Comment�� �ִ� �۾�
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
	if (token_table[token_line]->operator != NULL)	// START �� CSECT
	{
		if (!strcmp("START", token_table[token_line]->operator))
			locctr = atoi(token_table[token_line]->operand[0]);
		else if (!strcmp("CSECT", token_table[token_line]->operator))
			locctr = 0;
	}
	/*�߰� �κ� ����ó��*/
	if (token_table[token_line]->label != NULL)
	{
		/*�߰� �κ� ���̺� �ɺ����̺� �߰� �ϱ�*/
		strcpy(sym_table[sym_table_size].symbol, token_table[token_line]->label);
		sym_table[sym_table_size].addr = locctr;

		/*�߰� �κ� ���� ����*/
		if (!strcmp("EQU" , token_table[token_line]->operator) && strcmp("*", token_table[token_line]->operand[0]))	//EQU ó��
		{
			int len = strlen(token_table[token_line]->operand[0]);
			int operator = 1;	// 0 : -  , 1 : +
			int fh = -1;
			sym_table[sym_table_size].addr = 0;
			for (int j = 0; j < len; j++)
			{
				if (token_table[token_line]->operand[0][j] == '+' || token_table[token_line]->operand[0][j] == '-' || j == len - 1)	//�������϶� ���ܻ���
				{
					if (j == len - 1)	//���� ����
						++j;
					char temp_str[10] = { 0 };
					strncpy(temp_str, token_table[token_line]->operand[0] + fh + 1, j - fh - 1);
					for (int k = 0; k < sym_table_size; k++)	// k = 0 �Ƹ��� ���߿� ����
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
	if (token_table[token_line]->operand[0] != NULL)	// ���ͷ� üũ
	{
		if (token_table[token_line]->operand[0][0] == '=')	//�ϴ� �ļ�
		{
			if (token_table[token_line]->operand[0][1] == 'X')
				resv_addr += 1;
			else if (token_table[token_line]->operand[0][1] == 'C')
				resv_addr += 3;
		}
	}
	if (token_table[token_line]->operator != NULL)	// ���� �ּ� ���ϱ�
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
		else if (token_table[token_line]->operator[0] == '+')	//����
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
	
	/*�߰� �κ� ���� �ּҰ����ٰ� �ּ� ũ�Ⱚ ���ϱ�*/
	++token_line;	//���� ��ū�� �Ľ��ϱ����ؼ�
}

/* ----------------------------------------------------------------------------------
 * ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�. 
 * �Ű� : ��ū ������ ���е� ���ڿ� 
 * ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0 
 * ���� : 
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
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */

	/* input_data�� ���ڿ��� ���پ� �Է� �޾Ƽ� 
	 * token_parsing()�� ȣ���Ͽ� token_unit�� ����
	 */
	token_line = 0;
	for (int i = 0; i < line_num; i++)	//��ū���� ���μ��� ����
		token_parsing(input_data[i]);
}


/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ ��ɾ� ���� OPCODE�� ��ϵ� ǥ(���� 4��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*        ���� ���� 4�������� ���̴� �Լ��̹Ƿ� ������ ������Ʈ������ ������ �ʴ´�.
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
		if (token_table[i]->label == NULL && token_table[i]->operator == NULL)	//Comment�� �ִ°��
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
* ------------------------- ���� ������Ʈ���� ����� �Լ� --------------------------*
* --------------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
* -----------------------------------------------------------------------------------
*/

static int assem_pass2(void)
{
	int mc_lang = 0;	//��Ծ�
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
			else if (format_type == 3)	// 0 �� �ƴϸ� 3 ����
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
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
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
// �ɺ� �̸����� �ɺ� �ε����� ã���ִ� �Լ�
// return ��
// ���� : �ɺ��� �ε��� ��
// ���� : -1
int find_symbol(char * str)
{
	for (int i = 0; i < sym_table_size; i++)
	{
		if (!strcmp(str, sym_table[i].symbol))
			return i;
	}
	return -1;
}