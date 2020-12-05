#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <assert.h>
#include <string.h>
using namespace std;

struct frm
{
    int type;
    int seqnum;
    int acknum;
    string checksum;
    char payload[4];
};

string divisor = "10101";

string divide(string divident, string divisor){
    assert(divident.size() == divisor.size());
    string result;
    for(int i=0;i<divident.size();i++){
        if(divident[i] != divisor[i]){
            result.push_back('1');
        }else result.push_back('0');
    }
    return result;
}

pair<string, string> CRC(string divident, string divisor){
    int window_size = divisor.size();
    
    // append window_size - 1 zeroes
    for(int i=0;i<window_size-1;i++) divident.push_back('0');
    int divident_size = divident.size();
    // find the first 1
    int idx = -1;
    for(int i=0;i<divident_size;i++) {
        if(divident[i] == '1') {
            idx = i;
            break;
        }
    }
    assert(idx != -1);
    string divi_str = divident.substr(idx, window_size);
    for(int i=idx;i<=divident_size - window_size;i++){
        // divi_str = divisor.substr(i, window_size);
        string res = divide(divi_str, divisor);
        res.erase(res.begin());
        if(i + window_size < divident_size)
            divi_str = res + divident[i + window_size];
        else{
            divi_str = res;
        }
    }
    string rem = divi_str;
    for(int i=(int)rem.size() - 1;i>=0;i--){
        divident[i] = rem[i];
    }
    return {divident, rem};
}

string get_bin(int n, int sz){
    string bin;
    bin.resize(sz, '0');
    int idx = 0;
    while(n){
        assert(idx < sz);
        if(n % 2){
            // bin.push_back('1');
            bin[idx++] = '1';
        }else{
            // bin.push_back('0');
            bin[idx++] = '0';
        }
        n /= 2;
    }
    reverse(bin.begin(), bin.end());
    return bin;
}

string calculate_checksum_2(struct frm frame){
    string ack_bin = get_bin(frame.acknum, 32);
    string seq_bin = get_bin(frame.seqnum, 32);
    string type_bin = get_bin(frame.type, 32);
    string payload_bin;
    for(int i=0;frame.payload[i];i++){
        payload_bin += get_bin((int)frame.payload[i], 8);
    }
    // cout << "------------------\n";
    // cout << frame.acknum << " " << frame.seqnum << " " << frame.type << " " << frame.payload << endl;
    // cout << ack_bin << " " << seq_bin << " " << type_bin << " " << payload_bin << endl;
    return CRC(ack_bin + seq_bin + type_bin + payload_bin, divisor).first;
}

bool check_checksum(string checksum){
    string rem = CRC(checksum, divisor).second;
    for(auto ele : rem){
        if(ele != '0') return false;
    }
    return true;
}

int main(){
    // cout << get_bin(31, 32) << endl;
    char ch[] = "aaa"; 
    frm frame;
    frame.seqnum = 2;
    frame.acknum = 0;
    strcpy(frame.payload, ch);
    frame.checksum = calculate_checksum_2(frame);
    cout << frame.checksum << endl;
    cout << check_checksum(frame.checksum) << endl;

    
}