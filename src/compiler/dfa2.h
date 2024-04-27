#include <string>
std::string DFAstring = R"(
.STATES
start
ID!
ID1!
ID2!
INT!

IF!

ID3!
ID4!
ID5!
WAIN!

ID6!
ID7!
ID8!
WHILE!

ID9!
ID10!
ID11!
ELSE!

ID12!
ID13!
ID14!
ID15!
ID16!
ID17!
PRINTLN!

ID18!
ID19!
ID20!
ID21!
ID22!
RETURN!

ID23!
ID24!
NEW!

ID25!
ID26!
ID27!
ID28!
ID29!
DELETE!

ID30!
ID31!
ID32!
NULL!

NUM!

LPAREN!
RPAREN!
LBRACE!
RBRACE!
LBRACK!
RBRACK!
BECOMES!
PLUS!
MINUS!
STAR!
SLASH!
PCT!
AMP!
COMMA!
SEMI!
LT!
GT!
LE!
GE!
EQ!

NEND
NE!

?WHITESPACE!
?COMMENT!




.TRANSITIONS
start i                 ID1
ID1   n                 ID2
ID2   t                 INT
INT   a-z A-Z 0-9           ID
ID1   a-e g-m o-z A-Z 0-9   ID
ID2   a-s u-z A-Z 0-9      ID

ID1   f                 IF
IF    a-z A-Z 0-9          ID

start w                 ID3
ID3   a                 ID4
ID4   i                 ID5
ID5   n                 WAIN
WAIN  a-z A-Z 0-9          ID
ID3   b-g i-z A-Z 0-9      ID
ID4   a-h j-z A-Z 0-9      ID
ID5   a-m o-z A-Z 0-9      ID

ID3   h                 ID6
ID6   i                 ID7
ID7   l                 ID8
ID8   e                 WHILE
WHILE a-z A-Z 0-9          ID
ID6   a-h j-z A-Z 0-9      ID
ID7   a-k m-z A-Z 0-9      ID
ID8   a-d f-z A-Z 0-9      ID

start e                 ID9
ID9   l                 ID10
ID10  s                 ID11
ID11  e                 ELSE
ElSE  a-z A-Z 0-9          ID
ID9   a-k m-z A-Z 0-9      ID
ID10  a-r t-z A-Z 0-9      ID
ID11  a-d f-z A-Z 0-9      ID

start p                 ID12
ID12  r                 ID13
ID13  i                 ID14
ID14  n                 ID15
ID15  t                 ID16
ID16  l                 ID17
ID17  n                 PRINTLN
PRINTLN a-z A-Z 0-9        ID
ID12  a-q s-z A-Z 0-9      ID
ID13  a-h j-z A-Z 0-9      ID
ID14  a-m o-z A-Z 0-9      ID
ID15  a-s u-z A-Z 0-9      ID
ID16  a-k m-z A-Z 0-9      ID
ID17  a-m o-z A-Z 0-9      ID

start r                 ID18
ID18  e                 ID19
ID19  t                 ID20
ID20  u                 ID21
ID21  r                 ID22
ID22  n                 RETURN
RETURN a-z A-Z 0-9         ID
ID18  a-d f-z A-Z 0-9      ID
ID19  a-s u-z A-Z 0-9      ID
ID20  a-t v-z A-Z 0-9      ID
ID21  a-q s-z A-Z 0-9      ID
ID22  a-m o-z A-Z 0-9      ID

start n                 ID23
ID23  e                 ID24
ID24  w                 NEW
NEW   a-z A-Z 0-9          ID
ID23  a-d f-z A-Z 0-9      ID
ID24  a-v x-z A-Z 0-9      ID

start d                 ID25
ID25  e                 ID26
ID26  l                 ID27
ID27  e                 ID28
ID28  t                 ID29
ID29  e                 DELETE
DELETE a-z A-Z 0-9         ID
ID25 a-d f-z A-Z 0-9       ID
ID26 a-k m-z A-Z 0-9       ID
ID27 a-d f-z A-Z 0-9       ID
ID28 a-s u-z A-Z 0-9       ID
ID29 a-d f-z A-Z 0-9       ID

start N                 ID30
ID30  U                 ID31
ID31  L                 ID32
ID32  L                 NULL
NULL  a-z A-Z 0-9          ID
ID30  a-z A-T V-Z 0-9      ID
ID31  a-z A-K M-Z 0-9      ID
ID32  a-z A-K M-Z 0-9      ID

start a-c f-h j-m o q s-v x-z A-M O-Z 0-9 ID

ID    a-z A-Z 0-9           ID

start 0-9               NUM
NUM   0-9               NUM

start (                 LPAREN
start )                 RPAREN
start {                 LBRACE
start }                 RBRACE
start [                 LBRACK
start ]                 RBRACK
start =                 BECOMES
start +                 PLUS
start -                 MINUS
start *                 STAR
start /                 SLASH
start %                 PCT	
start &                 AMP
start ,                 COMMA
start ;                 SEMI
start <                 LT
start >                 GT
LT    =                 LE
GT    =                 GE
BECOMES =               EQ

start !                 NEND
NEND  =                 NE

start       \s \t       ?WHITESPACE
?WHITESPACE \s \t       ?WHITESPACE

SLASH     /              ?COMMENT
?COMMENT \x00-\x09 \x0B \x0C \x0E-\x7F ?COMMENT


)";
