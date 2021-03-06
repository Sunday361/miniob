
%{

#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.tab.h"
#include "sql/parser/lex.yy.h"
// #include "common/log/log.h" // 包含C++中的头文件

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct _ParserContext {
    Query * ssql;
    size_t select_length;
    size_t condition_length;
    size_t from_length;
    size_t value_length;
    Value values[MAX_NUM];
    Condition conditions[MAX_NUM];
    CompOp comp;
    char id[MAX_NUM];
} ParserContext;

typedef struct _ParserVector {
  ParserContext contexts[MAX_NUM];
  size_t len;
} ParserVector;
//获取子串
char *substr(const char *s,int n1,int n2)/*从s中提取下标为n1~n2的字符组成一个新字符串，然后返回这个新串的首地址*/
{
  char *sp = malloc(sizeof(char) * (n2 - n1 + 2));
  int i, j = 0;
  for (i = n1; i <= n2; i++) {
    sp[j++] = s[i];
  }
  sp[j] = 0;
  return sp;
}

void yyerror(yyscan_t scanner, const char *str)
{
  printf("parse sql failed. error=%s", str);
}

//ParserContext *get_context(yyscan_t scanner)
//{
//  return (ParserContext *)yyget_extra(scanner);
//}

ParserVector *get_vector(yyscan_t scanner)
{
  return (ParserVector *)yyget_extra(scanner);
}

ParserContext* get_context(yyscan_t scanner) {
ParserVector* v = get_vector(scanner);
size_t l = v->len;
return (ParserContext*)(&(v->contexts[l]));
}

ParserContext* get_last_context(yyscan_t scanner) {
ParserVector* v = get_vector(scanner);
size_t l = v->len;
return (ParserContext*)(&(v->contexts[l+1]));
}

ParserContext* get_parent_context(yyscan_t scanner) {
ParserVector* v = get_vector(scanner);
size_t l = v->len;
return (ParserContext*)(&(v->contexts[l-1]));
}

//#define CONTEXT get_context(scanner)
#define CONTEXT get_context(scanner)
#define PARENTCONTEXT get_parent_context(scanner)
#define CONTEXT_VECTOR get_vector(scanner)
%}

%define api.pure full
%lex-param { yyscan_t scanner }
%parse-param { void *scanner }

//标识tokens
%token  SEMICOLON
        CREATE
        DROP
        TABLE
        TABLES
        INDEX
        SELECT
        DESC
        SHOW
        SYNC
        INSERT
        DELETE
        UPDATE
        LBRACE
        RBRACE
        COMMA
        TRX_BEGIN
        TRX_COMMIT
        TRX_ROLLBACK
        INT_T
        STRING_T
        FLOAT_T
        DATE_T
        HELP
        EXIT
        DOT //QUOTE
        INTO
        VALUES
        FROM
        WHERE
        AND
        SET
        ON
        LOAD
        DATA
        INFILE
        EQ
        LT
        GT
        LE
        GE
        NE
        COUNT
        AVG
        MIN
        MAX
        COUNT_1
        INNER_JOIN
        NULLABLE
        NULLT
        NOT_NULLT
        GROUP_BY
        ORDER_BY
        UNIQUE
        OP_IS
        OP_NOT
        TEXT_T
        OP_IN
        OP_NOTIN
        ASC

%union {
  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  char *string;
  int number;
  float floats;
  char *position;
}

%token <number> NUMBER
%token <floats> FLOAT 
%token <string> ID
%token <string> PATH
%token <string> SSS
%token <string> STAR
%token <string> STRING_V
%token <string> DATE

//非终结符
%type <number> nullable;
%type <number> type;
%type <condition1> condition;
%type <value1> value;
%type <number> number;
%type <number> unique;
%type <number> order_asc;

%%

commands:		//commands or sqls. parser starts here.
    /* empty */
    | commands command
    ;

command:
	  select  
	| insert
	| update
	| delete
	| create_table
	| drop_table
	| show_tables
	| desc_table
	| create_index	
	| drop_index
	| sync
	| begin
	| commit
	| rollback
	| load_data
	| help
	| exit
    ;

