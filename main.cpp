#include <bits/stdc++.h>
using namespace std;

class Bank {
public:
    string name;
    int netAmount;
    set<string> types;
}; 

int getMinIndex(Bank listOfNetAmounts[], int numBanks) {
    int minAmount = INT_MAX, minIndex = -1;
    for(int i = 0; i < numBanks; i++) {
        if(listOfNetAmounts[i].netAmount == 0) continue;
        
        if(listOfNetAmounts[i].netAmount < minAmount) {
            minIndex = i;
            minAmount = listOfNetAmounts[i].netAmount;
        }
    }
    return minIndex;
}

int getSimpleMaxIndex(Bank listOfNetAmounts[], int numBanks) {
    int maxAmount = INT_MIN, maxIndex = -1;
    for(int i = 0; i < numBanks; i++) {
        if(listOfNetAmounts[i].netAmount == 0) continue;
        
        if(listOfNetAmounts[i].netAmount > maxAmount) {
            maxIndex = i;
            maxAmount = listOfNetAmounts[i].netAmount;
        }
    }
    return maxIndex;
}

pair<int, string> getMaxIndex(Bank listOfNetAmounts[], int numBanks, int minIndex, Bank input[], int maxNumTypes) {
    int maxAmount = INT_MIN;
    int maxIndex = -1;
    string matchingType;
    
    for(int i = 0; i < numBanks; i++) {
        if(listOfNetAmounts[i].netAmount == 0 || listOfNetAmounts[i].netAmount < 0) continue;
        
        vector<string> intersection(maxNumTypes);
        auto it = set_intersection(listOfNetAmounts[minIndex].types.begin(), listOfNetAmounts[minIndex].types.end(),
                                   listOfNetAmounts[i].types.begin(), listOfNetAmounts[i].types.end(),
                                   intersection.begin());
        
        if((it - intersection.begin()) != 0 && maxAmount < listOfNetAmounts[i].netAmount) {
            maxAmount = listOfNetAmounts[i].netAmount;
            maxIndex = i;
            matchingType = *intersection.begin();
        } 
    }
    
    return make_pair(maxIndex, matchingType);
}

void printAns(vector<vector<pair<int, string>>>& ansGraph, int numBanks, Bank input[]) {
    cout << "\nThe transactions for minimum cash flow are as follows:\n\n";
    
    for(int i = 0; i < numBanks; i++) {
        for(int j = 0; j < numBanks; j++) {
            if(i == j) continue;

            if(ansGraph[i][j].first != 0) {
                cout << input[i].name << " pays Rs " << ansGraph[i][j].first << " to " 
                     << input[j].name << " via " << ansGraph[i][j].second << endl;
                ansGraph[i][j].first = 0; // Reset after printing
            }
        }
    }
    cout << "\n";
}

