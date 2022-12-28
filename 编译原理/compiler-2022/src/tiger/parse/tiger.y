%filenames parser
%scanner tiger/lex/scanner.h
%baseclass-preinclude tiger/absyn/absyn.h

 /*
  * Please don't modify the lines above.
  */

%union {
  int ival;
  std::string* sval;
  sym::Symbol *sym;
  absyn::Exp *exp;
  absyn::ExpList *explist;
  absyn::Var *var;
  absyn::DecList *declist;
  absyn::Dec *dec;
  absyn::EFieldList *efieldlist;
  absyn::EField *efield;
  absyn::NameAndTyList *tydeclist;
  absyn::NameAndTy *tydec;
  absyn::FieldList *fieldlist;
  absyn::Field *field;
  absyn::FunDecList *fundeclist;
  absyn::FunDec *fundec;
  absyn::Ty *ty;
  }

%token <sym> ID
%token <sval> STRING
%token <ival> INT

%token
  COMMA COLON SEMICOLON LPAREN RPAREN LBRACK RBRACK
  LBRACE RBRACE DOT

  ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF
  BREAK NIL
  FUNCTION VAR TYPE

 /* token priority */
 /* TODO: Put your lab3 code here */
%right ASSIGN
%left OR
%left AND
%nonassoc EQ NEQ LT LE GT GE
%left PLUS MINUS
%left TIMES DIVIDE
%left UMINUS

 /* unterminal token define */
%type <exp> exp expseq
%type <explist>   sequencing_exps
%type <var> lvalue oneormore
%type <declist> decs
%type <dec> decs_nonempty_s vardec
%type <efieldlist> rec
%type <efield> rec_one
%type <tydeclist> tydec
%type <tydec> tydec_one
%type <fieldlist> tyfields
%type <field> tyfield
%type <ty> ty
%type <fundeclist> fundec
%type <fundec> fundec_one

 /* Use Burke-Fisher method to handle bugs */



%start program

%%
program:  exp  {absyn_tree_ = std::make_unique<absyn::AbsynTree>($1);};

lvalue:
     ID  {$$ = new absyn::SimpleVar(scanner_.GetTokPos(), $1);}
  |  oneormore  {$$ = $1;}
  ;



oneormore:
    ID DOT ID                   {$$ = new absyn::FieldVar(scanner_.GetTokPos(), new absyn::SimpleVar(scanner_.GetTokPos(), $1), $3);}
  | ID LBRACK exp RBRACK        {$$ = new absyn::SubscriptVar(scanner_.GetTokPos(), new absyn::SimpleVar(scanner_.GetTokPos(), $1), $3);}
  | oneormore DOT ID            {$$ = new absyn::FieldVar(scanner_.GetTokPos(), $1, $3);}
  | oneormore LBRACK exp RBRACK {$$ = new absyn::SubscriptVar(scanner_.GetTokPos(), $1, $3);}
  ;