exit:			
    EXIT SEMICOLON {
        CONTEXT->ssql->flag=SCF_EXIT;//"exit";
    };

help:
    HELP SEMICOLON {
        CONTEXT->ssql->flag=SCF_HELP;//"help";
    };

sync:
    SYNC SEMICOLON {
      CONTEXT->ssql->flag = SCF_SYNC;
    }
    ;

begin:
    TRX_BEGIN SEMICOLON {
      CONTEXT->ssql->flag = SCF_BEGIN;
    }
    ;

commit:
    TRX_COMMIT SEMICOLON {
      CONTEXT->ssql->flag = SCF_COMMIT;
    }
    ;

rollback:
    TRX_ROLLBACK SEMICOLON {
      CONTEXT->ssql->flag = SCF_ROLLBACK;
    }
    ;

drop_table:		/*drop table 语句的语法解析树*/
    DROP TABLE ID SEMICOLON {
        CONTEXT->ssql->flag = SCF_DROP_TABLE;//"drop_table";
        drop_table_init(&CONTEXT->ssql->sstr.drop_table, $3);
    };

show_tables:
    SHOW TABLES SEMICOLON {
      CONTEXT->ssql->flag = SCF_SHOW_TABLES;
    }
    ;

desc_table:
    DESC ID SEMICOLON {
      CONTEXT->ssql->flag = SCF_DESC_TABLE;
      desc_table_init(&CONTEXT->ssql->sstr.desc_table, $2);
    }
    ;

create_index:		/*create index 语句的语法解析树*/
    CREATE unique INDEX ID ON ID LBRACE index_attr RBRACE SEMICOLON
		{
			CONTEXT->ssql->flag = SCF_CREATE_INDEX;//"create_index";
			create_index_init(&CONTEXT->ssql->sstr.create_index, $4, $6, $2);
		}
		;

index_attr:
ID index_attr_lists{
create_index_append(&CONTEXT->ssql->sstr.create_index, $1);
}


index_attr_lists:
| COMMA ID index_attr_lists{
create_index_append(&CONTEXT->ssql->sstr.create_index, $2);
}

unique:
{
$$=0;
}
| UNIQUE {
$$=1;
}

drop_index:			/*drop index 语句的语法解析树*/
    DROP INDEX ID  SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_DROP_INDEX;//"drop_index";
			drop_index_init(&CONTEXT->ssql->sstr.drop_index, $3);
		}
    ;
create_table:		/*create table 语句的语法解析树*/
    CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE SEMICOLON 
		{
			CONTEXT->ssql->flag=SCF_CREATE_TABLE;//"create_table";
			// CONTEXT->ssql->sstr.create_table.attribute_count = CONTEXT->value_length;
			create_table_init_name(&CONTEXT->ssql->sstr.create_table, $3);
			//临时变量清零	
			CONTEXT->value_length = 0;
		}
    ;
attr_def_list:
    /* empty */
    | COMMA attr_def attr_def_list {    }
    ;
    
attr_def:
    ID_get type LBRACE number RBRACE nullable
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, $4, $6);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name =(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type = $2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length = $4;
			CONTEXT->value_length++;
		}
    |ID_get type nullable
		{
			AttrInfo attribute;
			attr_info_init(&attribute, CONTEXT->id, $2, 5, $3);
			create_table_append_attribute(&CONTEXT->ssql->sstr.create_table, &attribute);
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name=(char*)malloc(sizeof(char));
			// strcpy(CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].name, CONTEXT->id); 
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].type=$2;  
			// CONTEXT->ssql->sstr.create_table.attributes[CONTEXT->value_length].length=4; // default attribute length
			CONTEXT->value_length++;
		}
    ;

nullable:
{
$$ = 0;
}
| NOT_NULLT {
$$ = 0;
}
| NULLABLE {
$$ = 1;
}

number:
		NUMBER {$$ = $1;}
		;
