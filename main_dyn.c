
#include "Services/STD_types.h"
#include "Services/BIT_math.h"
#include"MCL/DIO_dynamic/DIO_dynamic_int.h"
#include"HAL/Keypad/Keypad_int.h"
#include"HAL/LCD/LCD_int.h"

#define stack_length 32
#define true  1
#define false 0
#define OP_PAR     '('
#define CL_PAR     ')'
#define DOT        '.'
#define Post_length  33
#define Pre_length   33
u8 exception=false;
u8 post_exp[Post_length]= {' '}; //   //global
typedef struct
{
    signed short top;
    char stk[stack_length];
} stack_char;
void stackc_init(stack_char* stk)
{
    stk->top=-1;
}
void stackc_push(stack_char* stk,char ele)
{
    if(stk->top<stack_length)
        stk->stk[++stk->top]=ele;
    return;
}
u8 stackc_empty(stack_char* stk)
{
    if(stk->top==-1)
        return true;
    return false;
}
void stackc_pop(stack_char* stk)
{
    if(stk->top>-1)
        stk->top--;
    return;
}
char stackc_top(stack_char* stk)
{
    if(stk->top<stack_length)
        return stk->stk[stk->top];
    else
        return '\0';
}

typedef struct
{
    signed short top;
    f32 stk[stack_length];
} stack_float;
void stackf_init(stack_float* stk)
{
    stk->top=-1;
}
void stackf_push(stack_float* stk,f32 ele)
{
    if(stk->top < stack_length)
        stk->stk[++stk->top]=ele;
    return;
}
u8 stackf_empty(stack_float* stk)
{
    if(stk->top==-1)
        return true;
    return false;
}
void stackf_pop(stack_float* stk)
{
    if(stk->top>-1)
        stk->top--;
    return;
}
f32 stackf_top(stack_float* stk)
{
    if(stk->top<stack_length)
        return stk->stk[stk->top];
    else
        return 0.0f;
}


