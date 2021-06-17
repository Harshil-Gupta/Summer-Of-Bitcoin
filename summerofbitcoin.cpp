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
    int block_id;
    double weightperfee; // weightperfee = weight of block divided by fee of block

    Block() {
        this->visited = false;
        this->isValid = true;
    }

    Block(string tx_id, string feeString, string weightString, string parentString, int block_id){ 
        // Constructor to initialise values to Block
        this->block_id = block_id;
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
    // Total Valid Blocks in mempool.csv => 5202
    ifstream myFileStream("mempool.csv");
    if(!myFileStream.is_open()){
        cerr << "Sorry! Could not open the input file" << endl;
        exit(1);
    }
    vector<Block> allblocks;
    string line;
    int block_id = 0;
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
        Block new_block(tx_id,feeString,weightString,parentString,block_id);
        allblocks.push_back(new_block); 
        block_id++;
    }
    myFileStream.close();
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
            continue;
        }

        if(allblocks[i].isValid == true){
            no_of_valid_blocks++;
        }
    }
    // Sorting all valid blocks in increasing value of weight per fee
    // This sorts the blocks as per most profitable block with least weight
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

    vector<Block> outputBlocks;

    for(int i=0;i<allblocks.size() && curr_weight<allowed_weight;i++){
        if(allblocks[i].isValid == false){
            continue;
        }
        last_fee = allblocks[i].fee;
        last_weight = allblocks[i].weight;
        curr_weight+=allblocks[i].weight;
        curr_fees += allblocks[i].fee;
        outputBlocks.push_back(allblocks[i]);
    }

    bool removelast = false;
    if(curr_weight>allowed_weight){
        curr_weight-= last_weight;
        curr_fees -=  last_fee;
        removelast = true;
    }

    // Sorting all blocks in order of their appearance in mempool.csv
    sort(allblocks.begin(), allblocks.end(), 
        [] (const Block& block1, const Block& block2)
        {
            return block1.block_id < block2.block_id;
        }
    );

    int n = outputBlocks.size();
    if(removelast == true){
        n = outputBlocks.size()-1; // So that last block will not be included
    }

    ofstream outdata; 
    outdata.open("block.txt"); 
    if(!outdata) {
      cerr << "Sorry! Could not open the output file" << endl;
      exit(1);
    }
    for(int i=0;i<n;i++){
        outdata << allblocks[i].tx_id << endl;
    }
    cout << "Operation Performed Successfully\n";
    cout << "Total weight of valid blocks: "<< curr_weight << "\n";
    cout << "Total fee profit possible: " << curr_fees << "\n";
    //cout << no_of_valid_blocks << " are valid out of 5214\n";
    outdata.close();
    return 0;
}