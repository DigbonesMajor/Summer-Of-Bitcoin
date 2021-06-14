# Summer-Of-Bitcoin

This is my solution for the problem statement given as part of Summer Of Bitcoin Challenge.

## Table of Contents:
* Code Overview
* Important Algorithm and Components
* Result Analysis

### 1. Code Overview

*The working of the code can be explained in the following 7 steps:*
1. Store all the transactions in a vector or array and map the txt_id of all the transactions to their corresponding indexes for easy access in future.
2. Calculate weight_cost and fee_cost for each block respectively and then Calculate their efficiency.
* weight_cost= The amount of weight required to accept a transaction in block, It is the sum of weights of all the ancestors of a transaction and the weight of transaction itself.
* fee_cost= The fee earned after we accept a transaction in block, It is the sum of fee of all the ancestors of a transaction and the fee of the transaction itself.
* efficiency= This is the ratio of fee_cost to weight_cost, the higher the efficiency, more favorable is the transaction. This will help us choose the transactions to include in block.
3. Store all the offspring's of all transactions in a vector or array. offspring's of a transaction are all other transaction in it's family tree which lie below it.
4. Iterate through all the transactions and find the most efficient transaction such that sum of it's weight cost with the weights of transaction accepted does not exceed weight limit.
5. Accept the most efficient transaction after using Depth First Approach to include all ancestors first. Mark all the transactions which have been accepted.
6. The impact of transactions approved in Step 5 on their offspring's (weight_cost, fee_cost,efficiency) need to be removed, as they no longer impact the remaining transaction. Iterate through their offspring's and subtract weight and fee of accepted transactions.
7. Repeat Step 5 and 6 until there is no space or there is no valid transaction to accept.

### 2. Important Algorithms and Components
1. #####DFS and BFS#####: Since, transactions may have parents, they can be represented by Directed Acyclic Graphs. Depth First Search and Breadth First Search are 2 most common graph Algorithms used in the code.
2. #####unordered_map and unordered_set#####: unordered_map and unordered_set can help significantly reduce the time complexity of the code by reducing querying time from the order O(n) to O(1).
3. #####OOPS(Object Oriented Programming)#####: Object Oriented Programming or OOPS helps in improving the structure, and readability of the code.

###3. Result Analysis
1. The block file which contains the result of the code stores 3362 transactions.
2. The total weight of the block is 3999936
3. The total fee of the block is 5577415