void minimizeCashFlow(int numBanks, Bank input[], unordered_map<string, int>& indexOf, int numTransactions, vector<vector<int>>& graph, int maxNumTypes) {
    
    // Calculate net amount for each bank
    Bank listOfNetAmounts[numBanks];
    
    for(int b = 0; b < numBanks; b++) {
        listOfNetAmounts[b].name = input[b].name;
        listOfNetAmounts[b].types = input[b].types;
        
        int amount = 0;
        
        // Calculate incoming edges (column traversal)
        for(int i = 0; i < numBanks; i++) {
            amount += graph[i][b];
        }
        
        // Calculate outgoing edges (row traversal)
        for(int j = 0; j < numBanks; j++) {
            amount -= graph[b][j];
        }
        
        listOfNetAmounts[b].netAmount = amount;
    }
    
    vector<vector<pair<int, string>>> ansGraph(numBanks, vector<pair<int, string>>(numBanks, {0, ""})); // Adjacency matrix
    
    int numZeroNetAmounts = 0;
    
    for(int i = 0; i < numBanks; i++) {
        if(listOfNetAmounts[i].netAmount == 0) numZeroNetAmounts++;
    }
    
    while(numZeroNetAmounts != numBanks) {
        int minIndex = getMinIndex(listOfNetAmounts, numBanks);
        pair<int, string> maxAns = getMaxIndex(listOfNetAmounts, numBanks, minIndex, input, maxNumTypes);
        
        int maxIndex = maxAns.first;
        
        if(maxIndex == -1) {
            // Handle case where no max index found
            ansGraph[minIndex][0].first += abs(listOfNetAmounts[minIndex].netAmount);
            ansGraph[minIndex][0].second = *(input[minIndex].types.begin());
            
            int simpleMaxIndex = getSimpleMaxIndex(listOfNetAmounts, numBanks);
            ansGraph[0][simpleMaxIndex].first += abs(listOfNetAmounts[minIndex].netAmount);
            ansGraph[0][simpleMaxIndex].second = *(input[simpleMaxIndex].types.begin());
            
            listOfNetAmounts[simpleMaxIndex].netAmount += listOfNetAmounts[minIndex].netAmount;
            listOfNetAmounts[minIndex].netAmount = 0;
            
            if(listOfNetAmounts[minIndex].netAmount == 0) numZeroNetAmounts++;
            if(listOfNetAmounts[simpleMaxIndex].netAmount == 0) numZeroNetAmounts++;
        } else {
            int transactionAmount = min(abs(listOfNetAmounts[minIndex].netAmount), listOfNetAmounts[maxIndex].netAmount);
            
            ansGraph[minIndex][maxIndex].first += transactionAmount;
            ansGraph[minIndex][maxIndex].second = maxAns.second;
            
            listOfNetAmounts[minIndex].netAmount += transactionAmount;
            listOfNetAmounts[maxIndex].netAmount -= transactionAmount;
            
            if(listOfNetAmounts[minIndex].netAmount == 0) numZeroNetAmounts++;
            if(listOfNetAmounts[maxIndex].netAmount == 0) numZeroNetAmounts++;
        }
    }
    
    printAns(ansGraph, numBanks, input);
}

int main() 
{ 
    cout << "\n\t\t\t\t********************* Welcome to CASH FLOW MINIMIZER SYSTEM ***********************\n\n\n";
    cout << "This system minimizes the number of transactions among multiple banks using different payment modes. "
         << "There is one world bank (with all payment modes) to act as an intermediary between banks without common payment modes.\n\n";
    cout << "Enter the number of banks participating in the transactions:\n";
    
    int numBanks; 
    cin >> numBanks;
    
    Bank input[numBanks];
    unordered_map<string, int> indexOf; // Stores index of a bank
    
    cout << "Enter the details of the banks and transactions:\n";
    cout << "Bank name, number of payment modes it has, and the payment modes (no spaces):\n";
    
    int maxNumTypes;
    for(int i = 0; i < numBanks; i++) {
        if(i == 0) {
            cout << "World Bank: ";
        } else {
            cout << "Bank " << i << ": ";
        }
        cin >> input[i].name;
        indexOf[input[i].name] = i;
        
        int numTypes;
        cin >> numTypes;
        
        if(i == 0) maxNumTypes = numTypes;
        
        string type;
        while(numTypes--) {
            cin >> type;
            input[i].types.insert(type);
        }   
    }
    
    cout << "Enter the number of transactions:\n";
    int numTransactions;
    cin >> numTransactions;
    
    vector<vector<int>> graph(numBanks, vector<int>(numBanks, 0)); // Adjacency matrix
    
    cout << "Enter the details of each transaction (Debtor Bank, Creditor Bank, and Amount):\n";
    for(int i = 0; i < numTransactions; i++) {
        cout << (i) << " th transaction: ";
        string debtor, creditor;
        int amount;
        cin >> debtor >> creditor >> amount;
        
        graph[indexOf[debtor]][indexOf[creditor]] = amount;
    }
     
    // Settle the transactions
    minimizeCashFlow(numBanks, input, indexOf, numTransactions, graph, maxNumTypes);
    
    return 0; 
}