type:
	INT_T { $$=INTS; }
       | STRING_T { $$=CHARS; }
       | FLOAT_T { $$=FLOATS; }
       | DATE_T { $$=DATES; }
       | TEXT_T {$$=TEXTS; }
       ;
ID_get:
	ID 
	{
		char *temp=$1; 
		snprintf(CONTEXT->id, sizeof(CONTEXT->id), "%s", temp);
	}
	;

	
insert:				/*insert   语句的语法解析树*/
    INSERT INTO ID VALUES LBRACE value value_list RBRACE values_list SEMICOLON
		{
			// CONTEXT->values[CONTEXT->value_length++] = *$6;

			CONTEXT->ssql->flag=SCF_INSERT;//"insert";
			// CONTEXT->ssql->sstr.insertion.relation_name = $3;
			// CONTEXT->ssql->sstr.insertion.value_num = CONTEXT->value_length;
			// for(i = 0; i < CONTEXT->value_length; i++){
			// 	CONTEXT->ssql->sstr.insertion.values[i] = CONTEXT->values[i];
      // }
			inserts_init(&CONTEXT->ssql->sstr.insertion, $3, CONTEXT->values, CONTEXT->value_length);

      //临时变量清零
      CONTEXT->value_length=0;
    }

values_list:
|COMMA LBRACE value value_list RBRACE values_list{

}

value_list:
    /* empty */
    {
    	CONTEXT->ssql->sstr.insertion.offsets[CONTEXT->ssql->sstr.insertion.insert_num++] = CONTEXT->value_length;
    }
    | COMMA value value_list  { 
  		// CONTEXT->values[CONTEXT->value_length++] = *$2;
	  }
    ;
