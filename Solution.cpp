// NOTE: No part of this code was taken from any resource available online or offline.
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <stack>

using namespace std;

//A block class is created, all transactions will be instances of this class
class block
{
public:
    string txt_id;
    int fee, weight;
    long long weight_cost, fee_cost;
    vector<string> parents;          //All parents of a transaction will be stored here.
    unordered_set<string> offspring; //Stores txt_id of all the children blocks(Includes indirect children too, like children of children and so on).
    double efficiency = 0;           //This will store the total fee/total weight ratio of including a transaction in the mempool leagally i.e, with all it's parents and so on.
    void add_block(string t);
};

//Global Variables
unordered_map<string, int> give_index;
unordered_map<string, bool> accepted;
int weight_limit = 4000000;
vector<block> block_data; //We store all the blocks(transactions) in this vector

//Function Declarations
void map_txt_id(vector<block> &, int);
void calculate_efficiency(block &, vector<bool>);
void solve(vector<block> &, int);
void print_all(block &, long long &, long long &);
void adjust(block &);
void set_offspring(vector<block> &, int);
void setter(int, unordered_set<string>);

int main()
{
    // STEP 1:- Taking input from file mempool.csv and storing all transactions(class block) in vector block_data.
    string t;
    ifstream in("mempool.csv");

    getline(in, t);
    while (getline(in, t))
    {
        block new_block;
        new_block.add_block(t);
        block_data.push_back(new_block);
        in.clear();
    }

    int total_transactions = block_data.size();

    // STEP 2:- Mapping txt_id of all blocks to corresponding index in vector block_data so we can optimally get the contents of a block if txt_id is known.
    map_txt_id(block_data, total_transactions);

    // STEP 3:- Storing txt_id of all the descendants of all blocks in offspring instance
    set_offspring(block_data, total_transactions);

    /* 
        STEP 4:- We calculate weight_cost and fee_cost and efficiency for all blocks, where,
                    
                weight_cost= Total weight of blocks we will have to incurr if we decide to select this block.
                        Example: If Block A has weight 10 and Block B is a parent of A with weight 5, so weight_cost of A will be 15.
                
                fee_cost= Total fee we get if we decide to select this block.  
                        Example: If Block A has fee 10 and Block B is a parent of A with fee 5, so fee_cost of A will be 15.

                efficiency= fee_cost of a block/weight_cost of a block

    */
    vector<bool> blocks_done(total_transactions, false);
    for (int i = 0; i < total_transactions; i++)
    {
        calculate_efficiency(block_data[i], blocks_done); //Calculates the weight and fee cost of each block
        block_data[i].efficiency = (double)block_data[i].fee_cost / block_data[i].weight_cost;
    }
    // cout<<block_data[0].weight_cost<<endl;
    // cout<<block_data[1].weight_cost<<endl;

    //Step 5:- We call the solve function which will print all the blocks in order maximizing fee.
     solve(block_data, total_transactions);
     system("pause");
    // return 0;
}

//add_block method of Class block iterates through the input string and intitalises the value of instance attributes accordingly, used in Step 1
void block::add_block(string t)
{
    int string_size = t.size();
    string temp_fee, temp_weight, parent_store;
    int hold = 0;
    for (int i = 0; i < string_size; i++)
    {
        if (t[i] == ',')
            hold++;
        else if (hold == 0)
            txt_id.push_back(t[i]);
        else if (hold == 1)
            temp_fee.push_back(t[i]);
        else if (hold == 2)
            temp_weight.push_back(t[i]);
        else if (hold > 2)
        {
            if (t[i] == ';')
            {
                parents.push_back(parent_store);
                parent_store.clear();
            }
            else
                parent_store.push_back(t[i]);
            if (i == string_size - 1)
            {
                parents.push_back(parent_store);
                parent_store.clear();
            }
        }
    }
    fee = stoi(temp_fee);
    fee_cost = fee;
    weight = stoi(temp_weight);
    weight_cost = weight;
}

//map_txt_id maps txt_id of a block to it's index, used in Step 2. Once implemented, mapping helps get_index in O(1) time
void map_txt_id(vector<block> &block_data, int total_transactions)
{
    for (int i = 0; i < total_transactions; i++)
        give_index[block_data[i].txt_id] = i;
}