exp:
    INT       {$$ = new absyn::IntExp(scanner_.GetTokPos(), $1);}
  | STRING    {$$ = new absyn::StringExp(scanner_.GetTokPos(), $1);}
  | NIL       {$$ = new absyn::NilExp(scanner_.GetTokPos());}
  | lvalue    {$$ = new absyn::VarExp(scanner_.GetTokPos(), $1);}
  | lvalue ASSIGN exp {$$ = new absyn::AssignExp(scanner_.GetTokPos(), $1, $3);}
  | LPAREN sequencing_exps  RPAREN {$$ = new absyn::SeqExp(scanner_.GetTokPos(), $2);}
  |	LPAREN expseq RPAREN {$$ = $2;}
  | ID LBRACE rec RBRACE     {$$ = new absyn::RecordExp(scanner_.GetTokPos(), $1, $3); }
  | ID LBRACE  RBRACE     {$$ = new absyn::RecordExp(scanner_.GetTokPos(), $1, new absyn::EFieldList()); }
  | LPAREN RPAREN         {$$ = new absyn::VoidExp(scanner_.GetTokPos());}
  | LPAREN exp RPAREN         {$$ = $2;}
  /* op expression */
  | exp PLUS exp  {$$ = new absyn::OpExp(scanner_.GetTokPos(), absyn::PLUS_OP, $1, $3);}
  | exp MINUS exp  {$$ = new absyn::OpExp(scanner_.GetTokPos(), absyn::MINUS_OP, $1, $3);}
  | exp TIMES exp  {$$ = new absyn::OpExp(scanner_.GetTokPos(), absyn::TIMES_OP, $1, $3);}
  | exp DIVIDE exp  {$$ = new absyn::OpExp(scanner_.GetTokPos(), absyn::DIVIDE_OP, $1, $3);}
  | MINUS exp %prec UMINUS {$$ = new absyn::OpExp(scanner_.GetTokPos(), absyn::MINUS_OP, new absyn::IntExp(scanner_.GetTokPos(), 0), $2);}
  | exp EQ exp  {$$ = new absyn::OpExp(scanner_.GetTokPos(), absyn::EQ_OP, $1, $3);}
  | exp NEQ exp  {$$ = new absyn::OpExp(scanner_.GetTokPos(), absyn::NEQ_OP, $1, $3);}
  | exp LT exp  {$$ = new absyn::OpExp(scanner_.GetTokPos(), absyn::LT_OP, $1, $3);}
  | exp LE exp  {$$ = new absyn::OpExp(scanner_.GetTokPos(), absyn::LE_OP, $1, $3);}
  | exp GT exp  {$$ = new absyn::OpExp(scanner_.GetTokPos(), absyn::GT_OP, $1, $3);}
  | exp GE exp  {$$ = new absyn::OpExp(scanner_.GetTokPos(), absyn::GE_OP, $1, $3);}
  | exp AND exp {$$ = new absyn::OpExp(scanner_.GetTokPos(), absyn::AND_OP, $1, $3);}
  | exp OR exp  {$$ = new absyn::OpExp(scanner_.GetTokPos(), absyn::OR_OP, $1, $3);}

  /* branch expression */
  | IF LPAREN exp RPAREN THEN exp  {$$ = new absyn::IfExp(scanner_.GetTokPos(), $3, $6, nullptr);}
  | IF exp THEN exp           {$$ = new absyn::IfExp(scanner_.GetTokPos(), $2, $4, nullptr);}
  | IF exp THEN exp ELSE exp  {$$ = new absyn::IfExp(scanner_.GetTokPos(), $2, $4, $6);}
  | IF LPAREN exp RPAREN THEN exp ELSE exp  {$$ = new absyn::IfExp(scanner_.GetTokPos(), $3, $6, $8);}


  | WHILE LPAREN exp RPAREN DO exp    {$$ = new absyn::WhileExp(scanner_.GetTokPos(), $3, $6);}
  | WHILE exp DO exp                  {$$ = new absyn::WhileExp(scanner_.GetTokPos(), $2, $4);}
  | FOR ID ASSIGN exp TO exp DO exp   {$$ = new absyn::ForExp(scanner_.GetTokPos(), $2, $4, $6, $8);}
  | BREAK                             {$$ = new absyn::BreakExp(scanner_.GetTokPos());}


  /* special expression */
  | ID LPAREN sequencing_exps RPAREN  {$$ = new absyn::CallExp(scanner_.GetTokPos(), $1, $3);}
  | ID LPAREN RPAREN                  {$$ = new absyn::CallExp(scanner_.GetTokPos(), $1, new absyn::ExpList());}
  | LET decs IN expseq END               {$$ = new absyn::LetExp(scanner_.GetTokPos(), $2, $4);}
  | ID LBRACK exp RBRACK OF exp       {$$ = new absyn::ArrayExp(scanner_.GetTokPos(), $1, $3, $6);}
  ;

rec:        /* <EFieldList> */
    rec_one               {$$ = new absyn::EFieldList($1);}
  | rec_one COMMA rec     {$$ = $3; $$->Prepend($1);}
  ;

rec_one:    /* <EField> */
    ID EQ exp    {$$ = new absyn::EField($1, $3);}
  ;

expseq:     /* <SeqExp> */
    sequencing_exps               {$$ = new absyn::SeqExp(scanner_.GetTokPos(), $1);}
   ;

