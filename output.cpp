#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <limits>
#include <ctime>
struct Value;
Value mhs_dispatch_method(Value, std::string, std::vector<Value>);
struct Value {
 int type = 0; long long iVal = 0; std::string sVal;
 std::shared_ptr<std::map<std::string, Value>> members;
 std::shared_ptr<std::vector<Value>> arrayVals;
 Value() : type(0) {}
 Value(int i) : type(1), iVal(i) {}
 Value(long long i) : type(1), iVal(i) {}
 Value(std::string s) : type(2), sVal(s) {}
 static Value make_array(std::vector<Value> elems) { Value v; v.type = 4; v.arrayVals = std::make_shared<std::vector<Value>>(elems); return v; }
 static Value make_map(std::map<std::string, Value> elems) { Value v; v.type = 5; v.members = std::make_shared<std::map<std::string, Value>>(elems); return v; }
 int len() const { if(type==4) return arrayVals->size(); if(type==5) return members->size(); return 0; }
 void array_push(Value v) const { if(type==4) arrayVals->push_back(v); }
 Value at(Value idx) const {
 if(type==4) { int i = idx.iVal; if(i < 0 || i >= arrayVals->size()) { std::cerr << "[PANIC] Index out of bounds" << std::endl; exit(1); } return arrayVals->at(i); }
 if(type==5) { std::string k = idx.sVal; if(members->find(k) == members->end()) return Value(); return members->at(k); }
 return Value();
 }
 void set(Value idx, Value val) const { if(type==4) (*arrayVals)[idx.iVal] = val; if(type==5) (*members)[idx.sVal] = val; }
 static Value make_struct(std::string name, std::vector<Value> args);
 Value get_safe(std::string name) const { if (members && members->count(name)) return members->at(name); return Value(); }
 bool is_true() const { return (type==1 && iVal!=0) || (type==2 && !sVal.empty()); }
 friend std::ostream& operator<<(std::ostream& os, const Value& v) {
 if(v.type==0) os << "null";
 else if(v.type==1) os << v.iVal;
 else if(v.type==2) os << v.sVal;
 else if(v.type==4) { os << "["; for(size_t i=0; i<v.arrayVals->size(); i++) { os << (*v.arrayVals)[i]; if(i<v.arrayVals->size()-1) os << ", "; } os << "]"; }
 else if(v.type==5) { os << "{"; int c=0; for(auto const& p : *v.members) { os << p.first << ": " << p.second; if(c++ < v.members->size()-1) os << ", "; } os << "}"; }
 else os << "[Object]";
 return os;
 }
 Value operator+(const Value& o) const { return Value(to_string() + o.to_string()); }
 Value operator*(const Value& o) const { long long res = iVal * o.iVal; if (res > std::numeric_limits<int>::max() || res < std::numeric_limits<int>::min()) { std::cerr << "[PANIC] Overflow" << std::endl; exit(1); } return Value((int)res); }
 Value operator-(const Value& o) const { return Value((int)(iVal - o.iVal)); }
 Value operator>(const Value& o) const { return Value((int)(iVal > o.iVal)); }
 Value operator<(const Value& o) const { return Value((int)(iVal < o.iVal)); }
 Value operator==(const Value& o) const { if(type!=o.type) return Value(0); if(type==1) return Value((int)(iVal==o.iVal)); return Value((int)(sVal==o.sVal)); }
 Value operator&&(const Value& o) const { return Value((int)(iVal && o.iVal)); }
 std::string to_string() const { std::stringstream ss; ss << *this; return ss.str(); }
};
Value mhs_main();
int main() { mhs_main(); return 0; }
Value mhs_main() {
std::cout << Value(std::string("Hello from MHS!")) << std::endl;
const Value var_list = Value::make_array({Value(1), Value(2), Value(3)});;
var_list.array_push(Value(4));
std::cout << (Value(std::string("List length: ")) + Value((int)var_list.len())) << std::endl;
return Value(0);
}

Value mhs_dispatch_method(Value obj, std::string method, std::vector<Value> args) {
std::cerr << "[PANIC] Method not found" << std::endl; exit(1);
return Value();
}
Value Value::make_struct(std::string name, std::vector<Value> args) {
Value v; v.type = 3; v.sVal = name; v.members = std::make_shared<std::map<std::string, Value>>();
return v;
}
