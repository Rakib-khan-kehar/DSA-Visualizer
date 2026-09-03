
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <thread>
#include <vector>

using namespace std;
using Clock = chrono::high_resolution_clock;

class AlgorithmLab {
private:
    vector<int> data;
    long long comparisons = 0;
    long long swaps = 0;
    int delayMs = 180;
    bool stepMode = false;

    void pause(const string& message) {
        cout << message << "\n";
        if (stepMode) {
            cout << "Press ENTER for next step...";
            cin.get();
        } else {
            this_thread::sleep_for(chrono::milliseconds(delayMs));
        }
    }

    int inputInt(const string& prompt, int low, int high) {
        int x;
        while (true) {
            cout << prompt;
            if (cin >> x && x >= low && x <= high) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return x;
            }
            cout << "Invalid input. Enter a value from " << low << " to " << high << ".\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    void display(int a = -1, int b = -1, int found = -1) const {
        cout << "\nValues: ";
        for (int i = 0; i < (int)data.size(); ++i) {
            if (i == found) cout << "{" << data[i] << "} ";
            else if (i == a || i == b) cout << "[" << data[i] << "] ";
            else cout << data[i] << " ";
        }
        cout << "\n\n";
        for (int i = 0; i < (int)data.size(); ++i) {
            cout << setw(3) << data[i] << " | ";
            for (int j = 0; j < min(data[i], 40); ++j) cout << '#';
            cout << '\n';
        }
        cout << "Comparisons: " << comparisons << " | Swaps: " << swaps << "\n";
    }

    void finish(const string& name, long long micros, const string& complexity) {
        cout << "\n========== " << name << " COMPLETE ==========\n";
        display();
        cout << "Execution time : " << micros << " microseconds\n";
        cout << "Time complexity: " << complexity << "\n";
    }

    void bubbleSort() {
        auto start = Clock::now();
        for (int pass = 0; pass < (int)data.size() - 1; ++pass) {
            bool changed = false;
            for (int j = 0; j < (int)data.size() - pass - 1; ++j) {
                ++comparisons;
                display(j, j + 1);
                if (data[j] > data[j + 1]) {
                    swap(data[j], data[j + 1]);
                    ++swaps;
                    changed = true;
                    pause("Swap performed.");
                } else pause("Comparison: already in correct order.");
            }
            cout << "Pass " << pass + 1 << " completed.\n";
            if (!changed) break;
        }
        auto end = Clock::now();
        finish("Bubble Sort", chrono::duration_cast<chrono::microseconds>(end-start).count(),
               "Best O(n), Average/Worst O(n^2)");
    }

    void selectionSort() {
        auto start = Clock::now();
        for (int i = 0; i < (int)data.size() - 1; ++i) {
            int minIndex = i;
            for (int j = i + 1; j < (int)data.size(); ++j) {
                ++comparisons;
                display(minIndex, j);
                pause("Comparing candidate minimum.");
                if (data[j] < data[minIndex]) minIndex = j;
            }
            if (minIndex != i) {
                swap(data[i], data[minIndex]);
                ++swaps;
                display(i, minIndex);
                pause("Minimum moved to sorted portion.");
            }
        }
        auto end = Clock::now();
        finish("Selection Sort", chrono::duration_cast<chrono::microseconds>(end-start).count(),
               "Best/Average/Worst O(n^2)");
    }

    void insertionSort() {
        auto start = Clock::now();
        for (int i = 1; i < (int)data.size(); ++i) {
            int j = i;
            while (j > 0) {
                ++comparisons;
                display(j - 1, j);
                if (data[j - 1] > data[j]) {
                    swap(data[j - 1], data[j]);
                    ++swaps;
                    pause("Adjacent elements swapped.");
                    --j;
                } else {
                    pause("Correct insertion position reached.");
                    break;
                }
            }
        }
        auto end = Clock::now();
        finish("Insertion Sort", chrono::duration_cast<chrono::microseconds>(end-start).count(),
               "Best O(n), Average/Worst O(n^2)");
    }

