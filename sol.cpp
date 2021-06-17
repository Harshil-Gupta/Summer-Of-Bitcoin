#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<iterator>
#include<algorithm>
#include<unordered_map>
using namespace std;

// Code Written By: Harshil Gupta
// https://github.com/Harshil-Gupta/Summer-Of-Bitcoin

unordered_map<string,int> mp; // Global map to check if block id is valid or not

class Block{
    public:
    string tx_id,parentString;
    int weight,fee;
    vector<string> parents;
    int no_of_parents;
    bool visited;
    bool isValid;
    double weightperfee;

    Block() {
        this->visited = false;
        this->isValid = true;
    }

    Block(string tx_id, string feeString, string weightString, string parentString){ 
        // Constructor to initialise values to Block
        this->visited = true;
        this->tx_id = tx_id;
        int weight = stoi(weightString);
        int fee = stoi(feeString);
        this->fee = fee;
        this->weight = weight;
        this->weightperfee = (this->weight)*1.0/(this->fee);
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

bool checkParents(string id){
    if(mp[id]>0){
        return true;
    }
    else{
        return false;
    }
}

int main(){
    // Total Blocks in mempool.csv => 5214
    // Total Valid Blocks in mempool.csv => 
    ifstream myFileStream("mempool.csv");
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
        if(parentString.length()>0)
            mp[parentString]++;
        Block new_block(tx_id,feeString,weightString,parentString);
        allblocks.push_back(new_block); 
    }
    ofstream outdata; 
    outdata.open("sampleoutput.txt"); 
    if(!outdata) {
      cerr << "Error: file could not be opened" << endl;
      exit(1);
   }
   int no_of_valid_blocks = 0;
   
    for(int i=0;i<allblocks.size();i++){
        if(!mp[allblocks[i].tx_id]){
            continue;
        }
        bool flag = true;

        allblocks[i].isValid = true;
        allblocks[i].visited = true;

        if(allblocks[i].no_of_parents > 0){
            int num = allblocks[i].no_of_parents;
            int k=0;
            while(k!=num){
                if(!checkParents(allblocks[i].parents[k])){
                    flag = false;
                    break;
                }
                k++;
            }
        }
        
        if(allblocks[i].no_of_parents>0 && flag == false){
            // Not Valid block
            allblocks[i].isValid = false;
            mp[allblocks[i].tx_id] = 0;
            for(int j=0;j<allblocks[i].no_of_parents;j++){
                if(mp[allblocks[i].parents[j]] > 0)
                    mp[allblocks[i].parents[j]]--;
            }
            //allblocks.erase(i);
            continue;
        }

        if(allblocks[i].isValid == true){
        no_of_valid_blocks++;
        /*outdata << "ID" << i+1 << ": " << allblocks[i].tx_id << " Fee: " << allblocks[i].fee << " Weight: " << allblocks[i].weight;
        outdata << " Parents: ";
        
        int temp = allblocks[i].no_of_parents;
        int k=0;
        while(temp--){
            outdata << allblocks[i].parents[k] << " ";
            k++;
        }
        outdata << endl;*/
        }
    }
    sort(allblocks.begin(), allblocks.end(), 
        [] (const Block& block1, const Block& block2)
        {
            return block1.weightperfee < block2.weightperfee;
        }
    );
    int allowed_weight = 4000000;
    int curr_weight = 0;
    int last_weight = 0;
    int last_fee = 0;
    long long curr_fees = 0;
    for(int i=0;i<allblocks.size() && curr_weight<allowed_weight;i++){
        if(allblocks[i].isValid == false){
            continue;
        }
        last_fee = allblocks[i].fee;
        last_weight = allblocks[i].weight;
        curr_weight+=allblocks[i].weight;
        curr_fees += allblocks[i].fee;
        outdata << "ID" << i+1 << ": " << allblocks[i].tx_id << " Fee: " << allblocks[i].fee << " Weight: " << allblocks[i].weight;
        outdata << " Parents: " << allblocks[i].no_of_parents << " ";
        outdata << "Weight per fee: " << allblocks[i].weightperfee << "\n";
    }
    if(curr_weight>allowed_weight){
        curr_weight-= last_weight;
        curr_fees -=  last_fee;
    }
    cout << "Total weight of valid blocks: "<< curr_weight << "\n";
    cout << "Total fee profit possible: " << curr_fees << "\n";
    cout << "Operation Performed Successfully\n";
    cout << no_of_valid_blocks << " are valid out of 5214\n";
    outdata.close();
    myFileStream.close();

    return 0;
}