u8 IsOperator(u8 Copy_u8_Op)
{
    if(Copy_u8_Op=='+'||Copy_u8_Op=='-'||Copy_u8_Op=='^'||Copy_u8_Op=='*'||Copy_u8_Op=='/'||Copy_u8_Op=='%')
        return true;
    else
        return false;
}
f32 Power(u32 Copy_u32_Power,u32 Copy_u32_Base)
{
    f32 Local_Res=1;
    while(Copy_u32_Power!=0)
    {
        if(Copy_u32_Power%2!=0)
            Local_Res=Local_Res*Copy_u32_Base;
        Copy_u32_Base*=Copy_u32_Base;
        Copy_u32_Power/=2;
    }
    return Local_Res;
}
u8 Check_Precedance(u8 Stack_Op,u8 Current_Op)  // Send the top of stack to Stack_Op ,send current operator to Current_Op
{
    if(Stack_Op=='^')
        return true;
    else if( (Stack_Op=='/' ||Stack_Op=='*'||Stack_Op=='%')&& (Current_Op=='-'||Current_Op=='+') )
        return true;
    else if(  (Stack_Op=='/' ||Stack_Op=='*'||Stack_Op=='%')&& (Current_Op=='/'||Current_Op=='*'||Stack_Op=='%'))
        return true;
    else if(  (Stack_Op=='+' ||Stack_Op=='-')&& (Current_Op=='+'||Current_Op=='-'))
        return true;
    return false;
}
f32 Operation(f32 Copy_f32_Op1,f32 Copy_f32_Op2, u8 Copy_u8_Op)
{
    f32 Local_Res=0;
    switch(Copy_u8_Op)
    {
    case '/':
        Local_Res=Copy_f32_Op1/Copy_f32_Op2;

        break;
    case '*':
        Local_Res=Copy_f32_Op1*Copy_f32_Op2;
        break;
    case '+':
        Local_Res=Copy_f32_Op1+Copy_f32_Op2;
        break;
    case '-':
        Local_Res=Copy_f32_Op1-Copy_f32_Op2;
        break;
    case '^':
        Local_Res=Power(Copy_f32_Op2,Copy_f32_Op1);
        break;
   case '%':
        Local_Res=Copy_f32_Op1/100.0*(u32)Copy_f32_Op2;
        break;
     }
    return Local_Res;
}
u8 IsOperand(u8 Copy_u8_Op)
{
    if(Copy_u8_Op>='0'&&Copy_u8_Op<='9')\
        return true;
    /*
    else  if(Copy_u8_Op>='a'&&Copy_u8_Op<='z')
        return true;
    else  if(Copy_u8_Op>='A'&&Copy_u8_Op<='Z')
        return true;
   */
    return false;
}
u8* In_Postfix(u8* infix_exp0)
{
    u8 counter=0;
    u8 infix_exp[Pre_length];
    u8 i=0,j=0;
    u8 flag=false;
    for(; infix_exp0[i]!='\0'; i++,   j++)
    {
        if(infix_exp0[i]==OP_PAR)
            counter++;
        else if(infix_exp0[i]==CL_PAR)
            counter--;
        if(i==0 &&(infix_exp0[i]=='-'||infix_exp0[i]=='+'|| infix_exp0[i]==DOT))
        {
            infix_exp[j]='0';
            flag=true;
        }
        else if (( (infix_exp0[i]=='-')||(infix_exp0[i]=='+') ) &&(!IsOperand(infix_exp0[i-1])&&IsOperand(infix_exp0[i+1] )))
        {
            infix_exp[j]='0';
            flag=true;
        }
        else if (( (infix_exp0[i]=='-')||(infix_exp0[i]=='+') ) &&(!IsOperand(infix_exp0[i-1])&&(IsOperand(infix_exp0[i+1]) )))
        {
            infix_exp[j]='0';
            flag=true;
        }
         else if ( infix_exp0[i]==DOT  &&!IsOperand(infix_exp0[i-1])&&IsOperand(infix_exp0[i+1]))
        {
            infix_exp[j]='0';
            flag=true;
        }
        else if ( infix_exp0[i]==DOT &&IsOperand(infix_exp0[i-1])&&!IsOperand(infix_exp0[i+1]))
        {
            infix_exp[j]=infix_exp0[i];
            infix_exp[++j]='0';
            continue;
        }
        if(flag==false) //flag==false
            infix_exp[j]=infix_exp0[i];
        else if(flag==true)
        {
            j++;
            infix_exp[j]=infix_exp0[i];
            flag=false;
        }

    }
    while(counter !=0)
    {
        infix_exp[j]=CL_PAR;
        counter--;
        j++;
    }
    infix_exp[j]='\0';
    stack_char stk;
    stackc_init(&stk);
    j=0; i=0;
    for(   ; infix_exp[i]!='\0'; i++)
    {
        if(i!=0)
            if(IsOperand(infix_exp[i-1]))
                j++;
        if( IsOperand(infix_exp[i]) )
        {
            post_exp[j]=infix_exp[i];
            if(! IsOperand(infix_exp[i+1])&& infix_exp[i+1]!=DOT)
                post_exp[++j]=' ';
        }
        else if(IsOperator(infix_exp[i]))
        {
            while(!stackc_empty(&stk))
            {
                while( stackc_top(&stk)!=OP_PAR &&Check_Precedance(stackc_top(&stk),infix_exp[i]) )
                {
                    post_exp[j]=stackc_top(&stk);
                    stackc_pop(&stk);
                    j++;
                }
                break;
            }
            stackc_push(&stk,infix_exp[i]);  // if empty push operator or has higher precdance
        }else if(infix_exp[i]==DOT){
             post_exp[j++]=infix_exp[i];
        }
        if(infix_exp[i]==OP_PAR)
        {
            stackc_push(&stk,infix_exp[i]);
        }
        else if(infix_exp[i]==CL_PAR)
        {
            while(!stackc_empty(&stk))
            {
                while(stackc_top(&stk)!=OP_PAR )
                {
                    post_exp[j]=stackc_top(&stk);
                    stackc_pop(&stk);
                    j++;
                }
                break;
            }
            stackc_pop(&stk); // pop opening parentheses

        }
    }
    while(!stackc_empty(&stk))
    {   j++;
        post_exp[j]=stackc_top(&stk);
        stackc_pop(&stk);

    }
    return post_exp;
}
f32 Evaluate(u8* post_exp){
    stack_float eval;stackf_init(&eval);
    f32 base=0.1f; f32 val1=0 ; f32 val2=0.0f ;u8 flag1=false,flag2=false;
for(u32 i=0;post_exp[i]!='\0';i++){
while(IsOperand(post_exp[i])){
    val1*=10.0;
     val1+=(post_exp[i]-'0');
     i++; flag1=true;
}
if(post_exp[i]==DOT){
    i++;
while(IsOperand(post_exp[i])){
     val2+=((f32)(post_exp[i]-'0'))*base;
     base/=10.0;
     i++; flag2=true;
}
}
 if(post_exp[i]==' '){
     if(flag1==true&&flag2==true)
    stackf_push(&eval,val1+val2);
    else if (flag1==true&&flag2==false)
         stackf_push(&eval,val1);
    else if(flag2==true&&flag1==false)
        stackf_push(&eval,val2);
 val1=val2=0.0f; flag1=flag2=false;base=0.1f;
} if(IsOperator ( post_exp[i]) ){
 f32 op2= stackf_top(&eval);
stackf_pop(&eval);
 f32 op1=stackf_top(&eval);
stackf_pop(&eval);
if(post_exp[i]=='/' && op2==0.0){
	exception=true;
	return 0;
}
 stackf_push(&eval,Operation(op1,op2,post_exp[i]));
}
}
return stackf_top(&eval);
}
void f32ToString2(char* str, f32 d, u8 n)
{
	//For integer part conversion
	u16 t = 1;
	u16 temp = 1;
	u16 i = 0;
	while(t <= (int)d)
	t *= 10;
	temp = t/10;
	for(i = 0; temp > 0; i++)
	{
		u16 digit = ((u16)d)/temp;
		str[i] = digit + '0';
		d = d - (double)(digit*temp);
		temp /= 10;
	}
	if(t==1)
		str[i++] = '0';
	//Add decimal point
	str[i++] = '.';

	//For after decimal point conversion
	for(; n > 0; n--)
	{
		d = d*10;
		str[i++] = (u8)d + '0';
		d -= (u8)d;
	}

	//Null end
	str[i] = 0;
}