    void mergePart(int l, int m, int r) {
        vector<int> left(data.begin()+l, data.begin()+m+1);
        vector<int> right(data.begin()+m+1, data.begin()+r+1);
        int i=0,j=0,k=l;
        while(i<(int)left.size() && j<(int)right.size()) {
            ++comparisons;
            if(left[i] <= right[j]) data[k++] = left[i++];
            else data[k++] = right[j++];
            display(k-1);
            pause("Merged one element into the working range.");
        }
        while(i<(int)left.size()) data[k++] = left[i++];
        while(j<(int)right.size()) data[k++] = right[j++];
        display();
        pause("Merge completed.");
    }

    void mergeRec(int l, int r) {
        if(l>=r) return;
        int m=l+(r-l)/2;
        mergeRec(l,m);
        mergeRec(m+1,r);
        mergePart(l,m,r);
    }

    void mergeSort() {
        auto start=Clock::now();
        if(!data.empty()) mergeRec(0,(int)data.size()-1);
        auto end=Clock::now();
        finish("Merge Sort", chrono::duration_cast<chrono::microseconds>(end-start).count(),
               "Best/Average/Worst O(n log n)");
    }

    int partitionArray(int l,int r) {
        int pivot=data[r], i=l-1;
        cout << "Pivot = " << pivot << "\n";
        for(int j=l;j<r;++j) {
            ++comparisons;
            display(j,r);
            pause("Comparing with pivot.");
            if(data[j] <= pivot) {
                ++i;
                if(i!=j) {
                    swap(data[i],data[j]);
                    ++swaps;
                    display(i,j);
                    pause("Element moved to pivot's left.");
                }
            }
        }
        if(i+1!=r) {
            swap(data[i+1],data[r]);
            ++swaps;
        }
        display(i+1,r);
        pause("Pivot placed at its final position.");
        return i+1;
    }

    void quickRec(int l,int r) {
        if(l<r) {
            int p=partitionArray(l,r);
            quickRec(l,p-1);
            quickRec(p+1,r);
        }
    }

    void quickSort() {
        auto start=Clock::now();
        if(!data.empty()) quickRec(0,(int)data.size()-1);
        auto end=Clock::now();
        finish("Quick Sort", chrono::duration_cast<chrono::microseconds>(end-start).count(),
               "Average O(n log n), Worst O(n^2)");
    }

    void linearSearch() {
        int target=inputInt("Search value (1-40): ",1,40);
        comparisons=swaps=0;
        auto start=Clock::now();
        for(int i=0;i<(int)data.size();++i) {
            ++comparisons;
            display(i);
            pause("Checking current element.");
            if(data[i]==target) {
                auto end=Clock::now();
                cout << "\nFOUND " << target << " at index " << i << ".\n";
                cout << "Comparisons: " << comparisons << "\n";
                cout << "Execution: " << chrono::duration_cast<chrono::microseconds>(end-start).count()
                     << " microseconds\nComplexity: O(n)\n";
                return;
            }
        }
        auto end=Clock::now();
        cout << "\n" << target << " was not found.\nComparisons: " << comparisons
             << "\nExecution: " << chrono::duration_cast<chrono::microseconds>(end-start).count()
             << " microseconds\nComplexity: O(n)\n";
    }

    void binarySearch() {
        if(data.empty()) return;
        int target=inputInt("Search value (1-40): ",1,40);
        sort(data.begin(),data.end());
        cout << "\nBinary Search uses a sorted working array:\n";
        display();
        comparisons=swaps=0;
        int l=0,r=(int)data.size()-1;
        auto start=Clock::now();
        while(l<=r) {
            int mid=l+(r-l)/2;
            ++comparisons;
            display(mid);
            pause("Checking middle element.");
            if(data[mid]==target) {
                auto end=Clock::now();
                cout << "\nFOUND " << target << " at index " << mid << ".\n";
                cout << "Comparisons: " << comparisons << "\nExecution: "
                     << chrono::duration_cast<chrono::microseconds>(end-start).count()
                     << " microseconds\nComplexity: O(log n)\n";
                return;
            }
            if(data[mid]<target) l=mid+1; else r=mid-1;
        }
        auto end=Clock::now();
        cout << "\n" << target << " was not found.\nComparisons: " << comparisons
             << "\nExecution: " << chrono::duration_cast<chrono::microseconds>(end-start).count()
             << " microseconds\nComplexity: O(log n)\n";
    }

