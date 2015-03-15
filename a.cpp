#include <cstdio>
#include <set>
#include <cassert>
#include <string>
#include <vector>
#include <cstring>
using namespace std;

vector<int> getListAtString(char *buf) {
    vector<int> ret;
    for ( char *p = strtok(buf," \t") ; p ; p = strtok(NULL," \t") ){
        int t;
        sscanf(p,"%d",&t);
        ret.push_back(t);
    }
    return ret;
}
void printDataList(vector<vector<int> > data) {
    for ( int i = 0 ; i < (int)data.size() ; i++ ) {
        for ( int j = 0 ; j < (int)data[i].size() ; j++ ) 
            printf("%d ",data[i][j]);
        puts("");
    }
}
bool isExistsSameData(vector<vector<int> > data) {
    bool isExists = false;
    for ( int i = 0 ; i < (int)data.size() ; i++ ) {
        for ( int j = 0 ; j < (int)data[i].size() ; j++ ) 
            for ( int k = 0 ; k < j ; k++ ) 
                if ( data[i][j] == data[i][k] ) 
                    isExists = true;
    }
    return isExists;
}
void printSet(set<int>& s) {
    printf("{ ");
    for ( set<int>::iterator it=s.begin();it!=s.end();it++ ) 
        printf("%d ",*it);
    printf("}\n");
}
void printVectorSet(vector<set<int> >& v) {
    for ( int i = 0 ; i < (int)v.size() ; i++ ) 
        printSet(v[i]);
}
bool isSetInSet(set<int>& s1,set<int> &s2) {
    bool ret = true;
    for ( set<int>::iterator it=s2.begin();it!=s2.end();it++ ) 
        if ( s1.count(*it) == 0 ) ret = false;
    return ret;
}

int main(int argc,char *argv[]) {
    assert(argc==4);

    int minSupport;
    string inputFilename;
    string outputFilename;

    sscanf(argv[1],"%d",&minSupport);
    inputFilename = string(argv[2]);
    outputFilename = string(argv[3]);

    FILE * fin = fopen(inputFilename.c_str(),"r");
    assert(fin!=NULL);

    char buf[1024];
    vector<vector<int> > data;
    while ( fscanf(fin,"%[^\n]\n",buf) != EOF ) {
        vector<int> curList = getListAtString(buf);
        data.push_back(curList);
    }
    assert(!isExistsSameData(data));

    vector<set<int> > transactionDataBase;
    for ( int i = 0 ; i < (int)data.size() ; i++ ) {
        set<int> s;
        for ( int j = 0 ; j < (int)data[i].size() ; j++ ) 
            s.insert(data[i][j]);
        transactionDataBase.push_back(s);
    }
    vector<pair<set<int>,int> > itemSet;
    for ( int i = 0 ; i < (int)transactionDataBase.size() ; i++ ) {
        for ( set<int>::iterator it=transactionDataBase[i].begin();
                it!=transactionDataBase[i].end();it++ ) {
            bool isExists = false;
            for ( int j = 0 ; j < (int)itemSet.size() ; j++ ) 
                if ( itemSet[j].first.count(*it) != 0 ) {
                    itemSet[j].second++;
                    isExists = true;
                    break;
                }
            if ( !isExists ) {
                set<int> s;
                s.insert(*it);
                itemSet.push_back(pair<set<int>,int>(s,1));
            }
        }
    }

    vector<set<int> > eraseSet;
    vector<set<int> > remainSet;
    for ( vector<pair<set<int>,int> >::iterator it=itemSet.begin();
            it!=itemSet.end();it++ ) {
        if ( (*it).second < minSupport )
            eraseSet.push_back((*it).first);
        else remainSet.push_back((*it).first);
    }
    for ( vector<pair<set<int>,int> >::iterator it=itemSet.begin();
            it!=itemSet.end();it++ ) {
        printSet((*it).first);
        printf("%d\n",(*it).second);
    }

    puts("remainSet");
    printVectorSet(remainSet);
    puts("eraseSet");
    printVectorSet(eraseSet);

    bool change = true;
    while ( change ) {
        change = false;
        if ( remainSet.empty() ) {
            printf("empty set\n");
            break;
        }
        int unionSize = (int)remainSet[0].size()-1;
        vector<set<int> > nextSet;
        for ( int i = 0 ; i < (int)remainSet.size() ; i++ ) 
            for ( int j = 0 ; j < (int)remainSet.size() ; j++ ) {
                if ( i == j ) continue;
                int unionCnt = 0;
                for ( set<int>::iterator it=remainSet[j].begin();
                        it!=remainSet[j].end();it++ ) 
                    if ( remainSet[i].count(*it) != 0 ) 
                        unionCnt ++;
                set<int> next = remainSet[i];
                for ( set<int>::iterator it=remainSet[j].begin();
                        it!=remainSet[j].end();it++ ) 
                    next.insert(*it);
                if ( unionCnt == unionSize ) {
                    bool ok = true;
                    for ( int k = 0 ; k < (int)eraseSet.size() ; k++ ) 
                        if ( isSetInSet(eraseSet[k],next) ) 
                            ok = false;
                    if ( ok ) 
                        nextSet.push_back(next);
                }
            }
        itemSet.clear();
        remainSet.clear();
        for ( int i = 0 ; i < (int)nextSet.size() ; i++ ) {
            int cnt = 0;
            bool ok = true;
            for ( int j = 0 ; j < (int)transactionDataBase.size() ; j++ ) 
                if ( isSetInSet(transactionDataBase[j],nextSet[i]) ) 
                    cnt++;
            if ( cnt < minSupport ) {
                change = true;
                bool isExists = false;
                for ( int j = 0 ; j < (int)eraseSet.size() ; j++ ) 
                    if ( nextSet[i] == eraseSet[j] ) 
                        isExists = true;
                if ( !isExists ) 
                    eraseSet.push_back(nextSet[i]);
            } else {
                bool isExists = false;
                for ( int j = 0 ; j < (int)remainSet.size() ; j++ ) 
                    if ( nextSet[i] == remainSet[j] ) 
                        isExists = true;
                if ( !isExists ) 
                    remainSet.push_back(nextSet[i]);
            }
        }

        puts("remainSet");
        printVectorSet(remainSet);
        puts("eraseSet");
        printVectorSet(eraseSet);
    }
    return 0;
}