int main(){
  u8 iter=0,x='\0'; u8 sign=false;
  u8 inf_exp[Pre_length]= {' '};
 LCD_vid_Init();
 LCD_vid_ClearScreen();
 Keypad_vid_Init();
 while(1){
	 	 LCD_vid_ClearScreen();
		for(u8 i=0;i<iter;i++)
			inf_exp[i]='!';
		iter=0;x='\0';
 while(1){
	 	 u8 tmp=iter;
 	x=Keypad_u8_Get_Button();
 	if(x!='\0'){
 		if(IsOperand(x)||IsOperator(x)||x==DOT||x==OP_PAR||x==CL_PAR){
 				inf_exp[iter]=x;

 				 if(iter>=16){
 					LCD_vid_GoTORowCol(1,tmp-16);
 					tmp++;
 				 }

 				LCD_vid_WriteCharacter(x);
 		}else if(x=='='||x=='C'){
 			break;
 		}
 		while(Keypad_u8_Get_Button()!='\0');
 		iter++;
 	 }
 	}
 if(x=='C')
	 continue;
 u8* str=In_Postfix(inf_exp);
 f32 RESULT=Evaluate(str);
 if(RESULT<0.0){
	 RESULT=-RESULT;
	 sign=true;
 }
 u8 buff[32]={'\0'};

 f32ToString2((char*) buff,RESULT,2);
 if(exception==true){
	 LCD_vid_ClearScreen();
	 LCD_vid_WriteString((u8*)"syntax error");
	 exception=false;
	 x='\0';
	 while(1){
	 	x=Keypad_u8_Get_Button();
	 	if(x!='\0')
	 		 if(x=='C'){
	 			break;
	 		 }
	 	while(Keypad_u8_Get_Button()!='\0');
	 	}
	}
 if(x=='C')
	 continue;
 if(iter>=16){
	 LCD_vid_ClearScreen();
	 if( sign==true)
		 LCD_vid_WriteCharacter('-');
	 LCD_vid_WriteString(buff);
 }else{
	 LCD_vid_WriteCharacter('=');
	 LCD_vid_GoTORowCol(1,0);
	 if( sign==true)
		 LCD_vid_WriteCharacter('-');
	 LCD_vid_WriteString(buff);
 }
 x='\0';
 while(1){
 	x=Keypad_u8_Get_Button();
 	if(x!='\0')
 		 if(x=='C'){
 			break;
 		 }
 	while(Keypad_u8_Get_Button()!='\0');
 	}
}
 return 0;
}