    void createArray() {
        int n=inputInt("Number of elements (2-20): ",2,20);
        data.clear();
        for(int i=0;i<n;++i)
            data.push_back(inputInt("Element "+to_string(i+1)+" (1-40): ",1,40));
        cout << "Array created.\n"; display();
    }

    void randomArray() {
        int n=inputInt("Number of elements (2-20): ",2,20);
        mt19937 gen(random_device{}());
        uniform_int_distribution<int> dist(1,40);
        data.resize(n);
        for(int& x:data) x=dist(gen);
        cout << "Random array generated.\n"; display();
    }

    void sortingMenu() {
        if(data.empty()){cout<<"Create an array first.\n";return;}
        cout<<"1. Bubble Sort\n2. Selection Sort\n3. Insertion Sort\n4. Merge Sort\n5. Quick Sort\n";
        int c=inputInt("Choose: ",1,5); comparisons=swaps=0;
        if(c==1)bubbleSort(); else if(c==2)selectionSort(); else if(c==3)insertionSort();
        else if(c==4)mergeSort(); else quickSort();
    }

public:
    void run() {
        while(true) {
            cout<<"\n=================================================\n";
            cout<<"           ALGORITHM LAB - DSA VISUALIZER\n";
            cout<<"=================================================\n";
            cout<<"1. Enter Custom Array\n2. Generate Random Array\n3. Sorting Lab\n";
            cout<<"4. Searching Lab\n5. Visualization Settings\n6. Show Current Array\n7. Exit\n";
            int c=inputInt("Choose: ",1,7);
            if(c==1)createArray();
            else if(c==2)randomArray();
            else if(c==3)sortingMenu();
            else if(c==4) {
                if(data.empty()){cout<<"Create an array first.\n";continue;}
                cout<<"1. Linear Search\n2. Binary Search\n";
                int s=inputInt("Choose: ",1,2);
                if(s==1)linearSearch(); else binarySearch();
            } else if(c==5) {
                cout<<"1. Automatic animation\n2. Step-by-step mode\n";
                int m=inputInt("Choose: ",1,2);
                stepMode=(m==2);
                if(!stepMode)delayMs=inputInt("Delay milliseconds (50-2000): ",50,2000);
            } else if(c==6) { if(data.empty())cout<<"No array.\n";else display(); }
            else {cout<<"Thank you for using Algorithm Lab!\n";break;}
            cout<<"\nPress ENTER to continue...";cin.get();
        }
    }
};

int main() {
    AlgorithmLab app;
    app.run();
    return 0;
}

# Algorithm Lab — DSA Visualizer (Minor Project)

## Project Description
Algorithm Lab is a console-based C++ DSA Visualizer created to demonstrate fundamental sorting and searching algorithms through an interactive, step-by-step simulation.

## Algorithms Implemented
### Sorting
1. Bubble Sort
2. Selection Sort
3. Insertion Sort
4. Merge Sort
5. Quick Sort

### Searching
1. Linear Search
2. Binary Search

## Functional Features
- Custom array input
- Random array generation
- Algorithm selection menu
- Step-by-step execution
- Comparisons display
- Swaps display
- Current pass/working state
- Execution time
- Time complexity
- Console bar visualization
- Automatic or manual step mode
- Input validation

## Technologies
- C++17
- STL (`vector`, `algorithm`, etc.)
- OOP (class and encapsulation)
- Recursion for Merge Sort and Quick Sort

## How to Run
### Windows / VS Code
Open this folder in VS Code and run:
```bash
g++ -std=c++17 main.cpp -o algorithm_lab.exe
.\algorithm_lab.exe
```

### Linux/macOS
```bash
g++ -std=c++17 main.cpp -o algorithm_lab
./algorithm_lab
```

## Suggested Submission Screenshots
- Main menu
- Custom/random array
- Bubble Sort steps
- Merge Sort steps
- Quick Sort pivot visualization
- Linear Search result
- Binary Search result
- Complexity and execution statistics

## Project Structure
```text
Minor_Project_Algorithm_Lab/
├── main.cpp
└── README.md
```
