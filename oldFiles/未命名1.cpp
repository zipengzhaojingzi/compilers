#include <bits/stdc++.h>
#include "lexicalAnaly.h" 

using namespace std;
map<char, int> getCharIndex; //��ȡ��Ӧ�ַ����ַ����� charArray �е�����
char charArray[100]; //��¼�ķ����������е��ַ�, charArray[0] = '#' vector<string> rule; //�ķ�����(����ʽ)����
string firstSet[100]; //��¼������Ӧ���ַ��� FIRST ��
string followSet[200]; //��¼������Ӧ���ַ��� FOLLOW ��
int analyticalTable[100][100]; //��¼������, ����Ϊ��Ӧ���ս���ͷ��ս��������
queue<char> remainSymbQueue; //ʣ���ַ�������
stack<char> analyticalStack; //����ջ
void print_AnalyticalTable();
/*�����ķ�*/
void input(){
getCharIndex['#'] = 0;
charArray[0] = '#';
//�����ķ������������ս��, ��������˳����ս���� 1 ��ʼ���
int num = 1;
char char_in;
cout << "�������ķ����������ս��: ";
while(cin.peek() != '\n'){
cin >> char_in;
getCharIndex[char_in] = num;
charArray[num++] = char_in;
}
//���������ս����,ʹ���ַ�@�����ַ���
getCharIndex['@'] = num;
charArray[num++] = '@';
//�����ķ����������з��ս�����������ս���ı�ż���Ϊ���ս�����б��
cout << "�������ķ��������ķ��ս��: ";
do{
cin >> char_in;
getCharIndex[char_in] = num;
charArray[num++] = char_in;
}while (cin.peek() != '\n');//�������е��ķ�����(����ʽ)<ʹ���ַ�@�����ַ���>, �����ʽ: S->aBc|adB
cout << "�������ķ����ķ�����(����ʽ, ����@����), ����: S->aBc|adB: " << endl;
string str;
while(cin >> str && str != "end"){
string tmp;
tmp += str[0];
for(int i=3; i<str.size(); i++){
//�ķ������� rule �����д洢�ĸ�ʽΪ <����ʽ��||����ʽ�Ҳ�>
//���� S->aBc|adB ��洢Ϊ�����ַ������ֱ�Ϊ SaBc��SadB
if(str[i] == '|'){
rule.push_back(tmp);
tmp.clear();
tmp += str[0];
}
else
{
tmp += str[i];
}
}
rule.push_back(tmp);
}
//��ӡ������ս������ս��
for(int i=0; i<num; i++){
cout << "���" << i << "�ַ�: " << charArray[i] << "\t";
}
cout << endl;
//��ӡ������ķ�����(����ʽ)
for(int i=0; i<rule.size(); i++){
cout << rule[i] << endl;
}
}
/*�ϲ��ַ��� a ���ַ��� b, ��ȥ�� a b �ַ������ظ���Ԫ�أ����ϲ�����ַ����ŵ����� a
��*/
void mergeStr(string &a, string b){
set<char> s;
for(int i=0; i<a.size(); i++){
s.insert(a[i]);
}
for(int i=0; i<b.size(); i++){
s.insert(b[i]);
}string ret;
for(set<char>::iterator i=s.begin(); i!=s.end(); i++){
ret += *i;
}
a = ret;
}
/*�ݹ��������ս���� FIRST ����(����ȡ��, �ŵ�������)*/
string getfirstSet(int charIndex, int& spaceFlag){
//���ַ�Ϊ�գ�������ַ������ܹ��������ַ�
if(charIndex == getCharIndex['@']){
spaceFlag = 1;
}
//�����ַ�Ϊ�ս��, ���ظ��ս���� FIRST ��
if(charIndex < getCharIndex['@']){
return firstSet[charIndex];
}
//��������ַ�����Ϊ���ս��, ��ݹ����� FIRST ����
string ret = "";
for(int i=0; i<rule.size(); i++){
if(getCharIndex[rule[i][0]] == charIndex){
ret += getfirstSet(getCharIndex[rule[i][1]], spaceFlag);
}
}
return ret;
}
/*��ʼ�� firstSet*/
void initfirstSet(){
//�����е��ս���� FIRST ������ʼ��Ϊ����
for(int i=1; i<=getCharIndex['@']; i++){
firstSet[i] += ('0' + i);
}
//ɨ�����еĲ���ʽ, ������ս���� FIRST ��
for(int i=0; i<rule.size(); i++){
int spaceFlag = 0;
int cnt = 0;
do{
spaceFlag = 0;
cnt++;
if(cnt == rule[i].size()){ //�Ŵ���firstSet[getCharIndex[rule[i][0]]] += ('0' + getCharIndex['@']);
break;
}
mergeStr(firstSet[getCharIndex[rule[i][0]]], getfirstSet(getCharIndex[rule[i][cnt]], spaceFlag));
}while(spaceFlag);
}
//��ӡ�����ַ��� FIRST ��
for(int i=1; i<strlen(charArray); i++){
cout << "FIRST(" << charArray[i] << ") = {";
for(int j=0; j<firstSet[i].size(); j++){
int index = firstSet[i][j] - '0';
if(j != firstSet[i].size()-1){
cout << charArray[index] << ", ";
}
else{
cout << charArray[index];
}
}
cout << "}" << endl;
}
}
/*��ʼ�� followSet*/
void initfollowSet(){
//Ĭ�ϵ�һ���ķ�����(����ʽ)���󲿵�δ�ս��Ϊ��ʼ��
mergeStr(followSet[getCharIndex[rule[0][0]]], "0"); //��ʼ����ӡ�#��
//ɨ�����е��ķ�����(����ʽ), �Դ�Ϊ������ʼ�� followSet
for(int i=0; i<rule.size(); i++){
for(int j=1; j<rule[i].size(); j++){ //�����ķ�����(����ʽ)�Ҳ��������ַ�
if(getCharIndex[rule[i][j]] <= getCharIndex['@']){ //���ַ�Ϊ�ս��, ������
(�ս���� FOLLOW ��Ϊ��)
continue;
}
int cnt = j;
int flag;
do{
flag = 0;
cnt++;
if(cnt == rule[i].size()){// �� �� �� �� �� �� Ϊ �� �� ʽ �� �� �� �� һ �� �� ��
(U->xA)
mergeStr(followSet[getCharIndex[rule[i][j]]],followSet[getCharIndex[rule[i][0]]]);
break; //�������һ���ַ��˻�����������ɶ�أ�
}
//���÷��ս���������һ���ַ�����Ϊ(U->xAy)
mergeStr(followSet[getCharIndex[rule[i][j]]], getfirstSet(getCharIndex[rule[i][cnt]], flag));
}while(flag); //ֱ�� getfirstSet ����ִ�н���, �������ս��������ַ�����
firstSet ���� followSet ��
}
}
//��ӡ���з��ս���� FOLLOW ��
for(int i=getCharIndex['@']+1; i<strlen(charArray); i++){
cout << "FOLLOW(" << charArray[i] << ") = {";
for(int j=0; j<followSet[i].size(); j++){
int index = followSet[i][j] - '0';
if(j != followSet[i].size()-1){
cout << charArray[index] << ", ";
}
else{
cout << charArray[index];
}
}
cout << "}" << endl;
}
}
/*��ʼ��������*/
void initAnalyticalTable(){
for(int i=0; i<100; i++){
for(int j=0; j<100; j++){
analyticalTable[i][j] = {-1};
}
}
for(int i=0; i<rule.size(); i++){
//���ķ�����(����ʽ)�Ҳ�Ϊ��(@)
if(rule[i][1] == '@'){
string sel = followSet[getCharIndex[rule[i][0]]]; //select ����
for(int j=0; j<sel.size(); j++){
analyticalTable[getCharIndex[rule[i][0]]][sel[j]-'0'] = i; //i��ָ��i������
ʽ
}
}// �� �� �� �� �� ( �� �� ʽ ) �� �� �� �� Ϊ ��
======================================================================
=======
//��ȡ����ʽ�Ҳ��� FIRST ��
string sel;
int flag=0, cnt=1;
while(!flag)
{
flag = 1;
if(cnt == rule[i].size()){
break;
}
sel += firstSet[getCharIndex[rule[i][cnt]]];
for(int j=0; j<firstSet[getCharIndex[rule[i][cnt]]].size(); j++){
if(firstSet[getCharIndex[rule[i][cnt]]][j]-'0' == getCharIndex['@']){
flag = 0;
}
}
++cnt;
}
for(int j=0; j<sel.size(); j++){//�ж��Ƿ��п�
if(sel[j]-'0' != getCharIndex['@']){
analyticalTable[getCharIndex[rule[i][0]]][sel[j]-'0'] = i;
}
//����ʽ�Ҳ��� FIRST �����һ��Ԫ����@ʱ, ��֤���Ҳ��ܹ�==>�š�Ϊʲô
��ô�жϣ�ֱ��������
if(j==sel.size()-1 && sel[j]-'0' == getCharIndex['@']){
sel += followSet[getCharIndex[rule[i][0]]];
}
}
}
}
/*��ӡ������*/
void print_AnalyticalTable(){
cout << "������:\n\t|";
for(int i=0; i<getCharIndex['@']; i++){
cout << charArray[i] << "\t|";
}
cout << endl;
for(int i=getCharIndex['@']+1; i<strlen(charArray); i++){
cout << charArray[i] << "\t|";
for(int j=0; j<getCharIndex['@']; j++){if(analyticalTable[i][j] != -1){
cout << rule[analyticalTable[i][j]] << "\t|";
}
else{
cout << "\t|";
}
}
cout << endl;
}
cout << endl;
}
string getAlyStackElement(){
// ����һ����ʱ stack ���洢����Ԫ��
stack<char> temp;
string ret = "";
while (!analyticalStack.empty()) {
ret += analyticalStack.top();
temp.push(analyticalStack.top());
analyticalStack.pop();
}
// �������Ԫ�أ��������� push ��ԭ���� stack ��
while (!temp.empty()) {
analyticalStack.push(temp.top());
temp.pop();
}
reverse(ret.begin(), ret.end());
return ret;
}
string getRemainQueueElement(){
// �������Ԫ�أ������������� push ��ԭ���� queue ��
string ret = "";
int size = remainSymbQueue.size();
for (int i = 0; i < size; i++) {
ret += remainSymbQueue.front();
remainSymbQueue.push(remainSymbQueue.front());
remainSymbQueue.pop();
}
return ret;
}
string getProduction(int index){if(index < 0){
return "";
}
string ret;
ret += rule[index][0];
ret += "->";
for(int i=1; i<rule[index].size(); i++){
ret += rule[index][i];
}
return ret;
}
bool analyseProcess(){
cout << "����ջ\t" << "�������Ŵ�\t" << "����ʽ\t" << "��һ��������Ϣ" << endl;
bool res = true;
//����ջ��ʼ��
analyticalStack.push('#');
analyticalStack.push(rule[0][0]); //�����ջ��ѹ���̬
while(!analyticalStack.empty() && res){
string analyseStr; //����ջ�ַ���
string remainsStr; //�����ַ���
string production; //����ʽ�������ӡ����
string message = ""; //��Ϣ�ַ���
//��ȡ����ջջ��Ԫ��
char top_analyse = analyticalStack.top();
analyticalStack.pop();
//��ȡ�����ַ������ж���Ԫ��
char top_remain = remainSymbQueue.front(); //����Ҫ���������� LL(1)�ķ��ķ�
�����̣�
// �������ջջ��Ԫ����'#' if(top_analyse == '#'){
//����ʱ�����ַ�������Ϊ 1,ʣ��һ��'#'ʱ���ɹ� success
analyseStr = "#";
remainsStr = getRemainQueueElement();
production = "";
if(remainSymbQueue.size() == 1){
message = "�ɹ���";
res = true;
}else{
message = "�ķ��������, ������׸��Ԫ��, ����ʧ�ܣ�";
res = false;
}
}
//������ջջ���ַ��������ַ��������ַ���ͬ���򵯳������ַ����Ķ����ַ�
else if(top_analyse == top_remain){
analyseStr = getAlyStackElement() + top_analyse;
remainsStr = getRemainQueueElement();
remainSymbQueue.pop();
production = "";
message += "����";
message += top_analyse;
}
//�������վ��ջ��Ԫ��Ϊ���ս��, Ѱ����Ӧ�Ĳ���ʽ��������ʽ�Ҳ�ѹ�����
ջ��
else if(getCharIndex[top_analyse]>getCharIndex['@'] &&
getCharIndex[top_analyse]<strlen(charArray)){
analyseStr = getAlyStackElement() + top_analyse;
remainsStr = getRemainQueueElement();
//���������д��ڶ�Ӧ�Ĳ���ʽ
if(analyticalTable[getCharIndex[top_analyse]][getCharIndex[top_remain]] != -1){
production =
getProduction(analyticalTable[getCharIndex[top_analyse]][getCharIndex[top_remain]]);
message += "����";
message += top_analyse;
message += ", ";
//����ѹջ
for(int i=production.size()-1; i>2; i--){
if(production[i] != '@'){
analyticalStack.push(production[i]);
}
message += production[i];
}
message += "��ջ";
}
//�������д��ڶ�Ӧ�Ĳ���ʽ, ��ȡ����ʽ������Ӧ�����ɷ���ѹ�����ջ��
else{
production = "";
message = "!!!�﷨��������, ��������Ӧ����ʽ";
res = false;
}
}
//�������ջջ��Ԫ��Ϊ�ս��, �����������Ŵ����ж��׵��ַ���ͬ, �﷨��������
else if(getCharIndex[top_analyse]<=getCharIndex['@'] &&
getCharIndex[top_analyse]>0){
analyseStr = getAlyStackElement() + top_analyse;
remainsStr = getRemainQueueElement() + top_remain;
production = "";
message = "!!!�﷨��������";
res = false;
}
else{
analyseStr = getAlyStackElement() + top_analyse;
remainsStr = getRemainQueueElement() + top_remain;
production = "";
message = "δ֪���󣡣�����";
res = false;
}
cout << analyseStr << "\t" << remainsStr << "\t\t" << production << "\t" << message <<
endl;
}
return res;
}
int main(int argc, char** argv){
input();
initfirstSet();
initfollowSet();
initfollowSet();
initAnalyticalTable();
print_AnalyticalTable();
string str;
str = getAnalyString();
// cout << "��������Ҫ�������ַ���: ";
// cin >> str;
for(int i=0; i<str.size(); i++){
remainSymbQueue.push(str[i]);
}
remainSymbQueue.push('#');
bool res = analyseProcess();
if(res){
cout << "Congratulations! Acceptting the string success!!" << endl;
}
else{
cout << "Failure! Acceptting the string error!!" << endl;}return 0; }
