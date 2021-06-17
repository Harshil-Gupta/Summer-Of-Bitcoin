#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<iterator>
#include<algorithm>
#include<unordered_map>
using namespace std;

unordered_map<string,int> mp; // Global map to check if block id is present or not

class Block{
    public:
    string tx_id,parentString;
    int weight,fee;
    vector<string> parents;
    int no_of_parents;
    bool visited;

    Block() {
        this->visited = false;
    }

    Block(string tx_id, string feeString, string weightString, string parentString){ // Constructor to initialise values to Block
        this->visited = true;
        this->tx_id = tx_id;
        int weight = stoi(weightString);
        int fee = stoi(feeString);
        this->fee = fee;
        this->weight = weight;
        int len_parents = parentString.length();
        no_of_parents = (len_parents + 1)/65;
        this->no_of_parents = no_of_parents;
        int temp = no_of_parents;
        int k=0;
        while(temp--){
            parents.push_back(parentString.substr(k,k+64));
            k+=65;
        }
    }
};    

int main(){
    ifstream myFileStream("mempool2.csv");
    if(!myFileStream.is_open()){
        cerr << "Sorry! Could not open the file" << endl;
        exit(1);
    }
    vector<Block> allblocks;
    string line;
    while(getline(myFileStream, line)){
        string tx_id, parentString, feeString, weightString;
        
        stringstream ss(line);
        getline(ss, tx_id, ',');
        if(tx_id == "tx_id"){
            continue; // Not including Heading
        }
        getline(ss, feeString, ',');
        getline(ss, weightString, ',');
        getline(ss, parentString, ',');
        mp[tx_id]++;
        Block new_block(tx_id,feeString,weightString,parentString);
        allblocks.push_back(new_block); 
    }
    ofstream outdata; 
    outdata.open("sampleoutput.txt"); 
    if(!outdata) {
      cerr << "Error: file could not be opened" << endl;
      exit(1);
   }
    for(int i=0;i<allblocks.size();i++){
        if(!mp[allblocks[i].tx_id])continue;
        outdata << "ID: " << allblocks[i].tx_id << " Fee: " << allblocks[i].fee << " Weight: " << allblocks[i].weight;
        outdata << " Parents: ";
        
        int temp = allblocks[i].no_of_parents;
        int k=0;
        while(temp--){
            outdata << allblocks[i].parents[k] << " ";
            k++;
        }
        outdata << endl;
    }
    cout << "Operation Performed Successfully\n";
    outdata.close();
    myFileStream.close();

    return 0;
}