value:
    NUMBER{	
  		value_init_integer(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
    |FLOAT{
  		value_init_float(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
    |SSS {
		$1 = substr($1,1,strlen($1)-2);
  		value_init_string(&CONTEXT->values[CONTEXT->value_length++], $1);
		}
    |DATE {
    		$1 = substr($1,1,strlen($1)-1);
      		date_init_string(&CONTEXT->values[CONTEXT->value_length++], $1);
    		}
    |NULLT {
      		value_init_null(&CONTEXT->values[CONTEXT->value_length++]);
      		    }
    ;
    
delete:		/*  delete 语句的语法解析树*/
    DELETE FROM ID where SEMICOLON 
		{
			CONTEXT->ssql->flag = SCF_DELETE;//"delete";
			deletes_init_relation(&CONTEXT->ssql->sstr.deletion, $3);
			deletes_set_conditions(&CONTEXT->ssql->sstr.deletion, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;	
    }
    ;
update:			/*  update 语句的语法解析树*/
    UPDATE ID SET ID EQ value where SEMICOLON
		{
			CONTEXT->ssql->flag = SCF_UPDATE;//"update";
			Value *value = &CONTEXT->values[0];
			updates_init(&CONTEXT->ssql->sstr.update, $2, $4, value, 
					CONTEXT->conditions, CONTEXT->condition_length);
			CONTEXT->condition_length = 0;
		}
    ;
select:				/*  select 语句的语法解析树*/
    SELECT select_attr FROM ID rel_list where group order SEMICOLON
		{
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, $4);
			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);

			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;

			//临时变量清零
			CONTEXT->condition_length=0;
			CONTEXT->from_length=0;
			CONTEXT->select_length=0;
			CONTEXT->value_length = 0;
	}
	;

group:
{

}| GROUP_BY ID group_list {
            			RelAttr attr;
            			relation_attr_init(&attr, NULL, $2, NO_AGG);
            			selects_append_groupbys(&CONTEXT->ssql->sstr.selection, &attr);
            		}
              	| GROUP_BY ID DOT ID group_list {
            			RelAttr attr;
            			relation_attr_init(&attr, $2, $4, NO_AGG);
            			selects_append_groupbys(&CONTEXT->ssql->sstr.selection, &attr);
            		}
;

group_list:
| COMMA ID DOT ID group_list {
RelAttr attr;
            			relation_attr_init(&attr, $2, $4, NO_AGG);
            			selects_append_groupbys(&CONTEXT->ssql->sstr.selection, &attr);
}
| COMMA ID group_list {
RelAttr attr;
            			relation_attr_init(&attr, NULL, $2, NO_AGG);
            			selects_append_groupbys(&CONTEXT->ssql->sstr.selection, &attr);
}


order:
{

}
| ORDER_BY ID order_asc order_list{
RelAttr attr;
            			relation_attr_init(&attr, NULL, $2, $3);
            			selects_append_orderbys(&CONTEXT->ssql->sstr.selection, &attr);
}
| ORDER_BY ID DOT ID order_asc order_list{
	RelAttr attr;
        relation_attr_init(&attr, $2, $4, $5);
        selects_append_orderbys(&CONTEXT->ssql->sstr.selection, &attr);
}

order_list:
| COMMA ID order_asc order_list {
RelAttr attr;
relation_attr_init(&attr, NULL, $2, $3);
selects_append_orderbys(&CONTEXT->ssql->sstr.selection, &attr);
}
| COMMA ID DOT ID order_asc order_list {
RelAttr attr;
relation_attr_init(&attr, $2, $4, $5);
selects_append_orderbys(&CONTEXT->ssql->sstr.selection, &attr);
}

order_asc:
{
$$=MIN_AGG;
}
| ASC {
$$=MIN_AGG;
}
| DESC {
$$=MAX_AGG;
}

select_attr:
    STAR attr_list{
			RelAttr attr;
			relation_attr_init(&attr, NULL, "*", NO_AGG);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
    | ID attr_list {
			RelAttr attr;
			relation_attr_init(&attr, NULL, $1, NO_AGG);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
  	| ID DOT ID attr_list {
			RelAttr attr;
			relation_attr_init(&attr, $1, $3, NO_AGG);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
		}
	| COUNT LBRACE ID RBRACE attr_list{
	RelAttr attr;
        			relation_attr_init(&attr, NULL, $3, COUNT_AGG);
        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
                 	}
                 	| COUNT LBRACE ID DOT ID RBRACE attr_list{
                 	RelAttr attr;
                        			relation_attr_init(&attr, $3, $5, COUNT_AGG);
                        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                  	}
                 	| COUNT LBRACE STAR RBRACE attr_list{
                 	RelAttr attr;
                        			relation_attr_init(&attr, NULL, "*", COUNT_AGG);
                        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                 	}
                 	| COUNT_1 attr_list{
                 	RelAttr attr;
                        			relation_attr_init(&attr, NULL, "1", COUNT_AGG);
                        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                 	}
                 	| MAX LBRACE ID RBRACE attr_list{
                 	RelAttr attr;
                        			relation_attr_init(&attr, NULL, $3, MAX_AGG);
                        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                 	}
                 	| MIN LBRACE ID RBRACE attr_list{
                 	RelAttr attr;
                        			relation_attr_init(&attr, NULL, $3, MIN_AGG);
                        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                 	}
                 	| AVG LBRACE ID RBRACE attr_list{
                 	RelAttr attr;
                        			relation_attr_init(&attr, NULL, $3, AVG_AGG);
                        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                 	}
                 	| MAX LBRACE ID DOT ID RBRACE attr_list{
                 	RelAttr attr;
                        			relation_attr_init(&attr, $3, $5, MAX_AGG);
                        			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                 	}
                         | MIN LBRACE ID DOT ID RBRACE attr_list{
                         RelAttr attr;
                         			relation_attr_init(&attr, $3, $5, MIN_AGG);
                         			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                 	}
                         | AVG LBRACE ID DOT ID RBRACE attr_list{
                         RelAttr attr;
                         			relation_attr_init(&attr, $3, $5, AVG_AGG);
                         			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                 	}
    ;

attr_list:
    /* empty */
    | COMMA ID attr_list {
			RelAttr attr;
			relation_attr_init(&attr, NULL, $2, NO_AGG);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
     	  // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].relation_name = NULL;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].attribute_name=$2;
      }
    | COMMA ID DOT ID attr_list {
			RelAttr attr;
			relation_attr_init(&attr, $2, $4, NO_AGG);
			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length].attribute_name=$4;
        // CONTEXT->ssql->sstr.selection.attributes[CONTEXT->select_length++].relation_name=$2;
  	  }
  	  | COMMA COUNT LBRACE ID RBRACE attr_list{
            	RelAttr attr;
                    			relation_attr_init(&attr, NULL, $4, COUNT_AGG);
                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);
                             	}
                             	| COMMA COUNT LBRACE ID DOT ID RBRACE attr_list{
                             	RelAttr attr;
                                    			relation_attr_init(&attr, $4, $6, COUNT_AGG);
                                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                              	}
                             	| COMMA COUNT LBRACE STAR RBRACE attr_list{
                             	RelAttr attr;
                                    			relation_attr_init(&attr, NULL, "*", COUNT_AGG);
                                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                             	}
                             	| COMMA COUNT_1 attr_list{
                             	RelAttr attr;
                                    			relation_attr_init(&attr, NULL, "1", COUNT_AGG);
                                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                             	}
                             	| COMMA MAX LBRACE ID RBRACE attr_list{
                             	RelAttr attr;
                                    			relation_attr_init(&attr, NULL, $4, MAX_AGG);
                                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                             	}
                             	| COMMA MIN LBRACE ID RBRACE attr_list{
                             	RelAttr attr;
                                    			relation_attr_init(&attr, NULL, $4, MIN_AGG);
                                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                             	}
                             	| COMMA AVG LBRACE ID RBRACE attr_list{
                             	RelAttr attr;
                                    			relation_attr_init(&attr, NULL, $4, AVG_AGG);
                                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                             	}
                             	| COMMA MAX LBRACE ID DOT ID RBRACE attr_list{
                             	RelAttr attr;
                                    			relation_attr_init(&attr, $4, $6, MAX_AGG);
                                    			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                             	}
                                     | COMMA MIN LBRACE ID DOT ID RBRACE attr_list{
                                     RelAttr attr;
                                     			relation_attr_init(&attr, $4, $6, MIN_AGG);
                                     			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                             	}
                                     | COMMA AVG LBRACE ID DOT ID RBRACE attr_list{
                                     RelAttr attr;
                                     			relation_attr_init(&attr, $4, $6, AVG_AGG);
                                     			selects_append_attribute(&CONTEXT->ssql->sstr.selection, &attr);

                                             	}
  	;

rel_list:
    /* empty */{
    }
    | COMMA ID rel_list {
				selects_append_relation(&CONTEXT->ssql->sstr.selection, $2);
		  }
	| INNER_JOIN ID on join_list {
				selects_append_relation(&CONTEXT->ssql->sstr.selection, $2);
	}
    ;
join_list:

	| INNER_JOIN ID on join_list {
	selects_append_relation(&CONTEXT->ssql->sstr.selection, $2);
	}
on:
    | ON condition condition_list {

    }

where:
    /* empty */ 
    | WHERE condition condition_list {	
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
    ;
condition_list:
    /* empty */
    | AND condition condition_list {
				// CONTEXT->conditions[CONTEXT->condition_length++]=*$2;
			}
    ;
condition:
    ID comOp value 
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1, NO_AGG);

			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$ = ( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name = NULL;
			// $$->left_attr.attribute_name= $1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 0;
			// $$->right_attr.relation_name = NULL;
			// $$->right_attr.attribute_name = NULL;
			// $$->right_value = *$3;

		}
		|value comOp value 
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 2];
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$ = ( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 0;
			// $$->right_attr.relation_name = NULL;
			// $$->right_attr.attribute_name = NULL;
			// $$->right_value = *$3;

		}
		|ID comOp ID 
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, NULL, $1, NO_AGG);
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $3, NO_AGG);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=$1;
			// $$->comp = CONTEXT->comp;
			// $$->right_is_attr = 1;
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=$3;

		}
    |value comOp ID
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];
			RelAttr right_attr;
			relation_attr_init(&right_attr, NULL, $3, NO_AGG);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;

			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp=CONTEXT->comp;
			
			// $$->right_is_attr = 1;
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=$3;
		
		}
    |ID DOT ID comOp value
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $1, $3, NO_AGG);
			Value *right_value = &CONTEXT->values[CONTEXT->value_length - 1];

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 0, NULL, right_value);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;

			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;
			// $$->left_attr.relation_name=$1;
			// $$->left_attr.attribute_name=$3;
			// $$->comp=CONTEXT->comp;
			// $$->right_is_attr = 0;   //属性值
			// $$->right_attr.relation_name=NULL;
			// $$->right_attr.attribute_name=NULL;
			// $$->right_value =*$5;			
							
    }
    |value comOp ID DOT ID
		{
			Value *left_value = &CONTEXT->values[CONTEXT->value_length - 1];

			RelAttr right_attr;
			relation_attr_init(&right_attr, $3, $5, NO_AGG);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 0, NULL, left_value, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 0;//属性值
			// $$->left_attr.relation_name=NULL;
			// $$->left_attr.attribute_name=NULL;
			// $$->left_value = *$1;
			// $$->comp =CONTEXT->comp;
			// $$->right_is_attr = 1;//属性
			// $$->right_attr.relation_name = $3;
			// $$->right_attr.attribute_name = $5;
									
    }
    |ID DOT ID comOp ID DOT ID
		{
			RelAttr left_attr;
			relation_attr_init(&left_attr, $1, $3, NO_AGG);
			RelAttr right_attr;
			relation_attr_init(&right_attr, $5, $7, NO_AGG);

			Condition condition;
			condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1, &right_attr, NULL);
			CONTEXT->conditions[CONTEXT->condition_length++] = condition;
			// $$=( Condition *)malloc(sizeof( Condition));
			// $$->left_is_attr = 1;		//属性
			// $$->left_attr.relation_name=$1;
			// $$->left_attr.attribute_name=$3;
			// $$->comp =CONTEXT->comp;
			// $$->right_is_attr = 1;		//属性
			// $$->right_attr.relation_name=$5;
			// $$->right_attr.attribute_name=$7;
    }
    | ID DOT ID comOp LBRACE sub_query RBRACE {

    RelAttr left_attr;
    			relation_attr_init(&left_attr, $1, $3, NO_AGG);
    			RelAttr right_attr;
    			relation_attr_init(&right_attr, NULL, $3, SUBQUERY);
    			Condition condition;
                        condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1+CONTEXT->ssql->sstr.selection.subquery_num, &right_attr, NULL);
              		CONTEXT->conditions[CONTEXT->condition_length++] = condition;

    }
    | ID comOp LBRACE sub_query RBRACE {

             RelAttr left_attr;
             			relation_attr_init(&left_attr, NULL, $1, NO_AGG);
             			RelAttr right_attr;
             			relation_attr_init(&right_attr, NULL, $1, SUBQUERY);
             			Condition condition;
                                condition_init(&condition, CONTEXT->comp, 1, &left_attr, NULL, 1+CONTEXT->ssql->sstr.selection.subquery_num, &right_attr, NULL);
                                CONTEXT->conditions[CONTEXT->condition_length++] = condition;

             }
    | LBRACE sub_query RBRACE comOp ID DOT ID {

        RelAttr left_attr;
        			relation_attr_init(&left_attr, NULL, $7, SUBQUERY);
        			RelAttr right_attr;
        			relation_attr_init(&right_attr, $5, $7, NO_AGG);
        			Condition condition;
                            condition_init(&condition, CONTEXT->comp, 1+CONTEXT->ssql->sstr.selection.subquery_num, &left_attr, NULL, 1, &right_attr, NULL);
                  		CONTEXT->conditions[CONTEXT->condition_length++] = condition;

        }
        | LBRACE sub_query RBRACE comOp ID {

                 RelAttr left_attr;
                 			relation_attr_init(&left_attr, NULL, $5, SUBQUERY);
                 			RelAttr right_attr;
                 			relation_attr_init(&right_attr, NULL, $5, NO_AGG);
                 			Condition condition;
                                    condition_init(&condition, CONTEXT->comp, 1+CONTEXT->ssql->sstr.selection.subquery_num, &left_attr, NULL, 1, &right_attr, NULL);
                                    CONTEXT->conditions[CONTEXT->condition_length++] = condition;

                 }
                 | LBRACE sub_query RBRACE comOp LBRACE sub_query RBRACE {
                         RelAttr left_attr;
                         			relation_attr_init(&left_attr, NULL, " ", SUBQUERY);
                         			RelAttr right_attr;
                         			relation_attr_init(&right_attr, NULL, " ", SUBQUERY);
                         			Condition condition;
                 condition_init(&condition, CONTEXT->comp, CONTEXT->ssql->sstr.selection.subquery_num, &left_attr, NULL, 1+CONTEXT->ssql->sstr.selection.subquery_num, &right_attr, NULL);
                                   		CONTEXT->conditions[CONTEXT->condition_length++] = condition;

                         }
    ;