sequencing_exps: /* <ExpList> */
    exp                             {$$ = new absyn::ExpList($1);}
  | exp COMMA sequencing_exps       {$$ = $3; $$->Prepend($1);}
  | exp SEMICOLON sequencing_exps   {$$ = $3; $$->Prepend($1);}
  ;

decs:   /* <DecList> */
    vardec                          {$$ = new absyn::DecList($1);}
  | vardec decs                     {$$ = $2; $$->Prepend($1);}
  | decs_nonempty_s                 {$$ = new absyn::DecList($1);}
  | decs_nonempty_s decs            {$$ = $2; $$->Prepend($1);}
  ;

vardec: /* <VarDec> */
    VAR ID ASSIGN exp               {$$ = new absyn::VarDec(scanner_.GetTokPos(), $2, nullptr, $4);}
  | VAR ID COLON ID ASSIGN exp      {$$ = new absyn::VarDec(scanner_.GetTokPos(), $2, $4, $6);}
  ;


decs_nonempty_s:   /* <TypeDec> or <FunctionDec> */
    tydec                           {$$ = new absyn::TypeDec(scanner_.GetTokPos(), $1);}
  | fundec                          {$$ = new absyn::FunctionDec(scanner_.GetTokPos(), $1);}
  ;

tydec:    /* <NameAndTyList> */
    TYPE tydec_one                        {$$ = new absyn::NameAndTyList($2);}   /* add new type to typeList */
  | TYPE tydec_one tydec                  {$$ = $3; $$->Prepend($2);}
  ;

tydec_one:   /* <NameAndTy> */
    ID EQ ty                       {$$ = new absyn::NameAndTy($1, $3);}
  ;


fundec:     /* <FunDecList> */
    FUNCTION fundec_one                 {$$ = new absyn::FunDecList($2);}
  | FUNCTION fundec_one fundec         {$$ = $3; $$->Prepend($2);}
  ;

fundec_one:   /* <FunDec> */
    ID LPAREN tyfields RPAREN COLON ID EQ exp {$$ = new absyn::FunDec(scanner_.GetTokPos(), $1, $3, $6, $8);}
  | ID LPAREN RPAREN EQ exp                   {$$ = new absyn::FunDec(scanner_.GetTokPos(), $1, new absyn::FieldList(), nullptr, $5);}
  | ID LPAREN tyfields RPAREN EQ exp          {$$ = new absyn::FunDec(scanner_.GetTokPos(), $1, $3, nullptr, $6);}
  | ID LPAREN RPAREN COLON ID EQ exp          {$$ = new absyn::FunDec(scanner_.GetTokPos(), $1, new absyn::FieldList(), $5, $7);}
  | ID LPAREN tyfields RPAREN COLON ID EQ LPAREN exp RPAREN {$$ = new absyn::FunDec(scanner_.GetTokPos(), $1, $3, $6, $9);}
  | ID LPAREN RPAREN EQ LPAREN exp RPAREN                   {$$ = new absyn::FunDec(scanner_.GetTokPos(), $1, new absyn::FieldList(), nullptr, $6);}
  | ID LPAREN tyfields RPAREN EQ LPAREN exp RPAREN          {$$ = new absyn::FunDec(scanner_.GetTokPos(), $1, $3, nullptr, $7);}
  | ID LPAREN RPAREN COLON ID EQ LPAREN exp RPAREN          {$$ = new absyn::FunDec(scanner_.GetTokPos(), $1, new absyn::FieldList(), $5, $8);}
  ;

ty:
    ARRAY OF ID {$$ = new absyn::ArrayTy(scanner_.GetTokPos(), $3);}
  | ID {$$ = new absyn::NameTy(scanner_.GetTokPos(), $1);}
  | LBRACE tyfields RBRACE {$$ = new absyn::RecordTy(scanner_.GetTokPos(), $2);}
  ;

tyfields:  /* <FieldList> */
    tyfield  {$$ = new absyn::FieldList($1);}
  | tyfield COMMA tyfields {$$ = $3; $$->Prepend($1);}
  ;

tyfield:
    ID COLON ID  {$$ = new absyn::Field(scanner_.GetTokPos(), $1, $3);}
  ;