//set_offspring stores all direct children of a block in the offspring attribute of the block, used in Step 3.
void set_offspring(vector<block> &block_data, int total_transactions)
{
    for (int i = 0; i < total_transactions; i++)
    {
        unordered_set<string> temp;
        temp.insert(block_data[i].txt_id);
        setter(i, temp);
    }
}

//Setter function is called on each index of block_data by set_offspring function. This function recursively finds out all the offsprings of block_data at index i.
void setter(int i, unordered_set<string> temp)
{
    for (auto x : block_data[i].parents)
    {
        block_data[give_index[x]].offspring.insert(temp.begin(), temp.end());
        unordered_set<string> temp2 = temp;
        temp2.insert(x);
        setter(give_index[x], temp2);
    }
}

//calculate_efficiency is called to calculate weight_cost and fee_cost of a block which will be the major factor in deciding the order of blocks to select, used in Step 4.
void calculate_efficiency(block &current, vector<bool> blocks_done)
{
    stack<string> s;
    for (auto x : current.parents)
    {
        s.push(x);
        current.weight_cost += block_data[give_index[x]].weight;
        current.fee_cost += block_data[give_index[x]].fee;
        blocks_done[give_index[x]] = true;
    }
    while (!s.empty())
    {
        string temp = s.top();
        s.pop();
        for (auto y : block_data[give_index[temp]].parents)
        {
            if (blocks_done[give_index[y]])
                continue;
            else
            {
                s.push(y);
                current.weight_cost += block_data[give_index[y]].weight;
                current.fee_cost += block_data[give_index[y]].fee;
                blocks_done[give_index[y]] = true;
            }
        }
    }
}

/*Solve function prints the blocks in order to maximize fee within permissible weight_limit, used in step 5.

    Working of Solve function: 
        
        Step 5.1-> Solve function iterates through the block_data and finds the index of the element with maximum efficiency, which has not been selected yet and is within permissible weight limit.
        Step 5.2-> Solve function then calls print_all function on the found index which uses DFS( Depth First Search) Technique on the block and prints all the ancestors of the block which have not been selected yet and also goes through all their offsprings to adjust their efficiency accordingly.
        Step 5.3-> Repeats Step 5.1 and Step 5.2 until there is no valid block of string left.
*/
void solve(vector<block> &block_data, int total_transactions)
{
    long long total_weight_accepted = 0, total_fee_earned = 0;
    while (1)
    {
        int index = -1;
        double eff = 0;
        for (int i = 0; i < total_transactions; i++)
        {
            if (accepted[block_data[i].txt_id])
                continue;
            if (total_weight_accepted + block_data[i].weight_cost > weight_limit)
                continue;
            else
            {
                if (block_data[i].efficiency > eff)
                {
                    index = i;
                    eff = block_data[i].efficiency;
                }
            }
        }
        if (index == -1)
            break;
        print_all(block_data[index], total_weight_accepted, total_fee_earned);
    }
}

// Used by Solve function to recursively print all the unselected ancestors of the given block in order and then finally print the block's txt_id.
void print_all(block &current, long long &total_weight_accepted, long long &total_fee_earned)
{
    for (auto x : current.parents)
    {
        if (accepted[block_data[give_index[x]].txt_id])
            continue;
        auto y = block_data[give_index[x]];
        print_all(y, total_weight_accepted, total_fee_earned);
    }
    accepted[current.txt_id] = true;
    cout << current.txt_id << endl;
    adjust(current);
    total_weight_accepted += current.weight;
    total_fee_earned += current.fee;
}

//Whenever we select a block ,we need to update weight_cost, fee_cost and efficiency of all it's offsprings accordingly as the selected block is not needed anymore and it has contribution in all it's offsprings attributes.
void adjust(block &current)
{
    for (auto x : current.offspring)
    {
        block_data[give_index[x]].weight_cost -= current.weight;
        block_data[give_index[x]].fee_cost -= current.fee;
        block_data[give_index[x]].efficiency = (double)block_data[give_index[x]].fee_cost / block_data[give_index[x]].weight_cost;
    }
}