sub_query:
	{
		CONTEXT_VECTOR->len++;
		CONTEXT->condition_length=0;
                CONTEXT->from_length=0;
                CONTEXT->select_length=0;
                CONTEXT->value_length = 0;
	}
	SELECT select_attr FROM ID rel_list where group order
		{
			// CONTEXT->ssql->sstr.selection.relations[CONTEXT->from_length++]=$4;
			selects_append_relation(&CONTEXT->ssql->sstr.selection, $5);
			CONTEXT->ssql->flag=SCF_SELECT;//"select";
			selects_append_conditions(&CONTEXT->ssql->sstr.selection, CONTEXT->conditions, CONTEXT->condition_length);
			selects_append_selects(&PARENTCONTEXT->ssql->sstr.selection, &CONTEXT->ssql->sstr.selection);
			// CONTEXT->ssql->sstr.selection.attr_num = CONTEXT->select_length;
			//临时变量清零
			CONTEXT_VECTOR->len--;
	}
	;


comOp:
  	  EQ { CONTEXT->comp = EQUAL_TO; }
    | LT { CONTEXT->comp = LESS_THAN; }
    | GT { CONTEXT->comp = GREAT_THAN; }
    | LE { CONTEXT->comp = LESS_EQUAL; }
    | GE { CONTEXT->comp = GREAT_EQUAL; }
    | NE { CONTEXT->comp = NOT_EQUAL; }
    | OP_IS {CONTEXT->comp = IS;}
    | OP_NOT {CONTEXT->comp = IS_NOT;}
    | OP_IN {CONTEXT->comp = IN;}
    | OP_NOTIN {CONTEXT->comp = NOT_IN;}

    ;

load_data:
		LOAD DATA INFILE SSS INTO TABLE ID SEMICOLON
		{
		  CONTEXT->ssql->flag = SCF_LOAD_DATA;
			load_data_init(&CONTEXT->ssql->sstr.load_data, $7, $4);
		}
		;
%%
//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, Query *sqls){
	ParserVector contextVector;
	memset(&contextVector, 0, sizeof(contextVector));

	for (int i = 1; i < MAX_NUM; i++) {
	contextVector.contexts[i].ssql = (Query*)malloc(sizeof(Query));
	memset(contextVector.contexts[i].ssql, 0, sizeof(Query));
	}

	yyscan_t scanner;
	yylex_init_extra(&contextVector, &scanner);
	contextVector.contexts[0].ssql = sqls;
	scan_string(s, scanner);
	int result = yyparse(scanner);
	yylex_destroy(scanner);

        for (int i = 1; i < MAX_NUM; i++) {
        	contextVector.contexts[i].ssql = (Query*)malloc(sizeof(Query));
        	memset(contextVector.contexts[i].ssql, 0, sizeof(Query));
        }
	return result;
}
