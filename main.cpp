#include <bits/stdc++.h>
#include <fstream>
#include "sha256.h"

using namespace std;

class block{
    public:
        unsigned int index;
        double timestamp;
        //Transaction map for hashing, contains sender, reciever and amount
        map<string,string> transactions;
        string previousHash;
        string selfHash;
        string POW; //Proof of work
};

class blockchain{
    public:
        SHA256 sha256;
        vector<block> chain;
        vector<block> toMine;
        
        blockchain(){
            //Genisis block, encryption does not follow conventions
            block init;
            init.transactions["sender"] = "Dhyey";
            init.transactions["reciever"] = "Bank";
            init.transactions["amount"]= "10";
            init.index = 0;
            init.previousHash = "999";
            init.selfHash = sha256("DhyeyBank109990000000000000000000000000");
            chain.push_back(init);

            ofstream blockFile;
            blockFile.open ("Blockchain.txt",std::fstream::app);
            blockFile<<"-----------NEW BLOCKCHAIN CREATED-----------"<<endl;
            blockFile.close();
        }
        
        void createNewBlock(string sender, string reciever, string amount){
            time_t timer;
            time(&timer);

            block current;
            current.transactions["sender"] = sender;
            current.transactions["reciever"] = reciever;
            current.transactions["amount"]= amount;
            current.timestamp = timer;
            toMine.push_back(current);
        }

        string random_string( size_t length ){
            auto randchar = []() -> char
            {
                const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
                const size_t max_index = (sizeof(charset) - 1);
                return charset[ rand() % max_index ];
            };
        string str(length,0);
        generate_n( str.begin(), length, randchar );
        return str;
        }

        bool verifyBlock(block needToVerify, string ProofOfWork){
            if (sha256(needToVerify.transactions["sender"]+needToVerify.transactions["reciever"]+needToVerify.transactions["amount"]+needToVerify.previousHash+to_string(needToVerify.timestamp)+ProofOfWork).substr(0,2)=="00"){
                return true;
            }
            return false;
        }

        void writeToTXT(block toWrite){
            ofstream blockFile;
            blockFile.open ("Blockchain.txt",std::fstream::app);
            blockFile << "Index: "<<toWrite.index<<endl;
            blockFile << "Time: "<<to_string(toWrite.timestamp)<<endl;
            blockFile << "Sender: "<<toWrite.transactions["sender"]<<endl;
            blockFile << "Reciever: "<<toWrite.transactions["reciever"]<<endl;
            blockFile << "Amount: "<<toWrite.transactions["amount"]<<endl;
            blockFile << "Previous Hash: "<<toWrite.previousHash<<endl;
            blockFile << "Self-Hash: "<<toWrite.selfHash<<endl;
            blockFile << "Proof of Work: "<<toWrite.POW<<endl;
            blockFile<<"\n";
            blockFile.close();
        }

        void mine(block needToMine){
            needToMine.index = chain.size();
            needToMine.previousHash = chain[needToMine.index-1].selfHash;
            while (true){
                string random_str = random_string(8);
                cout<<"Trying: "<<random_str<<endl;
                if (verifyBlock(needToMine,random_str)){
                    cout<<"Works! : "<<random_str<<endl;
                    needToMine.POW = random_str;
                    needToMine.selfHash = sha256(needToMine.transactions["sender"]+needToMine.transactions["reciever"]+needToMine.transactions["amount"]+needToMine.previousHash+to_string(needToMine.timestamp)+random_str);
                    chain.push_back(needToMine);
                    toMine.pop_back();
                    writeToTXT(needToMine);
                    break;
                }
            }
        }

        bool verifyBlockchainIntegrity(){
          for(int i=2;i<chain.size();i++){
            if (chain[i].previousHash != sha256(chain[i-1].transactions["sender"]+chain[i-1].transactions["reciever"]+chain[i-1].transactions["amount"]+chain[i-1].previousHash+to_string(chain[i-1].timestamp)+chain[i-1].POW)){
              return false;
            }  
          }
          return true;
        }
};


int main(){
    blockchain myChain;
    //Sample transactions are created and then mined
    myChain.createNewBlock("BankOfCanada","BankOfScotland","10");
    myChain.mine(myChain.toMine[0]);
    myChain.createNewBlock("Paul","Walmart","5");
    myChain.mine(myChain.toMine[0]);
    myChain.createNewBlock("Jerry","Sally","32");
    myChain.mine(myChain.toMine[0]);

    if(myChain.verifyBlockchainIntegrity()){
      cout<<"Blockchain reliable"<<endl;
    }
    else{
      cout<<"Blockchain has been compromised"<<endl;
    }

    //If any transaction is changed

    myChain.chain[1].transactions["amount"] = "999999";

    //The integrity of blockchain is compromised and the function detects it
    if(myChain.verifyBlockchainIntegrity()){
      cout<<"Blockchain reliable"<<endl;
    }
    else{
      cout<<"Blockchain has been compromised"<<endl;
    }   
}